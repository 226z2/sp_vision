// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from sp_msgs:msg/EnemyStatusMsg.idl
// generated code does not contain a copyright notice

#include "sp_msgs/msg/detail/enemy_status_msg__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_sp_msgs
const rosidl_type_hash_t *
sp_msgs__msg__EnemyStatusMsg__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xef, 0x4a, 0x44, 0xcb, 0x7c, 0xe5, 0x1f, 0x73,
      0xe9, 0x91, 0xb2, 0xce, 0xa0, 0x37, 0xdb, 0xd1,
      0x51, 0x6d, 0x37, 0x09, 0x63, 0x64, 0x7a, 0x87,
      0x6a, 0x43, 0x29, 0xf9, 0x0b, 0xef, 0xe0, 0x28,
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

static char sp_msgs__msg__EnemyStatusMsg__TYPE_NAME[] = "sp_msgs/msg/EnemyStatusMsg";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";

// Define type names, field names, and default values
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__timestamp[] = "timestamp";
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__invincible_enemy_ids[] = "invincible_enemy_ids";
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__game_stage[] = "game_stage";
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__enemy_count[] = "enemy_count";
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__fire_allowed[] = "fire_allowed";
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__enemy_team[] = "enemy_team";
static char sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__detected_enemy_ids[] = "detected_enemy_ids";

static rosidl_runtime_c__type_description__Field sp_msgs__msg__EnemyStatusMsg__FIELDS[] = {
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__timestamp, 9, 9},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__invincible_enemy_ids, 20, 20},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__game_stage, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__enemy_count, 11, 11},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__fire_allowed, 12, 12},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__enemy_team, 10, 10},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {sp_msgs__msg__EnemyStatusMsg__FIELD_NAME__detected_enemy_ids, 18, 18},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_INT8_UNBOUNDED_SEQUENCE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription sp_msgs__msg__EnemyStatusMsg__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
sp_msgs__msg__EnemyStatusMsg__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {sp_msgs__msg__EnemyStatusMsg__TYPE_NAME, 26, 26},
      {sp_msgs__msg__EnemyStatusMsg__FIELDS, 7, 7},
    },
    {sp_msgs__msg__EnemyStatusMsg__REFERENCED_TYPE_DESCRIPTIONS, 1, 1},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# \\xe4\\xb8\\x8a\\xe5\\xb1\\x82\\xe4\\xb8\\x8b\\xe5\\x8f\\x91\\xe6\\x95\\x8c\\xe6\\x96\\xb9\\xe6\\x95\\xb4\\xe4\\xbd\\x93\\xe7\\x8a\\xb6\\xe6\\x80\\x81\n"
  "\n"
  "# \\xe6\\xb6\\x88\\xe6\\x81\\xaf\\xe7\\x94\\x9f\\xe6\\x88\\x90\\xe6\\x97\\xb6\\xe9\\x97\\xb4\n"
  "builtin_interfaces/Time timestamp\n"
  "\n"
  "# \\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe5\\xa4\\x84\\xe4\\xba\\x8e\\xe2\\x80\\x9c\\xe6\\x97\\xa0\\xe6\\x95\\x8c\\xe7\\x8a\\xb6\\xe6\\x80\\x81\\xe2\\x80\\x9d\\xe7\\x9a\\x84\\xe6\\x95\\x8c\\xe6\\x96\\xb9 ID \\xe5\\x88\\x97\\xe8\\xa1\\xa8\n"
  "# \\xe4\\xbe\\x8b\\xe5\\xa6\\x82\\xef\\xbc\\x9a[3, 4] \\xe8\\xa1\\xa8\\xe7\\xa4\\xba 3 \\xe5\\x8f\\xb7\\xe4\\xb8\\x8e 4 \\xe5\\x8f\\xb7\\xe6\\x9a\\x82\\xe6\\x97\\xb6\\xe6\\x89\\x93\\xe4\\xb8\\x8d\\xe5\\x8a\\xa8\n"
  "int8[] invincible_enemy_ids\n"
  "\n"
  "# ===== \\xe4\\xbb\\xa5\\xe4\\xb8\\x8b\\xe4\\xb8\\xba\\xe5\\x8f\\xaf\\xe9\\x80\\x89\\xe6\\x89\\xa9\\xe5\\xb1\\x95\\xe5\\xad\\x97\\xe6\\xae\\xb5\\xef\\xbc\\x88\\xe5\\x8f\\xaf\\xe4\\xbb\\xa5\\xe6\\xa0\\xb9\\xe6\\x8d\\xae\\xe9\\x9c\\x80\\xe8\\xa6\\x81\\xe5\\x88\\xa0\\xe5\\x87\\x8f\\xe6\\x88\\x96\\xe4\\xb8\\x8d\\xe7\\x94\\xa8\\xef\\xbc\\x89 =====\n"
  "\n"
  "# \\xe6\\xaf\\x94\\xe8\\xb5\\x9b\\xe9\\x98\\xb6\\xe6\\xae\\xb5\\xef\\xbc\\x9a0=\\xe6\\x9c\\xaa\\xe7\\x9f\\xa5, 1=\\xe5\\x87\\x86\\xe5\\xa4\\x87, 2=\\xe5\\xaf\\xb9\\xe6\\x88\\x98\\xe4\\xb8\\xad, 3=\\xe5\\x8a\\xa0\\xe6\\x97\\xb6, 4=\\xe7\\xbb\\x93\\xe6\\x9d\\x9f\n"
  "int8 game_stage\n"
  "\n"
  "# \\xe5\\x9c\\xba\\xe4\\xb8\\x8a\\xe5\\xad\\x98\\xe6\\xb4\\xbb\\xe7\\x9a\\x84\\xe6\\x95\\x8c\\xe6\\x96\\xb9\\xe5\\x8d\\x95\\xe4\\xbd\\x8d\\xe6\\x95\\xb0\\xe9\\x87\\x8f\n"
  "int8 enemy_count\n"
  "\n"
  "# \\xe6\\x98\\xaf\\xe5\\x90\\xa6\\xe5\\x85\\x81\\xe8\\xae\\xb8\\xe5\\xbc\\x80\\xe7\\x81\\xab\\xef\\xbc\\x9atrue=\\xe5\\x85\\x81\\xe8\\xae\\xb8\\xef\\xbc\\x8cfalse=\\xe7\\xa6\\x81\\xe6\\xad\\xa2\\xef\\xbc\\x88\\xe6\\xaf\\x94\\xe5\\xa6\\x82\\xe8\\xa3\\x81\\xe5\\x88\\xa4\\xe5\\x8c\\xba\\xe7\\xad\\x89\\xef\\xbc\\x89\n"
  "bool fire_allowed\n"
  "\n"
  "# \\xe6\\x95\\x8c\\xe6\\x96\\xb9\\xe9\\x98\\xb5\\xe8\\x90\\xa5/\\xe9\\xa2\\x9c\\xe8\\x89\\xb2\\xef\\xbc\\x9a0=\\xe6\\x9c\\xaa\\xe7\\x9f\\xa5, 1=RED, 2=BLUE\n"
  "int8 enemy_team\n"
  "\n"
  "# \\xe4\\xb8\\x8a\\xe5\\xb1\\x82\\xe8\\xae\\xa4\\xe4\\xb8\\xba\\xe5\\xbd\\x93\\xe5\\x89\\x8d\\xe5\\xad\\x98\\xe5\\x9c\\xa8\\xe7\\x9a\\x84\\xe6\\x95\\x8c\\xe6\\x96\\xb9 ID \\xe5\\x88\\x97\\xe8\\xa1\\xa8\\xef\\xbc\\x88\\xe7\\x94\\xa8\\xe4\\xba\\x8e\\xe5\\x92\\x8c\\xe8\\xa7\\x86\\xe8\\xa7\\x89\\xe6\\xa3\\x80\\xe6\\xb5\\x8b\\xe7\\xbb\\x93\\xe6\\x9e\\x9c\\xe5\\xaf\\xb9\\xe6\\xaf\\x94\\xef\\xbc\\x89\n"
  "int8[] detected_enemy_ids";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
sp_msgs__msg__EnemyStatusMsg__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {sp_msgs__msg__EnemyStatusMsg__TYPE_NAME, 26, 26},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 420, 420},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
sp_msgs__msg__EnemyStatusMsg__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[2];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 2, 2};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *sp_msgs__msg__EnemyStatusMsg__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
