// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

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
#include <everest/logging.hpp>

#include "phyverso.pb.h"

#include "bsl_gpio.h"

namespace {
    LedState led_state_from_string(const std::string& led_state) {
        if (led_state == "Red")
            return LedState_RED;
        if (led_state == "Blue")
            return LedState_BLUE;
        if (led_state == "Green")
            return LedState_GREEN;
        return LedState_RED;
    }
}

evSerial::evSerial(evConfig& _verso_config) :
    fd(0), baud(0), reset_done_flag(false), forced_reset(false), verso_config(_verso_config) {
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

    if (handle_McuToEverest_packet(buf, len))
        return;
    else if (handle_OpaqueData_packet(buf, len))
        return;
    else
        printf("Cannot handle a packet");
}

bool evSerial::handle_McuToEverest_packet(uint8_t* buf, int len) {
    McuToEverest msg_in;
    pb_istream_t istream = pb_istream_from_buffer(buf, len);

    if (!pb_decode(&istream, McuToEverest_fields, &msg_in))
        return false;

    switch (msg_in.which_payload) {

    case McuToEverest_keep_alive_tag:
        signal_keep_alive(msg_in.payload.keep_alive);
        break;

    case McuToEverest_cp_state_tag:
        signal_cp_state(msg_in.connector, msg_in.payload.cp_state);
        break;

    case McuToEverest_set_coil_state_response_tag:
        signal_set_coil_state_response(msg_in.connector, msg_in.payload.set_coil_state_response);
        break;

    case McuToEverest_error_flags_tag:
        signal_error_flags(msg_in.connector, msg_in.payload.error_flags);
        break;

    case McuToEverest_telemetry_tag:
        signal_telemetry(msg_in.connector, msg_in.payload.telemetry);
        break;

    case McuToEverest_reset_tag:
        reset_done_flag = true;
        if (!forced_reset)
            signal_spurious_reset(msg_in.payload.reset);
        break;

    case McuToEverest_pp_state_tag:
        signal_pp_state(msg_in.connector, msg_in.payload.pp_state);
        break;

    case McuToEverest_fan_state_tag:
        signal_fan_state(msg_in.payload.fan_state);
        break;

    case McuToEverest_lock_state_tag:
        signal_lock_state(msg_in.connector, msg_in.payload.lock_state);
        break;

    case McuToEverest_config_request_tag:
        signal_config_request();
        break;
    }

    return true;
}

bool evSerial::handle_OpaqueData_packet(uint8_t* buf, int len) {
    OpaqueData data = OpaqueData_init_default;
    pb_istream_t istream = pb_istream_from_buffer(buf, len);
    if (!pb_decode(&istream, OpaqueData_fields, &data))
        return false;
    EVLOG_debug << "Received chunk " << data.id << " " << data.chunks_total << " " << data.chunk_current << " "
                << data.data_count;

    // Lambda for updating OpaqueDataHandler - here just to simplify the return
    // logic.
    [this](const OpaqueData& data) {
        auto iter = opaque_handlers.find(data.connector);
        if (iter == opaque_handlers.end()) {
            // The item does not exist - try to insert it.
            try {
                iter = opaque_handlers.emplace(data.connector, data).first;
            } catch (...) {
                // We can't do anything here - the chunk is ill formed and does
                // not start with 0.
                EVLOG_debug << "Stray chunk " << data.id << "; Ignoring";
                return;
            }
        } else {
            try {
                iter->second.insert(data);
            } catch (...) {
                // We've failed to insert the data.. Drop the current buffer.
                EVLOG_debug << "Stray chunk " << data.id << "; Resetting";
                opaque_handlers.erase(iter);
                return;
            }
        }
        if (!iter->second.is_complete())
            return;

        const auto readings = iter->second.get_data();
        EVLOG_debug << "Received sensor data with the size " << readings.size();
        signal_opaque_data(iter->first, readings);
        opaque_handlers.erase(iter);
    }(data);

    return true;
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
        // send keep alive
        keep_alive();
    }
}

void evSerial::read_thread() {
    uint8_t buf[2048];
    int n;

    cobs_decode_reset();
    while (true) {
        if (read_thread_handle.shouldExit())
            break;
        if (fd > 0) {
            n = read(fd, buf, sizeof buf);
            cobs_decode(buf, n);
        }
    }
}

bool evSerial::link_write(EverestToMcu* m) {
    if (fd <= 0) {
        return false;
    }

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

void evSerial::set_pwm(int target_connector, uint32_t duty_cycle_e2) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_pwm_duty_cycle_tag;
    msg_out.payload.pwm_duty_cycle = duty_cycle_e2;
    msg_out.connector = target_connector;
    link_write(&msg_out);
}

void evSerial::set_coil_state_request(int target_connector, CoilType type, bool power_on) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_set_coil_state_request_tag;
    msg_out.payload.set_coil_state_request.coil_type = type;
    msg_out.payload.set_coil_state_request.coil_state = power_on;
    msg_out.connector = target_connector;
    link_write(&msg_out);
}

void evSerial::set_led_state(int target_connector, const std::string& color, uint8_t brightness) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_set_led_state_tag;
    msg_out.payload.set_led_state.brightness = brightness;
    msg_out.payload.set_led_state.led_state = led_state_from_string(color);
    msg_out.connector = target_connector;
    link_write(&msg_out);
}


void evSerial::lock(int target_connector, bool _lock) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_connector_lock_tag;
    msg_out.payload.connector_lock = _lock;
    msg_out.connector = target_connector;
    link_write(&msg_out);
}

void evSerial::firmware_update() {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_firmware_update_tag;
    msg_out.connector = 0;
    link_write(&msg_out);
}

bool evSerial::reset(const int reset_pin) {

    reset_done_flag = false;
    forced_reset = true;

    if (reset_pin > 0) {
        printf("Hard reset\n");
        auto bsl_gpio = BSL_GPIO({.bank = 1, .pin = 12}, // BSL pins are unused here so keep defaults
                                 {.bank = static_cast<uint8_t>(verso_config.conf.reset_gpio_bank),
                                  .pin = static_cast<uint8_t>(verso_config.conf.reset_gpio_pin)});
        bsl_gpio.hard_reset();
    } else {
        // Try to soft reset phyVERSO controller to be in a known state
        EverestToMcu msg_out = EverestToMcu_init_default;
        msg_out.which_payload = EverestToMcu_reset_tag;
        msg_out.connector = 0;
        link_write(&msg_out);
    }

    bool success = false;

    // Wait for reset done message from uC
    for (int i = 0; i < 20; i++) {
        if (reset_done_flag) {
            success = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Reset flag to detect run time spurious resets of uC from now on
    reset_done_flag = false;
    forced_reset = false;

    // send some dummy packets to resync COBS etc.
    keep_alive();
    keep_alive();
    keep_alive();

    return success;
}

void evSerial::keep_alive() {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_keep_alive_tag;
    msg_out.payload.keep_alive.time_stamp = 0;
    msg_out.payload.keep_alive.hw_type = 0;
    msg_out.payload.keep_alive.hw_revision = 0;
    strcpy(msg_out.payload.keep_alive.sw_version_string, "n/a");
    msg_out.connector = 0;
    link_write(&msg_out);
}

void evSerial::send_config() {
    EverestToMcu config_packet = verso_config.get_config_packet();
    link_write(&config_packet);
}

void evSerial::set_fan_state(uint8_t fan_id, bool enabled, uint32_t duty) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_set_fan_state_tag;
    msg_out.payload.set_fan_state.fan_id = fan_id;
    msg_out.payload.set_fan_state.enabled = enabled;
    msg_out.payload.set_fan_state.duty = duty;
    msg_out.connector = 0;
    link_write(&msg_out);
}

void evSerial::set_rcd_test(int target_connector, bool _test) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_rcd_cmd_tag;
    msg_out.payload.rcd_cmd.test = _test;
    msg_out.payload.rcd_cmd.reset = false;
    msg_out.connector = target_connector;
    link_write(&msg_out);
}

void evSerial::reset_rcd(int target_connector, bool _reset) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_rcd_cmd_tag;
    msg_out.payload.rcd_cmd.test = false;
    msg_out.payload.rcd_cmd.reset = _reset;
    msg_out.connector = target_connector;
    link_write(&msg_out);
}