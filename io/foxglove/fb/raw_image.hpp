#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "RawImage_generated.h"

namespace utils::foxglove::fb {

struct raw_image_view final {
  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  std::uint32_t width{0};
  std::uint32_t height{0};
  std::uint32_t step{0};
  std::string_view encoding{"rgb8"};
  std::span<const std::uint8_t> data{};
};

[[nodiscard]] inline bool encode_raw_image(
    const raw_image_view& image,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(image.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: RawImage timestamp overflow";
    return false;
  }

  const auto frame = b.CreateString(image.frame_id.data(), image.frame_id.size());
  const auto encoding = b.CreateString(image.encoding.data(), image.encoding.size());
  const auto data = b.CreateVector(image.data.data(), image.data.size());
  const auto root = ::foxglove::CreateRawImage(
      b,
      &stamp,
      frame,
      image.width,
      image.height,
      encoding,
      image.step,
      data);
  ::foxglove::FinishRawImageBuffer(b, root);

  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
