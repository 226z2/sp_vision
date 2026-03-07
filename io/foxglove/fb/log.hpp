#pragma once

#include "io/foxglove/fb/builder.hpp"
#include "io/foxglove/fb/time.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

#include "Log_generated.h"

namespace utils::foxglove::fb {

struct log_view final {
  std::uint64_t stamp_ns{0};
  ::foxglove::LogLevel level{::foxglove::LogLevel_INFO};
  std::string_view message{};
  std::string_view name{"printk"};
  std::string_view file{};
  std::uint32_t line{0};
};

[[nodiscard]] inline bool encode_log(
    const log_view& log,
    std::vector<std::uint8_t>& bytes_out,
    std::string* error = nullptr) {
  auto& b = tls_builder();
  ::foxglove::Time stamp{};
  if (!time_from_ns(log.stamp_ns, stamp)) {
    if (error) *error = "utils.foxglove.fb: Log timestamp overflow";
    return false;
  }

  const auto message = b.CreateString(log.message.data(), log.message.size());
  const auto name = b.CreateString(log.name.data(), log.name.size());
  const auto file = log.file.empty() ? 0 : b.CreateString(log.file.data(), log.file.size());
  const auto root = ::foxglove::CreateLog(b, &stamp, log.level, message, name, file, log.line);
  ::foxglove::FinishLogBuffer(b, root);

  bytes_out.assign(b.GetBufferPointer(), b.GetBufferPointer() + b.GetSize());
  return true;
}

}  // namespace utils::foxglove::fb
