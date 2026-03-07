#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"
#include "io/foxglove/fb/types.hpp"

#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "FrameTransforms_generated.h"

namespace utils::foxglove::fb {

struct frame_transform final {
  std::string parent_frame_id{"map"};
  std::string child_frame_id{"base_link"};
  std::uint64_t stamp_ns{0};
  vec3 translation{};
  quat rotation{};
};

[[nodiscard]] inline bool encode_frame_transforms(
    std::span<const frame_transform> transforms,
    std::uint64_t fallback_stamp_ns,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();

  std::vector<flatbuffers::Offset<::foxglove::FrameTransform>> encoded{};
  encoded.reserve(transforms.size());

  for (const auto& tf : transforms) {
    if (tf.parent_frame_id.empty() || tf.child_frame_id.empty() || tf.parent_frame_id == tf.child_frame_id) {
      if (error) *error = "utils.foxglove.fb: invalid FrameTransform parent/child";
      return false;
    }

    ::foxglove::Time stamp{};
    const auto stamp_ns = tf.stamp_ns == 0 ? fallback_stamp_ns : tf.stamp_ns;
    if (!time_from_ns(stamp_ns, stamp)) {
      if (error) *error = "utils.foxglove.fb: FrameTransform timestamp overflow";
      return false;
    }

    const auto parent = b.CreateString(tf.parent_frame_id);
    const auto child = b.CreateString(tf.child_frame_id);
    const auto translation = fb_vec3(b, tf.translation);
    const auto rotation = fb_quat(b, tf.rotation);
    encoded.push_back(::foxglove::CreateFrameTransform(
        b,
        &stamp,
        parent,
        child,
        translation,
        rotation));
  }

  const auto vec = b.CreateVector(encoded);
  const auto root = ::foxglove::CreateFrameTransforms(b, vec);
  ::foxglove::FinishFrameTransformsBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
