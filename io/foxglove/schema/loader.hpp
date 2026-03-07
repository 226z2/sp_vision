#pragma once

#include "io/foxglove/schema/catalog.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <foxglove/websocket/base64.hpp>

namespace utils::foxglove::schema {

[[nodiscard]] inline std::string_view base_name(std::string_view schema_name) {
  if (schema_name.empty()) return schema_name;
  const auto pos = schema_name.rfind('.');
  if (pos == std::string_view::npos) return schema_name;
  return schema_name.substr(pos + 1);
}

[[nodiscard]] inline std::filesystem::path bfbs_filename(std::string_view schema_name) {
  if (const auto mapped = default_bfbs_name(schema_name); !mapped.empty()) {
    return std::filesystem::path(std::string(mapped));
  }
  return std::filesystem::path(std::string(base_name(schema_name)) + ".bfbs");
}

[[nodiscard]] inline std::optional<std::filesystem::path> resolve_exe_path() {
#if defined(__linux__)
  std::error_code ec{};
  auto exe = std::filesystem::read_symlink("/proc/self/exe", ec);
  if (!ec && !exe.empty()) return exe;
#endif
  return std::nullopt;
}

[[nodiscard]] inline std::vector<std::filesystem::path> discover_default_bfbs_dirs() {
  std::vector<std::filesystem::path> out{};
  out.reserve(10);

  auto add_dir = [&out](const std::filesystem::path& path) {
    std::error_code ec{};
    if (std::filesystem::is_directory(path, ec)) out.push_back(path);
  };

  if (const char* env = std::getenv("PRINTK_VISION_FOXGLOVE_SCHEMAS_DIR")) {
    add_dir(std::filesystem::path(env));
  }

  add_dir("generated/foxglove");
  add_dir("build/generated/foxglove");
  add_dir("../generated/foxglove");
  add_dir("../build/generated/foxglove");

  std::error_code ec{};
  const auto cwd = std::filesystem::current_path(ec);
  if (!ec) {
    add_dir(cwd / "generated" / "foxglove");
    add_dir(cwd / "build" / "generated" / "foxglove");
    add_dir(cwd / "printk_vision" / "generated" / "foxglove");
    add_dir(cwd / "printk_vision_docker" / "generated" / "foxglove");
  }

  if (const auto exe = resolve_exe_path(); exe.has_value()) {
    const auto bin = exe->parent_path();
    add_dir(bin / "generated" / "foxglove");
    add_dir(bin / ".." / "generated" / "foxglove");
    add_dir(bin / ".." / ".." / "generated" / "foxglove");
  }

  if (out.empty()) {
    out.push_back("generated/foxglove");
    out.push_back("build/generated/foxglove");
  }
  return out;
}

[[nodiscard]] inline const std::vector<std::filesystem::path>& default_bfbs_dirs() {
  static const std::vector<std::filesystem::path> dirs = discover_default_bfbs_dirs();
  return dirs;
}

[[nodiscard]] inline std::optional<std::filesystem::path> find_bfbs(
    std::string_view schema_name,
    const std::vector<std::filesystem::path>& dirs = default_bfbs_dirs()) {
  const auto file = bfbs_filename(schema_name);
  for (const auto& dir : dirs) {
    std::error_code ec{};
    const auto path = dir / file;
    if (std::filesystem::is_regular_file(path, ec)) return path;
  }
  return std::nullopt;
}

[[nodiscard]] inline std::optional<std::string> load_bfbs_base64(const std::filesystem::path& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return std::nullopt;

  const std::string bytes{
      std::istreambuf_iterator<char>(in),
      std::istreambuf_iterator<char>()};
  if (bytes.empty()) return std::nullopt;
  return ::foxglove::base64Encode(std::string_view(bytes));
}

[[nodiscard]] inline std::optional<std::string> load_schema_bfbs_base64(
    std::string_view schema_name,
    const std::vector<std::filesystem::path>& dirs = default_bfbs_dirs()) {
  const auto file = find_bfbs(schema_name, dirs);
  if (!file.has_value()) return std::nullopt;
  return load_bfbs_base64(*file);
}

}  // namespace utils::foxglove::schema
