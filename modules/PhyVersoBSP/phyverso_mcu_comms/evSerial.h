// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef YETI_SERIAL
#define YETI_SERIAL

#include "phyverso.pb.h"
#include <date/date.h>
#include <date/tz.h>
#include <sigslot/signal.hpp>
#include <stdint.h>
#include <termios.h>
#include <utils/thread.hpp>

class evSerial {

public:
    evSerial();
    ~evSerial();

    bool open_device(const char* device, int baud);

    void readThread();
    void run();

    bool reset(const int reset_pin);
    void firmware_update();
    void keep_alive();

    void set_pwm(int target_connector, uint32_t duty_cycle_e2);
    void allow_power_on(int target_connector, bool p);
    void lock(int target_connector, bool _lock);
    void unlock(int target_connector);

    sigslot::signal<KeepAlive> signal_keep_alive;
    sigslot::signal<int, CpState> signal_cp_state;
    sigslot::signal<int, bool> signal_relais_state;
    sigslot::signal<int, ErrorFlags> signal_error_flags;
    sigslot::signal<int, Telemetry> signal_telemetry;
    sigslot::signal<ResetReason> signal_spurious_reset;
    sigslot::signal<> signal_connection_timeout;
    sigslot::signal<int, PpState> signal_pp_state;
    sigslot::signal<FanState> signal_fan_state;
    sigslot::signal<int, LockState> signal_lock_state;

private:
    // Serial interface
    bool set_serial_attributes();
    int fd;
    int baud;

    // COBS de-/encoder
    void cobs_decode_reset();
    void handle_packet(uint8_t* buf, int len);
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
};

#endif
