#include "logger.hpp"

#include "foxglove_bridge.hpp"

#include <fmt/chrono.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>

namespace
{
#if defined(SPV_ENABLE_FOXGLOVE) && defined(SPV_LOGGER_FOXGLOVE_SINK)
class FoxgloveLogSink final : public spdlog::sinks::base_sink<std::mutex>
{
protected:
  void sink_it_(const spdlog::details::log_msg & msg) override
  {
    spdlog::memory_buf_t formatted;
    this->formatter_->format(msg, formatted);
    const std::string message(formatted.data(), formatted.size());
    const std::string_view logger_name(msg.logger_name.data(), msg.logger_name.size());
    const std::string_view file = msg.source.filename == nullptr ? "" : msg.source.filename;
    const auto line = msg.source.line <= 0 ? 0U : static_cast<std::uint32_t>(msg.source.line);
    (void)tools::foxglove_bridge::publish_log(
      static_cast<int>(msg.level), message, logger_name, file, line);
  }

  void flush_() override {}
};
#endif
}  // namespace

namespace tools
{
std::shared_ptr<spdlog::logger> logger_ = nullptr;

void set_logger()
{
  std::filesystem::create_directories("logs");
  auto file_name = fmt::format("logs/{:%Y-%m-%d_%H-%M-%S}.log", std::chrono::system_clock::now());
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_name, true);
  file_sink->set_level(spdlog::level::debug);

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::debug);

#if defined(SPV_ENABLE_FOXGLOVE) && defined(SPV_LOGGER_FOXGLOVE_SINK)
  auto foxglove_sink = std::make_shared<FoxgloveLogSink>();
  foxglove_sink->set_level(spdlog::level::info);
  logger_ =
    std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{file_sink, console_sink, foxglove_sink});
#else
  logger_ = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{file_sink, console_sink});
#endif
  logger_->set_level(spdlog::level::debug);
  logger_->flush_on(spdlog::level::info);
}

std::shared_ptr<spdlog::logger> logger()
{
  if (!logger_) set_logger();
  return logger_;
}

}  // namespace tools
