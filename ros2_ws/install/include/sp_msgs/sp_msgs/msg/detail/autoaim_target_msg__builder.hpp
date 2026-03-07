// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from sp_msgs:msg/AutoaimTargetMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/autoaim_target_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__BUILDER_HPP_
#define SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "sp_msgs/msg/detail/autoaim_target_msg__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace sp_msgs
{

namespace msg
{

namespace builder
{

class Init_AutoaimTargetMsg_shoot_command
{
public:
  explicit Init_AutoaimTargetMsg_shoot_command(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  ::sp_msgs::msg::AutoaimTargetMsg shoot_command(::sp_msgs::msg::AutoaimTargetMsg::_shoot_command_type arg)
  {
    msg_.shoot_command = std::move(arg);
    return std::move(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_confidence
{
public:
  explicit Init_AutoaimTargetMsg_confidence(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_shoot_command confidence(::sp_msgs::msg::AutoaimTargetMsg::_confidence_type arg)
  {
    msg_.confidence = std::move(arg);
    return Init_AutoaimTargetMsg_shoot_command(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_vz
{
public:
  explicit Init_AutoaimTargetMsg_vz(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_confidence vz(::sp_msgs::msg::AutoaimTargetMsg::_vz_type arg)
  {
    msg_.vz = std::move(arg);
    return Init_AutoaimTargetMsg_confidence(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_vy
{
public:
  explicit Init_AutoaimTargetMsg_vy(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_vz vy(::sp_msgs::msg::AutoaimTargetMsg::_vy_type arg)
  {
    msg_.vy = std::move(arg);
    return Init_AutoaimTargetMsg_vz(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_vx
{
public:
  explicit Init_AutoaimTargetMsg_vx(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_vy vx(::sp_msgs::msg::AutoaimTargetMsg::_vx_type arg)
  {
    msg_.vx = std::move(arg);
    return Init_AutoaimTargetMsg_vy(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_distance_m
{
public:
  explicit Init_AutoaimTargetMsg_distance_m(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_vx distance_m(::sp_msgs::msg::AutoaimTargetMsg::_distance_m_type arg)
  {
    msg_.distance_m = std::move(arg);
    return Init_AutoaimTargetMsg_vx(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_pitch_deg
{
public:
  explicit Init_AutoaimTargetMsg_pitch_deg(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_distance_m pitch_deg(::sp_msgs::msg::AutoaimTargetMsg::_pitch_deg_type arg)
  {
    msg_.pitch_deg = std::move(arg);
    return Init_AutoaimTargetMsg_distance_m(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_yaw_deg
{
public:
  explicit Init_AutoaimTargetMsg_yaw_deg(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_pitch_deg yaw_deg(::sp_msgs::msg::AutoaimTargetMsg::_yaw_deg_type arg)
  {
    msg_.yaw_deg = std::move(arg);
    return Init_AutoaimTargetMsg_pitch_deg(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_has_target
{
public:
  explicit Init_AutoaimTargetMsg_has_target(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_yaw_deg has_target(::sp_msgs::msg::AutoaimTargetMsg::_has_target_type arg)
  {
    msg_.has_target = std::move(arg);
    return Init_AutoaimTargetMsg_yaw_deg(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_main_target_id
{
public:
  explicit Init_AutoaimTargetMsg_main_target_id(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_has_target main_target_id(::sp_msgs::msg::AutoaimTargetMsg::_main_target_id_type arg)
  {
    msg_.main_target_id = std::move(arg);
    return Init_AutoaimTargetMsg_has_target(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_target_ids
{
public:
  explicit Init_AutoaimTargetMsg_target_ids(::sp_msgs::msg::AutoaimTargetMsg & msg)
  : msg_(msg)
  {}
  Init_AutoaimTargetMsg_main_target_id target_ids(::sp_msgs::msg::AutoaimTargetMsg::_target_ids_type arg)
  {
    msg_.target_ids = std::move(arg);
    return Init_AutoaimTargetMsg_main_target_id(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

class Init_AutoaimTargetMsg_timestamp
{
public:
  Init_AutoaimTargetMsg_timestamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_AutoaimTargetMsg_target_ids timestamp(::sp_msgs::msg::AutoaimTargetMsg::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return Init_AutoaimTargetMsg_target_ids(msg_);
  }

private:
  ::sp_msgs::msg::AutoaimTargetMsg msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::sp_msgs::msg::AutoaimTargetMsg>()
{
  return sp_msgs::msg::builder::Init_AutoaimTargetMsg_timestamp();
}

}  // namespace sp_msgs

#endif  // SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__BUILDER_HPP_
