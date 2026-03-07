#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace communication::dm_02
{

struct GimbalState
{
  std::int32_t enc_yaw{0};
  std::int32_t enc_pitch{0};
  std::int32_t yaw_udeg{0};
  std::int32_t pitch_udeg{0};
  std::int32_t roll_udeg{0};

  std::int32_t yaw_cmd_current{0};
  std::int32_t pitch_cmd_current{0};
  std::int32_t yaw_meas_current{0};
  std::int32_t pitch_meas_current{0};
  std::int32_t gyro_yaw_udeps{0};
  std::int32_t gyro_pitch_udeps{0};

  std::int32_t bullet_speed_x100{0};
  std::int32_t bullet_count{0};
  std::int32_t gimbal_mode{0};
  std::int32_t shoot_state{0};
  std::int32_t shooter_heat{0};
  std::int32_t shooter_heat_limit{0};
  std::int32_t projectile_allowance_17mm{0};
  std::int32_t projectile_allowance_42mm{0};

  std::uint64_t device_ts_us{0};
  std::uint64_t host_ts_ns{0};
};

struct GimbalDelta
{
  std::int32_t delta_yaw_udeg{0};
  std::int32_t delta_pitch_udeg{0};
  std::uint16_t status{0};
  std::uint64_t host_ts_ns{0};
  std::uint64_t device_ts_us{0};
};

struct GimbalTfmini
{
  std::uint16_t distance_cm{0};
  std::uint16_t strength{0};
  std::int16_t temp_cdeg{0};
  std::uint16_t status{0};
  std::uint64_t device_ts_us{0};
  std::uint64_t host_ts_ns{0};
};

struct CameraEvent
{
  std::uint32_t frame_id{0};
  std::uint64_t device_ts_us{0};
  std::uint64_t host_ts_ns{0};
};

struct RefereeStatus
{
  std::int32_t enemy_team{0};
  std::int32_t fire_allowed{0};
  std::int32_t robot_id{0};
  std::int32_t game_stage{0};
  std::uint16_t status{0};
  std::uint64_t device_ts_us{0};
  std::uint64_t host_ts_ns{0};
};

struct FireCommand
{
  std::int32_t fire_on{0};
  std::int32_t fire_mode{0};
  std::int32_t burst_count{0};
  std::uint16_t status{0};
  std::uint64_t host_ts_ns{0};
  std::uint64_t device_ts_us{0};
};

struct ChassisCommand
{
  std::int32_t vx_mm_s{0};
  std::int32_t vy_mm_s{0};
  std::int32_t wz_mdeg_s{0};
  std::int32_t mode{0};
  std::uint16_t status{0};
  std::uint64_t host_ts_ns{0};
  std::uint64_t device_ts_us{0};
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

class Transport
{
public:
  virtual ~Transport() = default;

  virtual bool open(const std::string & endpoint) = 0;
  virtual void close() = 0;
  virtual bool write_all(const std::uint8_t * data, std::size_t len) = 0;
  virtual long read_some(std::uint8_t * data, std::size_t cap) = 0;
  virtual int wait_readable(int timeout_ms) = 0;
  virtual int fd() const = 0;
  virtual std::uint16_t mtu() const = 0;
};

struct Config
{
  std::uint32_t handshake_timeout_ms{1000};
  std::uint32_t heartbeat_interval_ms{0};
  std::uint32_t default_timeout_ms{3000};
  std::uint8_t default_retries{1};
  bool auto_handshake{true};

  bool timesync_enable{true};
  std::uint32_t timesync_period_ms{1000};
  std::uint32_t timesync_max_rtt_us{0};
  bool timesync_initiator{true};

  bool gimbal_has_encoders{true};
  bool gimbal_has_imu{true};
};

struct Callbacks
{
  std::function<void(const GimbalState &)> on_gimbal_state{};
  std::function<void(const GimbalTfmini &)> on_gimbal_tfmini{};
  std::function<void(const CameraEvent &)> on_camera_event{};
  std::function<void(const RefereeStatus &)> on_referee_status{};
  std::function<void()> on_gimbal_delta_ack{};
  std::function<void(const TimeSyncStatus &)> on_timesync{};
  std::function<void(std::uint8_t, std::uint16_t, const std::uint8_t *, std::uint32_t)> on_mux_raw{};
  std::function<void(std::string_view)> on_uproto_event{};
};

class Driver
{
public:
  explicit Driver(std::unique_ptr<Transport> transport, Config cfg = {});
  ~Driver();

  Driver(const Driver &) = delete;
  Driver & operator=(const Driver &) = delete;
  Driver(Driver &&) noexcept;
  Driver & operator=(Driver &&) noexcept;

  bool open(const std::string & endpoint);
  void close();
  bool is_open() const;
  bool established() const;

  void set_callbacks(Callbacks cb);

  void poll(int timeout_ms);
  void tick();
  void step(int timeout_ms);

  bool send_gimbal_delta(const GimbalDelta & cmd);
  bool send_camera_reset();
  bool send_referee_query();
  bool send_fire_command(const FireCommand & cmd);
  bool send_chassis_command(const ChassisCommand & cmd);

  bool send_mux(std::uint8_t ch, std::uint16_t sid, const std::uint8_t * payload, std::uint16_t len);

  std::optional<GimbalState> last_gimbal_state() const;
  std::optional<GimbalTfmini> last_gimbal_tfmini() const;
  std::optional<CameraEvent> last_camera_event() const;
  std::optional<RefereeStatus> last_referee_status() const;

  TimeSyncStatus time_sync_status() const;
  std::optional<std::uint64_t> host_ns_to_device_us(std::uint64_t host_ns) const;
  std::optional<std::uint64_t> device_us_to_host_ns(std::uint64_t device_us) const;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace communication::dm_02
