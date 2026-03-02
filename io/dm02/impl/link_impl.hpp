#ifndef IO__DM02__IMPL__LINK_IMPL_HPP
#define IO__DM02__IMPL__LINK_IMPL_HPP

#include <Eigen/Geometry>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <tuple>

#include "io/dm02/dm02.hpp"
#include "tools/thread_safe_queue.hpp"

namespace io::dm02::impl
{

class LinkImpl
{
public:
  explicit LinkImpl(const std::string & config_path);
  ~LinkImpl();

  LinkImpl(const LinkImpl &) = delete;
  LinkImpl & operator=(const LinkImpl &) = delete;

  GimbalMode mode() const;
  GimbalState state() const;
  GimbalEncoders encoders() const;
  DeviceStatus device_status() const;
  ToFStatus tof() const;
  TimeSyncStatus timesync() const;

  std::optional<std::uint64_t> device_us_to_host_us(std::uint64_t device_ts_us) const;
  std::string str(GimbalMode mode) const;

  Eigen::Quaterniond q(std::chrono::steady_clock::time_point t);
  Eigen::Quaterniond imu_at(std::chrono::steady_clock::time_point t) { return q(t); }

  void send(
    bool control, bool target_valid, float yaw, float yaw_vel, float yaw_acc, float pitch,
    float pitch_vel, float pitch_acc);

private:
  void io_thread();
  bool open();
  void close();
  void reconnect();

private:
  struct DriverImpl;
  std::unique_ptr<DriverImpl> impl_;

  std::thread thread_;
  std::atomic<bool> quit_{false};

  mutable std::mutex mutex_;
  GimbalMode mode_{GimbalMode::AUTO_AIM};
  GimbalState state_{};
  GimbalEncoders encoders_{};
  DeviceStatus device_status_{};
  ToFStatus tof_{};
  TimeSyncStatus timesync_{};

  std::atomic<uint16_t> bullet_count_{0};
  float bullet_speed_default_{0.0F};

  tools::ThreadSafeQueue<std::tuple<Eigen::Quaterniond, std::chrono::steady_clock::time_point>>
    queue_{1000};
};

}  // namespace io::dm02::impl

#endif  // IO__DM02__IMPL__LINK_IMPL_HPP
