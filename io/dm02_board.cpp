#include "dm02_board.hpp"

#include "tools/yaml.hpp"

namespace io
{
namespace
{
ShootMode parse_shoot_mode(const std::string & s)
{
  if (s == "left_shoot") return ShootMode::left_shoot;
  if (s == "right_shoot") return ShootMode::right_shoot;
  if (s == "both_shoot") return ShootMode::both_shoot;
  return ShootMode::both_shoot;
}
}  // namespace

Dm02Board::Dm02Board(const std::string & config_path) : link_(config_path)
{
  // shoot_mode / outpost 等高级模式目前没有从设备侧接入，先允许通过 yaml 固定配置
  try {
    auto yaml = tools::load(config_path);
    if (yaml["dm02_shoot_mode"]) {
      shoot_mode = parse_shoot_mode(yaml["dm02_shoot_mode"].as<std::string>());
    }
  } catch (...) {
  }

  sync_cached_state();
}

Eigen::Quaterniond Dm02Board::imu_at(std::chrono::steady_clock::time_point timestamp)
{
  sync_cached_state();
  return link_.q(timestamp);
}

void Dm02Board::send(Command command)
{
  sync_cached_state();

  // 兼容旧 CBoard：持续发送云台 DELTA（无目标时发送 0 增量并清 target_valid）
  link_.send(
    command.control, command.control, static_cast<float>(command.yaw), 0.0F, 0.0F,
    static_cast<float>(command.pitch), 0.0F, 0.0F);
}

void Dm02Board::sync_cached_state()
{
  const auto gs = link_.state();
  bullet_speed = gs.bullet_speed;
  mode = to_mode(link_.mode());
}

Mode Dm02Board::to_mode(GimbalMode mode)
{
  switch (mode) {
    case GimbalMode::IDLE:
      return Mode::idle;
    case GimbalMode::AUTO_AIM:
      return Mode::auto_aim;
    default:
      return Mode::auto_aim;
  }
}

}  // namespace io
