#include "plotter.hpp"

#include "foxglove_bridge.hpp"

#include <arpa/inet.h>   // htons, inet_addr
#include <opencv2/imgcodecs.hpp>
#include <sys/socket.h>  // socket, sendto
#include <unistd.h>      // close

#include <cstdint>
#include <vector>

#ifndef SPV_FOXGLOVE_JPEG_QUALITY
#define SPV_FOXGLOVE_JPEG_QUALITY 60
#endif

namespace tools
{
Plotter::Plotter(std::string host, uint16_t port)
{
  socket_ = ::socket(AF_INET, SOCK_DGRAM, 0);

  destination_.sin_family = AF_INET;
  destination_.sin_port = ::htons(port);
  destination_.sin_addr.s_addr = ::inet_addr(host.c_str());
}

Plotter::~Plotter()
{
  if (socket_ >= 0) ::close(socket_);
}

void Plotter::plot_udp(const std::string & payload)
{
  if (socket_ < 0) return;
  ::sendto(
    socket_, payload.c_str(), payload.length(), 0, reinterpret_cast<sockaddr *>(&destination_),
    sizeof(destination_));
}

void Plotter::plot(const nlohmann::json & json)
{
  std::lock_guard<std::mutex> lock(mutex_);
  const auto payload = json.dump();
#if defined(SPV_ENABLE_FOXGLOVE) && defined(SPV_PLOTTER_BACKEND_FOXGLOVE)
  if (foxglove_bridge::publish_plot_json(payload)) return;
#endif
  plot_udp(payload);
}

void Plotter::plot_image(const cv::Mat & image, std::string_view frame_id)
{
#if defined(SPV_ENABLE_FOXGLOVE) && defined(SPV_PLOTTER_BACKEND_FOXGLOVE)
  if (image.empty()) return;
  std::vector<int> params{
    cv::IMWRITE_JPEG_QUALITY,
    SPV_FOXGLOVE_JPEG_QUALITY
  };
  std::vector<std::uint8_t> jpeg{};
  if (!cv::imencode(".jpg", image, jpeg, params)) return;
  (void)foxglove_bridge::publish_compressed_jpeg(jpeg, frame_id);
#else
  (void)image;
  (void)frame_id;
#endif
}

}  // namespace tools
