#pragma once

#if !defined(_WIN32)

#include <cstddef>
#include <cstdlib>
#include <string>

#include <fcntl.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "io/dm02/impl/dm02_driver.hpp"

namespace communication::dm_02
{

class SerialTransportPosix final : public Transport
{
public:
  SerialTransportPosix() = default;
  ~SerialTransportPosix() override { close(); }

  bool open(const std::string & endpoint) override;
  void close() override;
  bool write_all(const std::uint8_t * data, std::size_t len) override;
  long read_some(std::uint8_t * data, std::size_t cap) override;
  int wait_readable(int timeout_ms) override;

  int fd() const override { return fd_; }
  std::uint16_t mtu() const override { return 1024; }

private:
  static speed_t map_baud_(int baud);

  int fd_{-1};
};

inline bool SerialTransportPosix::open(const std::string & endpoint)
{
  std::string path = endpoint;
  int baud = 115200;
  if (endpoint.rfind("serial:", 0) == 0) {
    const auto q = endpoint.find('?');
    path = endpoint.substr(7, q == std::string::npos ? std::string::npos : q - 7);
    if (q != std::string::npos) {
      const auto bpos = endpoint.find("baud=", q);
      if (bpos != std::string::npos) {
        baud = std::atoi(endpoint.c_str() + static_cast<std::ptrdiff_t>(bpos + 5));
      }
    }
  }

  close();
  const int fd = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd < 0) return false;

  struct termios tio {};
  if (tcgetattr(fd, &tio) != 0) {
    ::close(fd);
    return false;
  }

  tio.c_iflag = 0;
  tio.c_oflag = 0;
  tio.c_lflag = 0;
  tio.c_cflag |= (CLOCAL | CREAD);
  tio.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
  tio.c_cflag |= CS8;
  const speed_t sp = map_baud_(baud);
  cfsetispeed(&tio, sp);
  cfsetospeed(&tio, sp);
#ifdef CRTSCTS
  tio.c_cflag &= ~CRTSCTS;
#endif
  tio.c_cc[VMIN] = 0;
  tio.c_cc[VTIME] = 1;
  if (tcsetattr(fd, TCSANOW, &tio) != 0) {
    ::close(fd);
    return false;
  }

  fd_ = fd;
  return true;
}

inline void SerialTransportPosix::close()
{
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
}

inline bool SerialTransportPosix::write_all(const std::uint8_t * data, std::size_t len)
{
  std::size_t written = 0;
  while (written < len) {
    const long n = ::write(fd_, data + written, len - written);
    if (n > 0) {
      written += static_cast<std::size_t>(n);
      continue;
    }
    if (n == 0) break;

    if (fd_ >= 0) {
      struct timeval tv {0, 20000};
      fd_set wfds;
      FD_ZERO(&wfds);
      FD_SET(fd_, &wfds);
      (void)select(fd_ + 1, nullptr, &wfds, nullptr, &tv);
    } else {
      struct timespec ts {0, 20 * 1000 * 1000};
      nanosleep(&ts, nullptr);
    }
  }

  return written == len;
}

inline long SerialTransportPosix::read_some(std::uint8_t * data, std::size_t cap)
{
  return ::read(fd_, data, cap);
}

inline int SerialTransportPosix::wait_readable(int timeout_ms)
{
  if (fd_ < 0) {
    struct timespec ts {
      timeout_ms / 1000,
      static_cast<long>((timeout_ms % 1000) * 1000000ul)
    };
    nanosleep(&ts, nullptr);
    return 0;
  }

  struct timeval tv {
    timeout_ms / 1000,
    static_cast<suseconds_t>((timeout_ms % 1000) * 1000)
  };
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(fd_, &rfds);
  return select(fd_ + 1, &rfds, nullptr, nullptr, &tv);
}

inline speed_t SerialTransportPosix::map_baud_(int baud)
{
  switch (baud) {
    case 9600:
      return B9600;
    case 19200:
      return B19200;
    case 38400:
      return B38400;
    case 57600:
      return B57600;
    case 115200:
    default:
      return B115200;
  }
}

}  // namespace communication::dm_02

#endif  // !defined(_WIN32)
