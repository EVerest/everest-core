// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef PHYVERSO_MCU_COMMS_EV_SERIAL_H
#define PHYVERSO_MCU_COMMS_EV_SERIAL_H

#include "evConfig.h"
#include "phyverso.pb.h"
#include <atomic>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <sigslot/signal.hpp>
#include <stdexcept>
#include <stdint.h>
#include <termios.h>
#include <utility>
#include <utils/thread.hpp>
#include <vector>

class evSerial {

public:
    evSerial(evConfig& _verso_config);
    ~evSerial();

    bool open_device(const char* device, int baud);
    bool is_open() {
        return fd > 0;
    };
    void flush_buffers();

    void read_thread();
    void run();

    bool reset(const int reset_pin);
    void firmware_update();
    void keep_alive();

    void set_pwm(int target_connector, uint32_t duty_cycle_e2);
    void set_coil_state_request(int target_connector, CoilType type, bool power_on);
    void lock(int target_connector, bool _lock);
    void unlock(int target_connector);
    void set_fan_state(uint8_t fan_id, bool enabled, uint32_t duty);
    void set_rcd_test(int target_connector, bool _test);
    void reset_rcd(int target_connector, bool _reset);

    void send_config();

    sigslot::signal<KeepAlive> signal_keep_alive;
    sigslot::signal<int, CpState> signal_cp_state;
    sigslot::signal<int, CoilState> signal_set_coil_state_response;
    sigslot::signal<int, ErrorFlags> signal_error_flags;
    sigslot::signal<int, Telemetry> signal_telemetry;
    sigslot::signal<ResetReason> signal_spurious_reset;
    sigslot::signal<> signal_connection_timeout;
    sigslot::signal<int, PpState> signal_pp_state;
    sigslot::signal<FanState> signal_fan_state;
    sigslot::signal<int, LockState> signal_lock_state;
    sigslot::signal<> signal_config_request;

private:
    // Serial interface
    bool set_serial_attributes();
    int fd;
    int baud;

    // COBS de-/encoder
    void cobs_decode_reset();
    void handle_packet(uint8_t* buf, int len);
    bool handle_McuToEverest_packet(uint8_t* buf, int len);
    void cobs_decode(uint8_t* buf, int len);
    void cobs_decode_byte(uint8_t byte);
    size_t cobs_encode(const void* data, size_t length, uint8_t* buffer);
    uint8_t msg[2048];
    uint8_t code;
    uint8_t block;
    uint8_t* decode;
    uint32_t crc32(uint8_t* buf, int len);

    // Read thread for serial port
    Everest::Thread read_thread_handle;
    Everest::Thread timeout_detection_thread_handle;

    bool link_write(EverestToMcu* m);
    std::atomic_bool reset_done_flag;
    std::atomic_bool forced_reset;

    bool serial_timed_out();
    void timeout_detection_thread();
    std::chrono::time_point<date::utc_clock> last_keep_alive_lo_timestamp;

    // config bridge (filled by json or everest module config)
    evConfig& verso_config;
};

#endif // PHYVERSO_MCU_COMMS_EV_SERIAL_H
