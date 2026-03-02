#include "io/dm02/dm02.hpp"

#include <memory>
#include <utility>

#include "io/dm02/impl/link_impl.hpp"
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

struct Dm02::Impl
{
  explicit Impl(const std::string & config_path)
  : link(std::make_unique<io::dm02::impl::LinkImpl>(config_path))
  {
  }

  std::unique_ptr<io::dm02::impl::LinkImpl> link;
};

Dm02::Dm02(const std::string & config_path) : impl_(std::make_unique<Impl>(config_path))
{
  try {
    auto yaml = tools::load(config_path);
    if (yaml["dm02_shoot_mode"]) {
      shoot_mode = parse_shoot_mode(yaml["dm02_shoot_mode"].as<std::string>());
    }
  } catch (...) {
  }

  sync_cached_state();
}

Dm02::~Dm02() = default;
Dm02::Dm02(Dm02 &&) noexcept = default;
Dm02 & Dm02::operator=(Dm02 &&) noexcept = default;

Eigen::Quaterniond Dm02::imu_at(std::chrono::steady_clock::time_point timestamp)
{
  sync_cached_state();
  return impl_->link->imu_at(timestamp);
}

Eigen::Quaterniond Dm02::q(std::chrono::steady_clock::time_point timestamp)
{
  sync_cached_state();
  return impl_->link->q(timestamp);
}

void Dm02::send(Command command)
{
  sync_cached_state();

  impl_->link->send(
    command.control, command.control, static_cast<float>(command.yaw), 0.0F, 0.0F,
    static_cast<float>(command.pitch), 0.0F, 0.0F);
}

void Dm02::send(
  bool control, bool target_valid, float yaw, float yaw_vel, float yaw_acc, float pitch,
  float pitch_vel, float pitch_acc)
{
  impl_->link->send(control, target_valid, yaw, yaw_vel, yaw_acc, pitch, pitch_vel, pitch_acc);
}

GimbalMode Dm02::gimbal_mode() const
{
  return impl_->link->mode();
}

GimbalState Dm02::state() const
{
  return impl_->link->state();
}

GimbalEncoders Dm02::encoders() const
{
  return impl_->link->encoders();
}

DeviceStatus Dm02::device_status() const
{
  return impl_->link->device_status();
}

ToFStatus Dm02::tof() const
{
  return impl_->link->tof();
}

TimeSyncStatus Dm02::timesync() const
{
  return impl_->link->timesync();
}

std::optional<std::uint64_t> Dm02::device_us_to_host_us(std::uint64_t device_ts_us) const
{
  return impl_->link->device_us_to_host_us(device_ts_us);
}

std::string Dm02::str(GimbalMode mode) const
{
  return impl_->link->str(mode);
}

void Dm02::sync_cached_state()
{
  const auto gs = impl_->link->state();
  bullet_speed = gs.bullet_speed;
  mode = to_mode(impl_->link->mode());
}

Mode Dm02::to_mode(GimbalMode mode)
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
