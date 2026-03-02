#include "io/dm02/impl/link_impl.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <utility>

#include "serial/serial.h"

#include "modules/communication/drivers/dm_02/dm_02.hpp"
#include "tools/logger.hpp"
#include "tools/math_tools.hpp"
#include "tools/yaml.hpp"

namespace io::dm02::impl
{
namespace
{
constexpr double kDeg2Rad = M_PI / 180.0;
constexpr double kRad2Deg = 180.0 / M_PI;

inline std::chrono::steady_clock::time_point steady_from_ns(std::uint64_t ns)
{
  return std::chrono::steady_clock::time_point(std::chrono::nanoseconds(ns));
}

inline std::uint64_t now_ns_steady()
{
  const auto now = std::chrono::steady_clock::now();
  return static_cast<std::uint64_t>(
    std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
}

inline GimbalMode parse_mode(const std::string & s)
{
  if (s == "IDLE" || s == "idle") return GimbalMode::IDLE;
  if (s == "AUTO_AIM" || s == "auto_aim") return GimbalMode::AUTO_AIM;
  return GimbalMode::AUTO_AIM;
}

class IoSerialTransport final : public communication::dm_02::Transport
{
public:
  IoSerialTransport() = default;
  ~IoSerialTransport() override { close(); }

  bool open(const std::string & endpoint) override
  {
    close();

    std::string port;
    int baud = 115200;
    if (!parse_endpoint(endpoint, port, baud)) return false;

    try {
      auto timeout = serial::Timeout::simpleTimeout(5);
      serial_ = std::make_unique<serial::Serial>(
        port, static_cast<uint32_t>(baud), timeout, serial::eightbits, serial::parity_none,
        serial::stopbits_one, serial::flowcontrol_none);
      if (!serial_->isOpen()) serial_->open();
      return serial_->isOpen();
    } catch (...) {
      serial_.reset();
      return false;
    }
  }

  void close() override
  {
    if (!serial_) return;
    try {
      if (serial_->isOpen()) serial_->close();
    } catch (...) {
    }
    serial_.reset();
  }

  bool write_all(const std::uint8_t * data, std::size_t len) override
  {
    if (!serial_ || !serial_->isOpen()) return false;
    if (!data && len > 0) return false;
    try {
      const auto n = serial_->write(data, len);
      return n == len;
    } catch (...) {
      return false;
    }
  }

  long read_some(std::uint8_t * data, std::size_t cap) override
  {
    if (!serial_ || !serial_->isOpen()) return -1;
    if (!data || cap == 0) return 0;
    try {
      const auto avail = serial_->available();
      if (avail == 0) return 0;
      const auto n = serial_->read(data, std::min(cap, avail));
      return static_cast<long>(n);
    } catch (...) {
      return -1;
    }
  }

  int wait_readable(int timeout_ms) override
  {
    if (!serial_ || !serial_->isOpen()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(std::max(timeout_ms, 0)));
      return 0;
    }

    if (timeout_ms <= 0) {
      return serial_->available() > 0 ? 1 : 0;
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline) {
      if (serial_->available() > 0) return 1;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return serial_->available() > 0 ? 1 : 0;
  }

  int fd() const override { return -1; }

  std::uint16_t mtu() const override { return 1024; }

private:
  static bool parse_endpoint(const std::string & endpoint, std::string & port, int & baud)
  {
    std::string target = endpoint;
    if (endpoint.rfind("serial:", 0) == 0) {
      const auto query_pos = endpoint.find('?');
      target = endpoint.substr(7, query_pos == std::string::npos ? std::string::npos : query_pos - 7);
    }

    if (target.empty()) return false;
    port = target;

    const auto baud_pos = endpoint.find("baud=");
    if (baud_pos != std::string::npos) {
      const auto * ptr = endpoint.c_str() + baud_pos + 5;
      const long parsed = std::strtol(ptr, nullptr, 10);
      if (parsed > 0) baud = static_cast<int>(parsed);
    }
    return true;
  }

  std::unique_ptr<serial::Serial> serial_;
};

}  // namespace

struct LinkImpl::DriverImpl
{
  communication::dm_02::Driver driver;
  std::string endpoint;

  explicit DriverImpl(std::string ep)
  : driver(std::make_unique<IoSerialTransport>(), communication::dm_02::Config{})
  , endpoint(std::move(ep))
  {
  }
};

LinkImpl::LinkImpl(const std::string & config_path)
{
  auto yaml = tools::load(config_path);

  std::string endpoint;
  try {
    endpoint = tools::read<std::string>(yaml, "dm02_endpoint");
  } catch (...) {
    const auto com_port = tools::read<std::string>(yaml, "com_port");
    int baud = 115200;
    try {
      baud = tools::read<int>(yaml, "dm02_baud");
    } catch (...) {
    }
    endpoint = "serial:" + com_port + "?baud=" + std::to_string(baud);
  }

  try {
    bullet_speed_default_ = tools::read<float>(yaml, "dm02_bullet_speed_default");
  } catch (...) {
    bullet_speed_default_ = 0.0F;
  }

  try {
    mode_ = parse_mode(tools::read<std::string>(yaml, "dm02_mode"));
  } catch (...) {
    mode_ = GimbalMode::AUTO_AIM;
  }

  impl_ = std::make_unique<DriverImpl>(endpoint);

  communication::dm_02::Callbacks cb;
  cb.on_gimbal_state = [this](const communication::dm_02::GimbalState & s) {
    const double yaw_rad = static_cast<double>(s.yaw_udeg) * 1e-6 * kDeg2Rad;
    const double pitch_rad = static_cast<double>(s.pitch_udeg) * 1e-6 * kDeg2Rad;
    const double roll_rad = static_cast<double>(s.roll_udeg) * 1e-6 * kDeg2Rad;

    Eigen::Quaterniond q =
      Eigen::AngleAxisd(yaw_rad, Eigen::Vector3d::UnitZ()) *
      Eigen::AngleAxisd(pitch_rad, Eigen::Vector3d::UnitY()) *
      Eigen::AngleAxisd(roll_rad, Eigen::Vector3d::UnitX());
    q.normalize();

    const auto t = steady_from_ns(s.host_ts_ns);
    queue_.push({q, t});

    std::lock_guard<std::mutex> lock(mutex_);
    state_.yaw = static_cast<float>(yaw_rad);
    state_.pitch = static_cast<float>(pitch_rad);
    state_.yaw_vel = static_cast<float>(static_cast<double>(s.gyro_yaw_udeps) * 1e-6 * kDeg2Rad);
    state_.pitch_vel =
      static_cast<float>(static_cast<double>(s.gyro_pitch_udeps) * 1e-6 * kDeg2Rad);

    if (s.bullet_speed_x100 > 0) {
      state_.bullet_speed = static_cast<float>(static_cast<double>(s.bullet_speed_x100) / 100.0);
    } else {
      state_.bullet_speed = bullet_speed_default_;
    }

    if (s.bullet_count >= 0) {
      state_.bullet_count = static_cast<std::uint16_t>(static_cast<std::uint32_t>(s.bullet_count));
      bullet_count_.store(state_.bullet_count);
    } else {
      state_.bullet_count = bullet_count_.load();
    }

    encoders_.yaw = s.enc_yaw;
    encoders_.pitch = s.enc_pitch;

    device_status_.shoot_state = s.shoot_state;
    device_status_.shooter_heat = s.shooter_heat;
    device_status_.shooter_heat_limit = s.shooter_heat_limit;
    device_status_.projectile_allowance_17mm = s.projectile_allowance_17mm;
    device_status_.projectile_allowance_42mm = s.projectile_allowance_42mm;

    if (s.gimbal_mode == 0) {
      mode_ = GimbalMode::IDLE;
    } else if (s.gimbal_mode >= 1 && s.gimbal_mode <= 3) {
      mode_ = GimbalMode::AUTO_AIM;
    }
  };

  cb.on_gimbal_tfmini = [this](const communication::dm_02::GimbalTfmini & tf) {
    std::lock_guard<std::mutex> lock(mutex_);
    tof_.valid = true;
    tof_.distance_cm = tf.distance_cm;
    tof_.device_ts_us = tf.device_ts_us;
    tof_.host_ts_ns = tf.host_ts_ns;
  };

  cb.on_timesync = [this](const communication::dm_02::TimeSyncStatus & ts) {
    std::lock_guard<std::mutex> lock(mutex_);
    timesync_.valid = ts.valid;
    timesync_.offset_us = ts.offset_us;
    timesync_.rtt_us = ts.rtt_us;
    timesync_.version = ts.version;
    timesync_.last_device_time_us = ts.last_device_time_us;
    timesync_.last_host_time_us = ts.last_host_time_us;
  };

  impl_->driver.set_callbacks(std::move(cb));

  if (!open()) {
    tools::logger()->error("[Dm02] Failed to open endpoint: {}", impl_->endpoint);
    std::exit(1);
  }

  thread_ = std::thread(&LinkImpl::io_thread, this);
  queue_.pop();
  tools::logger()->info("[Dm02] First state received.");
}

LinkImpl::~LinkImpl()
{
  quit_ = true;
  if (thread_.joinable()) thread_.join();
  close();
}

bool LinkImpl::open()
{
  if (!impl_) return false;
  const bool ok = impl_->driver.open(impl_->endpoint);
  if (ok) tools::logger()->info("[Dm02] Opened {}", impl_->endpoint);
  return ok;
}

void LinkImpl::close()
{
  if (!impl_) return;
  impl_->driver.close();
}

void LinkImpl::reconnect()
{
  constexpr int kMaxRetry = 10;
  for (int i = 0; i < kMaxRetry && !quit_; ++i) {
    tools::logger()->warn("[Dm02] Reconnecting, attempt {}/{}...", i + 1, kMaxRetry);
    try {
      close();
    } catch (...) {
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (open()) {
      queue_.clear();
      tools::logger()->info("[Dm02] Reconnected.");
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void LinkImpl::io_thread()
{
  tools::logger()->info("[Dm02] io_thread started.");
  int error_count = 0;

  while (!quit_) {
    if (!impl_ || !impl_->driver.is_open()) {
      reconnect();
      continue;
    }

    try {
      impl_->driver.step(5);
      error_count = 0;
    } catch (...) {
      ++error_count;
      if (error_count > 2000) {
        error_count = 0;
        reconnect();
      }
    }
  }

  tools::logger()->info("[Dm02] io_thread stopped.");
}

GimbalMode LinkImpl::mode() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return mode_;
}

GimbalState LinkImpl::state() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return state_;
}

GimbalEncoders LinkImpl::encoders() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return encoders_;
}

DeviceStatus LinkImpl::device_status() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return device_status_;
}

ToFStatus LinkImpl::tof() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return tof_;
}

TimeSyncStatus LinkImpl::timesync() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return timesync_;
}

std::optional<std::uint64_t> LinkImpl::device_us_to_host_us(std::uint64_t device_ts_us) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (!timesync_.valid) return std::nullopt;
  return static_cast<std::uint64_t>(static_cast<std::int64_t>(device_ts_us) + timesync_.offset_us);
}

std::string LinkImpl::str(GimbalMode mode) const
{
  switch (mode) {
    case GimbalMode::IDLE:
      return "IDLE";
    case GimbalMode::AUTO_AIM:
      return "AUTO_AIM";
    default:
      return "INVALID";
  }
}

Eigen::Quaterniond LinkImpl::q(std::chrono::steady_clock::time_point t)
{
  while (true) {
    auto [q_a, t_a] = queue_.pop();
    auto [q_b, t_b] = queue_.front();
    const auto t_ab = tools::delta_time(t_a, t_b);
    const auto t_ac = tools::delta_time(t_a, t);
    const auto k = t_ac / t_ab;
    const auto q_c = q_a.slerp(k, q_b).normalized();
    if (t < t_a) return q_c;
    if (!(t_a < t && t <= t_b)) continue;
    return q_c;
  }
}

void LinkImpl::send(
  bool control, bool target_valid, float yaw, float /*yaw_vel*/, float /*yaw_acc*/, float pitch,
  float /*pitch_vel*/, float /*pitch_acc*/)
{
  if (!impl_ || !impl_->driver.is_open()) return;

  float cur_yaw = 0.0F;
  float cur_pitch = 0.0F;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    cur_yaw = state_.yaw;
    cur_pitch = state_.pitch;
  }

  double dyaw_rad = 0.0;
  double dpitch_rad = 0.0;
  if (control) {
    dyaw_rad = static_cast<double>(yaw - cur_yaw);
    dpitch_rad = static_cast<double>(pitch - cur_pitch);
  }

  communication::dm_02::GimbalDelta cmd;
  cmd.delta_yaw_udeg = static_cast<std::int32_t>(dyaw_rad * kRad2Deg * 1e6);
  cmd.delta_pitch_udeg = static_cast<std::int32_t>(dpitch_rad * kRad2Deg * 1e6);
  cmd.status = target_valid ? 0x0001u : 0u;
  cmd.host_ts_ns = now_ns_steady();

  (void)impl_->driver.send_gimbal_delta(cmd);
}

}  // namespace io::dm02::impl
