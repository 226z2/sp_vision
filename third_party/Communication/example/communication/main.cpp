/**
  * @file        main.cpp
  * @brief       Communication driver demo application
  * @details     Demonstrates usage of the DM-02 communication driver
  *              for gimbal control and sensor data reception.
  */

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "modules/communication/communication.hpp"

#if defined(_WIN32)
int main() {
    std::cerr << "communication_demo: POSIX serial transport required\n";
    return 1;
}
#else
#include "modules/communication/drivers/dm_02/serial_transport_posix.hpp"

namespace {

/** @var g_stop
  * @brief Global stop flag for signal handler
  */
std::atomic<bool> g_stop{false};

/**
  * @brief          SIGINT signal handler for graceful shutdown
  * @param[in]      signum: Signal number (unused)
  */
void on_sigint(int) {
    g_stop.store(true, std::memory_order_release);
}

} // namespace

/**
  * @brief          Main entry point for communication demo
  * @param[in]      argc: Argument count
  * @param[in]      argv: Argument values (argv[1] = serial endpoint)
  * @retval         Exit code (0 = success, non-zero = error)
  * @details        Creates a DM-02 driver, registers callbacks for
  *                gimbal state, camera events, and time sync,
  *                then runs the event loop until Ctrl+C.
  */
int main(int argc, char** argv) {
    std::signal(SIGINT, on_sigint);

    std::string endpoint = "serial:/dev/ttyACM0?baud=115200";
    if (argc > 1 && argv[1]) endpoint = argv[1];

    auto transport = std::make_unique<communication::dm_02::SerialTransportPosix>();
    communication::dm_02::Config cfg{};
    cfg.timesync_enable = true;
    cfg.timesync_period_ms = 1000;
    cfg.timesync_initiator = true;

    communication::dm_02::Driver driver(std::move(transport), cfg);
    communication::dm_02::Callbacks cb{};

    cb.on_camera_event = [](const communication::dm_02::CameraEvent& ev) {
        std::cout << "[CAM] frame_id=" << ev.frame_id
                  << " device_ts_us=" << ev.device_ts_us
                  << " host_ts_ns=" << ev.host_ts_ns << "\n";
    };

    cb.on_gimbal_state = [](const communication::dm_02::GimbalState& st) {
        std::cout << "[GIMBAL] enc=(" << st.enc_yaw << "," << st.enc_pitch << ")"
                  << " imu=(" << st.yaw_udeg << "," << st.pitch_udeg << "," << st.roll_udeg << ")"
                  << " ts_us=" << st.device_ts_us << "\n";
    };

    cb.on_gimbal_tfmini = [](const communication::dm_02::GimbalTfmini& tf) {
        std::cout << "[TFMINI] dist_cm=" << tf.distance_cm
                  << " strength=" << tf.strength
                  << " temp_cdeg=" << tf.temp_cdeg
                  << " status=0x" << std::hex << tf.status << std::dec
                  << " ts_us=" << tf.device_ts_us << "\n";
    };

    cb.on_timesync = [](const communication::dm_02::TimeSyncStatus& ts) {
        std::cout << "[TS] valid=" << (ts.valid ? "true" : "false")
                  << " offset_us=" << ts.offset_us
                  << " rtt_us=" << ts.rtt_us
                  << " ver=" << ts.version << "\n";
    };

    cb.on_uproto_event = [](std::string_view ev) {
        std::cout << "[UPROTO] " << ev << "\n";
    };

    driver.set_callbacks(std::move(cb));

    if (!driver.open(endpoint)) {
        std::cerr << "communication_demo: failed to open " << endpoint << "\n";
        return 2;
    }

    std::cout << "communication_demo: " << endpoint << " (Ctrl+C to stop)\n";

    while (!g_stop.load(std::memory_order_acquire)) {
        driver.step(20);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    driver.close();
    return 0;
}
#endif
