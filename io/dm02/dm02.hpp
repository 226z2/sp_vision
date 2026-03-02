#ifndef IO__DM02__DM02_HPP
#define IO__DM02__DM02_HPP

#include <Eigen/Geometry>

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "io/command.hpp"
#include "io/dm02/types/control_types.hpp"
#include "io/dm02/types/gimbal_types.hpp"

namespace io
{

struct GimbalEncoders
{
  std::int32_t yaw{0};
  std::int32_t pitch{0};
};

struct DeviceStatus
{
  std::int32_t shoot_state{0};
  std::int32_t shooter_heat{0};
  std::int32_t shooter_heat_limit{0};
  std::int32_t projectile_allowance_17mm{0};
  std::int32_t projectile_allowance_42mm{0};
};

struct TimeSyncStatus
{
  bool valid{false};
  std::int64_t offset_us{0};
  std::uint32_t rtt_us{0};
  std::uint32_t version{0};
  std::uint64_t last_device_time_us{0};
  std::uint64_t last_host_time_us{0};
};

struct ToFStatus
{
  bool valid{false};
  std::uint16_t distance_cm{0};
  std::uint64_t device_ts_us{0};
  std::uint64_t host_ts_ns{0};
};

class Dm02
{
public:
  double bullet_speed{0.0};
  Mode mode{Mode::idle};
  ShootMode shoot_mode{ShootMode::both_shoot};
  double ft_angle{0.0};

  explicit Dm02(const std::string & config_path);
  ~Dm02();

  Dm02(const Dm02 &) = delete;
  Dm02 & operator=(const Dm02 &) = delete;
  Dm02(Dm02 &&) noexcept;
  Dm02 & operator=(Dm02 &&) noexcept;

  Eigen::Quaterniond imu_at(std::chrono::steady_clock::time_point timestamp);
  Eigen::Quaterniond q(std::chrono::steady_clock::time_point timestamp);

  void send(Command command);
  void send(
    bool control, bool target_valid, float yaw, float yaw_vel, float yaw_acc, float pitch,
    float pitch_vel, float pitch_acc);

  GimbalMode gimbal_mode() const;
  GimbalState state() const;
  GimbalEncoders encoders() const;
  DeviceStatus device_status() const;
  ToFStatus tof() const;
  TimeSyncStatus timesync() const;
  std::optional<std::uint64_t> device_us_to_host_us(std::uint64_t device_ts_us) const;
  std::string str(GimbalMode mode) const;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  void sync_cached_state();
  static Mode to_mode(GimbalMode mode);
};

}  // namespace io

#endif  // IO__DM02__DM02_HPP
