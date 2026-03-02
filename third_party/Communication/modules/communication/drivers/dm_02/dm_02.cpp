// dm_02.cpp (host-side driver wrapper implementation)
#include "dm_02.hpp"

#include <array>
#include <chrono>
#include <utility>

extern "C" {
#include "3rdparty/Communication/core/comm.h"
#include "3rdparty/Communication/core/comm_utils.h"
#include "3rdparty/Communication/core/uproto.h"
#include "3rdparty/Communication/example/shared/protocol_ids.h"
}

namespace communication::dm_02 {

namespace {

inline std::uint64_t now_ns_steady() {
    const auto now = std::chrono::steady_clock::now();
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
}

inline std::uint64_t now_us_steady() {
    return now_ns_steady() / 1000u;
}

class TimeSync {
public:
    using SendFn = std::function<int(std::uint8_t, std::uint16_t, const std::uint8_t*, std::uint16_t)>;

    TimeSync(std::uint8_t ch_id, std::uint32_t period_ms, bool initiator, SendFn send)
        : ch_id_(ch_id),
          period_ms_(period_ms ? period_ms : 1000u),
          initiator_(initiator),
          send_(std::move(send)) {}

    void set_period_ms(std::uint32_t v) { period_ms_ = v ? v : 1000u; }
    void set_initiator(bool v) { initiator_ = v; }
    void set_max_rtt_us(std::uint32_t v) { max_rtt_us_ = v; }

    void tick() {
        if (!initiator_) return;
        const std::uint64_t now = now_us_steady();
        if (now - last_req_us_ >= static_cast<std::uint64_t>(period_ms_) * 1000ULL) {
            last_req_us_ = now;
            send_req_();
        }
    }

    void on_mux(const std::uint8_t* pl, std::uint32_t len) {
        if (!pl || len < 2) return;
        const std::uint16_t sid = comm_read_u16_le(pl);
        if (sid == TS_SID_REQ) {
            if (len < 2 + 2 + 8) return;
            const std::uint16_t seq = comm_read_u16_le(pl + 2);
            (void)seq;
            const std::uint64_t t1 = now_us_steady();
            const std::uint64_t t2 = now_us_steady();
            std::uint8_t buf[2 + 2 + 8 + 8];
            comm_write_u16_le(&buf[0], TS_SID_RESP);
            comm_write_u16_le(&buf[2], seq);
            comm_write_u64_le(&buf[4], t1);
            comm_write_u64_le(&buf[12], t2);
            (void)send_(ch_id_, TS_SID_RESP, buf, sizeof(buf));
        } else if (sid == TS_SID_RESP) {
            if (len < 2 + 2 + 8 + 8) return;
            const std::uint16_t seq = comm_read_u16_le(pl + 2);
            const std::uint64_t t1 = comm_read_u64_le(pl + 4);
            const std::uint64_t t2 = comm_read_u64_le(pl + 12);
            const std::uint64_t t3 = now_us_steady();
            std::uint64_t t0 = 0;
            for (const auto& h : hist_) {
                if (h.seq == seq) {
                    t0 = h.t0;
                    break;
                }
            }
            if (!t0) return;
            std::uint64_t rtt = t3 - t0;
            if (t2 >= t1) rtt -= (t2 - t1);
            rtt_us_last_ = static_cast<std::uint32_t>(rtt);
            const std::int64_t offset = (static_cast<std::int64_t>(t1 - t0) +
                                         static_cast<std::int64_t>(t2 - t3)) / 2;
            if (max_rtt_us_ == 0 || rtt <= max_rtt_us_) {
                offset_us_ = (offset_us_ * 9 + offset) / 10;
                mapping_valid_ = true;
                mapping_version_++;
                last_device_time_us_ = t2;
                last_host_time_us_ = t3;
            }
        }
    }

    std::uint64_t now_device_us() const {
        const std::uint64_t now = now_us_steady();
        if (!mapping_valid_) return now;
        return static_cast<std::uint64_t>(static_cast<std::int64_t>(now) + offset_us_);
    }

    TimeSyncStatus status() const {
        TimeSyncStatus s{};
        s.valid = mapping_valid_;
        s.offset_us = offset_us_;
        s.rtt_us = rtt_us_last_;
        s.version = mapping_version_;
        s.last_device_time_us = last_device_time_us_;
        s.last_host_time_us = last_host_time_us_;
        return s;
    }

    std::optional<std::uint64_t> host_ns_to_device_us(std::uint64_t host_ns) const {
        if (!mapping_valid_) return std::nullopt;
        const double host_us = static_cast<double>(host_ns) * 1e-3;
        const double dev_us = host_us + static_cast<double>(offset_us_);
        if (dev_us < 0.0) return std::nullopt;
        return static_cast<std::uint64_t>(dev_us);
    }

    std::optional<std::uint64_t> device_us_to_host_ns(std::uint64_t device_us) const {
        if (!mapping_valid_) return std::nullopt;
        const double host_us = static_cast<double>(device_us) - static_cast<double>(offset_us_);
        if (host_us < 0.0) return std::nullopt;
        return static_cast<std::uint64_t>(host_us * 1e3);
    }

    std::uint32_t version() const { return mapping_version_; }

private:
    void send_req_() {
        if (!send_) return;
        const std::uint16_t seq = seq_++;
        const std::uint64_t t0 = now_us_steady();
        std::uint8_t buf[2 + 2 + 8];
        comm_write_u16_le(&buf[0], TS_SID_REQ);
        comm_write_u16_le(&buf[2], seq);
        comm_write_u64_le(&buf[4], t0);
        hist_[hist_pos_] = {seq, t0};
        hist_pos_ = static_cast<std::uint8_t>((hist_pos_ + 1) & 0x1F);
        (void)send_(ch_id_, TS_SID_REQ, buf, sizeof(buf));
    }

    struct Item { std::uint16_t seq; std::uint64_t t0; };

    std::uint8_t ch_id_{0};
    std::uint32_t period_ms_{1000};
    bool initiator_{false};
    SendFn send_{};

    std::uint16_t seq_{0};
    std::uint64_t last_req_us_{0};
    Item hist_[32]{};
    std::uint8_t hist_pos_{0};

    std::uint32_t max_rtt_us_{0};
    std::uint32_t rtt_us_last_{0};
    std::int64_t offset_us_{0};
    bool mapping_valid_{false};
    std::uint32_t mapping_version_{0};
    std::uint64_t last_device_time_us_{0};
    std::uint64_t last_host_time_us_{0};
};

} // namespace

struct Driver::Impl {
    explicit Impl(std::unique_ptr<Transport> t, Config cfg)
        : transport(std::move(t)), config(std::move(cfg)) {}

    std::unique_ptr<Transport> transport{};
    Config config{};
    Callbacks callbacks{};
    bool open{false};
    std::array<std::uint32_t, 256> seqs{};
    uproto_context_t uproto{};
    TimeSync ts{TS_CH_ID, 1000u, true, [this](std::uint8_t ch, std::uint16_t sid, const std::uint8_t* p, std::uint16_t l) {
        return send_mux(ch, sid, p, l) ? 0 : -1;
    }};
    std::uint32_t ts_version_seen{0};

    std::optional<GimbalState> last_gimbal{};
    std::optional<GimbalTfmini> last_tfmini{};
    std::optional<CameraEvent> last_camera{};

    static void on_mux_trampoline(uproto_context_t*, std::uint16_t, const std::uint8_t* data, std::uint32_t len, void* user) {
        auto* self = static_cast<Impl*>(user);
        if (!self) return;
        self->handle_mux(data, len);
    }

    static void on_uproto_event(uproto_context_t*, const char* event, void* user) {
        auto* self = static_cast<Impl*>(user);
        if (!self || !event) return;
        if (self->callbacks.on_uproto_event) self->callbacks.on_uproto_event(event);
    }

    void init_uproto() {
        uproto_port_ops_t pop{};
        pop.user = this;
        pop.get_mtu = [](void* u) -> std::uint16_t {
            auto* self = static_cast<Impl*>(u);
            return self && self->transport ? self->transport->mtu() : 1024;
        };
        pop.write = [](void* u, const std::uint8_t* d, std::uint32_t l) -> std::uint32_t {
            auto* self = static_cast<Impl*>(u);
            if (!self || !self->transport) return 0;
            return self->transport->write_all(d, l) ? l : 0u;
        };
        pop.flush = [](void*) {};

        uproto_time_ops_t to{};
        to.user = nullptr;
        to.now_ms = [](void*) -> std::uint32_t {
            return static_cast<std::uint32_t>(now_ns_steady() / 1000000ULL);
        };

        uproto_config_t cfg{};
        cfg.handshake_timeout_ms = config.handshake_timeout_ms;
        cfg.heartbeat_interval_ms = config.heartbeat_interval_ms;
        cfg.default_timeout_ms = config.default_timeout_ms;
        cfg.default_retries = config.default_retries;
        cfg.enable_auto_handshake = config.auto_handshake;
        cfg.event_cb = &Impl::on_uproto_event;
        cfg.event_user = this;

        uproto_init(&uproto, &pop, &to, &cfg);
        (void)uproto_register_handler(&uproto, UPROTO_MSG_MUX, &Impl::on_mux_trampoline, this);
        (void)uproto_start_handshake(&uproto);

        ts.set_period_ms(config.timesync_period_ms);
        ts.set_initiator(config.timesync_initiator);
        ts.set_max_rtt_us(config.timesync_max_rtt_us);
    }

    bool send_mux(std::uint8_t ch, std::uint16_t sid, const std::uint8_t* payload, std::uint16_t len) {
        if (!open) return false;
        std::uint8_t buf[COMM_MUX_TX_BUFFER_SIZE];
        mux_hdr_t hdr{};
        hdr.sof = MUX_SOF;
        hdr.ver = MUX_VER;
        hdr.channel = ch;
        hdr.len = len;
        hdr.sid = sid;
        hdr.seq = ++seqs[ch];
        std::uint32_t w = mux_encode(buf, sizeof(buf), &hdr, payload);
        if (!w) return false;
        return uproto_send_notify(&uproto, UPROTO_MSG_MUX, buf, w) == UPROTO_OK;
    }

    void handle_mux(const std::uint8_t* data, std::uint32_t len) {
        mux_hdr_t hdr{};
        const std::uint8_t* pl = nullptr;
        if (!mux_decode(data, len, &hdr, &pl)) return;
        if (!pl || hdr.len < 2) return;

        const std::uint16_t sid = comm_read_u16_le(pl);
        const std::uint8_t ch = hdr.channel;

        if (ch == TS_CH_ID && config.timesync_enable) {
            ts.on_mux(pl, hdr.len);
            if (callbacks.on_timesync && ts.version() != ts_version_seen) {
                ts_version_seen = ts.version();
                callbacks.on_timesync(ts.status());
            }
            return;
        }

        if (ch == CAM_CH_ID) {
            if (sid == CAM_SID_EVENT) {
                CameraEvent ev{};
                std::uint32_t pos = 2;
                if (hdr.len >= pos + 4) {
                    ev.frame_id = comm_read_u32_le(pl + pos);
                    pos += 4;
                }
                if (hdr.len >= pos + 8) {
                    ev.device_ts_us = comm_read_u64_le(pl + pos);
                }
                ev.host_ts_ns = now_ns_steady();
                last_camera = ev;
                if (callbacks.on_camera_event) callbacks.on_camera_event(ev);
            } else if (callbacks.on_mux_raw) {
                callbacks.on_mux_raw(ch, sid, pl, hdr.len);
            }
            return;
        }

        if (ch == GIMBAL_CH_ID) {
            if (sid == GIMBAL_SID_STATE) {
                GimbalState st{};
                const std::uint32_t base_words =
                    (config.gimbal_has_encoders ? 2u : 0u) +
                    (config.gimbal_has_imu ? 3u : 0u);
                const std::uint32_t base_bytes = base_words * 4u;
                if (hdr.len < 2 + base_bytes + 8) return;

                const std::uint8_t* p = pl + 2;

                std::uint32_t idx = 0;
                if (config.gimbal_has_encoders) {
                    st.enc_yaw = comm_read_i32_le(p + 4 * idx++);
                    st.enc_pitch = comm_read_i32_le(p + 4 * idx++);
                }
                if (config.gimbal_has_imu) {
                    st.yaw_udeg = comm_read_i32_le(p + 4 * idx++);
                    st.pitch_udeg = comm_read_i32_le(p + 4 * idx++);
                    st.roll_udeg = comm_read_i32_le(p + 4 * idx++);
                }

                const std::uint32_t ts_offset = 2u + base_bytes;
                st.device_ts_us = comm_read_u64_le(pl + ts_offset);

                const std::uint32_t ext_offset = ts_offset + 8u;
                const std::uint32_t ext_bytes = hdr.len > ext_offset ? hdr.len - ext_offset : 0u;
                const std::uint32_t ext_words = ext_bytes / 4u;
                const std::uint8_t* ext = pl + ext_offset;

                idx = 0;
                if (ext_words > idx) st.yaw_cmd_current = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.pitch_cmd_current = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.yaw_meas_current = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.pitch_meas_current = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.gyro_yaw_udeps = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.gyro_pitch_udeps = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.bullet_speed_x100 = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.bullet_count = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.gimbal_mode = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.shoot_state = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.shooter_heat = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.shooter_heat_limit = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.projectile_allowance_17mm = comm_read_i32_le(ext + 4 * idx++);
                if (ext_words > idx) st.projectile_allowance_42mm = comm_read_i32_le(ext + 4 * idx++);
                st.host_ts_ns = now_ns_steady();
                last_gimbal = st;
                if (callbacks.on_gimbal_state) callbacks.on_gimbal_state(st);
                return;
            }
            if (sid == GIMBAL_SID_TFMINI) {
                if (hdr.len < 2 + 2 + 2 + 2 + 2 + 8) return;
                GimbalTfmini tf{};
                tf.distance_cm = comm_read_u16_le(pl + 2);
                tf.strength = comm_read_u16_le(pl + 4);
                tf.temp_cdeg = static_cast<std::int16_t>(comm_read_u16_le(pl + 6));
                tf.status = comm_read_u16_le(pl + 8);
                tf.device_ts_us = comm_read_u64_le(pl + 10);
                tf.host_ts_ns = now_ns_steady();
                last_tfmini = tf;
                if (callbacks.on_gimbal_tfmini) callbacks.on_gimbal_tfmini(tf);
                return;
            }
            if (sid == GIMBAL_SID_DELTA && callbacks.on_gimbal_delta_ack) {
                callbacks.on_gimbal_delta_ack();
                return;
            }
        }

        if (callbacks.on_mux_raw) callbacks.on_mux_raw(ch, sid, pl, hdr.len);
    }
};

Driver::Driver(std::unique_ptr<Transport> transport, Config cfg)
    : impl_(std::make_unique<Impl>(std::move(transport), std::move(cfg))) {}

Driver::~Driver() = default;

Driver::Driver(Driver&&) noexcept = default;
Driver& Driver::operator=(Driver&&) noexcept = default;

bool Driver::open(const std::string& endpoint) {
    if (!impl_ || !impl_->transport) return false;
    if (!impl_->transport->open(endpoint)) return false;
    impl_->open = true;
    impl_->init_uproto();
    return true;
}

void Driver::close() {
    if (!impl_ || !impl_->transport) return;
    impl_->transport->close();
    impl_->open = false;
}

bool Driver::is_open() const {
    return impl_ && impl_->open;
}

bool Driver::established() const {
    if (!impl_) return false;
    return uproto_is_established(&impl_->uproto);
}

void Driver::set_callbacks(Callbacks cb) {
    if (!impl_) return;
    impl_->callbacks = std::move(cb);
}

void Driver::poll(int timeout_ms) {
    if (!impl_ || !impl_->open || !impl_->transport) return;
    (void)impl_->transport->wait_readable(timeout_ms);
    std::uint8_t buf[1024];
    while (true) {
        const long n = impl_->transport->read_some(buf, sizeof(buf));
        if (n > 0) {
            uproto_on_rx_bytes(&impl_->uproto, buf, static_cast<std::uint32_t>(n));
            continue;
        }
        break;
    }
}

void Driver::tick() {
    if (!impl_ || !impl_->open) return;
    uproto_tick(&impl_->uproto);
    if (impl_->config.timesync_enable) impl_->ts.tick();
}

void Driver::step(int timeout_ms) {
    poll(timeout_ms);
    tick();
}

bool Driver::send_gimbal_delta(const GimbalDelta& cmd) {
    if (!impl_) return false;
    std::uint64_t ts_us = 0;
    if (cmd.device_ts_us != 0) {
        ts_us = cmd.device_ts_us;
    } else if (cmd.host_ts_ns != 0) {
        auto mapped = host_ns_to_device_us(cmd.host_ts_ns);
        if (mapped) ts_us = *mapped;
    } else if (impl_->config.timesync_enable && impl_->ts.status().valid) {
        ts_us = impl_->ts.now_device_us();
    }

    std::uint8_t buf[2 + 4 + 4 + 2 + 8];
    comm_write_u16_le(&buf[0], GIMBAL_SID_DELTA);
    comm_write_i32_le(&buf[2], cmd.delta_yaw_udeg);
    comm_write_i32_le(&buf[6], cmd.delta_pitch_udeg);
    comm_write_u16_le(&buf[10], cmd.status);
    comm_write_u64_le(&buf[12], ts_us);
    return send_mux(GIMBAL_CH_ID, GIMBAL_SID_DELTA, buf, sizeof(buf));
}

bool Driver::send_camera_reset() {
    std::uint8_t buf[2];
    comm_write_u16_le(&buf[0], CAM_SID_RESET);
    return send_mux(CAM_CH_ID, CAM_SID_RESET, buf, sizeof(buf));
}

bool Driver::send_mux(std::uint8_t ch, std::uint16_t sid, const std::uint8_t* payload, std::uint16_t len) {
    if (!impl_) return false;
    return impl_->send_mux(ch, sid, payload, len);
}

std::optional<GimbalState> Driver::last_gimbal_state() const {
    if (!impl_) return std::nullopt;
    return impl_->last_gimbal;
}

std::optional<GimbalTfmini> Driver::last_gimbal_tfmini() const {
    if (!impl_) return std::nullopt;
    return impl_->last_tfmini;
}

std::optional<CameraEvent> Driver::last_camera_event() const {
    if (!impl_) return std::nullopt;
    return impl_->last_camera;
}

TimeSyncStatus Driver::time_sync_status() const {
    if (!impl_) return {};
    return impl_->ts.status();
}

std::optional<std::uint64_t> Driver::host_ns_to_device_us(std::uint64_t host_ns) const {
    if (!impl_) return std::nullopt;
    return impl_->ts.host_ns_to_device_us(host_ns);
}

std::optional<std::uint64_t> Driver::device_us_to_host_ns(std::uint64_t device_us) const {
    if (!impl_) return std::nullopt;
    return impl_->ts.device_us_to_host_ns(device_us);
}

} // namespace communication::dm_02
