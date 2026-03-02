#include "dm02_link.hpp"

#include <cmath>
#include <utility>

#include <cstdint>
#include <optional>
#include "tools/logger.hpp"
#include "tools/math_tools.hpp"
#include "tools/yaml.hpp"

#include "modules/communication/drivers/dm_02/dm_02.hpp"
#include "modules/communication/drivers/dm_02/serial_transport_posix.hpp"

namespace io
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

}  // namespace

struct Dm02Link::Impl
{
  communication::dm_02::Driver driver;
  std::string endpoint;

  explicit Impl(std::string ep)
  : driver(
      std::make_unique<communication::dm_02::SerialTransportPosix>(),
      communication::dm_02::Config{})
  , endpoint(std::move(ep))
  {
  }
};

Dm02Link::Dm02Link(const std::string & config_path)
{
  auto yaml = tools::load(config_path);

  // endpoint 优先级：dm02_endpoint > (com_port + dm02_baud)
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

  impl_ = std::make_unique<Impl>(endpoint);

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

    auto t = steady_from_ns(s.host_ts_ns);
    queue_.push({q, t});

    std::lock_guard<std::mutex> lock(mutex_);
    state_.yaw = static_cast<float>(yaw_rad);
    state_.pitch = static_cast<float>(pitch_rad);
    state_.yaw_vel = static_cast<float>(static_cast<double>(s.gyro_yaw_udeps) * 1e-6 * kDeg2Rad);
    state_.pitch_vel =
      static_cast<float>(static_cast<double>(s.gyro_pitch_udeps) * 1e-6 * kDeg2Rad);
    // 设置子弹速度：如果设备报告的速度有效，则使用；否则使用默认值
    if (s.bullet_speed_x100 > 0) {
      state_.bullet_speed = static_cast<float>(static_cast<double>(s.bullet_speed_x100) / 100.0);
    } else {
      state_.bullet_speed = bullet_speed_default_;
    }
    
    // 设置子弹计数：如果设备报告的计数有效，则使用；否则使用原子变量中缓存的值
    if (s.bullet_count >= 0) {
      state_.bullet_count = static_cast<std::uint16_t>(static_cast<std::uint32_t>(s.bullet_count));
    } else {
      state_.bullet_count = bullet_count_.load();
    }

    encoders_.yaw = s.enc_yaw;
    encoders_.pitch = s.enc_pitch;

    // 设置设备状态：射击状态、热量、热量限制、弹药数量等
    device_status_.shoot_state = s.shoot_state;
    device_status_.shooter_heat = s.shooter_heat;
    device_status_.shooter_heat_limit = s.shooter_heat_limit;
    device_status_.projectile_allowance_17mm = s.projectile_allowance_17mm;
    device_status_.projectile_allowance_42mm = s.projectile_allowance_42mm;

    // 可选的云台模式反馈 (0=IDLE, 1=AUTO_AIM, 2=SMALL_BUFF, 3=BIG_BUFF)
    if (s.gimbal_mode >= 0 && s.gimbal_mode <= 3) {
      mode_ = static_cast<GimbalMode>(s.gimbal_mode);
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
    tools::logger()->error("[Dm02Link] Failed to open endpoint: {}", impl_->endpoint);
    exit(1);
  }

  thread_ = std::thread(&Dm02Link::io_thread, this);

  queue_.pop();
  tools::logger()->info("[Dm02Link] First state received.");
}

Dm02Link::~Dm02Link()
{
  quit_ = true;
  if (thread_.joinable()) thread_.join();
  close();
}

bool Dm02Link::open()
{
  if (!impl_) return false;
  const bool ok = impl_->driver.open(impl_->endpoint);
  if (ok) {
    tools::logger()->info("[Dm02Link] Opened {}", impl_->endpoint);
  }
  return ok;
}

void Dm02Link::close()
{
  if (!impl_) return;
  impl_->driver.close();
}

void Dm02Link::reconnect()
{
  int max_retry = 10;
  for (int i = 0; i < max_retry && !quit_; ++i) {
    tools::logger()->warn("[Dm02Link] Reconnecting, attempt {}/{}...", i + 1, max_retry);
    try {
      close();
    } catch (...) {
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    if (open()) {
      queue_.clear();
      tools::logger()->info("[Dm02Link] Reconnected.");
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void Dm02Link::io_thread()
{
  tools::logger()->info("[Dm02Link] io_thread started.");
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
      error_count++;
      if (error_count > 2000) {
        error_count = 0;
        reconnect();
      }
    }
  }

  tools::logger()->info("[Dm02Link] io_thread stopped.");
}

GimbalMode Dm02Link::mode() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return mode_;
}

GimbalState Dm02Link::state() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return state_;
}

GimbalEncoders Dm02Link::encoders() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return encoders_;
}

/**
 * @brief 获取设备状态
 * @details 线程安全地获取当前DM02链路设备的状态。使用互斥锁保护对共享状态变量的访问。
 * @return DeviceStatus 设备的当前状态
 * @note 此函数是常量成员函数，不会修改对象的状态。
 * @thread_safe 此函数是线程安全的，使用互斥锁同步访问。
 */
DeviceStatus Dm02Link::device_status() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return device_status_;
}

ToFStatus Dm02Link::tof() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return tof_;
}

TimeSyncStatus Dm02Link::timesync() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return timesync_;
}

std::optional<std::uint64_t> Dm02Link::device_us_to_host_us(std::uint64_t device_ts_us) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (!timesync_.valid) return std::nullopt;
  // 约定：offset_us = host_us - device_us
  return static_cast<std::uint64_t>(static_cast<std::int64_t>(device_ts_us) + timesync_.offset_us);
}

std::string Dm02Link::str(GimbalMode mode) const
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

Eigen::Quaterniond Dm02Link::q(std::chrono::steady_clock::time_point t)
{
  while (true) {
    auto [q_a, t_a] = queue_.pop();
    auto [q_b, t_b] = queue_.front();
    auto t_ab = tools::delta_time(t_a, t_b);
    auto t_ac = tools::delta_time(t_a, t);
    auto k = t_ac / t_ab;
    Eigen::Quaterniond q_c = q_a.slerp(k, q_b).normalized();
    if (t < t_a) return q_c;
    if (!(t_a < t && t <= t_b)) continue;

    return q_c;
  }
}

/**
 * @brief 发送云台增量控制命令到DM02设备
 * 
 * 该函数计算当前云台位置与目标位置之间的增量，并将其打包成
 * GimbalDelta命令发送给DM02设备驱动。支持偏航和俯仰两个轴的控制。
 * 
 * @param control 是否启用控制，为true时计算增量，为false时增量为0
 * @param target_valid 目标是否有效标志位，将设置到命令状态字的bit0
 * @param yaw 目标偏航角，单位为弧度
 * @param yaw_vel 偏航角速度（未使用）
 * @param yaw_acc 偏航角加速度（未使用）
 * @param pitch 目标俯仰角，单位为弧度
 * @param pitch_vel 俯仰角速度（未使用）
 * @param pitch_acc 俯仰角加速度（未使用）
 * 
 * @note 
 * - 如果驱动未初始化或设备未打开，函数直接返回
 * - 增量计算公式：增量角度 = 目标角度 - 当前角度
 * - 最终发送的增量单位为微度（udeg，1e-6度）
 * - 使用mutex保护对当前云台状态的访问
 * - 命令中包含主机时间戳（steady_clock纳秒精度）
 * 
 * @see GimbalDelta, kRad2Deg
 */
void Dm02Link::send(
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
  // status bit0: target_valid (host has a valid target/command)
  cmd.status = target_valid ? 0x0001u : 0u;
  cmd.host_ts_ns = now_ns_steady();

  (void)impl_->driver.send_gimbal_delta(cmd);
}

}  // namespace io
