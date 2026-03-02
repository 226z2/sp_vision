#ifndef IO__DM02_BOARD_HPP
#define IO__DM02_BOARD_HPP

#include <Eigen/Geometry>

#include <chrono>
#include <string>

#include "io/command_sender.hpp"
#include "io/command.hpp"
#include "io/control_types.hpp"
#include "io/dm02_link/dm02_link.hpp"

namespace io
{
/**
 * @brief DM-02/third_party/Communication 兼容层（替代 io::CBoard）
 * @details
 * - 提供与 io::CBoard 类似的字段/接口，便于批量迁移
 * - 底层使用 io::Dm02Link（串口 + uproto/MUX + timesync）
 */
class Dm02Board
  : public CommandSender
{
public:
  double bullet_speed{0.0};
  Mode mode{Mode::idle};
  ShootMode shoot_mode{ShootMode::both_shoot};
  double ft_angle{0.0};  // UAV 专用（当前 DM02Link 未接入，默认 0）

  explicit Dm02Board(const std::string & config_path);

  Eigen::Quaterniond imu_at(std::chrono::steady_clock::time_point timestamp);

  void send(Command command) override;

  // 需要用到 Dm02Link 额外能力时可直接访问
  Dm02Link & link() { return link_; }
  const Dm02Link & link() const { return link_; }

private:
  Dm02Link link_;

  void sync_cached_state();
  static Mode to_mode(GimbalMode mode);
};

}  // namespace io

#endif  // IO__DM02_BOARD_HPP
