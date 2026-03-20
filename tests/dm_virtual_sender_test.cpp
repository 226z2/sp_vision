#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

#include "io/dm02/impl/dm02_driver.hpp"
#include "io/dm02/impl/serial_transport_posix.hpp"

extern "C" {
#include "io/dm02/protocol/channel/gimbal/gimbal_channel.h"
#include "io/dm02/protocol/channel/gimbal/gimbal_config.h"
#include "io/dm02/protocol/core/comm.h"
}

namespace {

std::uint64_t now_ns_steady() {
  return static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count());
}

std::uint64_t now_us_steady() { return now_ns_steady() / 1000ULL; }

void print_usage(const char* argv0) {
  std::printf(
      "Usage:\n"
      "  %s [serial:/dev/ttyXXX?baud=115200] [--quiet]\n",
      argv0 ? argv0 : "dm_virtual_sender_test");
}

bool send_gimbal_state(communication::dm_02::Driver& driver, int tick) {
  constexpr std::size_t kExtWords = 14;
  std::uint8_t payload[2 + 5 * 4 + 8 + kExtWords * 4]{};
  const auto ts_us = now_us_steady();
  const auto enc_yaw = 1000 + tick;
  const auto enc_pitch = 500 + tick;
  const auto yaw_udeg = 120000 + tick * 100;
  const auto pitch_udeg = -30000 - tick * 50;
  const auto roll_udeg = 0;

  const auto base_len = gimbal_pack_state(
      payload,
      GIMBAL_SID_STATE,
      enc_yaw,
      enc_pitch,
      yaw_udeg,
      pitch_udeg,
      roll_udeg,
      ts_us);

  std::size_t pos = base_len;
  const std::int32_t ext_values[kExtWords] = {
      11,            // yaw_cmd_current
      12,            // pitch_cmd_current
      13,            // yaw_meas_current
      14,            // pitch_meas_current
      1500,          // gyro_yaw_udeps
      -800,          // gyro_pitch_udeps
      2700,          // bullet_speed_x100
      100 + tick,    // bullet_count
      2,             // gimbal_mode
      1,             // shoot_state
      10,            // shooter_heat
      100,           // shooter_heat_limit
      50,            // projectile_allowance_17mm
      0              // projectile_allowance_42mm
  };
  for (std::size_t i = 0; i < kExtWords; ++i) {
    comm_write_i32_le(payload + pos, ext_values[i]);
    pos += 4;
  }

  return driver.send_mux(
      GIMBAL_CH_ID,
      GIMBAL_SID_STATE,
      payload,
      static_cast<std::uint16_t>(pos));
}

bool send_referee(communication::dm_02::Driver& driver, int tick) {
  std::uint8_t payload[2 + 4 + 4 + 4 + 4 + 2 + 8]{};
  comm_write_u16_le(payload + 0, GIMBAL_SID_REFEREE);
  comm_write_i32_le(payload + 2, 1);
  comm_write_i32_le(payload + 6, (tick % 2 == 0) ? 1 : 0);
  comm_write_i32_le(payload + 10, 7);
  comm_write_i32_le(payload + 14, 2);
  comm_write_u16_le(payload + 18, GIMBAL_REFEREE_STATUS_VALID);
  comm_write_u64_le(payload + 20, now_us_steady());
  return driver.send_mux(GIMBAL_CH_ID, GIMBAL_SID_REFEREE, payload, sizeof(payload));
}

bool send_tfmini(communication::dm_02::Driver& driver, int tick) {
  std::uint8_t payload[2 + 2 + 2 + 2 + 2 + 8]{};
  comm_write_u16_le(payload + 0, GIMBAL_SID_TFMINI);
  comm_write_u16_le(payload + 2, static_cast<std::uint16_t>(250 + tick));
  comm_write_u16_le(payload + 4, 99);
  comm_write_u16_le(payload + 6, static_cast<std::uint16_t>(2350));
  comm_write_u16_le(payload + 8, GIMBAL_TFMINI_STATUS_VALID);
  comm_write_u64_le(payload + 10, now_us_steady());
  return driver.send_mux(GIMBAL_CH_ID, GIMBAL_SID_TFMINI, payload, sizeof(payload));
}

}  // namespace

int main(int argc, char** argv) {
  std::string endpoint = "serial:/dev/ttyACM0?baud=115200";
  bool quiet = false;

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i] ? argv[i] : "";
    if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      return 0;
    }
    if (arg == "--quiet") {
      quiet = true;
      continue;
    }
    if (!arg.empty() && arg.rfind("-", 0) != 0) endpoint = arg;
  }

  auto transport = std::make_unique<communication::dm_02::SerialTransportPosix>();
  communication::dm_02::Config cfg{};
  cfg.timesync_enable = true;
  cfg.timesync_period_ms = 1000;
  cfg.timesync_initiator = false;

  communication::dm_02::Driver driver(std::move(transport), cfg);
  communication::dm_02::Callbacks cb{};
  cb.on_uproto_event = [quiet](std::string_view ev) {
    if (!quiet) std::printf("[UPROTO] %.*s\n", static_cast<int>(ev.size()), ev.data());
  };
  cb.on_gimbal_delta_ack = [quiet]() {
    if (!quiet) std::printf("[GIMBAL] delta ack\n");
  };
  cb.on_timesync = [quiet](const communication::dm_02::TimeSyncStatus& ts) {
    if (!quiet) {
      std::printf(
          "[TS] valid=%d offset_us=%lld rtt_us=%u ver=%u\n",
          ts.valid ? 1 : 0,
          static_cast<long long>(ts.offset_us),
          ts.rtt_us,
          ts.version);
    }
  };
  driver.set_callbacks(std::move(cb));

  if (!driver.open(endpoint)) {
    std::fprintf(stderr, "dm_virtual_sender_test: failed to open %s\n", endpoint.c_str());
    return 2;
  }

  std::printf("dm_virtual_sender_test: %s\n", endpoint.c_str());
  int tick = 0;
  auto next_pub = std::chrono::steady_clock::now();
  while (true) {
    driver.step(20);
    const auto now = std::chrono::steady_clock::now();
    if (driver.established() && now >= next_pub) {
      next_pub = now + std::chrono::milliseconds(100);
      (void)send_gimbal_state(driver, tick);
      (void)send_referee(driver, tick);
      (void)send_tfmini(driver, tick);
      if (!quiet) std::printf("[SIM] published tick=%d\n", tick);
      ++tick;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  return 0;
}
