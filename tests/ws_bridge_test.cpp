#include <rclcpp/rclcpp.hpp>

#include <thread>

#include "io/ros2/ros2.hpp"
#include "tools/exiter.hpp"
#include "tools/logger.hpp"

int main(int argc, char ** argv)
{
  tools::Exiter exiter;
  io::ROS2 ros2;
  rclcpp::Clock clock;

  auto enemy_publisher =
    ros2.create_publisher<sp_msgs::msg::EnemyStatusMsg>("ws_enemy_pub", "enemy_status", 10);
  auto target_publisher =
    ros2.create_publisher<sp_msgs::msg::AutoaimTargetMsg>("ws_target_pub", "autoaim_target", 10);

  int i = 0;
  while (!exiter.exit() && i < 30) {
    sp_msgs::msg::EnemyStatusMsg enemy{};
    enemy.timestamp = clock.now();
    enemy.invincible_enemy_ids = {1, 3};
    enemy.game_stage = 2;
    enemy.enemy_count = 4;
    enemy.fire_allowed = (i % 2 == 0);
    enemy.enemy_team = 1;
    enemy.detected_enemy_ids = {1, 3, 5};
    enemy_publisher->publish(enemy);

    sp_msgs::msg::AutoaimTargetMsg target{};
    target.timestamp = clock.now();
    target.target_ids = {5, 3, 1};
    target.main_target_id = 5;
    target.has_target = true;
    target.yaw_deg = 12.5F + static_cast<float>(i);
    target.pitch_deg = -3.0F;
    target.distance_m = 6.8F;
    target.vx = 0.1F;
    target.vy = 0.0F;
    target.vz = -0.2F;
    target.confidence = 0.9F;
    target.shoot_command = 1;
    target_publisher->publish(target);

    sp_msgs::msg::Dm02SerialCopyMsg serial_copy{};
    serial_copy.timestamp = clock.now();
    serial_copy.established = true;
    serial_copy.have_state = true;
    serial_copy.send_ok = static_cast<std::uint64_t>(i + 1);
    serial_copy.send_fail = 0;
    serial_copy.yaw_rad = 0.12F;
    serial_copy.pitch_rad = -0.03F;
    serial_copy.roll_rad = 0.0F;
    serial_copy.enc_yaw = 1234 + i;
    serial_copy.enc_pitch = 567 + i;
    serial_copy.yaw_vel_rad_s = 0.01F;
    serial_copy.pitch_vel_rad_s = -0.01F;
    serial_copy.bullet_speed_mps = 27.0F;
    serial_copy.bullet_count = static_cast<std::uint16_t>(100 + i);
    serial_copy.gimbal_mode = 2;
    serial_copy.shoot_state = 1;
    serial_copy.shooter_heat = 10;
    serial_copy.shooter_heat_limit = 100;
    serial_copy.projectile_allowance_17mm = 50;
    serial_copy.projectile_allowance_42mm = 0;
    serial_copy.state_device_ts_us = 1000 + i;
    serial_copy.state_host_ts_ns = 2000 + i;
    serial_copy.referee_valid = true;
    serial_copy.referee_enemy_team = 1;
    serial_copy.referee_fire_allowed = true;
    serial_copy.referee_robot_id = 7;
    serial_copy.referee_game_stage = 2;
    serial_copy.referee_status = 3;
    serial_copy.referee_device_ts_us = 3000 + i;
    serial_copy.referee_host_ts_ns = 4000 + i;
    serial_copy.tof_valid = true;
    serial_copy.tof_distance_cm = 250;
    serial_copy.tof_strength = 99;
    serial_copy.tof_temp_cdeg = 2350;
    serial_copy.tof_status = 1;
    serial_copy.tof_device_ts_us = 5000 + i;
    serial_copy.tof_host_ts_ns = 6000 + i;
    serial_copy.timesync_valid = true;
    serial_copy.timesync_offset_us = -15;
    serial_copy.timesync_rtt_us = 321;
    serial_copy.timesync_version = 1;
    serial_copy.timesync_last_device_time_us = 7000 + i;
    serial_copy.timesync_last_host_time_us = 8000 + i;
    ros2.publish_serial_copy(serial_copy);

    tools::logger()->info("ws_bridge_test published batch {}", i);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ++i;
  }

  return 0;
}
