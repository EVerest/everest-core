// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "evSerial.h"
#include "common.pb.h"
#include "hi2lo.pb.h"
#include "lo2hi.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include <fstream>
#include <iostream>
#include <string>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <unistd.h>

#include <thread>

#include <date/date.h>
#include <date/tz.h>

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

    LoToHi msg_in;
    pb_istream_t istream = pb_istream_from_buffer(buf, len);

    if (pb_decode(&istream, LoToHi_fields, &msg_in))
        switch (msg_in.which_payload) {

        case LoToHi_state_update_tag:
            // printf("Received state_update\n");
            signalStateUpdate(msg_in.payload.state_update);
            break;
        case LoToHi_debug_update_tag:
            // printf("Received debug_update\n");
            signalDebugUpdate(msg_in.payload.debug_update);
            break;
        case LoToHi_keep_alive_tag:
            // printf("Received keep_alive_lo\n");
            signalKeepAliveLo(msg_in.payload.keep_alive);
            // detect connection timeout if keep_alive packets stop coming...
            last_keep_alive_lo_timestamp = date::utc_clock::now();
            break;
        case LoToHi_power_meter_tag:
            // printf("Received power_meter\n");
            // FIXME: in the long run we could set the clock in the uC. But for now we just fix the timestamp here...
            {
                auto unix_timestamp = std::chrono::seconds(std::time(NULL));
                msg_in.payload.power_meter.time_stamp = unix_timestamp.count();
            }
            signalPowerMeter(msg_in.payload.power_meter);
            break;
        case LoToHi_simulation_feedback_tag:
            // printf("Received power_meter\n");
            signalSimulationFeedback(msg_in.payload.simulation_feedback);
            break;
        case LoToHi_event_tag:
            // printf("Received event %i\n",msg_in.payload.event);
            signalEvent(msg_in.payload.event);
            break;
        case LoToHi_reset_done_tag:
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
    readThreadHandle.handle = std::thread(&evSerial::readThread, this);
    timeoutDetectionThreadHandle.handle = std::thread(&evSerial::timeoutDetectionThread, this);
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

bool evSerial::linkWrite(HiToLo* m) {
    uint8_t tx_packet_buf[1024];
    uint8_t encode_buf[1500];
    pb_ostream_t ostream = pb_ostream_from_buffer(tx_packet_buf, sizeof(tx_packet_buf) - 4);
    bool status = pb_encode(&ostream, HiToLo_fields, m);

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

void evSerial::setMaxCurrent(float c) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_max_current_tag;
    msg_out.payload.set_max_current.ampere = c;
    linkWrite(&msg_out);
}

void evSerial::setThreePhases(bool n) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_three_phases_tag;
    msg_out.payload.set_three_phases.n = n;
    linkWrite(&msg_out);
}

void evSerial::enableRCD(bool e) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_enable_rcd_tag;
    msg_out.payload.enable_rcd.e = e;
    linkWrite(&msg_out);
}

void evSerial::setHasVentilation(bool v) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_has_ventilation_tag;
    msg_out.payload.set_has_ventilation.v = v;
    linkWrite(&msg_out);
}

void evSerial::setCountryCode(const char* iso3166_alpha2_code) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_country_code_tag;
    if (strlen(iso3166_alpha2_code) < 10) {
        strcpy(msg_out.payload.set_country_code.iso3166_alpha2_code, iso3166_alpha2_code);
        linkWrite(&msg_out);
    }
}

void evSerial::setControlMode(InterfaceControlMode c) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_control_mode_tag;
    msg_out.payload.set_control_mode.control_mode = c;
    linkWrite(&msg_out);
}

void evSerial::setAuth(const char* userid) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_auth_tag;
    if (strlen(userid) < 50) {
        strcpy(msg_out.payload.set_auth.userid, userid);
        linkWrite(&msg_out);
    }
}

void evSerial::setPWM(uint32_t mode, float dc) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_set_pwm_tag;
    msg_out.payload.set_pwm.mode = mode;
    msg_out.payload.set_pwm.duty_cycle = dc;
    linkWrite(&msg_out);
}

void evSerial::allowPowerOn(bool p) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_allow_power_on_tag;
    msg_out.payload.allow_power_on.p = p;
    linkWrite(&msg_out);
}

bool evSerial::forceUnlock() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_force_unlock_tag;
    linkWrite(&msg_out);
    return true; // FIXME: actuall return success value from uC
}

void evSerial::enable() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_enable_tag;
    linkWrite(&msg_out);
}

void evSerial::disable() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_disable_tag;
    linkWrite(&msg_out);
}

void evSerial::switchThreePhasesWhileCharging(bool n) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_switch_three_phases_while_charging_tag;
    msg_out.payload.switch_three_phases_while_charging.n = n;
    linkWrite(&msg_out);
}

void evSerial::pauseCharging() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_pause_charging_tag;
    linkWrite(&msg_out);
}

void evSerial::resumeCharging() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_resume_charging_tag;
    linkWrite(&msg_out);
}

void evSerial::restart() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_restart_tag;
    linkWrite(&msg_out);
}

bool evSerial::reset(const int reset_pin) {

    reset_done_flag = false;
    forced_reset = true;

    if (reset_pin > 0) {
        // Try to hardware reset Yeti controller to be in a known state
        char cmd[100];
        sprintf(cmd, "echo %i >/sys/class/gpio/export", reset_pin);
        system(cmd);
        sprintf(cmd, "echo out > /sys/class/gpio/gpio%i/direction", reset_pin);
        system(cmd);
        sprintf(cmd, "echo 0 > /sys/class/gpio/gpio%i/value", reset_pin);
        system(cmd);
        sprintf(cmd, "echo 1 > /sys/class/gpio/gpio%i/value", reset_pin);
        system(cmd);
    } else {
        // Try to soft reset Yeti controller to be in a known state
        HiToLo msg_out = HiToLo_init_default;
        msg_out.which_payload = HiToLo_reset_tag;
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
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_firmware_update_tag;
    msg_out.payload.firmware_update.invoke_rom_bootloader = rom;
    linkWrite(&msg_out);
}

void evSerial::enableSimulation(bool s) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_enable_simulation_tag;
    msg_out.payload.enable_simulation.s = s;
    linkWrite(&msg_out);
}

void evSerial::setSimulationData(SimulationData s) {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_simulation_data_tag;
    msg_out.payload.simulation_data = s;
    linkWrite(&msg_out);
}

void evSerial::keepAlive() {
    HiToLo msg_out = HiToLo_init_default;
    msg_out.which_payload = HiToLo_keep_alive_tag;
    msg_out.payload.keep_alive.time_stamp = 0;
    msg_out.payload.keep_alive.hw_type = 0;
    msg_out.payload.keep_alive.hw_revision = 0;
    msg_out.payload.keep_alive.protocol_version_major = 0;
    msg_out.payload.keep_alive.protocol_version_minor = 1;
    strcpy(msg_out.payload.keep_alive.sw_version_string, "n/a");
    linkWrite(&msg_out);
}
