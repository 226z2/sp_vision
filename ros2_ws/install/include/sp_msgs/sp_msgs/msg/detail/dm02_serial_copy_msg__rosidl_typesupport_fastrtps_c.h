// generated from rosidl_typesupport_fastrtps_c/resource/idl__rosidl_typesupport_fastrtps_c.h.em
// with input from sp_msgs:msg/Dm02SerialCopyMsg.idl
// generated code does not contain a copyright notice
#ifndef SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
#define SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_


#include <stddef.h>
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_interface/macros.h"
#include "sp_msgs/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "sp_msgs/msg/detail/dm02_serial_copy_msg__struct.h"
#include "fastcdr/Cdr.h"

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
bool cdr_serialize_sp_msgs__msg__Dm02SerialCopyMsg(
  const sp_msgs__msg__Dm02SerialCopyMsg * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
bool cdr_deserialize_sp_msgs__msg__Dm02SerialCopyMsg(
  eprosima::fastcdr::Cdr &,
  sp_msgs__msg__Dm02SerialCopyMsg * ros_message);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
size_t get_serialized_size_sp_msgs__msg__Dm02SerialCopyMsg(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
size_t max_serialized_size_sp_msgs__msg__Dm02SerialCopyMsg(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
bool cdr_serialize_key_sp_msgs__msg__Dm02SerialCopyMsg(
  const sp_msgs__msg__Dm02SerialCopyMsg * ros_message,
  eprosima::fastcdr::Cdr & cdr);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
size_t get_serialized_size_key_sp_msgs__msg__Dm02SerialCopyMsg(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
size_t max_serialized_size_key_sp_msgs__msg__Dm02SerialCopyMsg(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_sp_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, sp_msgs, msg, Dm02SerialCopyMsg)();

#ifdef __cplusplus
}
#endif

#endif  // SP_MSGS__MSG__DETAIL__DM02_SERIAL_COPY_MSG__ROSIDL_TYPESUPPORT_FASTRTPS_C_H_
