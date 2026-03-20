#include "io/ws/ws_bridge.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <stdexcept>

namespace io::ws {
namespace {

using server_t = websocketpp::server<websocketpp::config::asio>;
using connection_hdl = websocketpp::connection_hdl;

struct bridge_state final {
  std::mutex mutex{};
  server_t server{};
  std::set<connection_hdl, std::owner_less<connection_hdl>> connections{};
  std::unique_ptr<std::thread> thread{};
  bool started{false};
  bool start_failed{false};

  ~bridge_state() {
    if (!started) return;

    websocketpp::lib::error_code ec{};
    server.stop_listening(ec);
    for (const auto& hdl : connections) {
      server.close(hdl, websocketpp::close::status::going_away, "shutdown", ec);
    }
    server.stop_perpetual();
    server.stop();
    if (thread && thread->joinable()) thread->join();
  }
};

[[nodiscard]] bridge_state& shared_state() {
  static bridge_state state{};
  return state;
}

[[nodiscard]] std::uint64_t now_ns() {
  return static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

[[nodiscard]] bool env_flag_enabled(const char* name, bool default_value) {
  const char* value = std::getenv(name);
  if (value == nullptr || *value == '\0') return default_value;

  const std::string text(value);
  return !(text == "0" || text == "false" || text == "False" || text == "FALSE" ||
           text == "off" || text == "OFF");
}

[[nodiscard]] bool ensure_started_locked(bridge_state& state) {
  if (state.started) return true;
  if (state.start_failed || !enabled()) return false;

  try {
    state.server.clear_access_channels(websocketpp::log::alevel::all);
    state.server.clear_error_channels(websocketpp::log::elevel::all);
    state.server.init_asio();
    state.server.set_reuse_addr(true);
    state.server.start_perpetual();

    state.server.set_open_handler([&state](connection_hdl hdl) {
      std::lock_guard<std::mutex> lock(state.mutex);
      state.connections.insert(hdl);
      std::fprintf(stderr, "[ws_bridge] client connected, active=%zu\n", state.connections.size());
    });

    state.server.set_close_handler([&state](connection_hdl hdl) {
      std::lock_guard<std::mutex> lock(state.mutex);
      state.connections.erase(hdl);
      std::fprintf(
          stderr, "[ws_bridge] client disconnected, active=%zu\n", state.connections.size());
    });

    state.server.set_fail_handler([&state](connection_hdl hdl) {
      std::lock_guard<std::mutex> lock(state.mutex);
      state.connections.erase(hdl);
      std::fprintf(stderr, "[ws_bridge] client connection failed\n");
    });

    const auto host = configured_host();
    const auto port = configured_port();
    if (host.empty() || host == "0.0.0.0") {
      state.server.listen(port);
    } else {
      websocketpp::lib::error_code ec{};
      const auto address = websocketpp::lib::asio::ip::make_address(host, ec);
      if (ec) {
        throw std::runtime_error("invalid SPV_WS_BIND_HOST: " + host);
      }
      state.server.listen(websocketpp::lib::asio::ip::tcp::endpoint(address, port));
    }
    state.server.start_accept();
    state.thread = std::make_unique<std::thread>([&state]() {
      try {
        state.server.run();
      } catch (const std::exception& e) {
        std::fprintf(stderr, "[ws_bridge] event loop stopped: %s\n", e.what());
      } catch (...) {
        std::fprintf(stderr, "[ws_bridge] event loop stopped with unknown error\n");
      }
    });
    state.started = true;
    std::fprintf(stderr, "[ws_bridge] listening on ws://%s:%u\n", host.c_str(), port);
    return true;
  } catch (const std::exception& e) {
    std::fprintf(stderr, "[ws_bridge] start failed: %s\n", e.what());
    state.start_failed = true;
    return false;
  } catch (...) {
    std::fprintf(stderr, "[ws_bridge] start failed: unknown error\n");
    state.start_failed = true;
    return false;
  }
}

}  // namespace

bool enabled() { return env_flag_enabled("SPV_WS_ENABLE", true); }

std::uint16_t configured_port() {
  constexpr std::uint16_t kDefaultPort = 8878;
  const char* env = std::getenv("SPV_WS_PORT");
  if (env == nullptr || *env == '\0') return kDefaultPort;

  char* end = nullptr;
  const long parsed = std::strtol(env, &end, 10);
  if (end == env || *end != '\0' || parsed <= 0 || parsed > 65535) return kDefaultPort;
  return static_cast<std::uint16_t>(parsed);
}

std::string configured_host() {
  const char* env = std::getenv("SPV_WS_BIND_HOST");
  if (env == nullptr || *env == '\0') return "0.0.0.0";
  return env;
}

std::string default_source() {
  std::error_code ec{};
  const auto exe = std::filesystem::read_symlink("/proc/self/exe", ec);
  if (!ec && !exe.empty() && exe.has_filename()) return exe.filename().string();
  return "sp_vision";
}

bool publish_json(std::string_view payload) {
  auto& state = shared_state();
  std::lock_guard<std::mutex> lock(state.mutex);
  if (!ensure_started_locked(state)) return false;

  if (state.connections.empty()) return true;

  for (auto it = state.connections.begin(); it != state.connections.end();) {
    websocketpp::lib::error_code ec{};
    state.server.send(*it, payload.data(), payload.size(), websocketpp::frame::opcode::text, ec);
    if (ec) {
      std::fprintf(stderr, "[ws_bridge] send failed: %s\n", ec.message().c_str());
      it = state.connections.erase(it);
      continue;
    }
    ++it;
  }
  return true;
}

bool publish_message(
    std::string_view type,
    const nlohmann::json& payload,
    std::uint64_t timestamp_ns,
    std::string_view source) {
  nlohmann::json envelope{
      {"type", type},
      {"timestamp_ns", timestamp_ns == 0 ? now_ns() : timestamp_ns},
      {"source", source.empty() ? default_source() : std::string(source)},
      {"payload", payload}};
  return publish_json(envelope.dump());
}

}  // namespace io::ws
