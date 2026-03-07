#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

#include "CompressedImage_generated.h"

namespace utils::foxglove::fb {

struct compressed_image_view final {
  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  std::string_view format{"jpeg"};
  const std::uint8_t* data{nullptr};
  std::size_t size{0};
};

[[nodiscard]] inline bool encode_compressed_image(
    const compressed_image_view& image,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(image.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: CompressedImage timestamp overflow";
    return false;
  }

  const auto frame = b.CreateString(image.frame_id.data(), image.frame_id.size());
  const auto format = b.CreateString(image.format.data(), image.format.size());
  const auto data = b.CreateVector(image.data, image.size);
  const auto root = ::foxglove::CreateCompressedImage(b, &stamp, frame, data, format);
  ::foxglove::FinishCompressedImageBuffer(b, root);

  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
