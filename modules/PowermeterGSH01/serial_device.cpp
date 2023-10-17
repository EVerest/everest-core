// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "serial_device.hpp"

#include <cstring>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include <sys/select.h>
#include <sys/time.h>

#include <fmt/core.h>

namespace serial_device {

static std::string hexdump(uint8_t* msg, int msg_len) {
    std::stringstream ss;
    for (int i = 0; i < msg_len; i++) {
        ss << std::hex << (int)msg[i] << " ";
    }
    return ss.str();
}

static std::string hexdump(std::vector<uint8_t> msg) {
    std::stringstream ss;

    for (auto index : msg) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)index << " ";
    }
    return ss.str();
}

SerialDevice::~SerialDevice() {
    if (this->fd) {
        close(this->fd);
    }
}

bool SerialDevice::open_device(const std::string& device, int _baud, bool _ignore_echo) {

    this->ignore_echo = _ignore_echo;

    this->fd = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (this->fd < 0) {
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
    if (tcgetattr(this->fd, &tty) != 0) {
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

    if (tcsetattr(this->fd, TCSANOW, &tty) != 0) {
        printf("Serial: error %d from tcsetattr\n", errno);
        return false;
    }
    return true;
}

/*
    This function receives a byte array.
*/
int SerialDevice::rx(std::vector<uint8_t>& rxbuf,
                     std::optional<int> initial_timeout_ms, 
                     std::optional<int> in_msg_timeout_ms) {
    std::scoped_lock lock(this->serial_mutex);
    int _initial_timeout = SERIAL_RX_INITIAL_TIMEOUT_MS;
    if (initial_timeout_ms.has_value()) {
        _initial_timeout = initial_timeout_ms.value();
    }
    int _in_msg_timeout = SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS;
    if (in_msg_timeout_ms.has_value()) {
        _in_msg_timeout = in_msg_timeout_ms.value();
    }
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = _initial_timeout * 1000; // intial timeout until device responds
    fd_set set;
    FD_ZERO(&set);
    FD_SET(this->fd, &set);

    int bytes_read_total = 0;
    while (true) {
        int rv = select(this->fd + 1, &set, NULL, NULL, &timeout);
        timeout.tv_usec = _in_msg_timeout * 1000; // reduce timeout after first chunk, 
                                                  // no uneccesary waiting at the end of the message
        if (rv == -1) {         // error in select function call
            perror("rx: select:");
            break;
        } else if (rv == 0) { // no more bytes to read within timeout, so transfer is complete
            EVLOG_debug << "No more bytes to read within timeout. (rv == 0)";
            break;
        } else {              // received more bytes, add them to buffer
            // do we have space in the rx buffer left?
            if (bytes_read_total >= rxbuf.capacity()) {
                // no buffer space left, but more to read.
                EVLOG_info << "No buffer space left, but more to read. (Did you mean to set \"ignore_echo\" to \"false\"?)";
                break;
            }

            rxbuf.resize(rxbuf.capacity());
            int bytes_read = read(this->fd,
                                  (uint8_t*)(&rxbuf[0] + bytes_read_total), 
                                  (size_t)(rxbuf.capacity() - bytes_read_total));

            if (bytes_read > 0) {
                bytes_read_total += bytes_read;
                rxbuf.resize(bytes_read_total);
            } else if (bytes_read < 0) {
                EVLOG_error << "Error reading from device: " << strerror(errno);
            }
        }
    }
    return bytes_read_total;
}

/*
    This function transmits a byte vector.
*/
void SerialDevice::tx(std::vector<uint8_t>& request) {
    {
        std::scoped_lock lock(this->serial_mutex);
        // clear input and output buffer
        tcflush(this->fd, TCIOFLUSH);

        // EVLOG_info << "TXD: " << hexdump(request) << " size: " << request.size();

        // write to serial port
        write(this->fd, request.data(), request.size());
        tcdrain(this->fd);
    }
    if (this->ignore_echo) {
        // read back echo of what we sent and ignore it
        std::vector<uint8_t> req_buf{};
        req_buf.reserve(request.size() + 1);
        this->rx(req_buf, std::nullopt, std::nullopt);
    }
}

} // namespace serial_device
