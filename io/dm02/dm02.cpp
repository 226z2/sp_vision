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

constexpr std::uint64_t kRefereeStaleNs = 1000ULL * 1000ULL * 1000ULL;

bool referee_can_fire(const RefereeStatus & rs)
{
  if (!(rs.valid && rs.fire_allowed)) return false;
  if (rs.host_ts_ns == 0) return false;
  const auto now_ns = static_cast<std::uint64_t>(
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now().time_since_epoch())
      .count());
  if (now_ns < rs.host_ts_ns) return false;
  return (now_ns - rs.host_ts_ns) <= kRefereeStaleNs;
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
  send(
    command.control, command.shoot, static_cast<float>(command.yaw), 0.0F, 0.0F,
    static_cast<float>(command.pitch), 0.0F, 0.0F);
}

void Dm02::send(
  bool control, bool fire, float yaw, float yaw_vel, float yaw_acc, float pitch,
  float pitch_vel, float pitch_acc)
{
  sync_cached_state();

  // Keep delta command semantics: bit0 indicates whether this frame has valid host control.
  impl_->link->send(control, control, yaw, yaw_vel, yaw_acc, pitch, pitch_vel, pitch_acc);

  const auto ref = impl_->link->referee();
  const bool fire_on = control && fire && referee_can_fire(ref);
  impl_->link->send_fire(fire_on, 2, fire_on ? 2 : 0, control ? 0x0001u : 0u);

  // Keep chassis command path alive with explicit zero command.
  impl_->link->send_chassis(0, 0, 0, 3, 0u);
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

RefereeStatus Dm02::referee() const
{
  return impl_->link->referee();
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

void Dm02::send_fire(
  bool fire_on, std::int32_t fire_mode, std::int32_t burst_count, std::uint16_t status)
{
  impl_->link->send_fire(fire_on, fire_mode, burst_count, status);
}

void Dm02::send_chassis(
  std::int32_t vx_mm_s, std::int32_t vy_mm_s, std::int32_t wz_mdeg_s, std::int32_t mode,
  std::uint16_t status)
{
  impl_->link->send_chassis(vx_mm_s, vy_mm_s, wz_mdeg_s, mode, status);
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
