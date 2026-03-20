#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

namespace io::ws {

[[nodiscard]] bool enabled();
[[nodiscard]] std::uint16_t configured_port();
[[nodiscard]] std::string configured_host();
[[nodiscard]] std::string default_source();

[[nodiscard]] bool publish_json(std::string_view payload);
[[nodiscard]] bool publish_message(
    std::string_view type,
    const nlohmann::json& payload,
    std::uint64_t timestamp_ns = 0,
    std::string_view source = {});

}  // namespace io::ws
