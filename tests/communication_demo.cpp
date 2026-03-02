/**
  * @file        communication_demo.cpp
  * @brief       Minimal DM-02 communication demo (tests target)
  * @details     Opens a serial endpoint, runs uproto/MUX, prints callbacks.
  */

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "third_party/Communication/modules/communication/communication.hpp"

#if defined(_WIN32)
int main() {
    std::cerr << "communication_demo: POSIX serial transport required\n";
    return 1;
}
#else
#include "third_party/Communication/modules/communication/drivers/dm_02/serial_transport_posix.hpp"

namespace {
std::atomic<bool> g_stop{false};

void on_sigint(int) {
    g_stop.store(true, std::memory_order_release);
}

std::uint64_t now_ms_steady() {
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

std::uint64_t now_ns_steady() {
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

void print_usage(const char* argv0) {
    std::cout
        << "Usage:\n"
        << "  " << (argv0 ? argv0 : "communication_demo") << " [serial:/dev/ttyACM0?baud=115200] [--quiet] [--sine yaw|pitch:freq_hz:amp_deg[@rate_hz]]\n\n"
        << "Examples:\n"
        << "  " << (argv0 ? argv0 : "communication_demo") << " serial:/dev/ttyACM0?baud=115200\n"
        << "  " << (argv0 ? argv0 : "communication_demo") << " serial:/dev/ttyACM0?baud=115200 --sine yaw:1:10@50\n"
        << "\nNotes:\n"
        << "  --sine sends GIMBAL DELTA (SID=0x0202) periodically to validate that the gimbal moves.\n";
}

struct SineCfg {
    enum class Axis { None = 0, Yaw = 1, Pitch = 2 } axis{Axis::None};
    double freq_hz{0.0};
    double amp_deg{0.0};
    std::uint32_t rate_hz{50};
};

bool parse_sine_opt(const std::string& opt, SineCfg* out) {
    if (!out) return false;
    // format: axis:freq:amp[@rate]
    const auto p1 = opt.find(':');
    const auto p2 = opt.find(':', p1 == std::string::npos ? 0 : (p1 + 1));
    if (p1 == std::string::npos || p2 == std::string::npos) return false;

    const std::string axis_s = opt.substr(0, p1);
    if (axis_s == "yaw") out->axis = SineCfg::Axis::Yaw;
    else if (axis_s == "pitch") out->axis = SineCfg::Axis::Pitch;
    else out->axis = SineCfg::Axis::None;

    out->freq_hz = std::atof(opt.substr(p1 + 1, p2 - (p1 + 1)).c_str());
    const std::string tail = opt.substr(p2 + 1);
    const auto at = tail.find('@');
    if (at == std::string::npos) {
        out->amp_deg = std::atof(tail.c_str());
        out->rate_hz = 50;
    } else {
        out->amp_deg = std::atof(tail.substr(0, at).c_str());
        out->rate_hz = static_cast<std::uint32_t>(std::atoi(tail.substr(at + 1).c_str()));
        if (out->rate_hz == 0) out->rate_hz = 50;
    }

    return out->axis != SineCfg::Axis::None && out->freq_hz > 0.0 && out->amp_deg > 0.0;
}
} // namespace

int main(int argc, char** argv) {
    std::signal(SIGINT, on_sigint);

    std::string endpoint = "serial:/dev/ttyACM0?baud=115200";

    bool quiet = false;
    SineCfg sine{};
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
        if (arg == "--sine" && (i + 1) < argc) {
            const std::string opt = argv[++i] ? argv[i] : "";
            if (!parse_sine_opt(opt, &sine)) {
                std::cerr << "communication_demo: invalid --sine format: " << opt << "\n";
                print_usage(argv[0]);
                return 2;
            }
            continue;
        }
        if (!arg.empty() && arg.rfind("-", 0) != 0) {
            endpoint = arg;
            continue;
        }
    }

    auto transport = std::make_unique<communication::dm_02::SerialTransportPosix>();

    communication::dm_02::Config cfg{};
    cfg.timesync_enable = true;
    cfg.timesync_period_ms = 1000;
    cfg.timesync_initiator = true;

    communication::dm_02::Driver driver(std::move(transport), cfg);

    communication::dm_02::Callbacks cb{};

    cb.on_uproto_event = [quiet](std::string_view ev) {
        if (!quiet) std::cout << "[UPROTO] " << ev << "\n";
    };

    cb.on_timesync = [quiet](const communication::dm_02::TimeSyncStatus& ts) {
        if (quiet) return;
        std::cout << "[TS] valid=" << (ts.valid ? "true" : "false")
                  << " offset_us=" << ts.offset_us
                  << " rtt_us=" << ts.rtt_us
                  << " ver=" << ts.version << "\n";
    };

    cb.on_camera_event = [quiet](const communication::dm_02::CameraEvent& ev) {
        if (quiet) return;
        std::cout << "[CAM] frame_id=" << ev.frame_id
                  << " device_ts_us=" << ev.device_ts_us
                  << " host_ts_ns=" << ev.host_ts_ns << "\n";
    };

    cb.on_gimbal_tfmini = [quiet](const communication::dm_02::GimbalTfmini& tf) {
        if (quiet) return;
        std::cout << "[TFMINI] dist_cm=" << tf.distance_cm
                  << " strength=" << tf.strength
                  << " temp_cdeg=" << tf.temp_cdeg
                  << " status=0x" << std::hex << tf.status << std::dec
                  << " device_ts_us=" << tf.device_ts_us << "\n";
    };

    cb.on_gimbal_delta_ack = [quiet]() {
        if (quiet) return;
        std::cout << "[GIMBAL] delta ack\n";
    };

    cb.on_gimbal_state = [quiet](const communication::dm_02::GimbalState& st) {
        static std::uint64_t last_ms = 0;
        const std::uint64_t now = now_ms_steady();
        if (now - last_ms < 200) return;
        last_ms = now;
        if (quiet) return;
        std::cout << "[GIMBAL] enc=(" << st.enc_yaw << "," << st.enc_pitch << ")"
                  << " imu=(" << st.yaw_udeg << "," << st.pitch_udeg << "," << st.roll_udeg << ")"
                  << " gyro=(" << st.gyro_yaw_udeps << "," << st.gyro_pitch_udeps << ")"
                  << " device_ts_us=" << st.device_ts_us
                  << " host_ts_ns=" << st.host_ts_ns << "\n";
    };

    driver.set_callbacks(std::move(cb));

    if (!driver.open(endpoint)) {
        std::cerr << "communication_demo: failed to open " << endpoint << "\n";
        return 2;
    }

    std::cout << "communication_demo: " << endpoint << " (Ctrl+C to stop)\n";
    if (sine.axis != SineCfg::Axis::None) {
        const char* axis_s = (sine.axis == SineCfg::Axis::Yaw) ? "yaw" : "pitch";
        std::cout << "communication_demo: --sine enabled axis=" << axis_s
                  << " freq_hz=" << sine.freq_hz
                  << " amp_deg=" << sine.amp_deg
                  << " rate_hz=" << sine.rate_hz << "\n";
    }

    constexpr double kDegToUdeg = 1000000.0; // deg -> micro-degree
    const auto t_start = std::chrono::steady_clock::now();
    auto next_send = t_start;
    const auto send_period = (sine.rate_hz > 0)
        ? std::chrono::microseconds(static_cast<int>(1000000 / sine.rate_hz))
        : std::chrono::microseconds(20000);

    while (!g_stop.load(std::memory_order_acquire)) {
        driver.step(20);

        if (sine.axis != SineCfg::Axis::None && driver.established()) {
            const auto now = std::chrono::steady_clock::now();
            if (now >= next_send) {
                next_send += send_period;

                const double t_sec = std::chrono::duration<double>(now - t_start).count();
                const double err_deg = sine.amp_deg * std::sin(2.0 * M_PI * sine.freq_hz * t_sec);

                communication::dm_02::GimbalDelta cmd{};
                if (sine.axis == SineCfg::Axis::Yaw) {
                    cmd.delta_yaw_udeg = static_cast<std::int32_t>(std::llround(err_deg * kDegToUdeg));
                    cmd.delta_pitch_udeg = 0;
                } else {
                    cmd.delta_yaw_udeg = 0;
                    cmd.delta_pitch_udeg = static_cast<std::int32_t>(std::llround(err_deg * kDegToUdeg));
                }
                cmd.status = 0;
                cmd.host_ts_ns = now_ns_steady();

                if (!driver.send_gimbal_delta(cmd)) {
                    if (!quiet) std::cerr << "[GIMBAL] send_gimbal_delta failed\n";
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    driver.close();
    return 0;
}
#endif
