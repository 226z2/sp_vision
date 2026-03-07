// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/dm02_serial_copy_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__STRUCT_HPP_
#define SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'timestamp'
#include "builtin_interfaces/msg/detail/time__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__sp_msgs__msg__Dm02SerialCopyMsg __attribute__((deprecated))
#else
# define DEPRECATED__sp_msgs__msg__Dm02SerialCopyMsg __declspec(deprecated)
#endif

namespace sp_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Dm02SerialCopyMsg_
{
  using Type = Dm02SerialCopyMsg_<ContainerAllocator>;

  explicit Dm02SerialCopyMsg_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : timestamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->established = false;
      this->have_state = false;
      this->send_ok = 0ull;
      this->send_fail = 0ull;
      this->yaw_rad = 0.0f;
      this->pitch_rad = 0.0f;
      this->roll_rad = 0.0f;
      this->enc_yaw = 0l;
      this->enc_pitch = 0l;
      this->yaw_vel_rad_s = 0.0f;
      this->pitch_vel_rad_s = 0.0f;
      this->bullet_speed_mps = 0.0f;
      this->bullet_count = 0;
      this->gimbal_mode = 0l;
      this->shoot_state = 0l;
      this->shooter_heat = 0l;
      this->shooter_heat_limit = 0l;
      this->projectile_allowance_17mm = 0l;
      this->projectile_allowance_42mm = 0l;
      this->state_device_ts_us = 0ull;
      this->state_host_ts_ns = 0ull;
      this->referee_valid = false;
      this->referee_enemy_team = 0l;
      this->referee_fire_allowed = false;
      this->referee_robot_id = 0l;
      this->referee_game_stage = 0l;
      this->referee_status = 0;
      this->referee_device_ts_us = 0ull;
      this->referee_host_ts_ns = 0ull;
      this->tof_valid = false;
      this->tof_distance_cm = 0;
      this->tof_strength = 0;
      this->tof_temp_cdeg = 0;
      this->tof_status = 0;
      this->tof_device_ts_us = 0ull;
      this->tof_host_ts_ns = 0ull;
      this->timesync_valid = false;
      this->timesync_offset_us = 0ll;
      this->timesync_rtt_us = 0ul;
      this->timesync_version = 0ul;
      this->timesync_last_device_time_us = 0ull;
      this->timesync_last_host_time_us = 0ull;
    }
  }

  explicit Dm02SerialCopyMsg_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : timestamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->established = false;
      this->have_state = false;
      this->send_ok = 0ull;
      this->send_fail = 0ull;
      this->yaw_rad = 0.0f;
      this->pitch_rad = 0.0f;
      this->roll_rad = 0.0f;
      this->enc_yaw = 0l;
      this->enc_pitch = 0l;
      this->yaw_vel_rad_s = 0.0f;
      this->pitch_vel_rad_s = 0.0f;
      this->bullet_speed_mps = 0.0f;
      this->bullet_count = 0;
      this->gimbal_mode = 0l;
      this->shoot_state = 0l;
      this->shooter_heat = 0l;
      this->shooter_heat_limit = 0l;
      this->projectile_allowance_17mm = 0l;
      this->projectile_allowance_42mm = 0l;
      this->state_device_ts_us = 0ull;
      this->state_host_ts_ns = 0ull;
      this->referee_valid = false;
      this->referee_enemy_team = 0l;
      this->referee_fire_allowed = false;
      this->referee_robot_id = 0l;
      this->referee_game_stage = 0l;
      this->referee_status = 0;
      this->referee_device_ts_us = 0ull;
      this->referee_host_ts_ns = 0ull;
      this->tof_valid = false;
      this->tof_distance_cm = 0;
      this->tof_strength = 0;
      this->tof_temp_cdeg = 0;
      this->tof_status = 0;
      this->tof_device_ts_us = 0ull;
      this->tof_host_ts_ns = 0ull;
      this->timesync_valid = false;
      this->timesync_offset_us = 0ll;
      this->timesync_rtt_us = 0ul;
      this->timesync_version = 0ul;
      this->timesync_last_device_time_us = 0ull;
      this->timesync_last_host_time_us = 0ull;
    }
  }

  // field types and members
  using _timestamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _timestamp_type timestamp;
  using _established_type =
    bool;
  _established_type established;
  using _have_state_type =
    bool;
  _have_state_type have_state;
  using _send_ok_type =
    uint64_t;
  _send_ok_type send_ok;
  using _send_fail_type =
    uint64_t;
  _send_fail_type send_fail;
  using _yaw_rad_type =
    float;
  _yaw_rad_type yaw_rad;
  using _pitch_rad_type =
    float;
  _pitch_rad_type pitch_rad;
  using _roll_rad_type =
    float;
  _roll_rad_type roll_rad;
  using _enc_yaw_type =
    int32_t;
  _enc_yaw_type enc_yaw;
  using _enc_pitch_type =
    int32_t;
  _enc_pitch_type enc_pitch;
  using _yaw_vel_rad_s_type =
    float;
  _yaw_vel_rad_s_type yaw_vel_rad_s;
  using _pitch_vel_rad_s_type =
    float;
  _pitch_vel_rad_s_type pitch_vel_rad_s;
  using _bullet_speed_mps_type =
    float;
  _bullet_speed_mps_type bullet_speed_mps;
  using _bullet_count_type =
    uint16_t;
  _bullet_count_type bullet_count;
  using _gimbal_mode_type =
    int32_t;
  _gimbal_mode_type gimbal_mode;
  using _shoot_state_type =
    int32_t;
  _shoot_state_type shoot_state;
  using _shooter_heat_type =
    int32_t;
  _shooter_heat_type shooter_heat;
  using _shooter_heat_limit_type =
    int32_t;
  _shooter_heat_limit_type shooter_heat_limit;
  using _projectile_allowance_17mm_type =
    int32_t;
  _projectile_allowance_17mm_type projectile_allowance_17mm;
  using _projectile_allowance_42mm_type =
    int32_t;
  _projectile_allowance_42mm_type projectile_allowance_42mm;
  using _state_device_ts_us_type =
    uint64_t;
  _state_device_ts_us_type state_device_ts_us;
  using _state_host_ts_ns_type =
    uint64_t;
  _state_host_ts_ns_type state_host_ts_ns;
  using _referee_valid_type =
    bool;
  _referee_valid_type referee_valid;
  using _referee_enemy_team_type =
    int32_t;
  _referee_enemy_team_type referee_enemy_team;
  using _referee_fire_allowed_type =
    bool;
  _referee_fire_allowed_type referee_fire_allowed;
  using _referee_robot_id_type =
    int32_t;
  _referee_robot_id_type referee_robot_id;
  using _referee_game_stage_type =
    int32_t;
  _referee_game_stage_type referee_game_stage;
  using _referee_status_type =
    uint16_t;
  _referee_status_type referee_status;
  using _referee_device_ts_us_type =
    uint64_t;
  _referee_device_ts_us_type referee_device_ts_us;
  using _referee_host_ts_ns_type =
    uint64_t;
  _referee_host_ts_ns_type referee_host_ts_ns;
  using _tof_valid_type =
    bool;
  _tof_valid_type tof_valid;
  using _tof_distance_cm_type =
    uint16_t;
  _tof_distance_cm_type tof_distance_cm;
  using _tof_strength_type =
    uint16_t;
  _tof_strength_type tof_strength;
  using _tof_temp_cdeg_type =
    int16_t;
  _tof_temp_cdeg_type tof_temp_cdeg;
  using _tof_status_type =
    uint16_t;
  _tof_status_type tof_status;
  using _tof_device_ts_us_type =
    uint64_t;
  _tof_device_ts_us_type tof_device_ts_us;
  using _tof_host_ts_ns_type =
    uint64_t;
  _tof_host_ts_ns_type tof_host_ts_ns;
  using _timesync_valid_type =
    bool;
  _timesync_valid_type timesync_valid;
  using _timesync_offset_us_type =
    int64_t;
  _timesync_offset_us_type timesync_offset_us;
  using _timesync_rtt_us_type =
    uint32_t;
  _timesync_rtt_us_type timesync_rtt_us;
  using _timesync_version_type =
    uint32_t;
  _timesync_version_type timesync_version;
  using _timesync_last_device_time_us_type =
    uint64_t;
  _timesync_last_device_time_us_type timesync_last_device_time_us;
  using _timesync_last_host_time_us_type =
    uint64_t;
  _timesync_last_host_time_us_type timesync_last_host_time_us;

  // setters for named parameter idiom
  Type & set__timestamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->timestamp = _arg;
    return *this;
  }
  Type & set__established(
    const bool & _arg)
  {
    this->established = _arg;
    return *this;
  }
  Type & set__have_state(
    const bool & _arg)
  {
    this->have_state = _arg;
    return *this;
  }
  Type & set__send_ok(
    const uint64_t & _arg)
  {
    this->send_ok = _arg;
    return *this;
  }
  Type & set__send_fail(
    const uint64_t & _arg)
  {
    this->send_fail = _arg;
    return *this;
  }
  Type & set__yaw_rad(
    const float & _arg)
  {
    this->yaw_rad = _arg;
    return *this;
  }
  Type & set__pitch_rad(
    const float & _arg)
  {
    this->pitch_rad = _arg;
    return *this;
  }
  Type & set__roll_rad(
    const float & _arg)
  {
    this->roll_rad = _arg;
    return *this;
  }
  Type & set__enc_yaw(
    const int32_t & _arg)
  {
    this->enc_yaw = _arg;
    return *this;
  }
  Type & set__enc_pitch(
    const int32_t & _arg)
  {
    this->enc_pitch = _arg;
    return *this;
  }
  Type & set__yaw_vel_rad_s(
    const float & _arg)
  {
    this->yaw_vel_rad_s = _arg;
    return *this;
  }
  Type & set__pitch_vel_rad_s(
    const float & _arg)
  {
    this->pitch_vel_rad_s = _arg;
    return *this;
  }
  Type & set__bullet_speed_mps(
    const float & _arg)
  {
    this->bullet_speed_mps = _arg;
    return *this;
  }
  Type & set__bullet_count(
    const uint16_t & _arg)
  {
    this->bullet_count = _arg;
    return *this;
  }
  Type & set__gimbal_mode(
    const int32_t & _arg)
  {
    this->gimbal_mode = _arg;
    return *this;
  }
  Type & set__shoot_state(
    const int32_t & _arg)
  {
    this->shoot_state = _arg;
    return *this;
  }
  Type & set__shooter_heat(
    const int32_t & _arg)
  {
    this->shooter_heat = _arg;
    return *this;
  }
  Type & set__shooter_heat_limit(
    const int32_t & _arg)
  {
    this->shooter_heat_limit = _arg;
    return *this;
  }
  Type & set__projectile_allowance_17mm(
    const int32_t & _arg)
  {
    this->projectile_allowance_17mm = _arg;
    return *this;
  }
  Type & set__projectile_allowance_42mm(
    const int32_t & _arg)
  {
    this->projectile_allowance_42mm = _arg;
    return *this;
  }
  Type & set__state_device_ts_us(
    const uint64_t & _arg)
  {
    this->state_device_ts_us = _arg;
    return *this;
  }
  Type & set__state_host_ts_ns(
    const uint64_t & _arg)
  {
    this->state_host_ts_ns = _arg;
    return *this;
  }
  Type & set__referee_valid(
    const bool & _arg)
  {
    this->referee_valid = _arg;
    return *this;
  }
  Type & set__referee_enemy_team(
    const int32_t & _arg)
  {
    this->referee_enemy_team = _arg;
    return *this;
  }
  Type & set__referee_fire_allowed(
    const bool & _arg)
  {
    this->referee_fire_allowed = _arg;
    return *this;
  }
  Type & set__referee_robot_id(
    const int32_t & _arg)
  {
    this->referee_robot_id = _arg;
    return *this;
  }
  Type & set__referee_game_stage(
    const int32_t & _arg)
  {
    this->referee_game_stage = _arg;
    return *this;
  }
  Type & set__referee_status(
    const uint16_t & _arg)
  {
    this->referee_status = _arg;
    return *this;
  }
  Type & set__referee_device_ts_us(
    const uint64_t & _arg)
  {
    this->referee_device_ts_us = _arg;
    return *this;
  }
  Type & set__referee_host_ts_ns(
    const uint64_t & _arg)
  {
    this->referee_host_ts_ns = _arg;
    return *this;
  }
  Type & set__tof_valid(
    const bool & _arg)
  {
    this->tof_valid = _arg;
    return *this;
  }
  Type & set__tof_distance_cm(
    const uint16_t & _arg)
  {
    this->tof_distance_cm = _arg;
    return *this;
  }
  Type & set__tof_strength(
    const uint16_t & _arg)
  {
    this->tof_strength = _arg;
    return *this;
  }
  Type & set__tof_temp_cdeg(
    const int16_t & _arg)
  {
    this->tof_temp_cdeg = _arg;
    return *this;
  }
  Type & set__tof_status(
    const uint16_t & _arg)
  {
    this->tof_status = _arg;
    return *this;
  }
  Type & set__tof_device_ts_us(
    const uint64_t & _arg)
  {
    this->tof_device_ts_us = _arg;
    return *this;
  }
  Type & set__tof_host_ts_ns(
    const uint64_t & _arg)
  {
    this->tof_host_ts_ns = _arg;
    return *this;
  }
  Type & set__timesync_valid(
    const bool & _arg)
  {
    this->timesync_valid = _arg;
    return *this;
  }
  Type & set__timesync_offset_us(
    const int64_t & _arg)
  {
    this->timesync_offset_us = _arg;
    return *this;
  }
  Type & set__timesync_rtt_us(
    const uint32_t & _arg)
  {
    this->timesync_rtt_us = _arg;
    return *this;
  }
  Type & set__timesync_version(
    const uint32_t & _arg)
  {
    this->timesync_version = _arg;
    return *this;
  }
  Type & set__timesync_last_device_time_us(
    const uint64_t & _arg)
  {
    this->timesync_last_device_time_us = _arg;
    return *this;
  }
  Type & set__timesync_last_host_time_us(
    const uint64_t & _arg)
  {
    this->timesync_last_host_time_us = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator> *;
  using ConstRawPtr =
    const sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__sp_msgs__msg__Dm02SerialCopyMsg
    std::shared_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__sp_msgs__msg__Dm02SerialCopyMsg
    std::shared_ptr<sp_msgs::msg::Dm02SerialCopyMsg_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Dm02SerialCopyMsg_ & other) const
  {
    if (this->timestamp != other.timestamp) {
      return false;
    }
    if (this->established != other.established) {
      return false;
    }
    if (this->have_state != other.have_state) {
      return false;
    }
    if (this->send_ok != other.send_ok) {
      return false;
    }
    if (this->send_fail != other.send_fail) {
      return false;
    }
    if (this->yaw_rad != other.yaw_rad) {
      return false;
    }
    if (this->pitch_rad != other.pitch_rad) {
      return false;
    }
    if (this->roll_rad != other.roll_rad) {
      return false;
    }
    if (this->enc_yaw != other.enc_yaw) {
      return false;
    }
    if (this->enc_pitch != other.enc_pitch) {
      return false;
    }
    if (this->yaw_vel_rad_s != other.yaw_vel_rad_s) {
      return false;
    }
    if (this->pitch_vel_rad_s != other.pitch_vel_rad_s) {
      return false;
    }
    if (this->bullet_speed_mps != other.bullet_speed_mps) {
      return false;
    }
    if (this->bullet_count != other.bullet_count) {
      return false;
    }
    if (this->gimbal_mode != other.gimbal_mode) {
      return false;
    }
    if (this->shoot_state != other.shoot_state) {
      return false;
    }
    if (this->shooter_heat != other.shooter_heat) {
      return false;
    }
    if (this->shooter_heat_limit != other.shooter_heat_limit) {
      return false;
    }
    if (this->projectile_allowance_17mm != other.projectile_allowance_17mm) {
      return false;
    }
    if (this->projectile_allowance_42mm != other.projectile_allowance_42mm) {
      return false;
    }
    if (this->state_device_ts_us != other.state_device_ts_us) {
      return false;
    }
    if (this->state_host_ts_ns != other.state_host_ts_ns) {
      return false;
    }
    if (this->referee_valid != other.referee_valid) {
      return false;
    }
    if (this->referee_enemy_team != other.referee_enemy_team) {
      return false;
    }
    if (this->referee_fire_allowed != other.referee_fire_allowed) {
      return false;
    }
    if (this->referee_robot_id != other.referee_robot_id) {
      return false;
    }
    if (this->referee_game_stage != other.referee_game_stage) {
      return false;
    }
    if (this->referee_status != other.referee_status) {
      return false;
    }
    if (this->referee_device_ts_us != other.referee_device_ts_us) {
      return false;
    }
    if (this->referee_host_ts_ns != other.referee_host_ts_ns) {
      return false;
    }
    if (this->tof_valid != other.tof_valid) {
      return false;
    }
    if (this->tof_distance_cm != other.tof_distance_cm) {
      return false;
    }
    if (this->tof_strength != other.tof_strength) {
      return false;
    }
    if (this->tof_temp_cdeg != other.tof_temp_cdeg) {
      return false;
    }
    if (this->tof_status != other.tof_status) {
      return false;
    }
    if (this->tof_device_ts_us != other.tof_device_ts_us) {
      return false;
    }
    if (this->tof_host_ts_ns != other.tof_host_ts_ns) {
      return false;
    }
    if (this->timesync_valid != other.timesync_valid) {
      return false;
    }
    if (this->timesync_offset_us != other.timesync_offset_us) {
      return false;
    }
    if (this->timesync_rtt_us != other.timesync_rtt_us) {
      return false;
    }
    if (this->timesync_version != other.timesync_version) {
      return false;
    }
    if (this->timesync_last_device_time_us != other.timesync_last_device_time_us) {
      return false;
    }
    if (this->timesync_last_host_time_us != other.timesync_last_host_time_us) {
      return false;
    }
    return true;
  }
  bool operator!=(const Dm02SerialCopyMsg_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Dm02SerialCopyMsg_

// alias to use template instance with default allocator
using Dm02SerialCopyMsg =
  sp_msgs::msg::Dm02SerialCopyMsg_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace sp_msgs

#endif  // SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__STRUCT_HPP_
