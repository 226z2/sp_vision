#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "ImageAnnotations_generated.h"

namespace utils::foxglove::fb {

struct color_view final {
  double r{1.0};
  double g{1.0};
  double b{1.0};
  double a{1.0};
};

struct point2_view final {
  double x{0.0};
  double y{0.0};
};

enum class points_annotation_kind : std::uint8_t {
  Unknown = 0,
  Points = 1,
  LineLoop = 2,
  LineStrip = 3,
  LineList = 4,
};

struct circle_annotation_view final {
  std::uint64_t stamp_ns{0};
  point2_view center{};
  double diameter{0.0};
  double thickness{0.0};
  color_view fill_color{};
  color_view outline_color{};
};

struct points_annotation_view final {
  std::uint64_t stamp_ns{0};
  points_annotation_kind type{points_annotation_kind::Unknown};
  std::span<const point2_view> points{};
  color_view outline_color{};
  std::span<const color_view> outline_colors{};
  color_view fill_color{};
  double thickness{0.0};
};

struct text_annotation_view final {
  std::uint64_t stamp_ns{0};
  point2_view position{};
  std::string_view text{};
  double font_size{12.0};
  color_view text_color{};
  color_view background_color{};
};

struct image_annotations_view final {
  std::span<const circle_annotation_view> circles{};
  std::span<const points_annotation_view> points{};
  std::span<const text_annotation_view> texts{};
};

[[nodiscard]] inline ::flatbuffers::Offset<::foxglove::Color> create_color_(
    ::flatbuffers::FlatBufferBuilder& b,
    const color_view& value) {
  return ::foxglove::CreateColor(b, value.r, value.g, value.b, value.a);
}

[[nodiscard]] inline ::flatbuffers::Offset<::foxglove::Point2> create_point2_(
    ::flatbuffers::FlatBufferBuilder& b,
    const point2_view& value) {
  return ::foxglove::CreatePoint2(b, value.x, value.y);
}

[[nodiscard]] inline ::foxglove::PointsAnnotationType points_type_(points_annotation_kind kind) {
  switch (kind) {
    case points_annotation_kind::Unknown:
      return ::foxglove::PointsAnnotationType::UNKNOWN;
    case points_annotation_kind::Points:
      return ::foxglove::PointsAnnotationType::POINTS;
    case points_annotation_kind::LineLoop:
      return ::foxglove::PointsAnnotationType::LINE_LOOP;
    case points_annotation_kind::LineStrip:
      return ::foxglove::PointsAnnotationType::LINE_STRIP;
    case points_annotation_kind::LineList:
      return ::foxglove::PointsAnnotationType::LINE_LIST;
  }
  return ::foxglove::PointsAnnotationType::UNKNOWN;
}

[[nodiscard]] inline bool encode_image_annotations(
    const image_annotations_view& view,
    std::vector<std::uint8_t>& bytes_out) {
  auto& b = tls_builder();
  std::vector<::flatbuffers::Offset<::foxglove::CircleAnnotation>> circles{};
  circles.reserve(view.circles.size());
  for (const auto& item : view.circles) {
    ::foxglove::Time stamp{};
    if (!time_from_ns(item.stamp_ns, stamp)) return false;
    circles.push_back(::foxglove::CreateCircleAnnotation(
        b,
        &stamp,
        create_point2_(b, item.center),
        item.diameter,
        item.thickness,
        create_color_(b, item.fill_color),
        create_color_(b, item.outline_color)));
  }

  std::vector<::flatbuffers::Offset<::foxglove::PointsAnnotation>> points{};
  points.reserve(view.points.size());
  for (const auto& item : view.points) {
    ::foxglove::Time stamp{};
    if (!time_from_ns(item.stamp_ns, stamp)) return false;

    std::vector<::flatbuffers::Offset<::foxglove::Point2>> point_values{};
    point_values.reserve(item.points.size());
    for (const auto& point : item.points) {
      point_values.push_back(create_point2_(b, point));
    }

    std::vector<::flatbuffers::Offset<::foxglove::Color>> outline_colors{};
    outline_colors.reserve(item.outline_colors.size());
    for (const auto& color : item.outline_colors) {
      outline_colors.push_back(create_color_(b, color));
    }

    points.push_back(::foxglove::CreatePointsAnnotation(
        b,
        &stamp,
        points_type_(item.type),
        b.CreateVector(point_values),
        create_color_(b, item.outline_color),
        b.CreateVector(outline_colors),
        create_color_(b, item.fill_color),
        item.thickness));
  }

  std::vector<::flatbuffers::Offset<::foxglove::TextAnnotation>> texts{};
  texts.reserve(view.texts.size());
  for (const auto& item : view.texts) {
    ::foxglove::Time stamp{};
    if (!time_from_ns(item.stamp_ns, stamp)) return false;
    const auto text = b.CreateString(item.text.data(), item.text.size());
    texts.push_back(::foxglove::CreateTextAnnotation(
        b,
        &stamp,
        create_point2_(b, item.position),
        text,
        item.font_size,
        create_color_(b, item.text_color),
        create_color_(b, item.background_color)));
  }

  const auto root = ::foxglove::CreateImageAnnotations(
      b,
      b.CreateVector(circles),
      b.CreateVector(points),
      b.CreateVector(texts));
  ::foxglove::FinishImageAnnotationsBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
