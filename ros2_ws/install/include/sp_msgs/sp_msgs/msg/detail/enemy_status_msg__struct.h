// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sp_msgs:msg/EnemyStatusMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/enemy_status_msg.h"


#ifndef SP_MSGS__MSG__DETAIL__ENEMY_STATUS_MSG__STRUCT_H_
#define SP_MSGS__MSG__DETAIL__ENEMY_STATUS_MSG__STRUCT_H_

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
// Member 'invincible_enemy_ids'
// Member 'detected_enemy_ids'
#include "rosidl_runtime_c/primitives_sequence.h"

/// Struct defined in msg/EnemyStatusMsg in the package sp_msgs.
/**
  * 上层下发敌方整体状态
 */
typedef struct sp_msgs__msg__EnemyStatusMsg
{
  /// 消息生成时间
  builtin_interfaces__msg__Time timestamp;
  /// 当前处于“无敌状态”的敌方 ID 列表
  /// 例如：[3, 4] 表示 3 号与 4 号暂时打不动
  rosidl_runtime_c__int8__Sequence invincible_enemy_ids;
  /// ===== 以下为可选扩展字段（可以根据需要删减或不用） =====
  /// 比赛阶段：0=未知, 1=准备, 2=对战中, 3=加时, 4=结束
  int8_t game_stage;
  /// 场上存活的敌方单位数量
  int8_t enemy_count;
  /// 是否允许开火：true=允许，false=禁止（比如裁判区等）
  bool fire_allowed;
  /// 敌方阵营/颜色：0=未知, 1=RED, 2=BLUE
  int8_t enemy_team;
  /// 上层认为当前存在的敌方 ID 列表（用于和视觉检测结果对比）
  rosidl_runtime_c__int8__Sequence detected_enemy_ids;
} sp_msgs__msg__EnemyStatusMsg;

// Struct for a sequence of sp_msgs__msg__EnemyStatusMsg.
typedef struct sp_msgs__msg__EnemyStatusMsg__Sequence
{
  sp_msgs__msg__EnemyStatusMsg * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sp_msgs__msg__EnemyStatusMsg__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SP_MSGS__MSG__DETAIL__ENEMY_STATUS_MSG__STRUCT_H_
