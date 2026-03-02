#include <fmt/core.h>

#include <Eigen/Geometry>

#include <atomic>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>

#include "io/camera.hpp"
#include "io/control_types.hpp"
#include "io/ros2/publish2nav.hpp"
#include "io/ros2/ros2.hpp"
#include "io/usbcamera/usbcamera.hpp"
#include "tasks/auto_aim/aimer.hpp"
#include "tasks/auto_aim/shooter.hpp"
#include "tasks/auto_aim/solver.hpp"
#include "tasks/auto_aim/tracker.hpp"
#include "tasks/auto_aim/yolo.hpp"
#include "tasks/omniperception/decider.hpp"
#include "tools/exiter.hpp"
#include "tools/img_tools.hpp"
#include "tools/logger.hpp"
#include "tools/math_tools.hpp"
#include "tools/plotter.hpp"
#include "tools/recorder.hpp"
#include "tools/yaml.hpp"

#include "modules/communication/drivers/dm_02/dm_02.hpp"
#if !defined(_WIN32)
#include "modules/communication/drivers/dm_02/serial_transport_posix.hpp"
#endif

using namespace std::chrono;

const std::string keys =
  "{help h usage ? |                        | 输出命令行参数说明}"
  "{@config-path   | configs/sentry.yaml | 位置参数，yaml配置文件路径 }"
  // 下面几个参数是为了“对齐 tests/auto_aim_test.cpp 的通信方式”，便于在不改 yaml / 不改下位机固件的情况下调试：
  "{dm-endpoint d  |                     | 覆盖yaml中的DM-02 endpoint (e.g. serial:/dev/ttyACM0?baud=115200) }"
  "{dm-send        | true                | 是否向下位机发送云台DELTA控制(默认true) }"
  "{allow-shoot    | false               | 是否允许计算出的shoot生效(默认false，先只跟随) }"
  "{bullet-speed   | 0                   | 弹速(m/s)，0表示优先用下位机上报/否则用yaml默认 }";

namespace
{
constexpr double kDeg2Rad = 3.14159265358979323846 / 180.0;
constexpr double kRad2Udeg = 180.0 * 1000000.0 / 3.14159265358979323846;  // rad -> micro-degree

io::ShootMode parse_shoot_mode(const std::string & s)
{
  if (s == "left_shoot") return io::ShootMode::left_shoot;
  if (s == "right_shoot") return io::ShootMode::right_shoot;
  if (s == "both_shoot") return io::ShootMode::both_shoot;
  return io::ShootMode::both_shoot;
}

// 与 tests/auto_aim_test.cpp 保持一致：使用单独线程 step() + 异步发送 gimbal_delta
// 目的：
// 1) 主线程只负责“算指令”（识别/跟踪/解算/决策），不直接碰 driver->step()/send()
// 2) 通信线程负责“收包+握手+时钟同步+发送”，避免并发访问 driver（driver 本身不保证线程安全）
// 3) 发送策略：每次循环只保留最新一帧的 delta（pending 覆盖），防止积压导致云台滞后
struct DmWorker
{
  std::unique_ptr<communication::dm_02::Driver> driver;

  std::thread thread;
  std::atomic<bool> quit{false};

  std::atomic<bool> established{false};
  std::atomic<bool> have_state{false};
  // 下位机上报角度单位：微度 udeg = deg * 1e6（与 third_party/Communication 的 DM-02 协议一致）
  std::atomic<std::int32_t> yaw_udeg{0};
  std::atomic<std::int32_t> pitch_udeg{0};
  std::atomic<std::int32_t> roll_udeg{0};
  std::atomic<std::int32_t> bullet_speed_x100{0};  // 下位机上报：m/s * 100（0 表示无效）

  std::mutex cmd_mutex;
  std::optional<communication::dm_02::GimbalDelta> pending;
  std::atomic<std::uint64_t> send_ok{0};
  std::atomic<std::uint64_t> send_fail{0};

  ~DmWorker() { stop(); }

  void start()
  {
    if (!driver) return;
    quit.store(false, std::memory_order_release);
    thread = std::thread([this] {
      using namespace std::chrono_literals;
      while (!quit.load(std::memory_order_acquire)) {
        try {
          // step() 内部包含 poll + tick：处理握手、timesync、以及上行的 gimbal_state 回调
          driver->step(20);
        } catch (...) {
        }

        // established 表示握手完成；未建立时即使 send 也可能会失败/被丢弃
        established.store(driver->established(), std::memory_order_release);

        std::optional<communication::dm_02::GimbalDelta> cmd;
        {
          std::lock_guard<std::mutex> lock(cmd_mutex);
          cmd = pending;
          pending.reset();
        }

        if (cmd) {
          // 这里只发送云台“相对角度误差”（delta），shoot/模式切换等不在本文件实现（保持与测试用例一致）
          const bool ok = driver->send_gimbal_delta(*cmd);
          if (ok) {
            send_ok.fetch_add(1, std::memory_order_relaxed);
          } else {
            send_fail.fetch_add(1, std::memory_order_relaxed);
          }
        }

        std::this_thread::sleep_for(2ms);
      }
    });
  }

  void stop()
  {
    quit.store(true, std::memory_order_release);
    if (thread.joinable()) thread.join();
    if (driver) driver->close();
  }

  void queue_delta(const communication::dm_02::GimbalDelta & delta)
  {
    std::lock_guard<std::mutex> lock(cmd_mutex);
    pending = delta;
  }
};
}  // namespace

int main(int argc, char * argv[])
{
  tools::Exiter exiter;
  tools::Plotter plotter;
  tools::Recorder recorder;

  cv::CommandLineParser cli(argc, argv, keys);
  if (cli.has("help")) {
    cli.printMessage();
    return 0;
  }
  auto config_path = cli.get<std::string>(0);
  auto dm_endpoint_override = cli.get<std::string>("dm-endpoint");
  auto dm_send = cli.get<bool>("dm-send");
  auto allow_shoot = cli.get<bool>("allow-shoot");
  auto bullet_speed_override = cli.get<double>("bullet-speed");

  io::ROS2 ros2;
  io::Camera camera(config_path);
  io::Camera back_camera("configs/camera.yaml");

  auto_aim::YOLO yolo(config_path, false);
  auto_aim::Solver solver(config_path);
  auto_aim::Tracker tracker(config_path, solver);
  auto_aim::Aimer aimer(config_path);
  auto_aim::Shooter shooter(config_path);

  omniperception::Decider decider(config_path);

  // 从yaml读取通信/弹速/左右枪配置（尽量沿用现有configs，减少对下位机(H7_new)改动）
  // 注意：这里故意不再走 io::Dm02Board，是为了把本文件的“通信/收发/时戳/单位”与 tests/auto_aim_test.cpp 完全对齐，
  // 后续排查“能跟随但不能发射”这类协议缺失问题时，能够直接复用测试用例的经验。
  const YAML::Node yaml = tools::load(config_path);
  std::string dm_endpoint;
  if (!dm_endpoint_override.empty()) {
    // 最高优先级：命令行覆盖，方便快速切换串口
    dm_endpoint = dm_endpoint_override;
  } else if (yaml["dm02_endpoint"]) {
    // 次优先级：yaml 里显式写 dm02_endpoint（推荐）
    dm_endpoint = yaml["dm02_endpoint"].as<std::string>();
  } else if (yaml["com_port"]) {
    // 兼容旧配置：com_port + dm02_baud 组合成 endpoint
    const auto com_port = yaml["com_port"].as<std::string>();
    int baud = 115200;
    if (yaml["dm02_baud"]) baud = yaml["dm02_baud"].as<int>();
    dm_endpoint = "serial:" + com_port + "?baud=" + std::to_string(baud);
  }

  io::ShootMode shoot_mode = io::ShootMode::both_shoot;
  if (yaml["dm02_shoot_mode"]) shoot_mode = parse_shoot_mode(yaml["dm02_shoot_mode"].as<std::string>());

  double bullet_speed_default = 0.0;
  if (yaml["dm02_bullet_speed_default"]) bullet_speed_default = yaml["dm02_bullet_speed_default"].as<double>();

  // 与 tests/auto_aim_test.cpp 一致：直接使用 dm_02::Driver + DmWorker 线程模型
  DmWorker dm;
#if !defined(_WIN32)
  if (!dm_endpoint.empty()) {
    auto transport = std::make_unique<communication::dm_02::SerialTransportPosix>();
    communication::dm_02::Config cfg{};
    // 开启 timesync：用于把设备侧时间映射到主机时间轴（本文件暂不依赖该映射，但保留与测试用例一致的配置）
    cfg.timesync_enable = true;
    cfg.timesync_period_ms = 1000;
    cfg.timesync_initiator = true;

    dm.driver = std::make_unique<communication::dm_02::Driver>(std::move(transport), cfg);
    communication::dm_02::Callbacks cb{};
    cb.on_uproto_event = [](std::string_view ev) { tools::logger()->info("[UPROTO] {}", ev); };
    cb.on_timesync = [](const communication::dm_02::TimeSyncStatus & ts) {
      tools::logger()->info(
        "[TS] valid={} offset_us={} rtt_us={} ver={}", ts.valid, ts.offset_us, ts.rtt_us, ts.version);
    };
    cb.on_gimbal_state = [&dm](const communication::dm_02::GimbalState & st) {
      // 这里仅缓存“本帧最新的云台姿态/弹速”，主循环会读取这些原子变量来构造 q / bullet_speed。
      dm.yaw_udeg.store(st.yaw_udeg, std::memory_order_release);
      dm.pitch_udeg.store(st.pitch_udeg, std::memory_order_release);
      dm.roll_udeg.store(st.roll_udeg, std::memory_order_release);
      dm.bullet_speed_x100.store(st.bullet_speed_x100, std::memory_order_release);
      dm.have_state.store(true, std::memory_order_release);
    };
    dm.driver->set_callbacks(std::move(cb));

    if (!dm.driver->open(dm_endpoint)) {
      tools::logger()->warn("dm-endpoint open failed: {}", dm_endpoint);
      dm.driver.reset();
    } else {
      tools::logger()->info("dm-endpoint opened: {}", dm_endpoint);
      dm.start();
    }
  }
#else
  (void)dm_endpoint;
#endif

  cv::Mat img;

  std::chrono::steady_clock::time_point timestamp;
  io::Command last_command;

  while (!exiter.exit()) {
    camera.read(img, timestamp);
    // 通信侧上报的欧拉角为微度（deg * 1e6），与 tests/auto_aim_test.cpp 保持一致在主循环直接转四元数。
    // 注意：
    // - 这里不做 imu_at(timestamp-1ms) 的插值/对齐（Dm02Link 那套做法），以“测试用例”为准先跑通协议链路
    // - 如果需要更精细的时域对齐（比如录包/回放），可以后续再引入 timesync 映射与插值队列
    Eigen::Quaterniond q = Eigen::Quaterniond::Identity();
    if (dm.driver && dm.have_state.load(std::memory_order_acquire)) {
      const double yaw =
        (static_cast<double>(dm.yaw_udeg.load(std::memory_order_acquire)) / 1000000.0) * kDeg2Rad;
      const double pitch =
        (static_cast<double>(dm.pitch_udeg.load(std::memory_order_acquire)) / 1000000.0) * kDeg2Rad;
      const double roll =
        (static_cast<double>(dm.roll_udeg.load(std::memory_order_acquire)) / 1000000.0) * kDeg2Rad;

      q = Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitZ()) *
          Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY()) *
          Eigen::AngleAxisd(roll, Eigen::Vector3d::UnitX());
      q.normalize();
    }
    // recorder.record(img, q, timestamp);

    /// 自瞄核心逻辑
    solver.set_R_gimbal2world(q);

    Eigen::Vector3d gimbal_pos = tools::eulers(solver.R_gimbal2world(), 2, 1, 0);

    auto armors = yolo.detect(img);

    decider.get_invincible_armor(ros2.subscribe_enemy_status());

    decider.armor_filter(armors);

    // decider.get_auto_aim_target(armors, ros2.subscribe_autoaim_target());

    decider.set_priority(armors);

    auto targets = tracker.track(armors, timestamp);

    io::Command command{false, false, 0, 0};

    /// 全向感知逻辑
    if (tracker.state() == "lost")
      command = decider.decide(yolo, gimbal_pos, back_camera);
    else
      command = aimer.aim(
        targets, timestamp,
        // 弹速优先级：命令行 > 下位机上报 > yaml默认（与测试用例的“可手动覆盖”思路一致）
        (bullet_speed_override > 0.0)
          ? bullet_speed_override
          : ((dm.driver && dm.have_state.load(std::memory_order_acquire) &&
              dm.bullet_speed_x100.load(std::memory_order_acquire) > 0)
               ? (static_cast<double>(dm.bullet_speed_x100.load(std::memory_order_acquire)) / 100.0)
               : bullet_speed_default),
        shoot_mode);

    /// 发射逻辑
    command.shoot = shooter.shoot(command, aimer, targets, gimbal_pos);

    // 安全开关：默认不允许 shoot 生效（即使上层算法判断“该开火”，也先不让它变成有效指令）
    // 说明：当前 third_party/Communication 的 DM-02 host 侧示例（tests/auto_aim_test.cpp）只下发云台 delta，
    // 并没有下发“开火/发射”相关的下行协议字段；这里保持一致，避免误触发。
    if (!allow_shoot) command.shoot = false;

    // 与 tests/auto_aim_test.cpp 保持一致：向下位机发送“相对角度 delta”（无目标时发送 0 delta 并清 status bit0）
    if (dm.driver && dm_send && dm.established.load(std::memory_order_acquire)) {
      const auto ypr = tools::eulers(q, 2, 1, 0);
      const double gimbal_yaw = ypr[0];
      const double gimbal_pitch = ypr[1];

      double dyaw_rad = 0.0;
      double dpitch_rad = 0.0;
      if (command.control) {
        // 注意：yaw/pitch 都是“绝对目标角”（rad），这里转换成“相对误差角”（delta），由下位机闭环跟随
        dyaw_rad = tools::limit_rad(command.yaw - gimbal_yaw);
        dpitch_rad = tools::limit_rad(command.pitch - gimbal_pitch);
      }

      communication::dm_02::GimbalDelta delta{};
      delta.delta_yaw_udeg = static_cast<std::int32_t>(std::llround(dyaw_rad * kRad2Udeg));
      delta.delta_pitch_udeg = static_cast<std::int32_t>(std::llround(dpitch_rad * kRad2Udeg));
      // status bit0: target_valid（host 当前帧有有效目标/控制指令）
      delta.status = command.control ? 0x0001u : 0u;
      // host_ts_ns：本帧图像/算法对应的主机时间戳（steady_clock），用于下位机侧的时序对齐/记录（如果固件实现了）
      delta.host_ts_ns = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()).count());
      dm.queue_delta(delta);
    }

    /// ROS2通信
    Eigen::Vector4d target_info = decider.get_target_info(armors, targets);

    ros2.publish(target_info);
  }
  return 0;
}
