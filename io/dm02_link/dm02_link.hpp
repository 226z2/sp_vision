#ifndef IO__DM02_LINK__DM02_LINK_HPP
#define IO__DM02_LINK__DM02_LINK_HPP

#include <Eigen/Geometry>

#include <cstdint>
#include <optional>
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>

#include "io/gimbal_types.hpp"
#include "tools/thread_safe_queue.hpp"

namespace io
{

struct GimbalEncoders
{
  std::int32_t yaw{0};
  std::int32_t pitch{0};
};

struct DeviceStatus
{
  // Mirrors DM-02 GIMBAL_SID_STATE extension words (device -> host).
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

/**
 * @brief DM-02 单链路云台+IMU封装
 * @details
 * - 通过 third_party/Communication 的 DM-02 协议栈接入电控
 * - 从 GIMBAL_SID_STATE 提取 yaw/pitch/roll/gyro，并生成四元数队列用于时间插值
 * - 对外尽量提供与旧 io::Gimbal 接近的接口，便于迁移
 */
class Dm02Link
{
public:
  explicit Dm02Link(const std::string & config_path);
  ~Dm02Link();

  GimbalMode mode() const;
  GimbalState state() const;
  GimbalEncoders encoders() const;
  DeviceStatus device_status() const;
  ToFStatus tof() const;
  TimeSyncStatus timesync() const;

  // 使用 timesync 的 host-device 偏移将 device_ts_us 映射到 host 时间轴（微秒）
  // 若 timesync 无效则返回 std::nullopt。
  std::optional<std::uint64_t> device_us_to_host_us(std::uint64_t device_ts_us) const;
  std::string str(GimbalMode mode) const;

  // 与旧接口保持一致：按时间戳获取插值四元数
  Eigen::Quaterniond q(std::chrono::steady_clock::time_point t);

  // 兼容旧 DM_IMU 的命名
  Eigen::Quaterniond imu_at(std::chrono::steady_clock::time_point t) { return q(t); }

  // 与旧接口保持一致：发送控制（当前实现会转成“相对角度 delta”命令）
  void send(
    bool control, bool target_valid, float yaw, float yaw_vel, float yaw_acc, float pitch,
    float pitch_vel, float pitch_acc);

private:
  void io_thread();
  bool open();
  void close();
  void reconnect();

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

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

}  // namespace io

#endif  // IO__DM02_LINK__DM02_LINK_HPP
