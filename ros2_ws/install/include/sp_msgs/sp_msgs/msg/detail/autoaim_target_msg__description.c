// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sp_msgs:msg/AutoaimTargetMsg.idl
// generated code does not contain a copyright notice

#include "sp_msgs/msg/detail/autoaim_target_msg__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sp_msgs
const rosidl_type_hash_t *
sp_msgs__msg__AutoaimTargetMsg__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x51, 0xb4, 0x04, 0x2f, 0xb6, 0xa6, 0xf6, 0x97,
      0x04, 0x72, 0xf1, 0x65, 0x8a, 0x14, 0x60, 0x24,
      0x64, 0xf8, 0x64, 0x38, 0x55, 0xc7, 0x59, 0x43,
      0x31, 0xa1, 0x65, 0x53, 0xe0, 0xe8, 0x72, 0x0f,
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

static char sp_msgs__msg__AutoaimTargetMsg__TYPE_NAME[] = "sp_msgs/msg/AutoaimTargetMsg";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__timestamp[] = "timestamp";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__target_ids[] = "target_ids";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__main_target_id[] = "main_target_id";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__has_target[] = "has_target";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__yaw_deg[] = "yaw_deg";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__pitch_deg[] = "pitch_deg";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__distance_m[] = "distance_m";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__vx[] = "vx";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__vy[] = "vy";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__vz[] = "vz";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__confidence[] = "confidence";
static char sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__shoot_command[] = "shoot_command";

static rosidl_runtime_c__type_description__Field sp_msgs__msg__AutoaimTargetMsg__FIELDS[] = {
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__timestamp, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__target_ids, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__main_target_id, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__has_target, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__yaw_deg, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__pitch_deg, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__distance_m, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__vx, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__vy, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__vz, 2, 2},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__confidence, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_FLOAT,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__AutoaimTargetMsg__FIELD_NAME__shoot_command, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_UINT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sp_msgs__msg__AutoaimTargetMsg__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sp_msgs__msg__AutoaimTargetMsg__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sp_msgs__msg__AutoaimTargetMsg__TYPE_NAME, 28, 28},
      {sp_msgs__msg__AutoaimTargetMsg__FIELDS, 12, 12},
    },
    {sp_msgs__msg__AutoaimTargetMsg__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# \\xe4\\xb8\\x8a\\xe5\\xb1\\x82\\xe4\\xb8\\x8b\\xe5\\x8f\\x91\\xe8\\x87\\xaa\\xe7\\x9e\\x84\\xe7\\x9b\\xae\\xe6\\xa0\\x87\\xe4\\xbf\\xa1\\xe6\\x81\\xaf\n"
  "\n"
  "# \\xe6\\xb6\\x88\\xe6\\x81\\xaf\\xe7\\x94\\x9f\\xe6\\x88\\x90\\xe6\\x97\\xb6\\xe9\\x97\\xb4\n"
  "builtin_interfaces/Time timestamp\n"
  "\n"
  "# \\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe5\\x85\\xb3\\xe6\\xb3\\xa8/\\xe5\\x80\\x99\\xe9\\x80\\x89\\xe7\\x9b\\xae\\xe6\\xa0\\x87 ID \\xe5\\x88\\x97\\xe8\\xa1\\xa8\\xef\\xbc\\x8c\\xe5\\x89\\x8d\\xe9\\x9d\\xa2\\xe7\\x9a\\x84\\xe4\\xbc\\x98\\xe5\\x85\\x88\\xe7\\xba\\xa7\\xe6\\x9b\\xb4\\xe9\\xab\\x98\n"
  "# \\xe4\\xbe\\x8b\\xe5\\xa6\\x82\\xef\\xbc\\x9a[5, 3, 1] \\xe8\\xa1\\xa8\\xe7\\xa4\\xba\\xe4\\xbc\\x98\\xe5\\x85\\x88\\xe6\\x89\\x93 5 \\xe5\\x8f\\xb7\\xef\\xbc\\x8c\\xe5\\x86\\x8d\\xe6\\x98\\xaf 3 \\xe5\\x8f\\xb7\\xe5\\x92\\x8c 1 \\xe5\\x8f\\xb7\n"
  "int8[] target_ids\n"
  "\n"
  "# ===== \\xe4\\xbb\\xa5\\xe4\\xb8\\x8b\\xe4\\xb8\\xba\\xe5\\x8f\\xaf\\xe9\\x80\\x89\\xe6\\x89\\xa9\\xe5\\xb1\\x95\\xe5\\xad\\x97\\xe6\\xae\\xb5 =====\n"
  "\n"
  "# \\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe9\\xa6\\x96\\xe8\\xa6\\x81\\xe7\\x9b\\xae\\xe6\\xa0\\x87 ID\n"
  "# \\xe5\\xa6\\x82\\xe6\\x9e\\x9c\\xe6\\xb2\\xa1\\xe6\\x9c\\x89\\xe6\\x98\\x8e\\xe7\\xa1\\xae\\xe9\\xa6\\x96\\xe8\\xa6\\x81\\xe7\\x9b\\xae\\xe6\\xa0\\x87\\xef\\xbc\\x8c\\xe5\\x8f\\xaf\\xe4\\xbb\\xa5\\xe8\\xae\\xbe\\xe4\\xb8\\xba -1\n"
  "int8 main_target_id\n"
  "\n"
  "# \\xe6\\x98\\xaf\\xe5\\x90\\xa6\\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe6\\x9c\\x89\\xe6\\x9c\\x89\\xe6\\x95\\x88\\xe7\\x9b\\xae\\xe6\\xa0\\x87\n"
  "bool has_target\n"
  "\n"
  "# \\xe6\\x9c\\x9f\\xe6\\x9c\\x9b\\xe4\\xba\\x91\\xe5\\x8f\\xb0\\xe5\\xa4\\xa7\\xe8\\x87\\xb4\\xe6\\x9c\\x9d\\xe5\\x90\\x91\\xe7\\x9a\\x84\\xe6\\xb0\\xb4\\xe5\\xb9\\xb3\\xe8\\xa7\\x92\\xe5\\xba\\xa6\\xef\\xbc\\x88\\xe5\\xba\\xa6\\xef\\xbc\\x89\n"
  "float32 yaw_deg\n"
  "\n"
  "# \\xe6\\x9c\\x9f\\xe6\\x9c\\x9b\\xe4\\xba\\x91\\xe5\\x8f\\xb0\\xe5\\xa4\\xa7\\xe8\\x87\\xb4\\xe6\\x9c\\x9d\\xe5\\x90\\x91\\xe7\\x9a\\x84\\xe4\\xbf\\xaf\\xe4\\xbb\\xb0\\xe8\\xa7\\x92\\xef\\xbc\\x88\\xe5\\xba\\xa6\\xef\\xbc\\x89\n"
  "float32 pitch_deg\n"
  "\n"
  "# \\xe7\\x9b\\xae\\xe6\\xa0\\x87\\xe5\\xa4\\xa7\\xe8\\x87\\xb4\\xe8\\xb7\\x9d\\xe7\\xa6\\xbb\\xef\\xbc\\x88\\xe7\\xb1\\xb3\\xef\\xbc\\x89\n"
  "float32 distance_m\n"
  "\n"
  "# \\xe7\\x9b\\xae\\xe6\\xa0\\x87\\xe5\\x9c\\xa8\\xe6\\x9c\\xba\\xe5\\x99\\xa8\\xe4\\xba\\xba\\xe5\\x9d\\x90\\xe6\\xa0\\x87\\xe7\\xb3\\xbb\\xe4\\xb8\\x8b\\xe7\\x9a\\x84\\xe9\\x80\\x9f\\xe5\\xba\\xa6\\xe4\\xbc\\xb0\\xe8\\xae\\xa1\\xef\\xbc\\x88\\xe7\\xb1\\xb3/\\xe7\\xa7\\x92\\xef\\xbc\\x89\n"
  "float32 vx\n"
  "float32 vy\n"
  "float32 vz\n"
  "\n"
  "# \\xe4\\xb8\\x8a\\xe5\\xb1\\x82\\xe5\\xaf\\xb9\\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe7\\x9b\\xae\\xe6\\xa0\\x87\\xe9\\x9b\\x86\\xe5\\x90\\x88\\xe7\\x9a\\x84\\xe2\\x80\\x9c\\xe4\\xbf\\xa1\\xe5\\xbf\\x83\\xe2\\x80\\x9d\\xef\\xbc\\x8c\\xe8\\x8c\\x83\\xe5\\x9b\\xb4 0.0 ~ 1.0\n"
  "float32 confidence\n"
  "\n"
  "# \\xe5\\xb0\\x84\\xe5\\x87\\xbb\\xe6\\x8c\\x87\\xe4\\xbb\\xa4\\xef\\xbc\\x9a0=\\xe4\\xb8\\x8d\\xe5\\xb9\\xb2\\xe9\\xa2\\x84, 1=\\xe5\\xbb\\xba\\xe8\\xae\\xae\\xe5\\xbc\\x80\\xe7\\x81\\xab, 2=\\xe7\\xa6\\x81\\xe6\\xad\\xa2\\xe5\\xbc\\x80\\xe7\\x81\\xab\n"
  "uint8 shoot_command";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sp_msgs__msg__AutoaimTargetMsg__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sp_msgs__msg__AutoaimTargetMsg__TYPE_NAME, 28, 28},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 509, 509},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sp_msgs__msg__AutoaimTargetMsg__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sp_msgs__msg__AutoaimTargetMsg__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
