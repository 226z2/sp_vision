#include <fmt/core.h>

#include <Eigen/Geometry>

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <string>
#include <thread>

#include "io/camera.hpp"
#include "tasks/auto_aim/aimer.hpp"
#include "tasks/auto_aim/solver.hpp"
#include "tasks/auto_aim/tracker.hpp"
#include "tasks/auto_aim/yolo.hpp"
#include "tools/exiter.hpp"
#include "tools/img_tools.hpp"
#include "tools/logger.hpp"
#include "tools/math_tools.hpp"
#include "tools/plotter.hpp"

#include "io/dm02/impl/dm02_driver.hpp"
#if !defined(_WIN32)
#include "io/dm02/impl/serial_transport_posix.hpp"
#endif

namespace
{
enum class Source
{
  video,
  camera,
};

const char * to_cstr(Source s)
{
  switch (s) {
    case Source::video:
      return "video";
    case Source::camera:
      return "camera";
    default:
      return "unknown";
  }
}

Source parse_source(const std::string & s)
{
  if (s == "video") return Source::video;
  if (s == "camera") return Source::camera;
  throw std::runtime_error("Invalid --source: " + s + " (expected: video|camera)");
}
}  // namespace

const std::string keys =
  "{help h usage ? |                   | 输出命令行参数说明 }"
  "{config-path c  | configs/sentry.yaml | yaml配置文件的路径}"
  "{source         | camera            | 输入源: video/camera }"
  "{start-index s  | 0                 | 视频起始帧下标    }"
  "{end-index e    | 0                 | 视频结束帧下标    }"
  "{dm-endpoint d  | serial:/dev/ttyACM0?baud=115200 | DM-02 endpoint (e.g. serial:/dev/ttyACM0?baud=115200) }"
  "{dm-send        | true              | 打开dm-endpoint时是否向云台发送DELTA控制(默认true) }"
  "{allow-shoot    | false             | 是否允许发送shoot（默认false，建议先只跟随不发射） }"
  "{bullet-speed   | 27                | 弹速(m/s)，用于弹道/角度解算 }"
  "{@input-path    | assets/demo/demo  | avi和txt文件的路径}";

namespace
{
struct DmWorker
{
  std::unique_ptr<communication::dm_02::Driver> driver;

  std::thread thread;
  std::atomic<bool> quit{false};

  std::atomic<bool> established{false};
  std::atomic<bool> have_state{false};
  std::atomic<std::int32_t> yaw_udeg{0};
  std::atomic<std::int32_t> pitch_udeg{0};
  std::atomic<std::int32_t> roll_udeg{0};
  std::atomic<std::int32_t> yaw_cmd_current{0};
  std::atomic<std::int32_t> pitch_cmd_current{0};
  std::atomic<std::int32_t> yaw_meas_current{0};
  std::atomic<std::int32_t> pitch_meas_current{0};
  std::atomic<std::int32_t> gimbal_mode{0};

  std::mutex cmd_mutex;
  std::optional<communication::dm_02::GimbalDelta> pending;
  std::atomic<std::uint64_t> send_ok{0};
  std::atomic<std::uint64_t> send_fail{0};

  ~DmWorker()
  {
    stop();
  }

  void start()
  {
    if (!driver) return;
    quit.store(false, std::memory_order_release);
    thread = std::thread([this] {
      using namespace std::chrono_literals;
      while (!quit.load(std::memory_order_acquire)) {
        try {
          driver->step(20);
        } catch (...) {
        }

        established.store(driver->established(), std::memory_order_release);

        std::optional<communication::dm_02::GimbalDelta> cmd;
        {
          std::lock_guard<std::mutex> lock(cmd_mutex);
          cmd = pending;
          pending.reset();
        }

        if (cmd) {
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
  // 读取命令行参数
  cv::CommandLineParser cli(argc, argv, keys);
  if (cli.has("help")) {
    cli.printMessage();
    return 0;
  }
  auto input_path = cli.get<std::string>(0);
  auto config_path = cli.get<std::string>("config-path");
  Source source = Source::video;
  try {
    source = parse_source(cli.get<std::string>("source"));
  } catch (const std::exception & e) {
    tools::logger()->error("{}", e.what());
    cli.printMessage();
    return 2;
  }
  auto start_index = cli.get<int>("start-index");
  auto end_index = cli.get<int>("end-index");
  auto dm_endpoint = cli.get<std::string>("dm-endpoint");
  auto dm_send = cli.get<bool>("dm-send");
  auto allow_shoot = cli.get<bool>("allow-shoot");
  auto bullet_speed = cli.get<double>("bullet-speed");

  tools::logger()->info(
    "auto_aim_test args: source={} config-path={} dm-endpoint={} dm-send={} allow-shoot={} bullet-speed={}",
    to_cstr(source), config_path, dm_endpoint, dm_send, allow_shoot, bullet_speed);

  tools::Plotter plotter;
  tools::Exiter exiter;

  cv::VideoCapture video;
  std::ifstream text;
  std::unique_ptr<io::Camera> camera;
  auto t0 = std::chrono::steady_clock::now();
  if (source == Source::video) {
    auto video_path = fmt::format("{}.avi", input_path);
    auto text_path = fmt::format("{}.txt", input_path);
    video.open(video_path);
    text.open(text_path);
    if (!video.isOpened() || !text.is_open()) {
      tools::logger()->error("Failed to open video/txt from: {}", input_path);
      return 2;
    }
  } else {
    camera = std::make_unique<io::Camera>(config_path);
  }

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
    cb.on_timesync = [](const communication::dm_02::TimeSyncStatus& ts) {
      tools::logger()->info(
        "[TS] valid={} offset_us={} rtt_us={} ver={}", ts.valid, ts.offset_us, ts.rtt_us, ts.version);
    };
    cb.on_gimbal_state = [&dm](const communication::dm_02::GimbalState & st) {
      dm.yaw_udeg.store(st.yaw_udeg, std::memory_order_release);
      dm.pitch_udeg.store(st.pitch_udeg, std::memory_order_release);
      dm.roll_udeg.store(st.roll_udeg, std::memory_order_release);
      dm.yaw_cmd_current.store(st.yaw_cmd_current, std::memory_order_release);
      dm.pitch_cmd_current.store(st.pitch_cmd_current, std::memory_order_release);
      dm.yaw_meas_current.store(st.yaw_meas_current, std::memory_order_release);
      dm.pitch_meas_current.store(st.pitch_meas_current, std::memory_order_release);
      dm.gimbal_mode.store(st.gimbal_mode, std::memory_order_release);
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

  auto_aim::YOLO yolo(config_path);
  auto_aim::Solver solver(config_path);
  auto_aim::Tracker tracker(config_path, solver);
  auto_aim::Aimer aimer(config_path);

  cv::Mat img, drawing;

  auto_aim::Target last_target;
  io::Command last_command;
  double last_t = -1;

  constexpr double kDeg2Rad = 3.14159265358979323846 / 180.0;
  constexpr double kRad2Udeg = 180.0 * 1000000.0 / 3.14159265358979323846;  // rad -> micro-degree

  if (source == Source::video) {
    video.set(cv::CAP_PROP_POS_FRAMES, start_index);
    for (int i = 0; i < start_index; i++) {
      double t, w, x, y, z;
      text >> t >> w >> x >> y >> z;
    }
  }

  for (int frame_count = start_index; !exiter.exit(); frame_count++) {
    if (source == Source::video && end_index > 0 && frame_count > end_index) break;

    double t = 0.0;
    double w = 1.0, x = 0.0, y = 0.0, z = 0.0;
    std::chrono::steady_clock::time_point timestamp = std::chrono::steady_clock::now();

    if (source == Source::video) {
      video.read(img);
      if (img.empty()) break;

      text >> t >> w >> x >> y >> z;
      if (!text.good()) break;
      timestamp = t0 + std::chrono::microseconds(int(t * 1e6));
    } else {
      camera->read(img, timestamp);
      if (img.empty()) break;
    }

    if (dm.driver && dm.have_state.load(std::memory_order_acquire)) {
      // DM-02 gimbal protocol uses micro-degree (deg * 1e6) for angles.
      const double yaw =
        (static_cast<double>(dm.yaw_udeg.load(std::memory_order_acquire)) / 1000000.0) * kDeg2Rad;
      const double pitch = (static_cast<double>(dm.pitch_udeg.load(std::memory_order_acquire)) /
                            1000000.0) *
                           kDeg2Rad;
      const double roll =
        (static_cast<double>(dm.roll_udeg.load(std::memory_order_acquire)) / 1000000.0) * kDeg2Rad;

      Eigen::AngleAxisd yaw_aa(yaw, Eigen::Vector3d::UnitZ());
      Eigen::AngleAxisd pitch_aa(pitch, Eigen::Vector3d::UnitY());
      Eigen::AngleAxisd roll_aa(roll, Eigen::Vector3d::UnitX());
      const Eigen::Quaterniond q = yaw_aa * pitch_aa * roll_aa;
      w = q.w();
      x = q.x();
      y = q.y();
      z = q.z();
    }

    /// 自瞄核心逻辑

    solver.set_R_gimbal2world({w, x, y, z});

    auto yolo_start = std::chrono::steady_clock::now();
    auto armors = yolo.detect(img, frame_count);

    auto tracker_start = std::chrono::steady_clock::now();
    auto targets = tracker.track(armors, timestamp);

    auto aimer_start = std::chrono::steady_clock::now();
    auto command = aimer.aim(targets, timestamp, bullet_speed, false);
    if (!allow_shoot) command.shoot = false;

    double sent_dyaw_rad = 0.0;
    double sent_dpitch_rad = 0.0;
    bool sent_control = false;
    bool sent_target_valid = false;

    // If DM-02 is connected, send gimbal DELTA (error angles) to firmware.
    // Always send: when no valid target/command, send 0 delta and clear status bit0.
    if (dm.driver && dm_send && dm.established.load(std::memory_order_acquire)) {
      const Eigen::Quaterniond gimbal_q{w, x, y, z};
      const auto ypr = tools::eulers(gimbal_q, 2, 1, 0);
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
      // status bit0: target_valid (host has a valid target/command)
      delta.status = command.control ? 0x0001u : 0u;
      delta.host_ts_ns = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp.time_since_epoch()).count());
      dm.queue_delta(delta);
      sent_dyaw_rad = dyaw_rad;
      sent_dpitch_rad = dpitch_rad;
      sent_control = command.control;
      sent_target_valid = command.control;
    }

    if (
      allow_shoot &&
      !targets.empty() && aimer.debug_aim_point.valid &&
      std::abs(command.yaw - last_command.yaw) * 57.3 < 2)
      command.shoot = true;

    if (command.control) last_command = command;
    /// 调试输出

    auto finish = std::chrono::steady_clock::now();
    tools::logger()->info(
      "[{}] yolo: {:.1f}ms, tracker: {:.1f}ms, aimer: {:.1f}ms", frame_count,
      tools::delta_time(tracker_start, yolo_start) * 1e3,
      tools::delta_time(aimer_start, tracker_start) * 1e3,
      tools::delta_time(finish, aimer_start) * 1e3);

    tools::draw_text(
      img,
      fmt::format(
        "cmd:{} yaw:{:.2f} pitch:{:.2f} shoot:{} dm:{} est:{} send:{}/{}",
        command.control, command.yaw * 57.3, command.pitch * 57.3, command.shoot, (dm.driver != nullptr),
        dm.established.load(std::memory_order_acquire), dm.send_ok.load(), dm.send_fail.load()),
      {10, 60}, {154, 50, 205});

    Eigen::Quaternion gimbal_q = {w, x, y, z};
    tools::draw_text(
      img,
      fmt::format(
        "gimbal yaw{:.2f}", (tools::eulers(gimbal_q.toRotationMatrix(), 2, 1, 0) * 57.3)[0]),
      {10, 90}, {255, 255, 255});

    nlohmann::json data;

    // 装甲板原始观测数据
    data["armor_num"] = armors.size();
    data["target_num"] = targets.size();
    data["tracker_state"] = tracker.state();
    if (!armors.empty()) {
      const auto & armor = armors.front();
      data["armor_x"] = armor.xyz_in_world[0];
      data["armor_y"] = armor.xyz_in_world[1];
      data["armor_yaw"] = armor.ypr_in_world[0] * 57.3;
      data["armor_yaw_raw"] = armor.yaw_raw * 57.3;
      data["armor_center_x"] = armor.center_norm.x;
      data["armor_center_y"] = armor.center_norm.y;
    }

    Eigen::Quaternion q{w, x, y, z};
    auto yaw = tools::eulers(q, 2, 1, 0)[0];
    data["gimbal_yaw"] = yaw * 57.3;
    data["cmd_yaw"] = command.yaw * 57.3;
    data["control"] = command.control;
    data["dm_send_dyaw_deg"] = sent_dyaw_rad * 57.3;
    data["dm_send_dpitch_deg"] = sent_dpitch_rad * 57.3;
    data["dm_send_control"] = sent_control;
    data["dm_send_target_valid"] = sent_target_valid;
    data["dm_established"] = dm.established.load(std::memory_order_acquire);
    data["dm_send_ok"] = dm.send_ok.load(std::memory_order_acquire);
    data["dm_send_fail"] = dm.send_fail.load(std::memory_order_acquire);
    data["dm_pitch_cmd_current"] = dm.pitch_cmd_current.load(std::memory_order_acquire);
    data["dm_yaw_meas_current"] = dm.yaw_meas_current.load(std::memory_order_acquire);
    data["dm_pitch_meas_current"] = dm.pitch_meas_current.load(std::memory_order_acquire);
    data["shoot"] = command.shoot;

    if (!targets.empty()) {
      const bool has_current_measurement = !armors.empty();
      auto target = targets.front();

      if (last_t == -1) {
        last_target = target;
        last_t = t;
        continue;
      }

      std::vector<Eigen::Vector4d> armor_xyza_list;

      // 当前帧target更新后
      armor_xyza_list = target.armor_xyza_list();
      if (has_current_measurement) {
        for (const Eigen::Vector4d & xyza : armor_xyza_list) {
          auto image_points =
            solver.reproject_armor(xyza.head(3), xyza[3], target.armor_type, target.name);
          tools::draw_points(img, image_points, {0, 255, 0});
        }
      }

      // aimer瞄准位置
      auto aim_point = aimer.debug_aim_point;
      Eigen::Vector4d aim_xyza = aim_point.xyza;
      if (has_current_measurement) {
        auto image_points =
          solver.reproject_armor(aim_xyza.head(3), aim_xyza[3], target.armor_type, target.name);
        if (aim_point.valid) tools::draw_points(img, image_points, {0, 0, 255});
      }

      // 观测器内部数据
      Eigen::VectorXd x = target.ekf_x();
      data["x"] = x[0];
      data["vx"] = x[1];
      data["y"] = x[2];
      data["vy"] = x[3];
      data["z"] = x[4];
      data["vz"] = x[5];
      data["a"] = x[6] * 57.3;
      data["w"] = x[7];
      data["r"] = x[8];
      data["l"] = x[9];
      data["h"] = x[10];
      data["last_id"] = target.last_id;

      // 卡方检验数据
      data["residual_yaw"] = target.ekf().data.at("residual_yaw");
      data["residual_pitch"] = target.ekf().data.at("residual_pitch");
      data["residual_distance"] = target.ekf().data.at("residual_distance");
      data["residual_angle"] = target.ekf().data.at("residual_angle");
      data["nis"] = target.ekf().data.at("nis");
      data["nees"] = target.ekf().data.at("nees");
      data["nis_fail"] = target.ekf().data.at("nis_fail");
      data["nees_fail"] = target.ekf().data.at("nees_fail");
      data["recent_nis_failures"] = target.ekf().data.at("recent_nis_failures");
    }

    plotter.plot(data);

    // Headless debug output: publish frame to plotter backend (Foxglove when enabled).
    cv::Mat preview;
    cv::resize(img, preview, {}, 0.5, 0.5);
    plotter.plot_image(preview, "reprojection");
  }

  return 0;
}
