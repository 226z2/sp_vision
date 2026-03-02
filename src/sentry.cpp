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
#include "io/dm02/types/control_types.hpp"
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

// 直接使用 third_party/Communication 的 DM-02 host driver（与 tests/auto_aim_test.cpp 一致）
#include "modules/communication/drivers/dm_02/dm_02.hpp"
#if !defined(_WIN32)
#include "modules/communication/drivers/dm_02/serial_transport_posix.hpp"
#endif

using namespace std::chrono;

const std::string keys =
  "{help h usage ? |                        | 输出命令行参数说明}"
  "{@config-path   | configs/sentry.yaml | 位置参数，yaml配置文件路径 }"
  // 为了不改下位机固件(H7_new)的前提下调试通信，这里提供与 tests/auto_aim_test.cpp 同款参数：
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

// 与 tests/auto_aim_test.cpp 对齐：通信线程负责 step()+发送，主线程只负责“算指令并投递最新delta”
// 这样可以避免主线程和通信线程并发操作 driver（driver 不保证线程安全）。
struct DmWorker
{
  std::unique_ptr<communication::dm_02::Driver> driver;

  std::thread thread;
  std::atomic<bool> quit{false};

  std::atomic<bool> established{false};
  std::atomic<bool> have_state{false};
  // 下位机上报角度单位：微度 udeg = deg * 1e6
  std::atomic<std::int32_t> yaw_udeg{0};
  std::atomic<std::int32_t> pitch_udeg{0};
  std::atomic<std::int32_t> roll_udeg{0};
  std::atomic<std::int32_t> bullet_speed_x100{0};  // 下位机上报：m/s * 100（0 表示无效）

  std::mutex cmd_mutex;
  std::optional<communication::dm_02::GimbalDelta> pending;

  ~DmWorker() { stop(); }

  void start()
  {
    if (!driver) return;
    quit.store(false, std::memory_order_release);
    thread = std::thread([this] {
      using namespace std::chrono_literals;
      while (!quit.load(std::memory_order_acquire)) {
        try {
          // step() = poll + tick：包含握手、timesync、上行 state 回调等
          driver->step(20);
        } catch (...) {
        }

        established.store(driver->established(), std::memory_order_release);

        // 发送策略：只保留最新一帧 delta（pending 会被覆盖），防止积压导致云台滞后
        std::optional<communication::dm_02::GimbalDelta> cmd;
        {
          std::lock_guard<std::mutex> lock(cmd_mutex);
          cmd = pending;
          pending.reset();
        }
        if (cmd) (void)driver->send_gimbal_delta(*cmd);

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
  io::USBCamera usbcam1("video0", config_path);
  io::USBCamera usbcam2("video2", config_path);

  auto_aim::YOLO yolo(config_path, false);
  auto_aim::Solver solver(config_path);
  auto_aim::Tracker tracker(config_path, solver);
  auto_aim::Aimer aimer(config_path);
  auto_aim::Shooter shooter(config_path);

  omniperception::Decider decider(config_path);

  // 从yaml读取通信/弹速/左右枪配置（尽量沿用现有configs，减少对下位机(H7_new)改动）
  // 注意：这里不再使用 io::Dm02，是为了把发送/线程模型与 tests/auto_aim_test.cpp 完全对齐，便于后续补齐“发射下行协议”。
  const YAML::Node yaml = tools::load(config_path);
  std::string dm_endpoint;
  if (!dm_endpoint_override.empty()) {
    // 最高优先级：命令行覆盖
    dm_endpoint = dm_endpoint_override;
  } else if (yaml["dm02_endpoint"]) {
    dm_endpoint = yaml["dm02_endpoint"].as<std::string>();
  } else if (yaml["com_port"]) {
    const auto com_port = yaml["com_port"].as<std::string>();
    int baud = 115200;
    if (yaml["dm02_baud"]) baud = yaml["dm02_baud"].as<int>();
    dm_endpoint = "serial:" + com_port + "?baud=" + std::to_string(baud);
  }

  io::ShootMode shoot_mode = io::ShootMode::both_shoot;
  if (yaml["dm02_shoot_mode"]) shoot_mode = parse_shoot_mode(yaml["dm02_shoot_mode"].as<std::string>());

  double bullet_speed_default = 0.0;
  if (yaml["dm02_bullet_speed_default"]) bullet_speed_default = yaml["dm02_bullet_speed_default"].as<double>();

  // 通信线程：接收下位机 state + 下发 gimbal delta
  DmWorker dm;
#if !defined(_WIN32)
  if (!dm_endpoint.empty()) {
    auto transport = std::make_unique<communication::dm_02::SerialTransportPosix>();
    communication::dm_02::Config cfg{};
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
      // 仅缓存最新姿态/弹速，主循环直接读取这些原子变量计算 q 与 bullet_speed
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
    // 与 tests/auto_aim_test.cpp 一致：使用下位机“最新上报”的 yaw/pitch/roll 直接构造四元数
    // 注意：这里不做 imu_at(timestamp-1ms) 插值，先对齐通信链路；需要更精细对齐时再引入 timesync+插值队列。
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
      command = decider.decide(yolo, gimbal_pos, usbcam1, usbcam2, back_camera);
    else
      command = aimer.aim(
        targets, timestamp,
        // 弹速优先级：命令行 > 下位机上报 > yaml默认（保持与测试用例“可手动覆盖”的调试习惯一致）
        (bullet_speed_override > 0.0)
          ? bullet_speed_override
          : ((dm.driver && dm.have_state.load(std::memory_order_acquire) &&
              dm.bullet_speed_x100.load(std::memory_order_acquire) > 0)
               ? (static_cast<double>(dm.bullet_speed_x100.load(std::memory_order_acquire)) / 100.0)
               : bullet_speed_default),
        shoot_mode);

    /// 发射逻辑
    command.shoot = shooter.shoot(command, aimer, targets, gimbal_pos);

    // 安全开关：默认不允许 shoot 生效（当前 DM-02 下行仅实现云台 delta；“开火命令”协议需要另行补齐）
    if (!allow_shoot) command.shoot = false;

    // 与 tests/auto_aim_test.cpp 保持一致：下发云台“相对角度误差 delta”
    // - 有目标：发送误差角 + status bit0=1
    // - 无目标：发送 0 delta + status bit0=0（通知下位机清“有目标”状态）
    if (dm.driver && dm_send && dm.established.load(std::memory_order_acquire)) {
      const auto ypr = tools::eulers(q, 2, 1, 0);
      const double gimbal_yaw = ypr[0];
      const double gimbal_pitch = ypr[1];

      double dyaw_rad = 0.0;
      double dpitch_rad = 0.0;
      if (command.control) {
        dyaw_rad = tools::limit_rad(command.yaw - gimbal_yaw);
        dpitch_rad = tools::limit_rad(command.pitch - gimbal_pitch);
      }

      communication::dm_02::GimbalDelta delta{};
      delta.delta_yaw_udeg = static_cast<std::int32_t>(std::llround(dyaw_rad * kRad2Udeg));
      delta.delta_pitch_udeg = static_cast<std::int32_t>(std::llround(dpitch_rad * kRad2Udeg));
      delta.status = command.control ? 0x0001u : 0u;  // bit0=target_valid
      // 这里使用“本帧图像的时间戳”（而不是 now），方便下位机侧做时序对齐/记录（若固件实现了）
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
