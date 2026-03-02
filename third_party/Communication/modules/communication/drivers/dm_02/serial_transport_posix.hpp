/**
  * @file        serial_transport_posix.hpp
  * @brief       POSIX serial transport implementation for DM-02 driver
  * @details     Serial transport using POSIX termios for Linux/macOS.
  */

// serial_transport_posix.hpp (POSIX serial transport)
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

#include "dm_02.hpp"

namespace communication::dm_02 {

/**
  * @class       SerialTransportPosix
  * @brief        POSIX serial port transport
  * @details      Implements Transport interface for serial devices
  *              on POSIX systems (Linux, macOS) using termios.
  */
class SerialTransportPosix final : public Transport {
public:
    SerialTransportPosix() = default;
    ~SerialTransportPosix() override { close(); }

    /**
      * @brief          Open serial port connection
      * @param[in]      endpoint: Connection string "serial:/dev/path?baud=N"
      * @retval         true on success
      * @details        Parses endpoint like "serial:/dev/ttyACM0?baud=115200"
      */
    bool open(const std::string& endpoint) override;

    /**
      * @brief          Close serial port
      */
    void close() override;

    /**
      * @brief          Write all data to serial port
      * @param[in]      data: Data buffer
      * @param[in]      len: Length to write
      * @retval         true if all bytes written
      */
    bool write_all(const std::uint8_t* data, std::size_t len) override;

    /**
      * @brief          Read available data
      * @param[out]     data: Data buffer
      * @param[in]      cap: Buffer capacity
      * @retval         Number of bytes read (may be 0)
      */
    long read_some(std::uint8_t* data, std::size_t cap) override;

    /**
      * @brief          Wait for data to be readable
      * @param[in]      timeout_ms: Timeout in milliseconds
      * @retval         0 if timeout, >0 if ready, <0 if error
      */
    int wait_readable(int timeout_ms) override;

    /**
      * @brief          Get file descriptor
      * @retval         File descriptor or -1 if closed
      */
    int fd() const override { return fd_; }

    /**
      * @brief          Get maximum transmission unit
      * @retval         MTU in bytes (1024 for serial)
      */
    std::uint16_t mtu() const override { return 1024; }

private:
    /**
      * @brief          Map baud rate to termios constant
      * @param[in]      baud: Baud rate (9600, 115200, etc.)
      * @retval         termios speed_t constant
      */
    static speed_t map_baud_(int baud);

    int fd_{-1}; ///< File descriptor (-1 if closed)
};

inline bool SerialTransportPosix::open(const std::string& endpoint) {
    std::string path = endpoint;
    int baud = 115200;
    if (endpoint.rfind("serial:", 0) == 0) {
        auto q = endpoint.find('?');
        path = endpoint.substr(7, q == std::string::npos ? std::string::npos : q - 7);
        if (q != std::string::npos) {
            auto bpos = endpoint.find("baud=", q);
            if (bpos != std::string::npos) baud = std::atoi(endpoint.c_str() + bpos + 5);
        }
    }

    close();
    int fd = ::open(path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) return false;

    struct termios tio{};
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
    speed_t sp = map_baud_(baud);
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

inline void SerialTransportPosix::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

inline bool SerialTransportPosix::write_all(const std::uint8_t* data, std::size_t len) {
    std::size_t written = 0;
    while (written < len) {
        long n = ::write(fd_, data + written, len - written);
        if (n > 0) {
            written += static_cast<std::size_t>(n);
            continue;
        }
        if (n == 0) break;
        if (fd_ >= 0) {
            struct timeval tv{0, 20000};
            fd_set wfds;
            FD_ZERO(&wfds);
            FD_SET(fd_, &wfds);
            (void)select(fd_ + 1, nullptr, &wfds, nullptr, &tv);
        } else {
            struct timespec ts{0, 20 * 1000 * 1000};
            nanosleep(&ts, nullptr);
        }
    }
    return written == len;
}

inline long SerialTransportPosix::read_some(std::uint8_t* data, std::size_t cap) {
    return ::read(fd_, data, cap);
}

inline int SerialTransportPosix::wait_readable(int timeout_ms) {
    if (fd_ < 0) {
        struct timespec ts{timeout_ms / 1000, static_cast<long>((timeout_ms % 1000) * 1000000ul)};
        nanosleep(&ts, nullptr);
        return 0;
    }
    struct timeval tv{timeout_ms / 1000, static_cast<suseconds_t>((timeout_ms % 1000) * 1000)};
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd_, &rfds);
    return select(fd_ + 1, &rfds, nullptr, nullptr, &tv);
}

inline speed_t SerialTransportPosix::map_baud_(int baud) {
    switch (baud) {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200:
        default: return B115200;
    }
}

} // namespace communication::dm_02

#endif // !defined(_WIN32)
