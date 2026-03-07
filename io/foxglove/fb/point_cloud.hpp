#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "PointCloud_generated.h"

namespace utils::foxglove::fb {

struct point_cloud_view final {
  struct field_view final {
    std::string_view name{};
    std::uint32_t offset{0};
    enum class numeric_type : std::uint8_t {
      Unknown = 0,
      UInt8 = 1,
      Int8 = 2,
      UInt16 = 3,
      Int16 = 4,
      UInt32 = 5,
      Int32 = 6,
      Float32 = 7,
      Float64 = 8,
    };
    numeric_type type{numeric_type::Unknown};
  };

  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  std::uint32_t point_stride{0};
  std::span<const field_view> fields{};
  std::span<const std::uint8_t> data{};
};

[[nodiscard]] inline ::foxglove::NumericType to_numeric_type_(point_cloud_view::field_view::numeric_type type) {
  using Src = point_cloud_view::field_view::numeric_type;
  switch (type) {
    case Src::Unknown:
      return ::foxglove::NumericType::UNKNOWN;
    case Src::UInt8:
      return ::foxglove::NumericType::UINT8;
    case Src::Int8:
      return ::foxglove::NumericType::INT8;
    case Src::UInt16:
      return ::foxglove::NumericType::UINT16;
    case Src::Int16:
      return ::foxglove::NumericType::INT16;
    case Src::UInt32:
      return ::foxglove::NumericType::UINT32;
    case Src::Int32:
      return ::foxglove::NumericType::INT32;
    case Src::Float32:
      return ::foxglove::NumericType::FLOAT32;
    case Src::Float64:
      return ::foxglove::NumericType::FLOAT64;
  }
  return ::foxglove::NumericType::UNKNOWN;
}

[[nodiscard]] inline bool encode_point_cloud(
    const point_cloud_view& view,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(view.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: PointCloud timestamp overflow";
    return false;
  }

  const auto frame = b.CreateString(view.frame_id.data(), view.frame_id.size());
  std::vector<::flatbuffers::Offset<::foxglove::PackedElementField>> fields{};
  fields.reserve(view.fields.size());
  for (const auto& field : view.fields) {
    const auto name = b.CreateString(field.name.data(), field.name.size());
    fields.push_back(::foxglove::CreatePackedElementField(
        b,
        name,
        field.offset,
        to_numeric_type_(field.type)));
  }
  const auto fields_vec = b.CreateVector(fields);
  const auto bytes = b.CreateVector(view.data.data(), view.data.size());
  const auto root = ::foxglove::CreatePointCloud(
      b,
      &stamp,
      frame,
      0,
      view.point_stride,
      fields_vec,
      bytes);
  ::foxglove::FinishPointCloudBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
