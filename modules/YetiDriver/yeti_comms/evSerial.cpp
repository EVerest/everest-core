// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
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

#include <gpio.hpp>

#include "yeti.pb.h"

evSerial::evSerial() {
    fd = 0;
    baud = 0;
    reset_done_flag = false;
    forced_reset = false;
    cobsDecodeReset();
}

evSerial::~evSerial() {
    if (fd)
        close(fd);
}

bool evSerial::openDevice(const char* device, int _baud) {

    fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Serial: error %d opening %s: %s\n", errno, device, strerror(errno));
        return false;
    } // else printf ("Serial: opened %s as %i\n", device, fd);
    cobsDecodeReset();

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

    return setSerialAttributes();
}

bool evSerial::setSerialAttributes() {
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
    // printf ("Success setting tcsetattr\n");
    return true;
}

void evSerial::cobsDecodeReset() {
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
    // printf("%X",crc);
    return crc;
}

void evSerial::handlePacket(uint8_t* buf, int len) {
    // printf ("packet received len %u\n", len);

    // Check CRC32 (last 4 bytes)
    //  uint32_t crc = calculateCrc(rx_packet_buf, rx_packet_len);
    if (crc32(buf, len)) {
        printf("CRC mismatch\n");
        return;
    }

    len -= 4;

    McuToEverest msg_in;
    pb_istream_t istream = pb_istream_from_buffer(buf, len);

    if (pb_decode(&istream, McuToEverest_fields, &msg_in))
        switch (msg_in.which_payload) {

        case McuToEverest_keep_alive_tag:
            // printf("Received keep_alive_lo\n");
            signalKeepAliveLo(msg_in.payload.keep_alive);
            // detect connection timeout if keep_alive packets stop coming...
            last_keep_alive_lo_timestamp = date::utc_clock::now();
            break;
        case McuToEverest_power_meter_tag: {
            auto unix_timestamp = std::chrono::seconds(std::time(NULL));
            msg_in.payload.power_meter.time_stamp = unix_timestamp.count();
            signalPowerMeter(msg_in.payload.power_meter);
        } break;
        case McuToEverest_cp_state_tag:
            signalCPState(msg_in.payload.cp_state);
            break;
        case McuToEverest_pp_state_tag:
            signalPPState(msg_in.payload.pp_state);
            break;
        case McuToEverest_relais_state_tag:
            signalRelaisState(msg_in.payload.relais_state);
            break;
        case McuToEverest_lock_state_tag:
            signalLockState(msg_in.payload.lock_state);
            break;
        case McuToEverest_error_flags_tag:
            signalErrorFlags(msg_in.payload.error_flags);
            break;
        case McuToEverest_reset_tag:
            // printf("Received reset_done\n");
            reset_done_flag = true;
            if (!forced_reset)
                signalSpuriousReset();
            break;
        }
}

void evSerial::cobsDecode(uint8_t* buf, int len) {
    for (int i = 0; i < len; i++)
        cobsDecodeByte(buf[i]);
}

void evSerial::cobsDecodeByte(uint8_t byte) {
    // check max length
    if ((decode - msg == 2048 - 1) && byte != 0x00) {
        printf("cobsDecode: Buffer overflow\n");
        cobsDecodeReset();
    }

    if (block) {
        // we're currently decoding and should not get a 0
        if (byte == 0x00) {
            // probably found some garbage -> reset
            printf("cobsDecode: Garbage detected\n");
            cobsDecodeReset();
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
                handlePacket(msg, decode - 1 - msg);
            }
            cobsDecodeReset();
            return; // need to return here, because of block--
        }
    }
    block--;
}

void evSerial::run() {
    readThreadHandle = std::thread(&evSerial::readThread, this);
    timeoutDetectionThreadHandle = std::thread(&evSerial::timeoutDetectionThread, this);
}

void evSerial::timeoutDetectionThread() {
    while (true) {
        sleep(1);
        if (timeoutDetectionThreadHandle.shouldExit())
            break;
        if (serial_timed_out())
            signalConnectionTimeout();
    }
}

void evSerial::readThread() {
    uint8_t buf[2048];
    int n;

    cobsDecodeReset();
    while (true) {
        if (readThreadHandle.shouldExit())
            break;
        n = read(fd, buf, sizeof buf);
        // printf ("read %u bytes.\n", n);
        cobsDecode(buf, n);
    }
}

bool evSerial::linkWrite(EverestToMcu* m) {
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

    size_t tx_encode_len = cobsEncode(tx_packet_buf, tx_payload_len, encode_buf);
    // std::cout << "Write "<<tx_encode_len<<" bytes to serial port." << std::endl;
    write(fd, encode_buf, tx_encode_len);
    return true;
}

size_t evSerial::cobsEncode(const void* data, size_t length, uint8_t* buffer) {
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
    auto timeSinceLastKeepAlive =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_keep_alive_lo_timestamp).count();
    if (timeSinceLastKeepAlive >= 5000)
        return true;
    return false;
}

void evSerial::setPWM(uint32_t dc) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_pwm_duty_cycle_tag;
    msg_out.payload.pwm_duty_cycle = dc;
    linkWrite(&msg_out);
}

void evSerial::allowPowerOn(bool p) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_allow_power_on_tag;
    msg_out.payload.allow_power_on = p;
    linkWrite(&msg_out);
}

void evSerial::forceUnlock() {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_connector_lock_tag;
    msg_out.payload.connector_lock = false;
    linkWrite(&msg_out);
}

bool evSerial::reset(const std::string& reset_chip, const int reset_line) {

    reset_done_flag = false;
    forced_reset = true;

    if (not reset_chip.empty()) {
        // Try to hardware reset Yeti controller to be in a known state
        Everest::Gpio reset_gpio;
        reset_gpio.open(reset_chip, reset_line);
        reset_gpio.set_output(true);
        reset_gpio.set(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        reset_gpio.set(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        reset_gpio.set(true);
    } else {
        // Try to soft reset Yeti controller to be in a known state
        EverestToMcu msg_out = EverestToMcu_init_default;
        msg_out.which_payload = EverestToMcu_reset_tag;
        linkWrite(&msg_out);
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
    keepAlive();
    keepAlive();
    keepAlive();

    return success;
}

void evSerial::firmwareUpdate(bool rom) {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_firmware_update_tag;
    msg_out.payload.firmware_update.invoke_rom_bootloader = rom;
    linkWrite(&msg_out);
}

void evSerial::keepAlive() {
    EverestToMcu msg_out = EverestToMcu_init_default;
    msg_out.which_payload = EverestToMcu_keep_alive_tag;
    msg_out.payload.keep_alive.time_stamp = 0;
    msg_out.payload.keep_alive.hw_type = 0;
    msg_out.payload.keep_alive.hw_revision = 0;
    strcpy(msg_out.payload.keep_alive.sw_version_string, "n/a");
    linkWrite(&msg_out);
}
