// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from sp_msgs:msg/AutoaimTargetMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/autoaim_target_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__STRUCT_HPP_
#define SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__STRUCT_HPP_

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
# define DEPRECATED__sp_msgs__msg__AutoaimTargetMsg __attribute__((deprecated))
#else
# define DEPRECATED__sp_msgs__msg__AutoaimTargetMsg __declspec(deprecated)
#endif

namespace sp_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct AutoaimTargetMsg_
{
  using Type = AutoaimTargetMsg_<ContainerAllocator>;

  explicit AutoaimTargetMsg_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : timestamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->main_target_id = 0;
      this->has_target = false;
      this->yaw_deg = 0.0f;
      this->pitch_deg = 0.0f;
      this->distance_m = 0.0f;
      this->vx = 0.0f;
      this->vy = 0.0f;
      this->vz = 0.0f;
      this->confidence = 0.0f;
      this->shoot_command = 0;
    }
  }

  explicit AutoaimTargetMsg_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : timestamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->main_target_id = 0;
      this->has_target = false;
      this->yaw_deg = 0.0f;
      this->pitch_deg = 0.0f;
      this->distance_m = 0.0f;
      this->vx = 0.0f;
      this->vy = 0.0f;
      this->vz = 0.0f;
      this->confidence = 0.0f;
      this->shoot_command = 0;
    }
  }

  // field types and members
  using _timestamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _timestamp_type timestamp;
  using _target_ids_type =
    std::vector<int8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int8_t>>;
  _target_ids_type target_ids;
  using _main_target_id_type =
    int8_t;
  _main_target_id_type main_target_id;
  using _has_target_type =
    bool;
  _has_target_type has_target;
  using _yaw_deg_type =
    float;
  _yaw_deg_type yaw_deg;
  using _pitch_deg_type =
    float;
  _pitch_deg_type pitch_deg;
  using _distance_m_type =
    float;
  _distance_m_type distance_m;
  using _vx_type =
    float;
  _vx_type vx;
  using _vy_type =
    float;
  _vy_type vy;
  using _vz_type =
    float;
  _vz_type vz;
  using _confidence_type =
    float;
  _confidence_type confidence;
  using _shoot_command_type =
    uint8_t;
  _shoot_command_type shoot_command;

  // setters for named parameter idiom
  Type & set__timestamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->timestamp = _arg;
    return *this;
  }
  Type & set__target_ids(
    const std::vector<int8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int8_t>> & _arg)
  {
    this->target_ids = _arg;
    return *this;
  }
  Type & set__main_target_id(
    const int8_t & _arg)
  {
    this->main_target_id = _arg;
    return *this;
  }
  Type & set__has_target(
    const bool & _arg)
  {
    this->has_target = _arg;
    return *this;
  }
  Type & set__yaw_deg(
    const float & _arg)
  {
    this->yaw_deg = _arg;
    return *this;
  }
  Type & set__pitch_deg(
    const float & _arg)
  {
    this->pitch_deg = _arg;
    return *this;
  }
  Type & set__distance_m(
    const float & _arg)
  {
    this->distance_m = _arg;
    return *this;
  }
  Type & set__vx(
    const float & _arg)
  {
    this->vx = _arg;
    return *this;
  }
  Type & set__vy(
    const float & _arg)
  {
    this->vy = _arg;
    return *this;
  }
  Type & set__vz(
    const float & _arg)
  {
    this->vz = _arg;
    return *this;
  }
  Type & set__confidence(
    const float & _arg)
  {
    this->confidence = _arg;
    return *this;
  }
  Type & set__shoot_command(
    const uint8_t & _arg)
  {
    this->shoot_command = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator> *;
  using ConstRawPtr =
    const sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__sp_msgs__msg__AutoaimTargetMsg
    std::shared_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__sp_msgs__msg__AutoaimTargetMsg
    std::shared_ptr<sp_msgs::msg::AutoaimTargetMsg_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const AutoaimTargetMsg_ & other) const
  {
    if (this->timestamp != other.timestamp) {
      return false;
    }
    if (this->target_ids != other.target_ids) {
      return false;
    }
    if (this->main_target_id != other.main_target_id) {
      return false;
    }
    if (this->has_target != other.has_target) {
      return false;
    }
    if (this->yaw_deg != other.yaw_deg) {
      return false;
    }
    if (this->pitch_deg != other.pitch_deg) {
      return false;
    }
    if (this->distance_m != other.distance_m) {
      return false;
    }
    if (this->vx != other.vx) {
      return false;
    }
    if (this->vy != other.vy) {
      return false;
    }
    if (this->vz != other.vz) {
      return false;
    }
    if (this->confidence != other.confidence) {
      return false;
    }
    if (this->shoot_command != other.shoot_command) {
      return false;
    }
    return true;
  }
  bool operator!=(const AutoaimTargetMsg_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct AutoaimTargetMsg_

// alias to use template instance with default allocator
using AutoaimTargetMsg =
  sp_msgs::msg::AutoaimTargetMsg_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace sp_msgs

#endif  // SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__STRUCT_HPP_
