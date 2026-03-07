#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include "SceneUpdate_generated.h"

namespace utils::foxglove::fb {

struct scene_color_view final {
  double r{1.0};
  double g{1.0};
  double b{1.0};
  double a{1.0};
};

struct point3_view final {
  double x{0.0};
  double y{0.0};
  double z{0.0};
};

struct quat_view final {
  double x{0.0};
  double y{0.0};
  double z{0.0};
  double w{1.0};
};

struct pose_view final {
  point3_view position{};
  quat_view orientation{};
};

enum class line_type : std::uint8_t {
  LineStrip = 0,
  LineLoop = 1,
  LineList = 2,
};

struct line_primitive_view final {
  line_type type{line_type::LineStrip};
  pose_view pose{};
  double thickness{0.0};
  bool scale_invariant{false};
  std::vector<point3_view> points{};
  std::optional<scene_color_view> color{};
  std::vector<scene_color_view> colors{};
  std::vector<std::uint32_t> indices{};
};

struct text_primitive_view final {
  pose_view pose{};
  bool billboard{false};
  double font_size{12.0};
  bool scale_invariant{false};
  scene_color_view color{};
  std::string_view text{};
};

struct cube_primitive_view final {
  pose_view pose{};
  point3_view size{1.0, 1.0, 1.0};
  scene_color_view color{};
};

struct scene_entity_view final {
  std::uint64_t stamp_ns{0};
  std::string_view frame_id{};
  std::string_view id{};
  std::uint64_t lifetime_ns{0};
  bool frame_locked{false};
  std::vector<line_primitive_view> lines{};
  std::vector<text_primitive_view> texts{};
  std::vector<cube_primitive_view> cubes{};
};

enum class scene_deletion_type : std::uint8_t {
  MatchingId = 0,
  All = 1,
};

struct scene_deletion_view final {
  std::uint64_t stamp_ns{0};
  scene_deletion_type type{scene_deletion_type::MatchingId};
  std::string_view id{};
};

struct scene_update_view final {
  std::span<const scene_deletion_view> deletions{};
  std::span<const scene_entity_view> entities{};
};

[[nodiscard]] inline ::flatbuffers::Offset<::foxglove::Color> create_scene_color_(
    ::flatbuffers::FlatBufferBuilder& b,
    const scene_color_view& color) {
  return ::foxglove::CreateColor(b, color.r, color.g, color.b, color.a);
}

[[nodiscard]] inline ::flatbuffers::Offset<::foxglove::Point3> create_point3_(
    ::flatbuffers::FlatBufferBuilder& b,
    const point3_view& point) {
  return ::foxglove::CreatePoint3(b, point.x, point.y, point.z);
}

[[nodiscard]] inline ::flatbuffers::Offset<::foxglove::Pose> create_pose_(
    ::flatbuffers::FlatBufferBuilder& b,
    const pose_view& pose) {
  const auto pos = ::foxglove::CreateVector3(b, pose.position.x, pose.position.y, pose.position.z);
  const auto rot = ::foxglove::CreateQuaternion(b, pose.orientation.x, pose.orientation.y, pose.orientation.z, pose.orientation.w);
  return ::foxglove::CreatePose(b, pos, rot);
}

[[nodiscard]] inline ::foxglove::LineType line_type_(line_type type) {
  switch (type) {
    case line_type::LineStrip:
      return ::foxglove::LineType::LINE_STRIP;
    case line_type::LineLoop:
      return ::foxglove::LineType::LINE_LOOP;
    case line_type::LineList:
      return ::foxglove::LineType::LINE_LIST;
  }
  return ::foxglove::LineType::LINE_STRIP;
}

[[nodiscard]] inline bool encode_scene_update(
    const scene_update_view& view,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  std::vector<::flatbuffers::Offset<::foxglove::SceneEntityDeletion>> deletions{};
  deletions.reserve(view.deletions.size());
  for (const auto& item : view.deletions) {
    ::foxglove::Time stamp{};
    if (!time_from_ns(item.stamp_ns, stamp)) {
      if (error) *error = "utils.foxglove.fb: SceneUpdate deletion timestamp overflow";
      return false;
    }
    const auto id = b.CreateString(item.id.data(), item.id.size());
    const auto type = item.type == scene_deletion_type::All
        ? ::foxglove::SceneEntityDeletionType::ALL
        : ::foxglove::SceneEntityDeletionType::MATCHING_ID;
    deletions.push_back(::foxglove::CreateSceneEntityDeletion(b, &stamp, type, id));
  }

  std::vector<::flatbuffers::Offset<::foxglove::SceneEntity>> entities{};
  entities.reserve(view.entities.size());
  constexpr std::uint64_t kNsPerSec = 1000000000ULL;
  for (const auto& item : view.entities) {
    ::foxglove::Time stamp{};
    if (!time_from_ns(item.stamp_ns, stamp)) {
      if (error) *error = "utils.foxglove.fb: SceneUpdate entity timestamp overflow";
      return false;
    }

    const auto sec = item.lifetime_ns / kNsPerSec;
    const auto nsec = item.lifetime_ns % kNsPerSec;
    if (sec > static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max())) {
      if (error) *error = "utils.foxglove.fb: SceneUpdate lifetime overflow";
      return false;
    }
    const ::foxglove::Duration lifetime{
        static_cast<std::int32_t>(sec),
        static_cast<std::int32_t>(nsec)};

    std::vector<::flatbuffers::Offset<::foxglove::LinePrimitive>> lines{};
    lines.reserve(item.lines.size());
    for (const auto& line : item.lines) {
      std::vector<::flatbuffers::Offset<::foxglove::Point3>> points{};
      points.reserve(line.points.size());
      for (const auto& point : line.points) {
        points.push_back(create_point3_(b, point));
      }

      std::vector<::flatbuffers::Offset<::foxglove::Color>> colors{};
      colors.reserve(line.colors.size());
      for (const auto& color : line.colors) {
        colors.push_back(create_scene_color_(b, color));
      }

      ::flatbuffers::Offset<::foxglove::Color> color{};
      if (line.color.has_value()) {
        color = create_scene_color_(b, *line.color);
      }

      const auto pose = create_pose_(b, line.pose);
      const auto points_vec = b.CreateVector(points);
      const auto colors_vec = b.CreateVector(colors);
      const auto indices_vec = b.CreateVector(line.indices);
      lines.push_back(::foxglove::CreateLinePrimitive(
          b,
          line_type_(line.type),
          pose,
          line.thickness,
          line.scale_invariant,
          points_vec,
          color,
          colors_vec,
          indices_vec));
    }

    std::vector<::flatbuffers::Offset<::foxglove::TextPrimitive>> texts{};
    texts.reserve(item.texts.size());
    for (const auto& text : item.texts) {
      const auto pose = create_pose_(b, text.pose);
      const auto color = create_scene_color_(b, text.color);
      const auto text_str = b.CreateString(text.text.data(), text.text.size());
      texts.push_back(::foxglove::CreateTextPrimitive(
          b,
          pose,
          text.billboard,
          text.font_size,
          text.scale_invariant,
          color,
          text_str));
    }

    std::vector<::flatbuffers::Offset<::foxglove::CubePrimitive>> cubes{};
    cubes.reserve(item.cubes.size());
    for (const auto& cube : item.cubes) {
      const auto pose = create_pose_(b, cube.pose);
      const auto size = ::foxglove::CreateVector3(b, cube.size.x, cube.size.y, cube.size.z);
      const auto color = create_scene_color_(b, cube.color);
      cubes.push_back(::foxglove::CreateCubePrimitive(b, pose, size, color));
    }

    const auto frame_id = b.CreateString(item.frame_id.data(), item.frame_id.size());
    const auto id = b.CreateString(item.id.data(), item.id.size());
    const auto lines_vec = b.CreateVector(lines);
    const auto texts_vec = b.CreateVector(texts);
    const auto cubes_vec = b.CreateVector(cubes);
    entities.push_back(::foxglove::CreateSceneEntity(
        b,
        &stamp,
        frame_id,
        id,
        &lifetime,
        item.frame_locked,
        0,
        0,
        cubes_vec,
        0,
        0,
        lines_vec,
        0,
        texts_vec,
        0));
  }

  const auto deletions_vec = b.CreateVector(deletions);
  const auto entities_vec = b.CreateVector(entities);
  const auto root = ::foxglove::CreateSceneUpdate(b, deletions_vec, entities_vec);
  ::foxglove::FinishSceneUpdateBuffer(b, root);
  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
