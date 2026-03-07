#pragma once

#include <array>
#include <string_view>

namespace utils::foxglove::schema {

struct SchemaEntry final {
  std::string_view schema_name{};
  std::string_view root_file{};
};

inline constexpr std::array<SchemaEntry, 10> kDefaultSchemas{{
    {"foxglove.FrameTransforms", "FrameTransforms.bfbs"},
    {"foxglove.CompressedImage", "CompressedImage.bfbs"},
    {"foxglove.RawImage", "RawImage.bfbs"},
    {"foxglove.ImageAnnotations", "ImageAnnotations.bfbs"},
    {"foxglove.SceneUpdate", "SceneUpdate.bfbs"},
    {"foxglove.Log", "Log.bfbs"},
    {"foxglove.CameraCalibration", "CameraCalibration.bfbs"},
    {"foxglove.LaserScan", "LaserScan.bfbs"},
    {"foxglove.LocationFix", "LocationFix.bfbs"},
    {"foxglove.PointCloud", "PointCloud.bfbs"},
}};

[[nodiscard]] inline std::string_view default_bfbs_name(std::string_view schema_name) {
  for (const auto& entry : kDefaultSchemas) {
    if (entry.schema_name == schema_name) return entry.root_file;
  }
  return {};
}

}  // namespace utils::foxglove::schema
