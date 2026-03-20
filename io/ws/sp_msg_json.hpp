#pragma once

#include "io/ws/ws_bridge.hpp"

#include <cstdint>

#include <builtin_interfaces/msg/time.hpp>
#include <nlohmann/json.hpp>
#include <sp_msgs/msg/autoaim_target_msg.hpp>
#include <sp_msgs/msg/dm02_serial_copy_msg.hpp>
#include <sp_msgs/msg/enemy_status_msg.hpp>

namespace io::ws {

[[nodiscard]] inline std::uint64_t time_to_ns(const builtin_interfaces::msg::Time& stamp) {
  return static_cast<std::uint64_t>(stamp.sec) * 1000000000ULL +
         static_cast<std::uint64_t>(stamp.nanosec);
}

[[nodiscard]] inline nlohmann::json to_json(const sp_msgs::msg::EnemyStatusMsg& msg) {
  return nlohmann::json{
      {"invincible_enemy_ids", msg.invincible_enemy_ids},
      {"game_stage", msg.game_stage},
      {"enemy_count", msg.enemy_count},
      {"fire_allowed", msg.fire_allowed},
      {"enemy_team", msg.enemy_team},
      {"detected_enemy_ids", msg.detected_enemy_ids}};
}

[[nodiscard]] inline nlohmann::json to_json(const sp_msgs::msg::AutoaimTargetMsg& msg) {
  return nlohmann::json{
      {"target_ids", msg.target_ids},
      {"main_target_id", msg.main_target_id},
      {"has_target", msg.has_target},
      {"yaw_deg", msg.yaw_deg},
      {"pitch_deg", msg.pitch_deg},
      {"distance_m", msg.distance_m},
      {"vx", msg.vx},
      {"vy", msg.vy},
      {"vz", msg.vz},
      {"confidence", msg.confidence},
      {"shoot_command", msg.shoot_command}};
}

[[nodiscard]] inline nlohmann::json to_json(const sp_msgs::msg::Dm02SerialCopyMsg& msg) {
  return nlohmann::json{
      {"established", msg.established},
      {"have_state", msg.have_state},
      {"send_ok", msg.send_ok},
      {"send_fail", msg.send_fail},
      {"yaw_rad", msg.yaw_rad},
      {"pitch_rad", msg.pitch_rad},
      {"roll_rad", msg.roll_rad},
      {"enc_yaw", msg.enc_yaw},
      {"enc_pitch", msg.enc_pitch},
      {"yaw_vel_rad_s", msg.yaw_vel_rad_s},
      {"pitch_vel_rad_s", msg.pitch_vel_rad_s},
      {"bullet_speed_mps", msg.bullet_speed_mps},
      {"bullet_count", msg.bullet_count},
      {"gimbal_mode", msg.gimbal_mode},
      {"shoot_state", msg.shoot_state},
      {"shooter_heat", msg.shooter_heat},
      {"shooter_heat_limit", msg.shooter_heat_limit},
      {"projectile_allowance_17mm", msg.projectile_allowance_17mm},
      {"projectile_allowance_42mm", msg.projectile_allowance_42mm},
      {"state_device_ts_us", msg.state_device_ts_us},
      {"state_host_ts_ns", msg.state_host_ts_ns},
      {"referee_valid", msg.referee_valid},
      {"referee_enemy_team", msg.referee_enemy_team},
      {"referee_fire_allowed", msg.referee_fire_allowed},
      {"referee_robot_id", msg.referee_robot_id},
      {"referee_game_stage", msg.referee_game_stage},
      {"referee_status", msg.referee_status},
      {"referee_device_ts_us", msg.referee_device_ts_us},
      {"referee_host_ts_ns", msg.referee_host_ts_ns},
      {"tof_valid", msg.tof_valid},
      {"tof_distance_cm", msg.tof_distance_cm},
      {"tof_strength", msg.tof_strength},
      {"tof_temp_cdeg", msg.tof_temp_cdeg},
      {"tof_status", msg.tof_status},
      {"tof_device_ts_us", msg.tof_device_ts_us},
      {"tof_host_ts_ns", msg.tof_host_ts_ns},
      {"timesync_valid", msg.timesync_valid},
      {"timesync_offset_us", msg.timesync_offset_us},
      {"timesync_rtt_us", msg.timesync_rtt_us},
      {"timesync_version", msg.timesync_version},
      {"timesync_last_device_time_us", msg.timesync_last_device_time_us},
      {"timesync_last_host_time_us", msg.timesync_last_host_time_us}};
}

[[nodiscard]] inline bool publish_enemy_status(
    const sp_msgs::msg::EnemyStatusMsg& msg,
    std::string_view source = {}) {
  return publish_message("enemy_status", to_json(msg), time_to_ns(msg.timestamp), source);
}

[[nodiscard]] inline bool publish_autoaim_target(
    const sp_msgs::msg::AutoaimTargetMsg& msg,
    std::string_view source = {}) {
  return publish_message("autoaim_target", to_json(msg), time_to_ns(msg.timestamp), source);
}

[[nodiscard]] inline bool publish_dm02_serial_copy(
    const sp_msgs::msg::Dm02SerialCopyMsg& msg,
    std::string_view source = {}) {
  return publish_message("dm02_serial_copy", to_json(msg), time_to_ns(msg.timestamp), source);
}

}  // namespace io::ws
