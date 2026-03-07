#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

#include "LocationFix_generated.h"

namespace utils::foxglove::fb {

struct location_fix_view final {
  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  double latitude{0.0};
  double longitude{0.0};
  double altitude{0.0};
  std::array<double, 9> covariance{};
  ::foxglove::PositionCovarianceType covariance_type{::foxglove::PositionCovarianceType::UNKNOWN};
};

[[nodiscard]] inline bool encode_location_fix(
    const location_fix_view& view,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(view.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: LocationFix timestamp overflow";
    return false;
  }

  const auto frame = b.CreateString(view.frame_id.data(), view.frame_id.size());
  const auto cov = b.CreateVector(view.covariance.data(), view.covariance.size());
  const auto root = ::foxglove::CreateLocationFix(
      b,
      &stamp,
      frame,
      view.latitude,
      view.longitude,
      view.altitude,
      cov,
      view.covariance_type);
  ::foxglove::FinishLocationFixBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
