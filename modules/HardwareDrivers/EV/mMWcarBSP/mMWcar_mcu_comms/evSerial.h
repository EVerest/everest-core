/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#ifndef MMWCAR_MCU_COMMS_EV_SERIAL_H
#define MMWCAR_MCU_COMMS_EV_SERIAL_H

#include "mMWcar.pb.h"
#include <atomic>
#include <chrono>
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

    void read_thread();
    void run();

    bool soft_reset();
    bool get_ac_data_instant();
    bool get_ac_statistic();
    bool get_calibration_values();
    bool set_calibration_values(CalibrationValues& vals);
    bool request_ac_calibration(CalibrationRequest& req);
    bool set_cp_state(CpState cp_state);
    bool get_adc_raw(Adc_Raw& req);
    bool get_dc_data_instant();
    bool hard_reset(const std::string& reset_chip, const int reset_line);

    sigslot::signal<> signal_connection_timeout;
    sigslot::signal<KeepAlive> signal_keep_alive;
    sigslot::signal<ACMeasInstant> signal_ac_meas_instant;
    sigslot::signal<ACMeasStats> signal_ac_stats;
    sigslot::signal<CalibrationValues> signal_calibration_values;
    sigslot::signal<CalibrationResponse> signal_calibration_response;
    sigslot::signal<BasicChargingCommMeasInstant> signal_cp_pp_meas;
    sigslot::signal<Adc_Raw> signal_adc_raw_response;
    sigslot::signal<DCMeasInstant> signal_dc_meas_instant;

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

#endif // MMWCAR_MCU_COMMS_EV_SERIAL_H
