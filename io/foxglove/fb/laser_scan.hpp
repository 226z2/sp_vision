#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

#include "LaserScan_generated.h"

namespace utils::foxglove::fb {

struct laser_scan_view final {
  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  double start_angle{0.0};
  double end_angle{0.0};
  std::vector<double> ranges{};
  std::vector<double> intensities{};
};

[[nodiscard]] inline bool encode_laser_scan(
    const laser_scan_view& view,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(view.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: LaserScan timestamp overflow";
    return false;
  }

  const auto frame = b.CreateString(view.frame_id.data(), view.frame_id.size());
  const auto ranges = b.CreateVector(view.ranges);
  const auto intensities = b.CreateVector(view.intensities);
  const auto root = ::foxglove::CreateLaserScan(
      b,
      &stamp,
      frame,
      0,
      view.start_angle,
      view.end_angle,
      ranges,
      intensities);
  ::foxglove::FinishLaserScanBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
