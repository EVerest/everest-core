// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <cstring>
#include <errno.h>
#include <everest/io/serial/pty_handler.hpp>
#include <iostream>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

namespace everest::lib::io::serial {

bool pty_handler::tx(PayloadT& data) {
    auto status = ::write(m_dev.master_fd, data.data(), data.size());
    if (status == -1) {
        error_id = errno;
        std::cout << "ERROR: Failed to write to pty master." << std::endl;
        return false;
    }
    if (status < static_cast<ssize_t>(data.size())) {
        // We have a reference to the current data. Replace it with what is left to be written
        // and return false. This signals the current block cannot be removed from the buffer.
        data = {data.begin() + status, data.end()};
        return false;
    }
    error_id = 0;
    return true;
}

bool pty_handler::rx(PayloadT& data) {
    // This should not be expensive, since capacity is only touched once, since
    // data is expected to stay the same object during the livetime of this instance
    data.resize(buffer_size_limit);
    auto n_bytes = ::read(m_dev.master_fd, data.data(), data.size());
    if (n_bytes == -1) {
        error_id = errno;
        return false;
    }
    data.resize(n_bytes);
    error_id = 0;
    return true;
}

bool pty_handler::open() {
    auto mypty = serial::openpty();
    if (not mypty.has_value()) {
        error_id = errno;
        return false;
    }

    auto aware = serial::make_pty_mode_aware(mypty.value());
    if (not aware) {
        std::cout << "ERROR: Preparing pty for packet mode and extproc" << std::endl;
        error_id = errno;
        return false;
    }
    auto binary = serial::set_binary_mode(mypty->slave_fd);
    if (not binary) {
        std::cout << "ERROR: Preparing pty for binary mode" << std::endl;
        error_id = errno;
        return false;
    }
    m_dev = std::move(mypty.value());
    error_id = 0;
    return true;
}

int pty_handler::get_fd() const {
    return m_dev.master_fd;
}

pty_status pty_handler::get_status() {
    pty_status result;
    struct termios status;
    auto attr_res = tcgetattr(m_dev.master_fd, &status);
    if (attr_res == -1) {
        error_id = errno;
        std::cout << "Failed to get attributes" << std::endl;
        return result;
    }

    result.ixon = status.c_iflag & IXON;     // enable xon/xoff flow control on output
    result.ixoff = status.c_iflag & IXOFF;   // enable xon/xoff flow control on input
    result.cstopb = status.c_cflag & CSTOPB; // two stop bits instead of one
    result.cbaud = cfgetospeed(&status);

    error_id = 0;
    return result;
}

int pty_handler::get_error() const {
    return error_id;
}

std::string pty_handler::get_slave_path() const {
    return m_dev.slave_path;
}

} // namespace everest::lib::io::serial
