#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <websocketpp/common/connection_hdl.hpp>

#include <foxglove/websocket/common.hpp>
#include <foxglove/websocket/parameter.hpp>
#include <foxglove/websocket/server_factory.hpp>
#include <foxglove/websocket/server_interface.hpp>

namespace utils::foxglove {

class server final {
 public:
  using handle = websocketpp::connection_hdl;
  using options = ::foxglove::ServerOptions;
  using handlers = ::foxglove::ServerHandlers<handle>;
  using interface = ::foxglove::ServerInterface<handle>;
  using log_fn = std::function<void(::foxglove::WebSocketLogLevel, char const*)>;

  explicit server(
      std::string name = "printk",
      options opt = options{},
      log_fn logger = {})
      : name_(std::move(name)),
        options_(std::move(opt)),
        logger_(std::move(logger)) {
    if (!logger_) {
      logger_ = [](::foxglove::WebSocketLogLevel, char const*) {};
    }
  }

  ~server() noexcept {
    stop();
  }

  server(const server&) = delete;
  server& operator=(const server&) = delete;

  [[nodiscard]] bool running() const noexcept {
    return running_.load(std::memory_order_acquire);
  }

  [[nodiscard]] std::uint16_t port() const noexcept {
    return port_.load(std::memory_order_acquire);
  }

  void start(std::string host = "0.0.0.0", std::uint16_t port = 8765) {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    if (ws_) return;
    ws_ = ::foxglove::ServerFactory::createServer<handle>(name_, logger_, options_);
    if (!ws_) {
      throw std::runtime_error("utils.foxglove.server: createServer returned null");
    }

    auto copied = handlers_;
    ws_->setHandlers(std::move(copied));
    ws_->start(host, port);
    const auto actual_port = ws_->getPort();

    host_ = std::move(host);
    port_.store(actual_port == 0 ? port : actual_port, std::memory_order_release);
    running_.store(true, std::memory_order_release);
  }

  void stop() noexcept {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    if (!ws_) return;
    try {
      ws_->stop();
    } catch (...) {
    }
    ws_.reset();
    running_.store(false, std::memory_order_release);
    port_.store(0, std::memory_order_release);
  }

  template <class Fn>
  void with_handlers(Fn&& fn) {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    fn(handlers_);
    if (!ws_) return;
    auto copied = handlers_;
    ws_->setHandlers(std::move(copied));
  }

  void set_handlers(handlers handlers_value) {
    with_handlers([&handlers_value](handlers& cur) {
      cur = std::move(handlers_value);
    });
  }

  [[nodiscard]] std::vector<::foxglove::ChannelId> add_channels(const std::vector<::foxglove::ChannelWithoutId>& channels) {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    return ws_->addChannels(channels);
  }

  void remove_channels(const std::vector<::foxglove::ChannelId>& channel_ids) {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->removeChannels(channel_ids);
  }

  [[nodiscard]] std::vector<::foxglove::ServiceId> add_services(const std::vector<::foxglove::ServiceWithoutId>& services) {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    return ws_->addServices(services);
  }

  void remove_services(const std::vector<::foxglove::ServiceId>& service_ids) {
    std::unique_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->removeServices(service_ids);
  }

  void publish(
      ::foxglove::ChannelId channel_id,
      std::uint64_t timestamp_ns,
      const void* payload,
      std::size_t size) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->broadcastMessage(
        channel_id,
        timestamp_ns,
        static_cast<const std::uint8_t*>(payload),
        size);
  }

  void send_service_response(handle client, const ::foxglove::ServiceResponse& response) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->sendServiceResponse(client, response);
  }

  void send_service_failure(
      handle client,
      ::foxglove::ServiceId service_id,
      std::uint32_t call_id,
      const std::string& message) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->sendServiceFailure(client, service_id, call_id, message);
  }

  void send_status(const ::foxglove::Status& status) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->sendStatus(status);
  }

  void remove_status(const std::vector<std::string>& status_ids) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->removeStatus(status_ids);
  }

  void publish_parameter_values(
      handle client,
      const std::vector<::foxglove::Parameter>& values,
      const std::optional<std::string>& request_id) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->publishParameterValues(client, values, request_id);
  }

  void update_parameter_values(const std::vector<::foxglove::Parameter>& values) {
    std::shared_lock<std::shared_mutex> lk(mtx_);
    ensure_started_locked_();
    ws_->updateParameterValues(values);
  }

 private:
  void ensure_started_locked_() const {
    if (!ws_) {
      throw std::runtime_error("utils.foxglove.server: not started");
    }
  }

  std::string name_{};
  options options_{};
  log_fn logger_{};

  mutable std::shared_mutex mtx_{};
  std::unique_ptr<interface> ws_{};
  handlers handlers_{};
  std::string host_{};

  std::atomic_bool running_{false};
  std::atomic<std::uint16_t> port_{0};
};

}  // namespace utils::foxglove
