// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

// TODOs:
// - sometimes we receive 0 bytes from sofar, find out why
// - implement echo removal for chargebyte
// - implement GPIO to switch rx/tx

#include "tiny_modbus_rtu.hpp"

#include "crc16.hpp"
#include <errno.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>

namespace tinymod {

static void hexdump(const char* text, uint8_t* msg, int msg_len) {
    std::cout << text;
    for (int i = 0; i < msg_len; i++) {
        std::cout << std::hex << (int)msg[i] << " ";
    }
    std::cout << std::endl;
}

static void set_cksum(uint8_t* msg, int msg_len) {
    if (msg_len < 5)
        return;
    uint16_t crc_sum = MODBUS_CRC16(msg, msg_len - 2);
    memcpy(msg + msg_len - 2, &crc_sum, 2);
}

static bool validate_cksum(const uint8_t* msg, int msg_len) {
    if (msg_len < 5)
        return false;
    // check crc
    uint16_t crc_sum = MODBUS_CRC16(msg, msg_len - 2);
    uint16_t crc_msg;
    memcpy(&crc_msg, msg + msg_len - 2, 2);
    return (crc_msg == crc_sum);
}

static std::vector<uint16_t> decode_reply(const uint8_t* buf, int len, uint8_t device_address, function_code function) {
    std::vector<uint16_t> result;
    if (len < 5) {
        EVLOG_error << fmt::format("Packet too small: {} bytes.", len);
        return result;
    }
    if (device_address != buf[device_address_pos]) {
        EVLOG_error << fmt::format("Device address mismatch: expected: {} received: {}", device_address,
                                   buf[device_address_pos]);
        return result;
    }

    bool exception = false;
    uint8_t function_code_recvd = buf[function_code_pos];
    if (function_code_recvd & (1 << 7)) {
        // highest bit is set for exception reply
        exception = true;
        // clear error bit
        function_code_recvd &= (1 << 7);
    }

    if (function != function_code_recvd) {
        EVLOG_error << fmt::format("Function code mismatch: expected: {} received: {}", function, function_code_recvd);
        return result;
    }

    if (!validate_cksum(buf, len)) {
        EVLOG_error << "Checksum error";
        return result;
    }

    if (!exception) {
        // For a write reply we always get 4 bytes
        uint8_t byte_cnt = 4;
        int start_of_result = res_tx_start_of_payload;

        // Was it a read reply?
        if (function == function_code::read_coils || function == function_code::read_discrete_inputs ||
            function == function_code::read_multiple_holding_registers) {
            // adapt byte count and starting pos
            byte_cnt = buf[res_rx_len_pos];
            start_of_result = res_rx_start_of_payload;
        }

        // ready to copy actual result data to output
        result.reserve(byte_cnt / 2);

        for (int i = start_of_result; i < len - 2; i += 2) {
            uint16_t t;
            memcpy(&t, buf + i, 2);
            t = be16toh(t);
            result.push_back(t);
        }
        return result;
    } else {
        // handle exception message
        uint8_t err_code = buf[res_rx_start_of_payload];
        switch (err_code) {
        case 0x01:
            EVLOG_error << "Modbus exception: Illegal function";
            break;
        case 0x02:
            EVLOG_error << "Modbus exception: Illegal data address";
            break;
        case 0x03:
            EVLOG_error << "Modbus exception: Illegal data value";
            break;
        case 0x04:
            EVLOG_error << "Modbus exception: Client device failure";
            break;
        case 0x05:
            EVLOG_debug << "Modbus ACK";
            break;
        case 0x06:
            EVLOG_error << "Modbus exception: Client device busy";
            break;
        case 0x07:
            EVLOG_error << "Modbus exception: NACK";
            break;
        case 0x08:
            EVLOG_error << "Modbus exception: Memory parity error";
            break;
        case 0x0A:
            EVLOG_error << "Modbus exception: Gateway path unavailable";
            break;
        case 0x0B:
            EVLOG_error << "Modbus exception: Gateway target device failed to respond";
            break;
        }
        return result;
    }
}

TinyModbusRTU::~TinyModbusRTU() {
    if (fd)
        close(fd);
}

bool TinyModbusRTU::openDevice(const std::string& device, int _baud) {

    fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        EVLOG_error << fmt::format("Serial: error {} opening {}: {}\n", errno, device, strerror(errno));
        return false;
    }

    int baud;
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
        return false;
    }

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
    tty.c_lflag = 0;                   // no signaling chars, no echo,
                                       // no canonical processing
    tty.c_oflag = 0;                   // no remapping, no delays
    tty.c_cc[VMIN] = 1;                // read blocks
    tty.c_cc[VTIME] = 1;               // 0.1 seconds inter character read timeout after first byte was received

    tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                       // enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;            // 1 Stop bit
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Serial: error %d from tcsetattr\n", errno);
        return false;
    }
    return true;
}

/*
    This function transmits a modbus request and waits for the reply.
    Parameter request is optional and is only used for writing multiple registers.
*/
std::vector<uint16_t> TinyModbusRTU::txrx(uint8_t device_address, function_code function,
                                          uint16_t first_register_address, uint16_t register_quantity,
                                          bool wait_for_reply, std::vector<uint16_t> request) {
    // size of request
    int req_len = (request.size() == 0 ? 0 : 2 * request.size() + 1) + 8;
    uint8_t req[req_len];

    // add header
    req[device_address_pos] = device_address;
    req[function_code_pos] = function;

    first_register_address = htobe16(first_register_address);
    register_quantity = htobe16(register_quantity);
    memcpy(req + req_tx_first_register_addr_pos, &first_register_address, 2);
    memcpy(req + req_tx_quantity_pos, &register_quantity, 2);

    if (function == function_code::write_multiple_holding_registers) {
        // write byte count
        req[req_tx_multiple_reg_byte_count_pos] = request.size() * 2;
        // add request data
        int i = req_tx_multiple_reg_byte_count_pos + 1;
        for (uint16_t r : request) {
            r = htobe16(r);
            memcpy(req + i, &r, 2);
            i += 2;
        }
    }
    // set cksum in the last 2 bytes
    set_cksum(req, req_len);

    // hexdump("SEND: ", req, req_len);

    // clear input and output buffer
    tcflush(fd, TCIOFLUSH);

    // write to serial port
    write(fd, req, req_len);
    tcdrain(fd);

    if (wait_for_reply) {
        // wait for reply
        uint8_t rxbuf[255 + 6];
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500 * 1000; // 500msec intial timeout until device responds
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);

        int bytes_read_total = 0;
        while (true) {
            int rv = select(fd + 1, &set, NULL, NULL, &timeout);
            timeout.tv_usec =
                100 * 1000; // reduce timeout after first chunk, no uneccesary waiting at the end of the message
            if (rv == -1) {
                perror("txrx: select:");
                break;
            } else if (rv == 0) {
                break;
            } else {
                // do we have space in the rx buffer left?
                if (bytes_read_total >= sizeof(rxbuf))
                    break;
                    
                int bytes_read = read(fd, rxbuf + bytes_read_total, sizeof(rxbuf) - bytes_read_total);
                if (bytes_read > 0) {
                    bytes_read_total += bytes_read;
                    // hexdump("RECVD: ", rxbuf, bytes_read_total);
                }
            }
        }
        return decode_reply(rxbuf, bytes_read_total, device_address, function);
    }

    return std::vector<uint16_t>();
}

} // namespace tinymod