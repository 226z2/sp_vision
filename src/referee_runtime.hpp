#ifndef SRC__REFEREE_RUNTIME_HPP
#define SRC__REFEREE_RUNTIME_HPP

#include <chrono>
#include <cstdint>
#include <optional>

#include "io/dm02/dm02.hpp"
#include "tasks/auto_aim/armor.hpp"

namespace referee_runtime
{
constexpr std::uint64_t kRefereeStaleNs = 1000ULL * 1000ULL * 1000ULL;

struct RefereeView
{
  bool valid{false};
  bool fire_allowed{false};
  std::int32_t enemy_team{0};
  std::uint64_t host_ts_ns{0};
};

inline std::uint64_t now_ns_steady()
{
  return static_cast<std::uint64_t>(
    std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now().time_since_epoch())
      .count());
}

inline bool is_fresh(const RefereeView & ref, std::uint64_t stale_ns = kRefereeStaleNs)
{
  if (ref.host_ts_ns == 0) return false;
  const std::uint64_t now_ns = now_ns_steady();
  if (now_ns < ref.host_ts_ns) return false;
  return (now_ns - ref.host_ts_ns) <= stale_ns;
}

inline bool can_fire(const RefereeView & ref, std::uint64_t stale_ns = kRefereeStaleNs)
{
  return ref.valid && is_fresh(ref, stale_ns) && ref.fire_allowed;
}

inline std::optional<auto_aim::Color> enemy_color(const RefereeView & ref)
{
  if (!(ref.valid && is_fresh(ref))) return std::nullopt;
  if (ref.enemy_team == 1) return auto_aim::Color::red;
  if (ref.enemy_team == 2) return auto_aim::Color::blue;
  return std::nullopt;
}

inline RefereeView from_io(const io::RefereeStatus & rs)
{
  RefereeView out{};
  out.valid = rs.valid;
  out.fire_allowed = rs.fire_allowed;
  out.enemy_team = rs.enemy_team;
  out.host_ts_ns = rs.host_ts_ns;
  return out;
}

}  // namespace referee_runtime

#endif  // SRC__REFEREE_RUNTIME_HPP
