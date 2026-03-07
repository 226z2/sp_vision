// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sp_msgs:msg/AutoaimTargetMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/autoaim_target_msg.h"


#ifndef SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__STRUCT_H_
#define SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Constants defined in the message

// Include directives for member types
// Member 'timestamp'
#include "builtin_interfaces/msg/detail/time__struct.h"
// Member 'target_ids'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/AutoaimTargetMsg in the package sp_msgs.
/**
  * 上层下发自瞄目标信息
 */
typedef struct sp_msgs__msg__AutoaimTargetMsg
{
  /// 消息生成时间
  builtin_interfaces__msg__Time timestamp;
  /// 当前关注/候选目标 ID 列表，前面的优先级更高
  /// 例如：[5, 3, 1] 表示优先打 5 号，再是 3 号和 1 号
  rosidl_runtime_c__int8__Sequence target_ids;
  /// ===== 以下为可选扩展字段 =====
  /// 当前首要目标 ID
  /// 如果没有明确首要目标，可以设为 -1
  int8_t main_target_id;
  /// 是否当前有有效目标
  bool has_target;
  /// 期望云台大致朝向的水平角度（度）
  float yaw_deg;
  /// 期望云台大致朝向的俯仰角（度）
  float pitch_deg;
  /// 目标大致距离（米）
  float distance_m;
  /// 目标在机器人坐标系下的速度估计（米/秒）
  float vx;
  float vy;
  float vz;
  /// 上层对当前目标集合的“信心”，范围 0.0 ~ 1.0
  float confidence;
  /// 射击指令：0=不干预, 1=建议开火, 2=禁止开火
  uint8_t shoot_command;
} sp_msgs__msg__AutoaimTargetMsg;

// Struct for a sequence of sp_msgs__msg__AutoaimTargetMsg.
typedef struct sp_msgs__msg__AutoaimTargetMsg__Sequence
{
  sp_msgs__msg__AutoaimTargetMsg * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sp_msgs__msg__AutoaimTargetMsg__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SP_MSGS__MSG__DETAIL__AUTOAIM_TARGET_MSG__STRUCT_H_
