// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice

#include "sp_msgs/msg/detail/dm02_serial_copy_msg__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sp_msgs
const rosidl_type_hash_t *
sp_msgs__msg__Dm02SerialCopyMsg__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x32, 0x57, 0xe0, 0x7a, 0xc7, 0xe5, 0xa0, 0x13,
      0x89, 0x73, 0xe1, 0xf1, 0x1a, 0xdc, 0xe1, 0xed,
      0xca, 0x6c, 0x47, 0x70, 0x90, 0x27, 0x9c, 0xd7,
      0xb3, 0x2d, 0x3d, 0xff, 0x71, 0x5c, 0x83, 0x90,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
#endif

static char sp_msgs__msg__Dm02SerialCopyMsg__TYPE_NAME[] = "sp_msgs/msg/Dm02SerialCopyMsg";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timestamp[] = "timestamp";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__established[] = "established";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__have_state[] = "have_state";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__send_ok[] = "send_ok";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__send_fail[] = "send_fail";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__yaw_rad[] = "yaw_rad";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__pitch_rad[] = "pitch_rad";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__roll_rad[] = "roll_rad";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__enc_yaw[] = "enc_yaw";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__enc_pitch[] = "enc_pitch";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__yaw_vel_rad_s[] = "yaw_vel_rad_s";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__pitch_vel_rad_s[] = "pitch_vel_rad_s";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__bullet_speed_mps[] = "bullet_speed_mps";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__bullet_count[] = "bullet_count";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__gimbal_mode[] = "gimbal_mode";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__shoot_state[] = "shoot_state";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__shooter_heat[] = "shooter_heat";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__shooter_heat_limit[] = "shooter_heat_limit";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__projectile_allowance_17mm[] = "projectile_allowance_17mm";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__projectile_allowance_42mm[] = "projectile_allowance_42mm";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__state_device_ts_us[] = "state_device_ts_us";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__state_host_ts_ns[] = "state_host_ts_ns";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_valid[] = "referee_valid";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_enemy_team[] = "referee_enemy_team";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_fire_allowed[] = "referee_fire_allowed";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_robot_id[] = "referee_robot_id";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_game_stage[] = "referee_game_stage";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_status[] = "referee_status";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_device_ts_us[] = "referee_device_ts_us";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_host_ts_ns[] = "referee_host_ts_ns";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_valid[] = "tof_valid";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_distance_cm[] = "tof_distance_cm";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_strength[] = "tof_strength";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_temp_cdeg[] = "tof_temp_cdeg";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_status[] = "tof_status";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_device_ts_us[] = "tof_device_ts_us";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_host_ts_ns[] = "tof_host_ts_ns";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_valid[] = "timesync_valid";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_offset_us[] = "timesync_offset_us";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_rtt_us[] = "timesync_rtt_us";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_version[] = "timesync_version";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_last_device_time_us[] = "timesync_last_device_time_us";
static char sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_last_host_time_us[] = "timesync_last_host_time_us";

static rosidl_runtime_c__type_description__Field sp_msgs__msg__Dm02SerialCopyMsg__FIELDS[] = {
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timestamp, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__established, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__have_state, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__send_ok, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__send_fail, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__yaw_rad, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__pitch_rad, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__roll_rad, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__enc_yaw, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__enc_pitch, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__yaw_vel_rad_s, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__pitch_vel_rad_s, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__bullet_speed_mps, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__bullet_count, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__gimbal_mode, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__shoot_state, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__shooter_heat, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__shooter_heat_limit, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__projectile_allowance_17mm, 25, 25},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__projectile_allowance_42mm, 25, 25},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__state_device_ts_us, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__state_host_ts_ns, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_valid, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_enemy_team, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_fire_allowed, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_robot_id, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_game_stage, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_status, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_device_ts_us, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__referee_host_ts_ns, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_valid, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_distance_cm, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_strength, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_temp_cdeg, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_status, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT16,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_device_ts_us, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__tof_host_ts_ns, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_valid, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_offset_us, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_rtt_us, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_version, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT32,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_last_device_time_us, 28, 28},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__Dm02SerialCopyMsg__FIELD_NAME__timesync_last_host_time_us, 26, 26},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT64,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sp_msgs__msg__Dm02SerialCopyMsg__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sp_msgs__msg__Dm02SerialCopyMsg__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sp_msgs__msg__Dm02SerialCopyMsg__TYPE_NAME, 29, 29},
      {sp_msgs__msg__Dm02SerialCopyMsg__FIELDS, 43, 43},
    },
    {sp_msgs__msg__Dm02SerialCopyMsg__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Message timestamp\n"
  "builtin_interfaces/Time timestamp\n"
  "\n"
  "# Link/driver status\n"
  "bool established\n"
  "bool have_state\n"
  "uint64 send_ok\n"
  "uint64 send_fail\n"
  "\n"
  "# Gimbal state copied from DM02 serial\n"
  "float32 yaw_rad\n"
  "float32 pitch_rad\n"
  "float32 roll_rad\n"
  "int32 enc_yaw\n"
  "int32 enc_pitch\n"
  "float32 yaw_vel_rad_s\n"
  "float32 pitch_vel_rad_s\n"
  "float32 bullet_speed_mps\n"
  "uint16 bullet_count\n"
  "int32 gimbal_mode\n"
  "int32 shoot_state\n"
  "int32 shooter_heat\n"
  "int32 shooter_heat_limit\n"
  "int32 projectile_allowance_17mm\n"
  "int32 projectile_allowance_42mm\n"
  "uint64 state_device_ts_us\n"
  "uint64 state_host_ts_ns\n"
  "\n"
  "# Referee state copied from DM02 serial\n"
  "bool referee_valid\n"
  "int32 referee_enemy_team\n"
  "bool referee_fire_allowed\n"
  "int32 referee_robot_id\n"
  "int32 referee_game_stage\n"
  "uint16 referee_status\n"
  "uint64 referee_device_ts_us\n"
  "uint64 referee_host_ts_ns\n"
  "\n"
  "# ToF/TFMini state copied from DM02 serial\n"
  "bool tof_valid\n"
  "uint16 tof_distance_cm\n"
  "uint16 tof_strength\n"
  "int16 tof_temp_cdeg\n"
  "uint16 tof_status\n"
  "uint64 tof_device_ts_us\n"
  "uint64 tof_host_ts_ns\n"
  "\n"
  "# Time sync state copied from DM02 serial\n"
  "bool timesync_valid\n"
  "int64 timesync_offset_us\n"
  "uint32 timesync_rtt_us\n"
  "uint32 timesync_version\n"
  "uint64 timesync_last_device_time_us\n"
  "uint64 timesync_last_host_time_us";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sp_msgs__msg__Dm02SerialCopyMsg__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sp_msgs__msg__Dm02SerialCopyMsg__TYPE_NAME, 29, 29},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 1173, 1173},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sp_msgs__msg__Dm02SerialCopyMsg__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sp_msgs__msg__Dm02SerialCopyMsg__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
