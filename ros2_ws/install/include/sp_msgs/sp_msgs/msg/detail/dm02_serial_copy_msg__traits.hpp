// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/dm02_serial_copy_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__TRAITS_HPP_
#define SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "sp_msgs/msg/detail/dm02_serial_copy_msg__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'timestamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace sp_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Dm02SerialCopyMsg & msg,
  std::ostream & out)
{
  out << "{";
  // member: timestamp
  {
    out << "timestamp: ";
    to_flow_style_yaml(msg.timestamp, out);
    out << ", ";
  }

  // member: established
  {
    out << "established: ";
    rosidl_generator_traits::value_to_yaml(msg.established, out);
    out << ", ";
  }

  // member: have_state
  {
    out << "have_state: ";
    rosidl_generator_traits::value_to_yaml(msg.have_state, out);
    out << ", ";
  }

  // member: send_ok
  {
    out << "send_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.send_ok, out);
    out << ", ";
  }

  // member: send_fail
  {
    out << "send_fail: ";
    rosidl_generator_traits::value_to_yaml(msg.send_fail, out);
    out << ", ";
  }

  // member: yaw_rad
  {
    out << "yaw_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_rad, out);
    out << ", ";
  }

  // member: pitch_rad
  {
    out << "pitch_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.pitch_rad, out);
    out << ", ";
  }

  // member: roll_rad
  {
    out << "roll_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.roll_rad, out);
    out << ", ";
  }

  // member: enc_yaw
  {
    out << "enc_yaw: ";
    rosidl_generator_traits::value_to_yaml(msg.enc_yaw, out);
    out << ", ";
  }

  // member: enc_pitch
  {
    out << "enc_pitch: ";
    rosidl_generator_traits::value_to_yaml(msg.enc_pitch, out);
    out << ", ";
  }

  // member: yaw_vel_rad_s
  {
    out << "yaw_vel_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_vel_rad_s, out);
    out << ", ";
  }

  // member: pitch_vel_rad_s
  {
    out << "pitch_vel_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.pitch_vel_rad_s, out);
    out << ", ";
  }

  // member: bullet_speed_mps
  {
    out << "bullet_speed_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.bullet_speed_mps, out);
    out << ", ";
  }

  // member: bullet_count
  {
    out << "bullet_count: ";
    rosidl_generator_traits::value_to_yaml(msg.bullet_count, out);
    out << ", ";
  }

  // member: gimbal_mode
  {
    out << "gimbal_mode: ";
    rosidl_generator_traits::value_to_yaml(msg.gimbal_mode, out);
    out << ", ";
  }

  // member: shoot_state
  {
    out << "shoot_state: ";
    rosidl_generator_traits::value_to_yaml(msg.shoot_state, out);
    out << ", ";
  }

  // member: shooter_heat
  {
    out << "shooter_heat: ";
    rosidl_generator_traits::value_to_yaml(msg.shooter_heat, out);
    out << ", ";
  }

  // member: shooter_heat_limit
  {
    out << "shooter_heat_limit: ";
    rosidl_generator_traits::value_to_yaml(msg.shooter_heat_limit, out);
    out << ", ";
  }

  // member: projectile_allowance_17mm
  {
    out << "projectile_allowance_17mm: ";
    rosidl_generator_traits::value_to_yaml(msg.projectile_allowance_17mm, out);
    out << ", ";
  }

  // member: projectile_allowance_42mm
  {
    out << "projectile_allowance_42mm: ";
    rosidl_generator_traits::value_to_yaml(msg.projectile_allowance_42mm, out);
    out << ", ";
  }

  // member: state_device_ts_us
  {
    out << "state_device_ts_us: ";
    rosidl_generator_traits::value_to_yaml(msg.state_device_ts_us, out);
    out << ", ";
  }

  // member: state_host_ts_ns
  {
    out << "state_host_ts_ns: ";
    rosidl_generator_traits::value_to_yaml(msg.state_host_ts_ns, out);
    out << ", ";
  }

  // member: referee_valid
  {
    out << "referee_valid: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_valid, out);
    out << ", ";
  }

  // member: referee_enemy_team
  {
    out << "referee_enemy_team: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_enemy_team, out);
    out << ", ";
  }

  // member: referee_fire_allowed
  {
    out << "referee_fire_allowed: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_fire_allowed, out);
    out << ", ";
  }

  // member: referee_robot_id
  {
    out << "referee_robot_id: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_robot_id, out);
    out << ", ";
  }

  // member: referee_game_stage
  {
    out << "referee_game_stage: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_game_stage, out);
    out << ", ";
  }

  // member: referee_status
  {
    out << "referee_status: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_status, out);
    out << ", ";
  }

  // member: referee_device_ts_us
  {
    out << "referee_device_ts_us: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_device_ts_us, out);
    out << ", ";
  }

  // member: referee_host_ts_ns
  {
    out << "referee_host_ts_ns: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_host_ts_ns, out);
    out << ", ";
  }

  // member: tof_valid
  {
    out << "tof_valid: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_valid, out);
    out << ", ";
  }

  // member: tof_distance_cm
  {
    out << "tof_distance_cm: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_distance_cm, out);
    out << ", ";
  }

  // member: tof_strength
  {
    out << "tof_strength: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_strength, out);
    out << ", ";
  }

  // member: tof_temp_cdeg
  {
    out << "tof_temp_cdeg: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_temp_cdeg, out);
    out << ", ";
  }

  // member: tof_status
  {
    out << "tof_status: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_status, out);
    out << ", ";
  }

  // member: tof_device_ts_us
  {
    out << "tof_device_ts_us: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_device_ts_us, out);
    out << ", ";
  }

  // member: tof_host_ts_ns
  {
    out << "tof_host_ts_ns: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_host_ts_ns, out);
    out << ", ";
  }

  // member: timesync_valid
  {
    out << "timesync_valid: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_valid, out);
    out << ", ";
  }

  // member: timesync_offset_us
  {
    out << "timesync_offset_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_offset_us, out);
    out << ", ";
  }

  // member: timesync_rtt_us
  {
    out << "timesync_rtt_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_rtt_us, out);
    out << ", ";
  }

  // member: timesync_version
  {
    out << "timesync_version: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_version, out);
    out << ", ";
  }

  // member: timesync_last_device_time_us
  {
    out << "timesync_last_device_time_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_last_device_time_us, out);
    out << ", ";
  }

  // member: timesync_last_host_time_us
  {
    out << "timesync_last_host_time_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_last_host_time_us, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Dm02SerialCopyMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: timestamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timestamp:\n";
    to_block_style_yaml(msg.timestamp, out, indentation + 2);
  }

  // member: established
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "established: ";
    rosidl_generator_traits::value_to_yaml(msg.established, out);
    out << "\n";
  }

  // member: have_state
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "have_state: ";
    rosidl_generator_traits::value_to_yaml(msg.have_state, out);
    out << "\n";
  }

  // member: send_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "send_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.send_ok, out);
    out << "\n";
  }

  // member: send_fail
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "send_fail: ";
    rosidl_generator_traits::value_to_yaml(msg.send_fail, out);
    out << "\n";
  }

  // member: yaw_rad
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "yaw_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_rad, out);
    out << "\n";
  }

  // member: pitch_rad
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pitch_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.pitch_rad, out);
    out << "\n";
  }

  // member: roll_rad
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "roll_rad: ";
    rosidl_generator_traits::value_to_yaml(msg.roll_rad, out);
    out << "\n";
  }

  // member: enc_yaw
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "enc_yaw: ";
    rosidl_generator_traits::value_to_yaml(msg.enc_yaw, out);
    out << "\n";
  }

  // member: enc_pitch
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "enc_pitch: ";
    rosidl_generator_traits::value_to_yaml(msg.enc_pitch, out);
    out << "\n";
  }

  // member: yaw_vel_rad_s
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "yaw_vel_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_vel_rad_s, out);
    out << "\n";
  }

  // member: pitch_vel_rad_s
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pitch_vel_rad_s: ";
    rosidl_generator_traits::value_to_yaml(msg.pitch_vel_rad_s, out);
    out << "\n";
  }

  // member: bullet_speed_mps
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "bullet_speed_mps: ";
    rosidl_generator_traits::value_to_yaml(msg.bullet_speed_mps, out);
    out << "\n";
  }

  // member: bullet_count
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "bullet_count: ";
    rosidl_generator_traits::value_to_yaml(msg.bullet_count, out);
    out << "\n";
  }

  // member: gimbal_mode
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "gimbal_mode: ";
    rosidl_generator_traits::value_to_yaml(msg.gimbal_mode, out);
    out << "\n";
  }

  // member: shoot_state
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "shoot_state: ";
    rosidl_generator_traits::value_to_yaml(msg.shoot_state, out);
    out << "\n";
  }

  // member: shooter_heat
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "shooter_heat: ";
    rosidl_generator_traits::value_to_yaml(msg.shooter_heat, out);
    out << "\n";
  }

  // member: shooter_heat_limit
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "shooter_heat_limit: ";
    rosidl_generator_traits::value_to_yaml(msg.shooter_heat_limit, out);
    out << "\n";
  }

  // member: projectile_allowance_17mm
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "projectile_allowance_17mm: ";
    rosidl_generator_traits::value_to_yaml(msg.projectile_allowance_17mm, out);
    out << "\n";
  }

  // member: projectile_allowance_42mm
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "projectile_allowance_42mm: ";
    rosidl_generator_traits::value_to_yaml(msg.projectile_allowance_42mm, out);
    out << "\n";
  }

  // member: state_device_ts_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "state_device_ts_us: ";
    rosidl_generator_traits::value_to_yaml(msg.state_device_ts_us, out);
    out << "\n";
  }

  // member: state_host_ts_ns
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "state_host_ts_ns: ";
    rosidl_generator_traits::value_to_yaml(msg.state_host_ts_ns, out);
    out << "\n";
  }

  // member: referee_valid
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_valid: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_valid, out);
    out << "\n";
  }

  // member: referee_enemy_team
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_enemy_team: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_enemy_team, out);
    out << "\n";
  }

  // member: referee_fire_allowed
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_fire_allowed: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_fire_allowed, out);
    out << "\n";
  }

  // member: referee_robot_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_robot_id: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_robot_id, out);
    out << "\n";
  }

  // member: referee_game_stage
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_game_stage: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_game_stage, out);
    out << "\n";
  }

  // member: referee_status
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_status: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_status, out);
    out << "\n";
  }

  // member: referee_device_ts_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_device_ts_us: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_device_ts_us, out);
    out << "\n";
  }

  // member: referee_host_ts_ns
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "referee_host_ts_ns: ";
    rosidl_generator_traits::value_to_yaml(msg.referee_host_ts_ns, out);
    out << "\n";
  }

  // member: tof_valid
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_valid: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_valid, out);
    out << "\n";
  }

  // member: tof_distance_cm
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_distance_cm: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_distance_cm, out);
    out << "\n";
  }

  // member: tof_strength
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_strength: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_strength, out);
    out << "\n";
  }

  // member: tof_temp_cdeg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_temp_cdeg: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_temp_cdeg, out);
    out << "\n";
  }

  // member: tof_status
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_status: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_status, out);
    out << "\n";
  }

  // member: tof_device_ts_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_device_ts_us: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_device_ts_us, out);
    out << "\n";
  }

  // member: tof_host_ts_ns
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tof_host_ts_ns: ";
    rosidl_generator_traits::value_to_yaml(msg.tof_host_ts_ns, out);
    out << "\n";
  }

  // member: timesync_valid
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timesync_valid: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_valid, out);
    out << "\n";
  }

  // member: timesync_offset_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timesync_offset_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_offset_us, out);
    out << "\n";
  }

  // member: timesync_rtt_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timesync_rtt_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_rtt_us, out);
    out << "\n";
  }

  // member: timesync_version
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timesync_version: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_version, out);
    out << "\n";
  }

  // member: timesync_last_device_time_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timesync_last_device_time_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_last_device_time_us, out);
    out << "\n";
  }

  // member: timesync_last_host_time_us
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timesync_last_host_time_us: ";
    rosidl_generator_traits::value_to_yaml(msg.timesync_last_host_time_us, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Dm02SerialCopyMsg & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace sp_msgs

namespace rosidl_generator_traits
{

[[deprecated("use sp_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const sp_msgs::msg::Dm02SerialCopyMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  sp_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use sp_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const sp_msgs::msg::Dm02SerialCopyMsg & msg)
{
  return sp_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<sp_msgs::msg::Dm02SerialCopyMsg>()
{
  return "sp_msgs::msg::Dm02SerialCopyMsg";
}

template<>
inline const char * name<sp_msgs::msg::Dm02SerialCopyMsg>()
{
  return "sp_msgs/msg/Dm02SerialCopyMsg";
}

template<>
struct has_fixed_size<sp_msgs::msg::Dm02SerialCopyMsg>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct has_bounded_size<sp_msgs::msg::Dm02SerialCopyMsg>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct is_message<sp_msgs::msg::Dm02SerialCopyMsg>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__TRAITS_HPP_
