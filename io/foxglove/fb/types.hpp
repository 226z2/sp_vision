#pragma once

#include <flatbuffers/flatbuffers.h>

#include "Quaternion_generated.h"
#include "Vector3_generated.h"

namespace utils::foxglove::fb {

struct vec3 final {
  double x{0.0};
  double y{0.0};
  double z{0.0};
};

struct quat final {
  double x{0.0};
  double y{0.0};
  double z{0.0};
  double w{1.0};
};

[[nodiscard]] inline flatbuffers::Offset<::foxglove::Vector3> fb_vec3(flatbuffers::FlatBufferBuilder& b, const vec3& v) {
  return ::foxglove::CreateVector3(b, v.x, v.y, v.z);
}

[[nodiscard]] inline flatbuffers::Offset<::foxglove::Quaternion> fb_quat(flatbuffers::FlatBufferBuilder& b, const quat& q) {
  return ::foxglove::CreateQuaternion(b, q.x, q.y, q.z, q.w);
}

}  // namespace utils::foxglove::fb
