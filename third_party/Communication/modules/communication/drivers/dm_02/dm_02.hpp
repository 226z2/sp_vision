/**
  * @file        dm_02.hpp
  * @brief       DM-02 communication driver host-side interface
  * @details     Host-side driver wrapper over 3rdparty/Communication
  *              for gimbal control, camera sync, and time synchronization.
  */

// dm_02.hpp (host-side driver wrapper over 3rdparty/Communication)
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace communication::dm_02 {

/**
  * @struct       GimbalState
  * @brief        Gimbal sensor state data
  * @details      Contains encoder positions, IMU angles, commands,
  *              measurements, gyroscope data, and timestamps.
  */
struct GimbalState {
    std::int32_t enc_yaw{0};          ///< Yaw encoder position
    std::int32_t enc_pitch{0};        ///< Pitch encoder position
    std::int32_t yaw_udeg{0};         ///< Yaw angle in micro-degree (deg * 1e6)
    std::int32_t pitch_udeg{0};       ///< Pitch angle in micro-degree (deg * 1e6)
    std::int32_t roll_udeg{0};        ///< Roll angle in micro-degree (deg * 1e6)

    std::int32_t yaw_cmd_current{0};  ///< Current yaw command
    std::int32_t pitch_cmd_current{0};///< Current pitch command
    std::int32_t yaw_meas_current{0}; ///< Current yaw measurement
    std::int32_t pitch_meas_current{0};///< Current pitch measurement
    std::int32_t gyro_yaw_udeps{0};   ///< Yaw gyro rate in micro-degree/s (deg/s * 1e6)
    std::int32_t gyro_pitch_udeps{0}; ///< Pitch gyro rate in micro-degree/s (deg/s * 1e6)

    // Optional extension words (device -> host).
    // These are appended after the 6 words above in the GIMBAL_SID_STATE payload.
    std::int32_t bullet_speed_x100{0}; ///< Bullet speed (m/s * 100), from referee or estimation
    std::int32_t bullet_count{0};      ///< Total shots counter (monotonic)
    std::int32_t gimbal_mode{0};       ///< 0=IDLE, 1=AUTO_AIM, 2=SMALL_BUFF, 3=BIG_BUFF
    std::int32_t shoot_state{0};       ///< Shoot state machine enum value (firmware-defined)
    std::int32_t shooter_heat{0};      ///< Current heat (unit per referee definition)
    std::int32_t shooter_heat_limit{0};///< Heat limit (unit per referee definition)
    std::int32_t projectile_allowance_17mm{0}; ///< Remaining/allowance (referee 0x0208)
    std::int32_t projectile_allowance_42mm{0}; ///< Remaining/allowance (referee 0x0208)

    std::uint64_t device_ts_us{0};    ///< Device timestamp in microseconds
    std::uint64_t host_ts_ns{0};      ///< Host timestamp in nanoseconds
};

/**
  * @struct       GimbalDelta
  * @brief        Gimbal angle delta command
  * @details      Sends relative angle changes to the gimbal.
  */
struct GimbalDelta {
    std::int32_t delta_yaw_udeg{0};  ///< Yaw delta in 0.01 degree units
    std::int32_t delta_pitch_udeg{0};///< Pitch delta in 0.01 degree units
    std::uint16_t status{0};          ///< Command status flags
    std::uint64_t host_ts_ns{0};      ///< Host timestamp in nanoseconds
    std::uint64_t device_ts_us{0};    ///< Device timestamp in microseconds
};

/**
  * @struct       GimbalTfmini
  * @brief        TFMini LiDAR sensor data
  * @details      Distance measurement from onboard TFMini sensor.
  */
struct GimbalTfmini {
    std::uint16_t distance_cm{0};     ///< Distance in centimeters
    std::uint16_t strength{0};        ///< Signal strength
    std::int16_t temp_cdeg{0};        ///< Temperature in 0.01 degree Celsius
    std::uint16_t status{0};          ///< Sensor status code
    std::uint64_t device_ts_us{0};    ///< Device timestamp in microseconds
    std::uint64_t host_ts_ns{0};      ///< Host timestamp in nanoseconds
};

/**
  * @struct       CameraEvent
  * @brief        Camera frame synchronization event
  * @details      Timestamp notification for camera frame capture.
  */
struct CameraEvent {
    std::uint32_t frame_id{0};        ///< Frame sequence number
    std::uint64_t device_ts_us{0};    ///< Device timestamp in microseconds
    std::uint64_t host_ts_ns{0};      ///< Host timestamp in nanoseconds
};

/**
  * @struct       TimeSyncStatus
  * @brief        Time synchronization status
  * @details      Status of host-device time synchronization protocol.
  */
struct TimeSyncStatus {
    bool valid{false};                ///< Synchronization validity flag
    std::int64_t offset_us{0};        ///< Host-device offset in microseconds
    std::uint32_t rtt_us{0};          ///< Round-trip time in microseconds
    std::uint32_t version{0};         ///< Mapping version number
    std::uint64_t last_device_time_us{0}; ///< Last device time reference
    std::uint64_t last_host_time_us{0};   ///< Last host time reference
};

/**
  * @class       Transport
  * @brief        Abstract transport layer for DM-02 communication
  * @details      Defines interface for serial/UDP/other transports.
  */
class Transport {
public:
    virtual ~Transport() = default;

    /**
      * @brief          Open transport connection
      * @param[in]      endpoint: Connection string (e.g., "serial:/dev/ttyACM0?baud=115200")
      * @retval         true on success
      */
    virtual bool open(const std::string& endpoint) = 0;

    /**
      * @brief          Close transport connection
      */
    virtual void close() = 0;

    /**
      * @brief          Write all data to transport
      * @param[in]      data: Data buffer
      * @param[in]      len: Length to write
      * @retval         true if all bytes written
      */
    virtual bool write_all(const std::uint8_t* data, std::size_t len) = 0;

    /**
      * @brief          Read available data
      * @param[out]     data: Data buffer
      * @param[in]      cap: Buffer capacity
      * @retval         Number of bytes read (may be 0)
      */
    virtual long read_some(std::uint8_t* data, std::size_t cap) = 0;

    /**
      * @brief          Wait for data to be readable
      * @param[in]      timeout_ms: Timeout in milliseconds
      * @retval         0 if timeout, >0 if ready, <0 if error
      */
    virtual int wait_readable(int timeout_ms) = 0;

    /**
      * @brief          Get file descriptor for polling
      * @retval         File descriptor or -1 if unavailable
      */
    virtual int fd() const = 0;

    /**
      * @brief          Get maximum transmission unit
      * @retval         MTU in bytes
      */
    virtual std::uint16_t mtu() const = 0;
};

/**
  * @struct       Config
  * @brief        DM-02 driver configuration
  */
struct Config {
    std::uint32_t handshake_timeout_ms{1000};  ///< Handshake timeout
    std::uint32_t heartbeat_interval_ms{0};     ///< Heartbeat interval (0 = disabled)
    std::uint32_t default_timeout_ms{3000};     ///< Default request timeout
    std::uint8_t default_retries{1};            ///< Default retry count
    bool auto_handshake{true};                  ///< Auto-start handshake

    bool timesync_enable{true};                 ///< Enable time synchronization
    std::uint32_t timesync_period_ms{1000};      ///< Time sync period in ms
    std::uint32_t timesync_max_rtt_us{0};       ///< Max RTT for valid sync (0 = no limit)
    bool timesync_initiator{true};              ///< Act as time sync initiator

    bool gimbal_has_encoders{true};             ///< Gimbal has encoder data
    bool gimbal_has_imu{true};                  ///< Gimbal has IMU data
};

/**
  * @struct       Callbacks
  * @brief        Event callback registration
  */
struct Callbacks {
    std::function<void(const GimbalState&)> on_gimbal_state{};          ///< Gimbal state update
    std::function<void(const GimbalTfmini&)> on_gimbal_tfmini{};        ///< TFMini sensor data
    std::function<void(const CameraEvent&)> on_camera_event{};          ///< Camera frame event
    std::function<void()> on_gimbal_delta_ack{};                        ///< Gimbal delta acknowledgment
    std::function<void(const TimeSyncStatus&)> on_timesync{};          ///< Time sync status update
    std::function<void(std::uint8_t ch, std::uint16_t sid, const std::uint8_t* payload, std::uint32_t len)> on_mux_raw{}; ///< Raw mux packet
    std::function<void(std::string_view event)> on_uproto_event{};     ///< UProto protocol event
};

/**
  * @class       Driver
  * @brief        DM-02 communication driver
  * @details      Main driver class for gimbal communication with
  *              automatic protocol handling and time synchronization.
  */
class Driver {
public:
    /**
      * @brief          Construct driver with transport
      * @param[in]      transport: Transport layer implementation
      * @param[in]      cfg: Driver configuration
      */
    explicit Driver(std::unique_ptr<Transport> transport, Config cfg = {});

    ~Driver();

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;
    Driver(Driver&&) noexcept;
    Driver& operator=(Driver&&) noexcept;

    /**
      * @brief          Open connection to device
      * @param[in]      endpoint: Connection string
      * @retval         true on success
      */
    bool open(const std::string& endpoint);

    /**
      * @brief          Close connection
      */
    void close();

    /**
      * @brief          Check if connection is open
      * @retval         true if open
      */
    bool is_open() const;

    /**
      * @brief          Check if protocol is established
      * @retval         true if handshake complete
      */
    bool established() const;

    /**
      * @brief          Register event callbacks
      * @param[in]      cb: Callback structure
      */
    void set_callbacks(Callbacks cb);

    /**
      * @brief          Poll for incoming data
      * @param[in]      timeout_ms: Poll timeout
      */
    void poll(int timeout_ms);

    /**
      * @brief          Process protocol timers
      */
    void tick();

    /**
      * @brief          Combined poll and tick
      * @param[in]      timeout_ms: Poll timeout
      */
    void step(int timeout_ms);

    /**
      * @brief          Send gimbal delta command
      * @param[in]      cmd: Delta command
      * @retval         true if sent
      */
    bool send_gimbal_delta(const GimbalDelta& cmd);

    /**
      * @brief          Send camera reset command
      * @retval         true if sent
      */
    bool send_camera_reset();

    /**
      * @brief          Send raw mux packet
      * @param[in]      ch: Channel ID
      * @param[in]      sid: Service ID
      * @param[in]      payload: Payload data (starts with 16-bit SID)
      * @param[in]      len: Payload length
      * @retval         true if sent
      */
    bool send_mux(std::uint8_t ch, std::uint16_t sid, const std::uint8_t* payload, std::uint16_t len);

    /**
      * @brief          Get last received gimbal state
      * @retval         Optional gimbal state
      */
    std::optional<GimbalState> last_gimbal_state() const;

    /**
      * @brief          Get last received TFMini data
      * @retval         Optional TFMini data
      */
    std::optional<GimbalTfmini> last_gimbal_tfmini() const;

    /**
      * @brief          Get last received camera event
      * @retval         Optional camera event
      */
    std::optional<CameraEvent> last_camera_event() const;

    /**
      * @brief          Get time synchronization status
      * @retval         Current sync status
      */
    TimeSyncStatus time_sync_status() const;

    /**
      * @brief          Convert host nanoseconds to device microseconds
      * @param[in]      host_ns: Host timestamp in nanoseconds
      * @retval         Optional device time in microseconds
      */
    std::optional<std::uint64_t> host_ns_to_device_us(std::uint64_t host_ns) const;

    /**
      * @brief          Convert device microseconds to host nanoseconds
      * @param[in]      device_us: Device timestamp in microseconds
      * @retval         Optional host time in nanoseconds
      */
    std::optional<std::uint64_t> device_us_to_host_ns(std::uint64_t device_us) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace communication::dm_02
