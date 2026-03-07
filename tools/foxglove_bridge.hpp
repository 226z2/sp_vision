#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#if defined(SPV_ENABLE_FOXGLOVE)

#include "io/foxglove/fb/compressed_image.hpp"
#include "io/foxglove/fb/log.hpp"
#include "io/foxglove/schema/loader.hpp"
#include "io/foxglove/server/server.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

namespace tools::foxglove_bridge {

struct state final {
  std::mutex mutex{};
  std::unique_ptr<utils::foxglove::server> server{};
  bool start_failed{false};
  std::optional<::foxglove::ChannelId> plot_json_channel{};
  std::optional<::foxglove::ChannelId> image_channel{};
  std::optional<::foxglove::ChannelId> log_channel{};
};

[[nodiscard]] inline state& shared_state() {
  static state value{};
  return value;
}

[[nodiscard]] inline std::uint64_t now_ns() {
  return static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

[[nodiscard]] inline std::uint16_t configured_port() {
  constexpr std::uint16_t kDefaultPort = 8765;
  const char* env = std::getenv("SPV_FOXGLOVE_PORT");
  if (env == nullptr || *env == '\0') return kDefaultPort;
  char* end = nullptr;
  const long parsed = std::strtol(env, &end, 10);
  if (end == env || *end != '\0' || parsed <= 0 || parsed > 65535) return kDefaultPort;
  return static_cast<std::uint16_t>(parsed);
}

[[nodiscard]] inline bool ensure_server_started_locked(state& ctx) {
  if (ctx.server) return true;
  if (ctx.start_failed) return false;

  try {
    auto options = ::foxglove::ServerOptions{};
    // Advertise only capabilities we actually handle to avoid "operation not supported" noise.
    options.capabilities.clear();
    options.supportedEncodings = {"json", "flatbuffer"};

    auto logger = [](::foxglove::WebSocketLogLevel, const char*) {};
    ctx.server = std::make_unique<utils::foxglove::server>("sp_vision", options, logger);
    // ws-protocol requires subscribe/unsubscribe handlers to be present.
    // Data routing still uses internal subscription bookkeeping; these callbacks can be no-op.
    utils::foxglove::server::handlers handlers{};
    handlers.subscribeHandler = [](::foxglove::ChannelId, utils::foxglove::server::handle) {};
    handlers.unsubscribeHandler = [](::foxglove::ChannelId, utils::foxglove::server::handle) {};
    ctx.server->set_handlers(std::move(handlers));
    ctx.server->start("0.0.0.0", configured_port());
    std::fprintf(stderr, "[foxglove] websocket server started on 0.0.0.0:%u\n", configured_port());
    return true;
  } catch (const std::exception& e) {
    std::fprintf(stderr, "[foxglove] websocket server start failed: %s\n", e.what());
    ctx.start_failed = true;
    return false;
  } catch (...) {
    std::fprintf(stderr, "[foxglove] websocket server start failed: unknown error\n");
    ctx.start_failed = true;
    return false;
  }
}

[[nodiscard]] inline std::optional<::foxglove::ChannelId> add_channel_locked(
    state& ctx,
    const ::foxglove::ChannelWithoutId& channel) {
  auto ids = ctx.server->add_channels({channel});
  if (ids.empty()) return std::nullopt;
  return ids.front();
}

[[nodiscard]] inline std::optional<::foxglove::ChannelId> ensure_plot_json_channel_locked(
    state& ctx) {
  if (ctx.plot_json_channel.has_value()) return ctx.plot_json_channel;
  ::foxglove::ChannelWithoutId channel{};
  channel.topic = "sp_vision/debug/plotter";
  channel.encoding = "json";
  channel.schemaName = "";
  channel.schema = "";
  ctx.plot_json_channel = add_channel_locked(ctx, channel);
  return ctx.plot_json_channel;
}

[[nodiscard]] inline std::optional<::foxglove::ChannelId> ensure_image_channel_locked(
    state& ctx) {
  if (ctx.image_channel.has_value()) return ctx.image_channel;
  ::foxglove::ChannelWithoutId channel{};
  channel.topic = "sp_vision/debug/image/compressed";
  channel.encoding = "flatbuffer";
  channel.schemaName = "foxglove.CompressedImage";
  if (const auto schema = utils::foxglove::schema::load_schema_bfbs_base64(channel.schemaName)) {
    channel.schema = *schema;
    channel.schemaEncoding = std::string("flatbuffer");
  }
  ctx.image_channel = add_channel_locked(ctx, channel);
  return ctx.image_channel;
}

[[nodiscard]] inline std::optional<::foxglove::ChannelId> ensure_log_channel_locked(
    state& ctx) {
  if (ctx.log_channel.has_value()) return ctx.log_channel;
  ::foxglove::ChannelWithoutId channel{};
  channel.topic = "sp_vision/debug/log";
  channel.encoding = "flatbuffer";
  channel.schemaName = "foxglove.Log";
  if (const auto schema = utils::foxglove::schema::load_schema_bfbs_base64(channel.schemaName)) {
    channel.schema = *schema;
    channel.schemaEncoding = std::string("flatbuffer");
  }
  ctx.log_channel = add_channel_locked(ctx, channel);
  return ctx.log_channel;
}

[[nodiscard]] inline bool publish_plot_json(
    std::string_view payload,
    std::uint64_t stamp_ns = now_ns()) {
  auto& ctx = shared_state();
  std::lock_guard<std::mutex> lock(ctx.mutex);
  if (!ensure_server_started_locked(ctx)) return false;
  const auto channel = ensure_plot_json_channel_locked(ctx);
  if (!channel.has_value()) return false;
  try {
    ctx.server->publish(*channel, stamp_ns, payload.data(), payload.size());
    return true;
  } catch (...) {
    return false;
  }
}

[[nodiscard]] inline bool publish_compressed_jpeg(
    const std::vector<std::uint8_t>& jpeg_bytes,
    std::string_view frame_id,
    std::uint64_t stamp_ns = now_ns()) {
  if (jpeg_bytes.empty()) return false;
  auto& ctx = shared_state();
  std::lock_guard<std::mutex> lock(ctx.mutex);
  if (!ensure_server_started_locked(ctx)) return false;
  const auto channel = ensure_image_channel_locked(ctx);
  if (!channel.has_value()) return false;

  auto view = utils::foxglove::fb::compressed_image_view{};
  view.stamp_ns = stamp_ns;
  view.frame_id = frame_id;
  view.format = "jpeg";
  view.data = jpeg_bytes.data();
  view.size = jpeg_bytes.size();

  std::vector<std::uint8_t> payload{};
  if (!utils::foxglove::fb::encode_compressed_image(view, payload)) return false;

  try {
    ctx.server->publish(*channel, stamp_ns, payload.data(), payload.size());
    return true;
  } catch (...) {
    return false;
  }
}

[[nodiscard]] inline ::foxglove::LogLevel map_log_level(const int level) {
  switch (level) {
    case 0:
    case 1:
      return ::foxglove::LogLevel_DEBUG;
    case 2:
      return ::foxglove::LogLevel_INFO;
    case 3:
      return ::foxglove::LogLevel_WARNING;
    case 4:
      return ::foxglove::LogLevel_ERROR;
    case 5:
      return ::foxglove::LogLevel_FATAL;
    default:
      return ::foxglove::LogLevel_UNKNOWN;
  }
}

[[nodiscard]] inline bool publish_log(
    int level,
    std::string_view message,
    std::string_view name,
    std::string_view file,
    std::uint32_t line,
    std::uint64_t stamp_ns = now_ns()) {
  auto& ctx = shared_state();
  std::lock_guard<std::mutex> lock(ctx.mutex);
  if (!ensure_server_started_locked(ctx)) return false;
  const auto channel = ensure_log_channel_locked(ctx);
  if (!channel.has_value()) return false;

  auto view = utils::foxglove::fb::log_view{};
  view.stamp_ns = stamp_ns;
  view.level = map_log_level(level);
  view.message = message;
  view.name = name;
  view.file = file;
  view.line = line;

  std::vector<std::uint8_t> payload{};
  if (!utils::foxglove::fb::encode_log(view, payload)) return false;

  try {
    ctx.server->publish(*channel, stamp_ns, payload.data(), payload.size());
    return true;
  } catch (...) {
    return false;
  }
}

}  // namespace tools::foxglove_bridge

#else

namespace tools::foxglove_bridge {

[[nodiscard]] inline bool publish_plot_json(std::string_view, std::uint64_t = 0) { return false; }
[[nodiscard]] inline bool publish_compressed_jpeg(
    const std::vector<std::uint8_t>&,
    std::string_view,
    std::uint64_t = 0) {
  return false;
}
[[nodiscard]] inline bool publish_log(
    int,
    std::string_view,
    std::string_view,
    std::string_view,
    std::uint32_t,
    std::uint64_t = 0) {
  return false;
}

}  // namespace tools::foxglove_bridge

#endif
