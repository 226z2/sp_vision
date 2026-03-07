#pragma once

#include <cstdint>
#include <limits>

#include "Time_generated.h"

namespace utils::foxglove::fb {

[[nodiscard]] inline bool time_from_ns(std::uint64_t ns, ::foxglove::Time& out) {
  constexpr std::uint64_t kNsPerSec = 1000000000ULL;
  const auto sec = ns / kNsPerSec;
  const auto nsec = ns % kNsPerSec;
  if (sec > static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max())) {
    return false;
  }
  out = ::foxglove::Time{
      static_cast<std::uint32_t>(sec),
      static_cast<std::uint32_t>(nsec)};
  return true;
}

}  // namespace utils::foxglove::fb
