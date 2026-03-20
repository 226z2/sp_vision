#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "io/dm02/impl/dm02_driver.hpp"
#include "io/dm02/impl/serial_transport_posix.hpp"
#include "io/ros2/ros2.hpp"
#include "tools/exiter.hpp"
#include "tools/logger.hpp"

namespace {

struct SharedState final {
  std::atomic<bool> have_state{false};
  std::atomic<bool> have_referee{false};
  std::atomic<bool> have_tfmini{false};
  std::atomic<bool> established{false};
  std::atomic<std::uint64_t> send_ok{0};
  std::atomic<std::uint64_t> send_fail{0};

  communication::dm_02::GimbalState gimbal{};
  communication::dm_02::RefereeStatus referee{};
  communication::dm_02::GimbalTfmini tfmini{};
  communication::dm_02::TimeSyncStatus timesync{};
  std::mutex mutex{};
};

void print_usage(const char* argv0) {
  std::printf(
      "Usage:\n"
      "  %s [serial:/dev/ttyXXX?baud=115200]\n",
      argv0 ? argv0 : "dm_ws_bridge_test");
}

}  // namespace

int main(int argc, char** argv) {
  std::string endpoint = "serial:/dev/ttyACM0?baud=115200";
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i] ? argv[i] : "";
    if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      return 0;
    }
    if (!arg.empty() && arg.rfind("-", 0) != 0) endpoint = arg;
  }

  tools::Exiter exiter;
  io::ROS2 ros2;
  rclcpp::Clock clock;
  SharedState shared{};

  auto transport = std::make_unique<communication::dm_02::SerialTransportPosix>();
  communication::dm_02::Config cfg{};
  cfg.timesync_enable = true;
  cfg.timesync_period_ms = 1000;
  cfg.timesync_initiator = true;

  communication::dm_02::Driver driver(std::move(transport), cfg);
  communication::dm_02::Callbacks cb{};
  cb.on_uproto_event = [](std::string_view ev) { tools::logger()->info("[UPROTO] {}", ev); };
  cb.on_gimbal_state = [&shared](const communication::dm_02::GimbalState& st) {
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.gimbal = st;
    shared.have_state.store(true, std::memory_order_release);
  };
  cb.on_referee_status = [&shared](const communication::dm_02::RefereeStatus& st) {
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.referee = st;
    shared.have_referee.store(true, std::memory_order_release);
  };
  cb.on_gimbal_tfmini = [&shared](const communication::dm_02::GimbalTfmini& st) {
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.tfmini = st;
    shared.have_tfmini.store(true, std::memory_order_release);
  };
  cb.on_timesync = [&shared](const communication::dm_02::TimeSyncStatus& ts) {
    std::lock_guard<std::mutex> lock(shared.mutex);
    shared.timesync = ts;
  };
  driver.set_callbacks(std::move(cb));

  if (!driver.open(endpoint)) {
    tools::logger()->error("dm_ws_bridge_test: failed to open {}", endpoint);
    return 2;
  }
  tools::logger()->info("dm_ws_bridge_test opened {}", endpoint);

  auto next_publish = std::chrono::steady_clock::now();
  while (!exiter.exit()) {
    driver.step(20);
    shared.established.store(driver.established(), std::memory_order_release);

    const auto now = std::chrono::steady_clock::now();
    if (now < next_publish) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      continue;
    }
    next_publish = now + std::chrono::milliseconds(100);

    sp_msgs::msg::Dm02SerialCopyMsg msg{};
    msg.timestamp = clock.now();
    msg.established = shared.established.load(std::memory_order_acquire);
    msg.have_state = shared.have_state.load(std::memory_order_acquire);
    msg.send_ok = shared.send_ok.load(std::memory_order_acquire);
    msg.send_fail = shared.send_fail.load(std::memory_order_acquire);

    {
      std::lock_guard<std::mutex> lock(shared.mutex);
      msg.yaw_rad = static_cast<float>(shared.gimbal.yaw_udeg / 1000000.0 * 3.14159265358979323846 / 180.0);
      msg.pitch_rad = static_cast<float>(shared.gimbal.pitch_udeg / 1000000.0 * 3.14159265358979323846 / 180.0);
      msg.roll_rad = static_cast<float>(shared.gimbal.roll_udeg / 1000000.0 * 3.14159265358979323846 / 180.0);
      msg.enc_yaw = shared.gimbal.enc_yaw;
      msg.enc_pitch = shared.gimbal.enc_pitch;
      msg.yaw_vel_rad_s = static_cast<float>(shared.gimbal.gyro_yaw_udeps / 1000000.0 * 3.14159265358979323846 / 180.0);
      msg.pitch_vel_rad_s = static_cast<float>(shared.gimbal.gyro_pitch_udeps / 1000000.0 * 3.14159265358979323846 / 180.0);
      msg.bullet_speed_mps = static_cast<float>(shared.gimbal.bullet_speed_x100 / 100.0);
      msg.bullet_count = static_cast<std::uint16_t>(shared.gimbal.bullet_count);
      msg.gimbal_mode = shared.gimbal.gimbal_mode;
      msg.shoot_state = shared.gimbal.shoot_state;
      msg.shooter_heat = shared.gimbal.shooter_heat;
      msg.shooter_heat_limit = shared.gimbal.shooter_heat_limit;
      msg.projectile_allowance_17mm = shared.gimbal.projectile_allowance_17mm;
      msg.projectile_allowance_42mm = shared.gimbal.projectile_allowance_42mm;
      msg.state_device_ts_us = shared.gimbal.device_ts_us;
      msg.state_host_ts_ns = shared.gimbal.host_ts_ns;

      msg.referee_valid = shared.have_referee.load(std::memory_order_acquire);
      msg.referee_enemy_team = shared.referee.enemy_team;
      msg.referee_fire_allowed = shared.referee.fire_allowed != 0;
      msg.referee_robot_id = shared.referee.robot_id;
      msg.referee_game_stage = shared.referee.game_stage;
      msg.referee_status = shared.referee.status;
      msg.referee_device_ts_us = shared.referee.device_ts_us;
      msg.referee_host_ts_ns = shared.referee.host_ts_ns;

      msg.tof_valid = shared.have_tfmini.load(std::memory_order_acquire);
      msg.tof_distance_cm = shared.tfmini.distance_cm;
      msg.tof_strength = shared.tfmini.strength;
      msg.tof_temp_cdeg = shared.tfmini.temp_cdeg;
      msg.tof_status = shared.tfmini.status;
      msg.tof_device_ts_us = shared.tfmini.device_ts_us;
      msg.tof_host_ts_ns = shared.tfmini.host_ts_ns;

      msg.timesync_valid = shared.timesync.valid;
      msg.timesync_offset_us = shared.timesync.offset_us;
      msg.timesync_rtt_us = shared.timesync.rtt_us;
      msg.timesync_version = shared.timesync.version;
      msg.timesync_last_device_time_us = shared.timesync.last_device_time_us;
      msg.timesync_last_host_time_us = shared.timesync.last_host_time_us;
    }

    ros2.publish_serial_copy(msg);
    shared.send_ok.fetch_add(1, std::memory_order_acq_rel);
  }

  return 0;
}
