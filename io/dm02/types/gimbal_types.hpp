#ifndef IO__DM02__TYPES__GIMBAL_TYPES_HPP
#define IO__DM02__TYPES__GIMBAL_TYPES_HPP

#include <cstdint>

namespace io
{

enum class GimbalMode
{
  IDLE,
  AUTO_AIM
};

struct GimbalState
{
  float yaw{0.0F};
  float yaw_vel{0.0F};
  float pitch{0.0F};
  float pitch_vel{0.0F};
  float bullet_speed{0.0F};
  std::uint16_t bullet_count{0};
};

}  // namespace io

#endif  // IO__DM02__TYPES__GIMBAL_TYPES_HPP
