#include <fmt/core.h>

#include <chrono>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>
#include <thread>

#include "io/camera.hpp"
#include "io/dm02/dm02.hpp"
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
#include "src/referee_runtime.hpp"

using namespace std::chrono;

const std::string keys =
  "{help h usage ? |                        | 输出命令行参数说明}"
  "{@config-path   | configs/sentry.yaml | 位置参数，yaml配置文件路径 }";

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

  io::ROS2 ros2;
  io::Dm02 cboard(config_path);
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

  cv::Mat img;

  std::chrono::steady_clock::time_point timestamp;
  io::Command last_command;

  while (!exiter.exit()) {
    camera.read(img, timestamp);
    Eigen::Quaterniond q = cboard.imu_at(timestamp - 1ms);
    // recorder.record(img, q, timestamp);

    /// 自瞄核心逻辑
    solver.set_R_gimbal2world(q);

    Eigen::Vector3d gimbal_pos = tools::eulers(solver.R_gimbal2world(), 2, 1, 0);

    auto armors = yolo.detect(img);

    const auto ref_io = cboard.referee();
    const auto ref = referee_runtime::from_io(ref_io);
    if (const auto color = referee_runtime::enemy_color(ref)) {
      tracker.set_enemy_color(*color);
      decider.set_enemy_color(*color);
    } else {
      tracker.reset_enemy_color();
      decider.reset_enemy_color();
    }

    decider.armor_filter(armors);

    decider.set_priority(armors);

    auto targets = tracker.track(armors, timestamp);

    io::Command command{false, false, 0, 0};

    /// 全向感知逻辑
    if (tracker.state() == "lost")
      command = decider.decide(yolo, gimbal_pos, usbcam1, usbcam2, back_camera);
    else
      command = aimer.aim(targets, timestamp, cboard.bullet_speed, cboard.shoot_mode);

    /// 发射逻辑
    command.shoot = shooter.shoot(command, aimer, targets, gimbal_pos);
    if (!referee_runtime::can_fire(ref)) command.shoot = false;

    cboard.send(command);

    /// ROS2通信
    Eigen::Vector4d target_info = decider.get_target_info(armors, targets);

    ros2.publish(target_info);

    const auto state = cboard.state();
    const auto enc = cboard.encoders();
    const auto dev = cboard.device_status();
    const auto tof = cboard.tof();
    const auto ts = cboard.timesync();
    sp_msgs::msg::Dm02SerialCopyMsg serial_copy{};
    serial_copy.timestamp = rclcpp::Clock().now();
    serial_copy.established = true;
    serial_copy.have_state = true;
    serial_copy.send_ok = 0;
    serial_copy.send_fail = 0;

    serial_copy.yaw_rad = state.yaw;
    serial_copy.pitch_rad = state.pitch;
    serial_copy.roll_rad = 0.0F;
    serial_copy.enc_yaw = enc.yaw;
    serial_copy.enc_pitch = enc.pitch;
    serial_copy.yaw_vel_rad_s = state.yaw_vel;
    serial_copy.pitch_vel_rad_s = state.pitch_vel;
    serial_copy.bullet_speed_mps = state.bullet_speed;
    serial_copy.bullet_count = state.bullet_count;
    serial_copy.gimbal_mode = static_cast<int>(cboard.gimbal_mode());
    serial_copy.shoot_state = dev.shoot_state;
    serial_copy.shooter_heat = dev.shooter_heat;
    serial_copy.shooter_heat_limit = dev.shooter_heat_limit;
    serial_copy.projectile_allowance_17mm = dev.projectile_allowance_17mm;
    serial_copy.projectile_allowance_42mm = dev.projectile_allowance_42mm;
    serial_copy.state_device_ts_us = 0;
    serial_copy.state_host_ts_ns = 0;

    serial_copy.referee_valid = ref_io.valid;
    serial_copy.referee_enemy_team = ref_io.enemy_team;
    serial_copy.referee_fire_allowed = ref_io.fire_allowed;
    serial_copy.referee_robot_id = ref_io.robot_id;
    serial_copy.referee_game_stage = ref_io.game_stage;
    serial_copy.referee_status = ref_io.status;
    serial_copy.referee_device_ts_us = ref_io.device_ts_us;
    serial_copy.referee_host_ts_ns = ref_io.host_ts_ns;

    serial_copy.tof_valid = tof.valid;
    serial_copy.tof_distance_cm = tof.distance_cm;
    serial_copy.tof_strength = 0;
    serial_copy.tof_temp_cdeg = 0;
    serial_copy.tof_status = 0;
    serial_copy.tof_device_ts_us = tof.device_ts_us;
    serial_copy.tof_host_ts_ns = tof.host_ts_ns;

    serial_copy.timesync_valid = ts.valid;
    serial_copy.timesync_offset_us = ts.offset_us;
    serial_copy.timesync_rtt_us = ts.rtt_us;
    serial_copy.timesync_version = ts.version;
    serial_copy.timesync_last_device_time_us = ts.last_device_time_us;
    serial_copy.timesync_last_host_time_us = ts.last_host_time_us;

    ros2.publish_serial_copy(serial_copy);

    /// debug
    tools::draw_text(img, fmt::format("[{}]", tracker.state()), {10, 30}, {255, 255, 255});

    nlohmann::json data;

    // 装甲板原始观测数据
    data["armor_num"] = armors.size();
    if (!armors.empty()) {
      auto min_x = 1e10;
      auto & armor = armors.front();
      for (auto & a : armors) {
        if (a.center.x < min_x) {
          min_x = a.center.x;
          armor = a;
        }
      }  //always left
      solver.solve(armor);
      data["armor_x"] = armor.xyz_in_world[0];
      data["armor_y"] = armor.xyz_in_world[1];
      data["armor_yaw"] = armor.ypr_in_world[0] * 57.3;
      data["armor_yaw_raw"] = armor.yaw_raw * 57.3;
    }

    if (!targets.empty()) {
      auto target = targets.front();

      // 当前帧target更新后
      std::vector<Eigen::Vector4d> armor_xyza_list = target.armor_xyza_list();
      for (const Eigen::Vector4d & xyza : armor_xyza_list) {
        auto image_points =
          solver.reproject_armor(xyza.head(3), xyza[3], target.armor_type, target.name);
        tools::draw_points(img, image_points, {0, 255, 0});
      }

      // aimer瞄准位置
      auto aim_point = aimer.debug_aim_point;
      Eigen::Vector4d aim_xyza = aim_point.xyza;
      auto image_points =
        solver.reproject_armor(aim_xyza.head(3), aim_xyza[3], target.armor_type, target.name);
      if (aim_point.valid)
        tools::draw_points(img, image_points, {0, 0, 255});
      else
        tools::draw_points(img, image_points, {255, 0, 0});

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

    // 云台响应情况
    data["gimbal_yaw"] = gimbal_pos[0] * 57.3;
    data["gimbal_pitch"] = -gimbal_pos[1] * 57.3;
    data["shootmode"] = cboard.shoot_mode;
    if (command.control) {
      data["cmd_yaw"] = command.yaw * 57.3;
      data["cmd_pitch"] = command.pitch * 57.3;
      data["cmd_shoot"] = command.shoot;
    }

    data["bullet_speed"] = cboard.bullet_speed;

    plotter.plot(data);

#if defined(SPV_PLOTTER_BACKEND_FOXGLOVE)
    plotter.plot_image(img, "reprojection");
#else
    cv::resize(img, img, {}, 0.5, 0.5);  // 显示时缩小图片尺寸
    cv::imshow("reprojection", img);
    auto key = cv::waitKey(1);
    if (key == 'q') break;
#endif
  }
  return 0;
}
