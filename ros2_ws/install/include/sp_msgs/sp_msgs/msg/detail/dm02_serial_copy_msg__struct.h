// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sp_msgs/msg/dm02_serial_copy_msg.h"


#ifndef SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__STRUCT_H_
#define SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__STRUCT_H_

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

/// Struct defined in msg/Dm02SerialCopyMsg in the package sp_msgs.
/**
  * Message timestamp
 */
typedef struct sp_msgs__msg__Dm02SerialCopyMsg
{
  builtin_interfaces__msg__Time timestamp;
  /// Link/driver status
  bool established;
  bool have_state;
  uint64_t send_ok;
  uint64_t send_fail;
  /// Gimbal state copied from DM02 serial
  float yaw_rad;
  float pitch_rad;
  float roll_rad;
  int32_t enc_yaw;
  int32_t enc_pitch;
  float yaw_vel_rad_s;
  float pitch_vel_rad_s;
  float bullet_speed_mps;
  uint16_t bullet_count;
  int32_t gimbal_mode;
  int32_t shoot_state;
  int32_t shooter_heat;
  int32_t shooter_heat_limit;
  int32_t projectile_allowance_17mm;
  int32_t projectile_allowance_42mm;
  uint64_t state_device_ts_us;
  uint64_t state_host_ts_ns;
  /// Referee state copied from DM02 serial
  bool referee_valid;
  int32_t referee_enemy_team;
  bool referee_fire_allowed;
  int32_t referee_robot_id;
  int32_t referee_game_stage;
  uint16_t referee_status;
  uint64_t referee_device_ts_us;
  uint64_t referee_host_ts_ns;
  /// ToF/TFMini state copied from DM02 serial
  bool tof_valid;
  uint16_t tof_distance_cm;
  uint16_t tof_strength;
  int16_t tof_temp_cdeg;
  uint16_t tof_status;
  uint64_t tof_device_ts_us;
  uint64_t tof_host_ts_ns;
  /// Time sync state copied from DM02 serial
  bool timesync_valid;
  int64_t timesync_offset_us;
  uint32_t timesync_rtt_us;
  uint32_t timesync_version;
  uint64_t timesync_last_device_time_us;
  uint64_t timesync_last_host_time_us;
} sp_msgs__msg__Dm02SerialCopyMsg;

// Struct for a sequence of sp_msgs__msg__Dm02SerialCopyMsg.
typedef struct sp_msgs__msg__Dm02SerialCopyMsg__Sequence
{
  sp_msgs__msg__Dm02SerialCopyMsg * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sp_msgs__msg__Dm02SerialCopyMsg__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__STRUCT_H_
