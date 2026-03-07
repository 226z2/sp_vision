// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice
#include "sp_msgs/msg/detail/dm02_serial_copy_msg__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `timestamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
sp_msgs__msg__Dm02SerialCopyMsg__init(sp_msgs__msg__Dm02SerialCopyMsg * msg)
{
  if (!msg) {
    return false;
  }
  // timestamp
  if (!builtin_interfaces__msg__Time__init(&msg->timestamp)) {
    sp_msgs__msg__Dm02SerialCopyMsg__fini(msg);
    return false;
  }
  // established
  // have_state
  // send_ok
  // send_fail
  // yaw_rad
  // pitch_rad
  // roll_rad
  // enc_yaw
  // enc_pitch
  // yaw_vel_rad_s
  // pitch_vel_rad_s
  // bullet_speed_mps
  // bullet_count
  // gimbal_mode
  // shoot_state
  // shooter_heat
  // shooter_heat_limit
  // projectile_allowance_17mm
  // projectile_allowance_42mm
  // state_device_ts_us
  // state_host_ts_ns
  // referee_valid
  // referee_enemy_team
  // referee_fire_allowed
  // referee_robot_id
  // referee_game_stage
  // referee_status
  // referee_device_ts_us
  // referee_host_ts_ns
  // tof_valid
  // tof_distance_cm
  // tof_strength
  // tof_temp_cdeg
  // tof_status
  // tof_device_ts_us
  // tof_host_ts_ns
  // timesync_valid
  // timesync_offset_us
  // timesync_rtt_us
  // timesync_version
  // timesync_last_device_time_us
  // timesync_last_host_time_us
  return true;
}

void
sp_msgs__msg__Dm02SerialCopyMsg__fini(sp_msgs__msg__Dm02SerialCopyMsg * msg)
{
  if (!msg) {
    return;
  }
  // timestamp
  builtin_interfaces__msg__Time__fini(&msg->timestamp);
  // established
  // have_state
  // send_ok
  // send_fail
  // yaw_rad
  // pitch_rad
  // roll_rad
  // enc_yaw
  // enc_pitch
  // yaw_vel_rad_s
  // pitch_vel_rad_s
  // bullet_speed_mps
  // bullet_count
  // gimbal_mode
  // shoot_state
  // shooter_heat
  // shooter_heat_limit
  // projectile_allowance_17mm
  // projectile_allowance_42mm
  // state_device_ts_us
  // state_host_ts_ns
  // referee_valid
  // referee_enemy_team
  // referee_fire_allowed
  // referee_robot_id
  // referee_game_stage
  // referee_status
  // referee_device_ts_us
  // referee_host_ts_ns
  // tof_valid
  // tof_distance_cm
  // tof_strength
  // tof_temp_cdeg
  // tof_status
  // tof_device_ts_us
  // tof_host_ts_ns
  // timesync_valid
  // timesync_offset_us
  // timesync_rtt_us
  // timesync_version
  // timesync_last_device_time_us
  // timesync_last_host_time_us
}

bool
sp_msgs__msg__Dm02SerialCopyMsg__are_equal(const sp_msgs__msg__Dm02SerialCopyMsg * lhs, const sp_msgs__msg__Dm02SerialCopyMsg * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // timestamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->timestamp), &(rhs->timestamp)))
  {
    return false;
  }
  // established
  if (lhs->established != rhs->established) {
    return false;
  }
  // have_state
  if (lhs->have_state != rhs->have_state) {
    return false;
  }
  // send_ok
  if (lhs->send_ok != rhs->send_ok) {
    return false;
  }
  // send_fail
  if (lhs->send_fail != rhs->send_fail) {
    return false;
  }
  // yaw_rad
  if (lhs->yaw_rad != rhs->yaw_rad) {
    return false;
  }
  // pitch_rad
  if (lhs->pitch_rad != rhs->pitch_rad) {
    return false;
  }
  // roll_rad
  if (lhs->roll_rad != rhs->roll_rad) {
    return false;
  }
  // enc_yaw
  if (lhs->enc_yaw != rhs->enc_yaw) {
    return false;
  }
  // enc_pitch
  if (lhs->enc_pitch != rhs->enc_pitch) {
    return false;
  }
  // yaw_vel_rad_s
  if (lhs->yaw_vel_rad_s != rhs->yaw_vel_rad_s) {
    return false;
  }
  // pitch_vel_rad_s
  if (lhs->pitch_vel_rad_s != rhs->pitch_vel_rad_s) {
    return false;
  }
  // bullet_speed_mps
  if (lhs->bullet_speed_mps != rhs->bullet_speed_mps) {
    return false;
  }
  // bullet_count
  if (lhs->bullet_count != rhs->bullet_count) {
    return false;
  }
  // gimbal_mode
  if (lhs->gimbal_mode != rhs->gimbal_mode) {
    return false;
  }
  // shoot_state
  if (lhs->shoot_state != rhs->shoot_state) {
    return false;
  }
  // shooter_heat
  if (lhs->shooter_heat != rhs->shooter_heat) {
    return false;
  }
  // shooter_heat_limit
  if (lhs->shooter_heat_limit != rhs->shooter_heat_limit) {
    return false;
  }
  // projectile_allowance_17mm
  if (lhs->projectile_allowance_17mm != rhs->projectile_allowance_17mm) {
    return false;
  }
  // projectile_allowance_42mm
  if (lhs->projectile_allowance_42mm != rhs->projectile_allowance_42mm) {
    return false;
  }
  // state_device_ts_us
  if (lhs->state_device_ts_us != rhs->state_device_ts_us) {
    return false;
  }
  // state_host_ts_ns
  if (lhs->state_host_ts_ns != rhs->state_host_ts_ns) {
    return false;
  }
  // referee_valid
  if (lhs->referee_valid != rhs->referee_valid) {
    return false;
  }
  // referee_enemy_team
  if (lhs->referee_enemy_team != rhs->referee_enemy_team) {
    return false;
  }
  // referee_fire_allowed
  if (lhs->referee_fire_allowed != rhs->referee_fire_allowed) {
    return false;
  }
  // referee_robot_id
  if (lhs->referee_robot_id != rhs->referee_robot_id) {
    return false;
  }
  // referee_game_stage
  if (lhs->referee_game_stage != rhs->referee_game_stage) {
    return false;
  }
  // referee_status
  if (lhs->referee_status != rhs->referee_status) {
    return false;
  }
  // referee_device_ts_us
  if (lhs->referee_device_ts_us != rhs->referee_device_ts_us) {
    return false;
  }
  // referee_host_ts_ns
  if (lhs->referee_host_ts_ns != rhs->referee_host_ts_ns) {
    return false;
  }
  // tof_valid
  if (lhs->tof_valid != rhs->tof_valid) {
    return false;
  }
  // tof_distance_cm
  if (lhs->tof_distance_cm != rhs->tof_distance_cm) {
    return false;
  }
  // tof_strength
  if (lhs->tof_strength != rhs->tof_strength) {
    return false;
  }
  // tof_temp_cdeg
  if (lhs->tof_temp_cdeg != rhs->tof_temp_cdeg) {
    return false;
  }
  // tof_status
  if (lhs->tof_status != rhs->tof_status) {
    return false;
  }
  // tof_device_ts_us
  if (lhs->tof_device_ts_us != rhs->tof_device_ts_us) {
    return false;
  }
  // tof_host_ts_ns
  if (lhs->tof_host_ts_ns != rhs->tof_host_ts_ns) {
    return false;
  }
  // timesync_valid
  if (lhs->timesync_valid != rhs->timesync_valid) {
    return false;
  }
  // timesync_offset_us
  if (lhs->timesync_offset_us != rhs->timesync_offset_us) {
    return false;
  }
  // timesync_rtt_us
  if (lhs->timesync_rtt_us != rhs->timesync_rtt_us) {
    return false;
  }
  // timesync_version
  if (lhs->timesync_version != rhs->timesync_version) {
    return false;
  }
  // timesync_last_device_time_us
  if (lhs->timesync_last_device_time_us != rhs->timesync_last_device_time_us) {
    return false;
  }
  // timesync_last_host_time_us
  if (lhs->timesync_last_host_time_us != rhs->timesync_last_host_time_us) {
    return false;
  }
  return true;
}

bool
sp_msgs__msg__Dm02SerialCopyMsg__copy(
  const sp_msgs__msg__Dm02SerialCopyMsg * input,
  sp_msgs__msg__Dm02SerialCopyMsg * output)
{
  if (!input || !output) {
    return false;
  }
  // timestamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->timestamp), &(output->timestamp)))
  {
    return false;
  }
  // established
  output->established = input->established;
  // have_state
  output->have_state = input->have_state;
  // send_ok
  output->send_ok = input->send_ok;
  // send_fail
  output->send_fail = input->send_fail;
  // yaw_rad
  output->yaw_rad = input->yaw_rad;
  // pitch_rad
  output->pitch_rad = input->pitch_rad;
  // roll_rad
  output->roll_rad = input->roll_rad;
  // enc_yaw
  output->enc_yaw = input->enc_yaw;
  // enc_pitch
  output->enc_pitch = input->enc_pitch;
  // yaw_vel_rad_s
  output->yaw_vel_rad_s = input->yaw_vel_rad_s;
  // pitch_vel_rad_s
  output->pitch_vel_rad_s = input->pitch_vel_rad_s;
  // bullet_speed_mps
  output->bullet_speed_mps = input->bullet_speed_mps;
  // bullet_count
  output->bullet_count = input->bullet_count;
  // gimbal_mode
  output->gimbal_mode = input->gimbal_mode;
  // shoot_state
  output->shoot_state = input->shoot_state;
  // shooter_heat
  output->shooter_heat = input->shooter_heat;
  // shooter_heat_limit
  output->shooter_heat_limit = input->shooter_heat_limit;
  // projectile_allowance_17mm
  output->projectile_allowance_17mm = input->projectile_allowance_17mm;
  // projectile_allowance_42mm
  output->projectile_allowance_42mm = input->projectile_allowance_42mm;
  // state_device_ts_us
  output->state_device_ts_us = input->state_device_ts_us;
  // state_host_ts_ns
  output->state_host_ts_ns = input->state_host_ts_ns;
  // referee_valid
  output->referee_valid = input->referee_valid;
  // referee_enemy_team
  output->referee_enemy_team = input->referee_enemy_team;
  // referee_fire_allowed
  output->referee_fire_allowed = input->referee_fire_allowed;
  // referee_robot_id
  output->referee_robot_id = input->referee_robot_id;
  // referee_game_stage
  output->referee_game_stage = input->referee_game_stage;
  // referee_status
  output->referee_status = input->referee_status;
  // referee_device_ts_us
  output->referee_device_ts_us = input->referee_device_ts_us;
  // referee_host_ts_ns
  output->referee_host_ts_ns = input->referee_host_ts_ns;
  // tof_valid
  output->tof_valid = input->tof_valid;
  // tof_distance_cm
  output->tof_distance_cm = input->tof_distance_cm;
  // tof_strength
  output->tof_strength = input->tof_strength;
  // tof_temp_cdeg
  output->tof_temp_cdeg = input->tof_temp_cdeg;
  // tof_status
  output->tof_status = input->tof_status;
  // tof_device_ts_us
  output->tof_device_ts_us = input->tof_device_ts_us;
  // tof_host_ts_ns
  output->tof_host_ts_ns = input->tof_host_ts_ns;
  // timesync_valid
  output->timesync_valid = input->timesync_valid;
  // timesync_offset_us
  output->timesync_offset_us = input->timesync_offset_us;
  // timesync_rtt_us
  output->timesync_rtt_us = input->timesync_rtt_us;
  // timesync_version
  output->timesync_version = input->timesync_version;
  // timesync_last_device_time_us
  output->timesync_last_device_time_us = input->timesync_last_device_time_us;
  // timesync_last_host_time_us
  output->timesync_last_host_time_us = input->timesync_last_host_time_us;
  return true;
}

sp_msgs__msg__Dm02SerialCopyMsg *
sp_msgs__msg__Dm02SerialCopyMsg__create(void)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sp_msgs__msg__Dm02SerialCopyMsg * msg = (sp_msgs__msg__Dm02SerialCopyMsg *)allocator.allocate(sizeof(sp_msgs__msg__Dm02SerialCopyMsg), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(sp_msgs__msg__Dm02SerialCopyMsg));
  bool success = sp_msgs__msg__Dm02SerialCopyMsg__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
sp_msgs__msg__Dm02SerialCopyMsg__destroy(sp_msgs__msg__Dm02SerialCopyMsg * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    sp_msgs__msg__Dm02SerialCopyMsg__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
sp_msgs__msg__Dm02SerialCopyMsg__Sequence__init(sp_msgs__msg__Dm02SerialCopyMsg__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sp_msgs__msg__Dm02SerialCopyMsg * data = NULL;

  if (size) {
    data = (sp_msgs__msg__Dm02SerialCopyMsg *)allocator.zero_allocate(size, sizeof(sp_msgs__msg__Dm02SerialCopyMsg), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = sp_msgs__msg__Dm02SerialCopyMsg__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        sp_msgs__msg__Dm02SerialCopyMsg__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
sp_msgs__msg__Dm02SerialCopyMsg__Sequence__fini(sp_msgs__msg__Dm02SerialCopyMsg__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      sp_msgs__msg__Dm02SerialCopyMsg__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

sp_msgs__msg__Dm02SerialCopyMsg__Sequence *
sp_msgs__msg__Dm02SerialCopyMsg__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  sp_msgs__msg__Dm02SerialCopyMsg__Sequence * array = (sp_msgs__msg__Dm02SerialCopyMsg__Sequence *)allocator.allocate(sizeof(sp_msgs__msg__Dm02SerialCopyMsg__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = sp_msgs__msg__Dm02SerialCopyMsg__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
sp_msgs__msg__Dm02SerialCopyMsg__Sequence__destroy(sp_msgs__msg__Dm02SerialCopyMsg__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    sp_msgs__msg__Dm02SerialCopyMsg__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
sp_msgs__msg__Dm02SerialCopyMsg__Sequence__are_equal(const sp_msgs__msg__Dm02SerialCopyMsg__Sequence * lhs, const sp_msgs__msg__Dm02SerialCopyMsg__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!sp_msgs__msg__Dm02SerialCopyMsg__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
sp_msgs__msg__Dm02SerialCopyMsg__Sequence__copy(
  const sp_msgs__msg__Dm02SerialCopyMsg__Sequence * input,
  sp_msgs__msg__Dm02SerialCopyMsg__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(sp_msgs__msg__Dm02SerialCopyMsg);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    sp_msgs__msg__Dm02SerialCopyMsg * data =
      (sp_msgs__msg__Dm02SerialCopyMsg *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!sp_msgs__msg__Dm02SerialCopyMsg__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          sp_msgs__msg__Dm02SerialCopyMsg__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!sp_msgs__msg__Dm02SerialCopyMsg__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
