#ifndef IO__DM02__TYPES__CONTROL_TYPES_HPP
#define IO__DM02__TYPES__CONTROL_TYPES_HPP

#include <array>
#include <string_view>

namespace io
{
enum Mode
{
  idle,
  auto_aim,
  outpost
};

inline constexpr std::array<std::string_view, 3> MODES = {"idle", "auto_aim", "outpost"};

enum ShootMode
{
  left_shoot,
  right_shoot,
  both_shoot
};

inline constexpr std::array<std::string_view, 3> SHOOT_MODES = {
  "left_shoot", "right_shoot", "both_shoot"};

}  // namespace io

#endif  // IO__DM02__TYPES__CONTROL_TYPES_HPP
