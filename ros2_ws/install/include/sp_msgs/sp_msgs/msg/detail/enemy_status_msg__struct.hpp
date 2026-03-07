// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from sp_msgs:msg/EnemyStatusMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/enemy_status_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__ENEMY_STATUS_MSG__STRUCT_HPP_
#define SP_MSGS__MSG__DETAIL__ENEMY_STATUS_MSG__STRUCT_HPP_

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
# define DEPRECATED__sp_msgs__msg__EnemyStatusMsg __attribute__((deprecated))
#else
# define DEPRECATED__sp_msgs__msg__EnemyStatusMsg __declspec(deprecated)
#endif

namespace sp_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct EnemyStatusMsg_
{
  using Type = EnemyStatusMsg_<ContainerAllocator>;

  explicit EnemyStatusMsg_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : timestamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->game_stage = 0;
      this->enemy_count = 0;
      this->fire_allowed = false;
      this->enemy_team = 0;
    }
  }

  explicit EnemyStatusMsg_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : timestamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->game_stage = 0;
      this->enemy_count = 0;
      this->fire_allowed = false;
      this->enemy_team = 0;
    }
  }

  // field types and members
  using _timestamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _timestamp_type timestamp;
  using _invincible_enemy_ids_type =
    std::vector<int8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int8_t>>;
  _invincible_enemy_ids_type invincible_enemy_ids;
  using _game_stage_type =
    int8_t;
  _game_stage_type game_stage;
  using _enemy_count_type =
    int8_t;
  _enemy_count_type enemy_count;
  using _fire_allowed_type =
    bool;
  _fire_allowed_type fire_allowed;
  using _enemy_team_type =
    int8_t;
  _enemy_team_type enemy_team;
  using _detected_enemy_ids_type =
    std::vector<int8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int8_t>>;
  _detected_enemy_ids_type detected_enemy_ids;

  // setters for named parameter idiom
  Type & set__timestamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->timestamp = _arg;
    return *this;
  }
  Type & set__invincible_enemy_ids(
    const std::vector<int8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int8_t>> & _arg)
  {
    this->invincible_enemy_ids = _arg;
    return *this;
  }
  Type & set__game_stage(
    const int8_t & _arg)
  {
    this->game_stage = _arg;
    return *this;
  }
  Type & set__enemy_count(
    const int8_t & _arg)
  {
    this->enemy_count = _arg;
    return *this;
  }
  Type & set__fire_allowed(
    const bool & _arg)
  {
    this->fire_allowed = _arg;
    return *this;
  }
  Type & set__enemy_team(
    const int8_t & _arg)
  {
    this->enemy_team = _arg;
    return *this;
  }
  Type & set__detected_enemy_ids(
    const std::vector<int8_t, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<int8_t>> & _arg)
  {
    this->detected_enemy_ids = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator> *;
  using ConstRawPtr =
    const sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__sp_msgs__msg__EnemyStatusMsg
    std::shared_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__sp_msgs__msg__EnemyStatusMsg
    std::shared_ptr<sp_msgs::msg::EnemyStatusMsg_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const EnemyStatusMsg_ & other) const
  {
    if (this->timestamp != other.timestamp) {
      return false;
    }
    if (this->invincible_enemy_ids != other.invincible_enemy_ids) {
      return false;
    }
    if (this->game_stage != other.game_stage) {
      return false;
    }
    if (this->enemy_count != other.enemy_count) {
      return false;
    }
    if (this->fire_allowed != other.fire_allowed) {
      return false;
    }
    if (this->enemy_team != other.enemy_team) {
      return false;
    }
    if (this->detected_enemy_ids != other.detected_enemy_ids) {
      return false;
    }
    return true;
  }
  bool operator!=(const EnemyStatusMsg_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct EnemyStatusMsg_

// alias to use template instance with default allocator
using EnemyStatusMsg =
  sp_msgs::msg::EnemyStatusMsg_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace sp_msgs

#endif  // SP_MSGS__MSG__DETAIL__ENEMY_STATUS_MSG__STRUCT_HPP_
