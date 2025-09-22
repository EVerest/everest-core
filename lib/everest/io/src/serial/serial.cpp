// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <cstdlib>
#include <everest/io/serial/serial.hpp>
#include <fcntl.h>
#include <optional>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace everest::lib::io::serial {

std::optional<pty> openpty() {
    pty result;

    result.master_fd = event::unique_fd(::posix_openpt(O_RDWR | O_NOCTTY));
    if (not result.master_fd.is_fd()) {
        return std::nullopt;
    }
    if (::grantpt(result.master_fd) == -1) {
        return std::nullopt;
    }
    if (::unlockpt(result.master_fd) == -1) {
        return std::nullopt;
    }
    auto mb_name = ::ptsname(result.master_fd);
    if (mb_name == 0) {
        return std::nullopt;
    }
    result.slave_path = mb_name;
    result.slave_fd = event::unique_fd(::open(mb_name, 0));
    if (not result.slave_fd.is_fd()) {
        return std::nullopt;
    }
    return result;
}

bool set_packet_mode(int fd) {
    int nonzero = 1;
    return ioctl(fd, TIOCPKT, &nonzero) == 0;
}

bool set_extproc_flag(int fd) {
    struct termios tio;

    if (tcgetattr(fd, &tio) == -1) {
        return false;
    }
    tio.c_lflag |= EXTPROC;

    if (tcsetattr(fd, TCSANOW, &tio) == -1) {
        return false;
    }
    return true;
}

bool set_binary_mode(int fd) {
    struct termios tty;
    if (tcgetattr(fd, &tty) < 0) {
        return false;
    }
    cfmakeraw(&tty);

    if (tcsetattr(fd, TCSAFLUSH, &tty) < 0) {
        return false;
    }

    return true;
}

bool make_pty_mode_aware(pty const& item) {
    return set_packet_mode(item.master_fd) && set_extproc_flag(item.slave_fd);
}

} // namespace everest::lib::io::serial
