// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from sp_msgs:msg/AutoaimTargetMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/autoaim_target_msg.hpp"


#ifndef SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__TRAITS_HPP_
#define SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "sp_msgs/msg/detail/autoaim_target_msg__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'timestamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace sp_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const AutoaimTargetMsg & msg,
  std::ostream & out)
{
  out << "{";
  // member: timestamp
  {
    out << "timestamp: ";
    to_flow_style_yaml(msg.timestamp, out);
    out << ", ";
  }

  // member: target_ids
  {
    if (msg.target_ids.size() == 0) {
      out << "target_ids: []";
    } else {
      out << "target_ids: [";
      size_t pending_items = msg.target_ids.size();
      for (auto item : msg.target_ids) {
        rosidl_generator_traits::value_to_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
    out << ", ";
  }

  // member: main_target_id
  {
    out << "main_target_id: ";
    rosidl_generator_traits::value_to_yaml(msg.main_target_id, out);
    out << ", ";
  }

  // member: has_target
  {
    out << "has_target: ";
    rosidl_generator_traits::value_to_yaml(msg.has_target, out);
    out << ", ";
  }

  // member: yaw_deg
  {
    out << "yaw_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_deg, out);
    out << ", ";
  }

  // member: pitch_deg
  {
    out << "pitch_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.pitch_deg, out);
    out << ", ";
  }

  // member: distance_m
  {
    out << "distance_m: ";
    rosidl_generator_traits::value_to_yaml(msg.distance_m, out);
    out << ", ";
  }

  // member: vx
  {
    out << "vx: ";
    rosidl_generator_traits::value_to_yaml(msg.vx, out);
    out << ", ";
  }

  // member: vy
  {
    out << "vy: ";
    rosidl_generator_traits::value_to_yaml(msg.vy, out);
    out << ", ";
  }

  // member: vz
  {
    out << "vz: ";
    rosidl_generator_traits::value_to_yaml(msg.vz, out);
    out << ", ";
  }

  // member: confidence
  {
    out << "confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.confidence, out);
    out << ", ";
  }

  // member: shoot_command
  {
    out << "shoot_command: ";
    rosidl_generator_traits::value_to_yaml(msg.shoot_command, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const AutoaimTargetMsg & msg,
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

  // member: target_ids
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.target_ids.size() == 0) {
      out << "target_ids: []\n";
    } else {
      out << "target_ids:\n";
      for (auto item : msg.target_ids) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "- ";
        rosidl_generator_traits::value_to_yaml(item, out);
        out << "\n";
      }
    }
  }

  // member: main_target_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "main_target_id: ";
    rosidl_generator_traits::value_to_yaml(msg.main_target_id, out);
    out << "\n";
  }

  // member: has_target
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "has_target: ";
    rosidl_generator_traits::value_to_yaml(msg.has_target, out);
    out << "\n";
  }

  // member: yaw_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "yaw_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.yaw_deg, out);
    out << "\n";
  }

  // member: pitch_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pitch_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.pitch_deg, out);
    out << "\n";
  }

  // member: distance_m
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "distance_m: ";
    rosidl_generator_traits::value_to_yaml(msg.distance_m, out);
    out << "\n";
  }

  // member: vx
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "vx: ";
    rosidl_generator_traits::value_to_yaml(msg.vx, out);
    out << "\n";
  }

  // member: vy
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "vy: ";
    rosidl_generator_traits::value_to_yaml(msg.vy, out);
    out << "\n";
  }

  // member: vz
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "vz: ";
    rosidl_generator_traits::value_to_yaml(msg.vz, out);
    out << "\n";
  }

  // member: confidence
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.confidence, out);
    out << "\n";
  }

  // member: shoot_command
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "shoot_command: ";
    rosidl_generator_traits::value_to_yaml(msg.shoot_command, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const AutoaimTargetMsg & msg, bool use_flow_style = false)
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
  const sp_msgs::msg::AutoaimTargetMsg & msg,
  std::ostream & out, size_t indentation = 0)
{
  sp_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use sp_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const sp_msgs::msg::AutoaimTargetMsg & msg)
{
  return sp_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<sp_msgs::msg::AutoaimTargetMsg>()
{
  return "sp_msgs::msg::AutoaimTargetMsg";
}

template<>
inline const char * name<sp_msgs::msg::AutoaimTargetMsg>()
{
  return "sp_msgs/msg/AutoaimTargetMsg";
}

template<>
struct has_fixed_size<sp_msgs::msg::AutoaimTargetMsg>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<sp_msgs::msg::AutoaimTargetMsg>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<sp_msgs::msg::AutoaimTargetMsg>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__TRAITS_HPP_
