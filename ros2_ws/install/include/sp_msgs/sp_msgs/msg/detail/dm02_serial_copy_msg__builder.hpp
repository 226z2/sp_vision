// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/dm02_serial_copy_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__BUILDER_HPP_
#define SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "sp_msgs/msg/detail/dm02_serial_copy_msg__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace sp_msgs
{

namespace msg
{

namespace builder
{

class Init_Dm02SerialCopyMsg_timesync_last_host_time_us
{
public:
  explicit Init_Dm02SerialCopyMsg_timesync_last_host_time_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  ::sp_msgs::msg::Dm02SerialCopyMsg timesync_last_host_time_us(::sp_msgs::msg::Dm02SerialCopyMsg::_timesync_last_host_time_us_type arg)
  {
    msg_.timesync_last_host_time_us = std::move(arg);
    return std::move(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_timesync_last_device_time_us
{
public:
  explicit Init_Dm02SerialCopyMsg_timesync_last_device_time_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_timesync_last_host_time_us timesync_last_device_time_us(::sp_msgs::msg::Dm02SerialCopyMsg::_timesync_last_device_time_us_type arg)
  {
    msg_.timesync_last_device_time_us = std::move(arg);
    return Init_Dm02SerialCopyMsg_timesync_last_host_time_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_timesync_version
{
public:
  explicit Init_Dm02SerialCopyMsg_timesync_version(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_timesync_last_device_time_us timesync_version(::sp_msgs::msg::Dm02SerialCopyMsg::_timesync_version_type arg)
  {
    msg_.timesync_version = std::move(arg);
    return Init_Dm02SerialCopyMsg_timesync_last_device_time_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_timesync_rtt_us
{
public:
  explicit Init_Dm02SerialCopyMsg_timesync_rtt_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_timesync_version timesync_rtt_us(::sp_msgs::msg::Dm02SerialCopyMsg::_timesync_rtt_us_type arg)
  {
    msg_.timesync_rtt_us = std::move(arg);
    return Init_Dm02SerialCopyMsg_timesync_version(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_timesync_offset_us
{
public:
  explicit Init_Dm02SerialCopyMsg_timesync_offset_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_timesync_rtt_us timesync_offset_us(::sp_msgs::msg::Dm02SerialCopyMsg::_timesync_offset_us_type arg)
  {
    msg_.timesync_offset_us = std::move(arg);
    return Init_Dm02SerialCopyMsg_timesync_rtt_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_timesync_valid
{
public:
  explicit Init_Dm02SerialCopyMsg_timesync_valid(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_timesync_offset_us timesync_valid(::sp_msgs::msg::Dm02SerialCopyMsg::_timesync_valid_type arg)
  {
    msg_.timesync_valid = std::move(arg);
    return Init_Dm02SerialCopyMsg_timesync_offset_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_host_ts_ns
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_host_ts_ns(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_timesync_valid tof_host_ts_ns(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_host_ts_ns_type arg)
  {
    msg_.tof_host_ts_ns = std::move(arg);
    return Init_Dm02SerialCopyMsg_timesync_valid(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_device_ts_us
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_device_ts_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_host_ts_ns tof_device_ts_us(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_device_ts_us_type arg)
  {
    msg_.tof_device_ts_us = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_host_ts_ns(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_status
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_status(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_device_ts_us tof_status(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_status_type arg)
  {
    msg_.tof_status = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_device_ts_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_temp_cdeg
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_temp_cdeg(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_status tof_temp_cdeg(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_temp_cdeg_type arg)
  {
    msg_.tof_temp_cdeg = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_status(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_strength
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_strength(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_temp_cdeg tof_strength(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_strength_type arg)
  {
    msg_.tof_strength = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_temp_cdeg(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_distance_cm
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_distance_cm(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_strength tof_distance_cm(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_distance_cm_type arg)
  {
    msg_.tof_distance_cm = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_strength(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_tof_valid
{
public:
  explicit Init_Dm02SerialCopyMsg_tof_valid(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_distance_cm tof_valid(::sp_msgs::msg::Dm02SerialCopyMsg::_tof_valid_type arg)
  {
    msg_.tof_valid = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_distance_cm(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_host_ts_ns
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_host_ts_ns(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_tof_valid referee_host_ts_ns(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_host_ts_ns_type arg)
  {
    msg_.referee_host_ts_ns = std::move(arg);
    return Init_Dm02SerialCopyMsg_tof_valid(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_device_ts_us
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_device_ts_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_host_ts_ns referee_device_ts_us(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_device_ts_us_type arg)
  {
    msg_.referee_device_ts_us = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_host_ts_ns(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_status
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_status(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_device_ts_us referee_status(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_status_type arg)
  {
    msg_.referee_status = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_device_ts_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_game_stage
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_game_stage(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_status referee_game_stage(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_game_stage_type arg)
  {
    msg_.referee_game_stage = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_status(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_robot_id
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_robot_id(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_game_stage referee_robot_id(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_robot_id_type arg)
  {
    msg_.referee_robot_id = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_game_stage(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_fire_allowed
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_fire_allowed(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_robot_id referee_fire_allowed(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_fire_allowed_type arg)
  {
    msg_.referee_fire_allowed = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_robot_id(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_enemy_team
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_enemy_team(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_fire_allowed referee_enemy_team(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_enemy_team_type arg)
  {
    msg_.referee_enemy_team = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_fire_allowed(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_referee_valid
{
public:
  explicit Init_Dm02SerialCopyMsg_referee_valid(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_enemy_team referee_valid(::sp_msgs::msg::Dm02SerialCopyMsg::_referee_valid_type arg)
  {
    msg_.referee_valid = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_enemy_team(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_state_host_ts_ns
{
public:
  explicit Init_Dm02SerialCopyMsg_state_host_ts_ns(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_referee_valid state_host_ts_ns(::sp_msgs::msg::Dm02SerialCopyMsg::_state_host_ts_ns_type arg)
  {
    msg_.state_host_ts_ns = std::move(arg);
    return Init_Dm02SerialCopyMsg_referee_valid(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_state_device_ts_us
{
public:
  explicit Init_Dm02SerialCopyMsg_state_device_ts_us(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_state_host_ts_ns state_device_ts_us(::sp_msgs::msg::Dm02SerialCopyMsg::_state_device_ts_us_type arg)
  {
    msg_.state_device_ts_us = std::move(arg);
    return Init_Dm02SerialCopyMsg_state_host_ts_ns(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_projectile_allowance_42mm
{
public:
  explicit Init_Dm02SerialCopyMsg_projectile_allowance_42mm(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_state_device_ts_us projectile_allowance_42mm(::sp_msgs::msg::Dm02SerialCopyMsg::_projectile_allowance_42mm_type arg)
  {
    msg_.projectile_allowance_42mm = std::move(arg);
    return Init_Dm02SerialCopyMsg_state_device_ts_us(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_projectile_allowance_17mm
{
public:
  explicit Init_Dm02SerialCopyMsg_projectile_allowance_17mm(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_projectile_allowance_42mm projectile_allowance_17mm(::sp_msgs::msg::Dm02SerialCopyMsg::_projectile_allowance_17mm_type arg)
  {
    msg_.projectile_allowance_17mm = std::move(arg);
    return Init_Dm02SerialCopyMsg_projectile_allowance_42mm(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_shooter_heat_limit
{
public:
  explicit Init_Dm02SerialCopyMsg_shooter_heat_limit(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_projectile_allowance_17mm shooter_heat_limit(::sp_msgs::msg::Dm02SerialCopyMsg::_shooter_heat_limit_type arg)
  {
    msg_.shooter_heat_limit = std::move(arg);
    return Init_Dm02SerialCopyMsg_projectile_allowance_17mm(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_shooter_heat
{
public:
  explicit Init_Dm02SerialCopyMsg_shooter_heat(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_shooter_heat_limit shooter_heat(::sp_msgs::msg::Dm02SerialCopyMsg::_shooter_heat_type arg)
  {
    msg_.shooter_heat = std::move(arg);
    return Init_Dm02SerialCopyMsg_shooter_heat_limit(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_shoot_state
{
public:
  explicit Init_Dm02SerialCopyMsg_shoot_state(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_shooter_heat shoot_state(::sp_msgs::msg::Dm02SerialCopyMsg::_shoot_state_type arg)
  {
    msg_.shoot_state = std::move(arg);
    return Init_Dm02SerialCopyMsg_shooter_heat(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_gimbal_mode
{
public:
  explicit Init_Dm02SerialCopyMsg_gimbal_mode(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_shoot_state gimbal_mode(::sp_msgs::msg::Dm02SerialCopyMsg::_gimbal_mode_type arg)
  {
    msg_.gimbal_mode = std::move(arg);
    return Init_Dm02SerialCopyMsg_shoot_state(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_bullet_count
{
public:
  explicit Init_Dm02SerialCopyMsg_bullet_count(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_gimbal_mode bullet_count(::sp_msgs::msg::Dm02SerialCopyMsg::_bullet_count_type arg)
  {
    msg_.bullet_count = std::move(arg);
    return Init_Dm02SerialCopyMsg_gimbal_mode(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_bullet_speed_mps
{
public:
  explicit Init_Dm02SerialCopyMsg_bullet_speed_mps(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_bullet_count bullet_speed_mps(::sp_msgs::msg::Dm02SerialCopyMsg::_bullet_speed_mps_type arg)
  {
    msg_.bullet_speed_mps = std::move(arg);
    return Init_Dm02SerialCopyMsg_bullet_count(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_pitch_vel_rad_s
{
public:
  explicit Init_Dm02SerialCopyMsg_pitch_vel_rad_s(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_bullet_speed_mps pitch_vel_rad_s(::sp_msgs::msg::Dm02SerialCopyMsg::_pitch_vel_rad_s_type arg)
  {
    msg_.pitch_vel_rad_s = std::move(arg);
    return Init_Dm02SerialCopyMsg_bullet_speed_mps(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_yaw_vel_rad_s
{
public:
  explicit Init_Dm02SerialCopyMsg_yaw_vel_rad_s(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_pitch_vel_rad_s yaw_vel_rad_s(::sp_msgs::msg::Dm02SerialCopyMsg::_yaw_vel_rad_s_type arg)
  {
    msg_.yaw_vel_rad_s = std::move(arg);
    return Init_Dm02SerialCopyMsg_pitch_vel_rad_s(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_enc_pitch
{
public:
  explicit Init_Dm02SerialCopyMsg_enc_pitch(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_yaw_vel_rad_s enc_pitch(::sp_msgs::msg::Dm02SerialCopyMsg::_enc_pitch_type arg)
  {
    msg_.enc_pitch = std::move(arg);
    return Init_Dm02SerialCopyMsg_yaw_vel_rad_s(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_enc_yaw
{
public:
  explicit Init_Dm02SerialCopyMsg_enc_yaw(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_enc_pitch enc_yaw(::sp_msgs::msg::Dm02SerialCopyMsg::_enc_yaw_type arg)
  {
    msg_.enc_yaw = std::move(arg);
    return Init_Dm02SerialCopyMsg_enc_pitch(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_roll_rad
{
public:
  explicit Init_Dm02SerialCopyMsg_roll_rad(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_enc_yaw roll_rad(::sp_msgs::msg::Dm02SerialCopyMsg::_roll_rad_type arg)
  {
    msg_.roll_rad = std::move(arg);
    return Init_Dm02SerialCopyMsg_enc_yaw(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_pitch_rad
{
public:
  explicit Init_Dm02SerialCopyMsg_pitch_rad(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_roll_rad pitch_rad(::sp_msgs::msg::Dm02SerialCopyMsg::_pitch_rad_type arg)
  {
    msg_.pitch_rad = std::move(arg);
    return Init_Dm02SerialCopyMsg_roll_rad(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_yaw_rad
{
public:
  explicit Init_Dm02SerialCopyMsg_yaw_rad(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_pitch_rad yaw_rad(::sp_msgs::msg::Dm02SerialCopyMsg::_yaw_rad_type arg)
  {
    msg_.yaw_rad = std::move(arg);
    return Init_Dm02SerialCopyMsg_pitch_rad(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_send_fail
{
public:
  explicit Init_Dm02SerialCopyMsg_send_fail(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_yaw_rad send_fail(::sp_msgs::msg::Dm02SerialCopyMsg::_send_fail_type arg)
  {
    msg_.send_fail = std::move(arg);
    return Init_Dm02SerialCopyMsg_yaw_rad(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_send_ok
{
public:
  explicit Init_Dm02SerialCopyMsg_send_ok(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_send_fail send_ok(::sp_msgs::msg::Dm02SerialCopyMsg::_send_ok_type arg)
  {
    msg_.send_ok = std::move(arg);
    return Init_Dm02SerialCopyMsg_send_fail(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_have_state
{
public:
  explicit Init_Dm02SerialCopyMsg_have_state(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_send_ok have_state(::sp_msgs::msg::Dm02SerialCopyMsg::_have_state_type arg)
  {
    msg_.have_state = std::move(arg);
    return Init_Dm02SerialCopyMsg_send_ok(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_established
{
public:
  explicit Init_Dm02SerialCopyMsg_established(::sp_msgs::msg::Dm02SerialCopyMsg & msg)
  : msg_(msg)
  {}
  Init_Dm02SerialCopyMsg_have_state established(::sp_msgs::msg::Dm02SerialCopyMsg::_established_type arg)
  {
    msg_.established = std::move(arg);
    return Init_Dm02SerialCopyMsg_have_state(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

class Init_Dm02SerialCopyMsg_timestamp
{
public:
  Init_Dm02SerialCopyMsg_timestamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Dm02SerialCopyMsg_established timestamp(::sp_msgs::msg::Dm02SerialCopyMsg::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_Dm02SerialCopyMsg_established(msg_);
  }

private:
  ::sp_msgs::msg::Dm02SerialCopyMsg msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::sp_msgs::msg::Dm02SerialCopyMsg>()
{
  return sp_msgs::msg::builder::Init_Dm02SerialCopyMsg_timestamp();
}

}  // namespace sp_msgs

#endif  // SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__BUILDER_HPP_
