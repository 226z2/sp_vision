#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "CameraCalibration_generated.h"

namespace utils::foxglove::fb {

struct camera_calibration_view final {
  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  std::uint32_t width{0};
  std::uint32_t height{0};
  std::string_view distortion_model{};
  std::span<const double> d{};
  std::span<const double> k{};
  std::span<const double> r{};
  std::span<const double> p{};
};

[[nodiscard]] inline bool encode_camera_calibration(
    const camera_calibration_view& view,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(view.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: CameraCalibration timestamp overflow";
    return false;
  }

  const auto frame = b.CreateString(view.frame_id.data(), view.frame_id.size());
  const auto distortion_model = b.CreateString(view.distortion_model.data(), view.distortion_model.size());
  const auto d = b.CreateVector(view.d.data(), view.d.size());
  const auto k = b.CreateVector(view.k.data(), view.k.size());
  const auto r = b.CreateVector(view.r.data(), view.r.size());
  const auto p = b.CreateVector(view.p.data(), view.p.size());
  const auto root = ::foxglove::CreateCameraCalibration(
      b,
      &stamp,
      frame,
      view.width,
      view.height,
      distortion_model,
      d,
      k,
      r,
      p);
  ::foxglove::FinishCameraCalibrationBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
