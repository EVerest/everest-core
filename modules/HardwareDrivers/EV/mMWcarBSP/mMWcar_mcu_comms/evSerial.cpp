// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evSerial.h"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <fcntl.h>
#include <unistd.h>

#include <date/date.h>
#include <date/tz.h>

#include <everest/3rd_party/nanopb/pb_decode.h>
#include <everest/3rd_party/nanopb/pb_encode.h>

#include <everest/gpio/gpio.hpp>

#include "mMWcar.pb.h"

evSerial::evSerial() : fd(0), baud(0), reset_done_flag(false), forced_reset(false) {
    cobs_decode_reset();
}

evSerial::~evSerial() {
    if (fd) {
        close(fd);
    }
}

bool evSerial::open_device(const char* device, int _baud) {

    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Serial: error %d opening %s: %s\n", errno, device, strerror(errno));
        return false;
    } // else printf ("Serial: opened %s as %i\n", device, fd);
    cobs_decode_reset();

    switch (_baud) {
    case 9600:
        baud = B9600;
        break;
    case 19200:
        baud = B19200;
        break;
    case 38400:
        baud = B38400;
        break;
    case 57600:
        baud = B57600;
        break;
    case 115200:
        baud = B115200;
        break;
    case 230400:
        baud = B230400;
        break;
    default:
        baud = 0;
        return false;
    }

    return set_serial_attributes();
}

bool evSerial::set_serial_attributes() {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        printf("Serial: error %d from tcgetattr\n", errno);
        return false;
    }

    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);
    tty.c_lflag = 0;     // no signaling chars, no echo,
                         // no canonical processing
    tty.c_oflag = 0;     // no remapping, no delays
    tty.c_cc[VMIN] = 0;  // read blocks
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Serial: error %d from tcsetattr\n", errno);
        return false;
    }
    return true;
}

void evSerial::cobs_decode_reset() {
    code = 0xff;
    block = 0;
    decode = msg;
}

uint32_t evSerial::crc32(uint8_t* buf, int len) {
    int i, j;
    uint32_t b, crc, msk;

    i = 0;
    crc = 0xFFFFFFFF;
    while (i < len) {
        b = buf[i];
        crc = crc ^ b;
        for (j = 7; j >= 0; j--) {
            msk = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & msk);
        }
        i = i + 1;
    }
    return crc;
}

void evSerial::handle_packet(uint8_t* buf, int len) {

    if (crc32(buf, len)) {
        printf("CRC mismatch\n");
        return;
    }

    len -= 4;

    McuToEverest msg_in;
    pb_istream_t istream = pb_istream_from_buffer(buf, len);

    if (pb_decode(&istream, McuToEverest_fields, &msg_in)) {
        switch (msg_in.which_payload) {

        case McuToEverest_keep_alive_tag:
            signal_keep_alive(msg_in.payload.keep_alive);
            break;

        case McuToEverest_ac_meas_instant_tag:
            signal_ac_meas_instant(msg_in.payload.ac_meas_instant);
            break;

        case McuToEverest_ac_meas_statistics_tag:
            signal_ac_stats(msg_in.payload.ac_meas_statistics);
            break;

        case McuToEverest_calibration_values_tag:
            signal_calibration_values(msg_in.payload.calibration_values);
            break;

        case McuToEverest_response_calibration_tag:
            signal_calibration_response(msg_in.payload.response_calibration);
            break;

        case McuToEverest_basic_charging_meas_instant_tag:
            signal_cp_pp_meas(msg_in.payload.basic_charging_meas_instant);
            break;

        case McuToEverest_adc_raw_response_tag:
            signal_adc_raw_response(msg_in.payload.adc_raw_response);
            break;

        case McuToEverest_dc_meas_instant_tag:
            signal_dc_meas_instant(msg_in.payload.dc_meas_instant);
            break;

        case McuToEverest_sweep_cp_response_tag:
            signal_sweep_cp_response(msg_in.payload.sweep_cp_response);
            break;

        case McuToEverest_edge_timing_statistics_tag:
            signal_edge_timing_statistics(msg_in.payload.edge_timing_statistics);
            break;

        default:
            break;
        }
    }
}

void evSerial::cobs_decode(uint8_t* buf, int len) {
    for (int i = 0; i < len; i++)
        cobs_decode_byte(buf[i]);
}

void evSerial::cobs_decode_byte(uint8_t byte) {
    // check max length
    if ((decode - msg == 2048 - 1) && byte != 0x00) {
        printf("cobsDecode: Buffer overflow\n");
        cobs_decode_reset();
    }

    if (block) {
        // we're currently decoding and should not get a 0
        if (byte == 0x00) {
            // probably found some garbage -> reset
            printf("cobsDecode: Garbage detected\n");
            cobs_decode_reset();
            return;
        }
        *decode++ = byte;
    } else {
        if (code != 0xff) {
            *decode++ = 0;
        }
        block = code = byte;
        if (code == 0x00) {
            // we're finished, reset everything and commit
            if (decode == msg) {
                // we received nothing, just a 0x00
                printf("cobsDecode: Received nothing\n");
            } else {
                // set back decode with one, as it gets post-incremented
                handle_packet(msg, decode - 1 - msg);
            }
            cobs_decode_reset();
            return; // need to return here, because of block--
        }
    }
    block--;
}

void evSerial::run() {
    read_thread_handle = std::thread(&evSerial::read_thread, this);
    timeout_detection_thread_handle = std::thread(&evSerial::timeout_detection_thread, this);
}

void evSerial::timeout_detection_thread() {
    while (true) {
        sleep(1);
        if (timeout_detection_thread_handle.shouldExit())
            break;
        if (serial_timed_out())
            signal_connection_timeout();
    }
}

void evSerial::read_thread() {
    uint8_t buf[2048];
    int n;

    cobs_decode_reset();
    while (true) {
        if (read_thread_handle.shouldExit())
            break;
        n = read(fd, buf, sizeof buf);
        // printf ("read %u bytes.\n", n);
        cobs_decode(buf, n);
    }
}

bool evSerial::link_write(EverestToMcu* m) {
    uint8_t tx_packet_buf[1024];
    uint8_t encode_buf[1500];
    pb_ostream_t ostream = pb_ostream_from_buffer(tx_packet_buf, sizeof(tx_packet_buf) - 4);
    bool status = pb_encode(&ostream, EverestToMcu_fields, m);

    if (!status) {
        // couldn't encode
        return false;
    }

    size_t tx_payload_len = ostream.bytes_written;

    // add crc32 (CRC-32/JAMCRC)
    uint32_t crc = crc32(tx_packet_buf, tx_payload_len);

    for (int byte_pos = 0; byte_pos < 4; ++byte_pos) {
        tx_packet_buf[tx_payload_len] = (uint8_t)crc & 0xFF;
        crc = crc >> 8;
        tx_payload_len++;
    }

    size_t tx_encode_len = cobs_encode(tx_packet_buf, tx_payload_len, encode_buf);
    write(fd, encode_buf, tx_encode_len);
    return true;
}

size_t evSerial::cobs_encode(const void* data, size_t length, uint8_t* buffer) {
    uint8_t* encode = buffer;  // Encoded byte pointer
    uint8_t* codep = encode++; // Output code pointer
    uint8_t code = 1;          // Code value

    for (const uint8_t* byte = (const uint8_t*)data; length--; ++byte) {
        if (*byte) // Byte not zero, write it
            *encode++ = *byte, ++code;

        if (!*byte || code == 0xff) // Input is zero or block completed, restart
        {
            *codep = code, code = 1, codep = encode;
            if (!*byte || length)
                ++encode;
        }
    }
    *codep = code; // Write final code value

    // add final 0
    *encode++ = 0x00;

    return encode - buffer;
}

bool evSerial::serial_timed_out() {
    auto now = date::utc_clock::now();
    auto time_since_last_keep_alive =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_keep_alive_lo_timestamp).count();
    if (time_since_last_keep_alive >= 5000)
        return true;
    return false;
}

bool evSerial::soft_reset() {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_reset_tag;
    msg_out.payload.reset = true;

    return link_write(&msg_out);
}

bool evSerial::get_ac_data_instant() {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_request_measurement_tag;
    msg_out.payload.request_measurement.type = MeasRequestType_AC_INSTANT;
    return link_write(&msg_out);
}

bool evSerial::get_ac_statistic() {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_request_measurement_tag;
    msg_out.payload.request_measurement.type = MeasRequestType_AC_STATISTICS;
    return link_write(&msg_out);
}

bool evSerial::get_calibration_values() {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_get_calibration_tag;
    msg_out.payload.get_calibration = true;
    return link_write(&msg_out);
}

bool evSerial::set_calibration_values(CalibrationValues& vals) {
    EverestToMcu msg_out;

    msg_out.which_payload = EverestToMcu_set_calibration_tag;
    msg_out.payload.set_calibration = vals;

    return link_write(&msg_out);
}

bool evSerial::request_ac_calibration(CalibrationRequest& req) {
    EverestToMcu msg_out;

    msg_out.which_payload = EverestToMcu_request_calibration_tag;
    msg_out.payload.request_calibration = req;

    return link_write(&msg_out);
}

bool evSerial::set_cp_state(CpState cp_state) {
    EverestToMcu msg_out;

    msg_out.which_payload = EverestToMcu_set_cp_state_tag;
    msg_out.payload.set_cp_state = cp_state;

    return link_write(&msg_out);
}

bool evSerial::get_adc_raw(Adc_Raw& req) {
    EverestToMcu msg_out;

    msg_out.which_payload = EverestToMcu_get_adc_raw_tag;
    msg_out.payload.get_adc_raw.channel = req.channel;

    return link_write(&msg_out);
}

bool evSerial::get_dc_data_instant() {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_request_measurement_tag;
    msg_out.payload.request_measurement.type = MeasRequestType_DC_INSTANT;
    return link_write(&msg_out);
}

bool evSerial::hard_reset(const std::string& reset_chip, const int reset_line) {
    Everest::Gpio reset_gpio;
    bool success = true;

    if (!reset_gpio.open(reset_chip, reset_line)) {
        printf("Could not open reset_gpio!\n");
        success = false;
    }

    if (!reset_gpio.is_ready()) {
        printf("reset_gpio not ready\n");
        success = false;
    }

    if (!reset_gpio.set_output(true)) {
        printf("Could not change reset_gpio direction\n");
        success = false;
    }

    reset_gpio.set(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    reset_gpio.set(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    reset_gpio.set(true);

    // TODO: do some proper checking if uC resetted properly
    reset_gpio.close_all();
    return success;
}

bool evSerial::sweep_cp() {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_sweep_cp_tag;
    msg_out.payload.sweep_cp = true;
    return link_write(&msg_out);
}

bool evSerial::set_cp_lut(CpLUT& lut) {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_set_cp_lut_tag;
    msg_out.payload.set_cp_lut = lut;
    return link_write(&msg_out);
}

bool evSerial::set_cp_voltage(float voltage) {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_set_cp_voltage_tag;
    msg_out.payload.set_cp_voltage = voltage;
    return link_write(&msg_out);
}

bool evSerial::set_cp_load_en(bool& state) {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_set_cp_load_en_tag;
    msg_out.payload.set_cp_load_en = state;
    return link_write(&msg_out);
}

bool evSerial::set_cp_short_to_gnd_en(bool& state) {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_set_cp_short_to_gnd_en_tag;
    msg_out.payload.set_cp_short_to_gnd_en = state;
    return link_write(&msg_out);
}

bool evSerial::set_cp_diode_fault_en(bool& state) {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_set_cp_diode_fault_en_tag;
    msg_out.payload.set_cp_diode_fault_en = state;
    return link_write(&msg_out);
}

bool evSerial::trigger_edge_timing_measurement(int& num_periods, bool& force_start) {
    EverestToMcu msg_out;
    msg_out.which_payload = EverestToMcu_request_measurement_tag;
    msg_out.payload.request_measurement.type = MeasRequestType_EDGE_TIMING;
    msg_out.payload.request_measurement.which_arguments = MeasRequest_edge_timing_tag;
    msg_out.payload.request_measurement.arguments.edge_timing.num_periods = num_periods;
    msg_out.payload.request_measurement.arguments.edge_timing.force_start = force_start;
    return link_write(&msg_out);
}
