// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <cstring>
#include <everest/io/tun_tap/tap_handler.hpp>
#include <fcntl.h>
#include <unistd.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <everest/io/socket/socket.hpp>

namespace everest::lib::io::tun_tap {

bool tap_handler::open(std::string const& device, std::string const& ip, std::string const& netmask, int mtu) {
    m_mtu = mtu;
    try {
        m_fd = socket::create_tap_device(device);
    } catch (...) {
        m_error = EPERM;
        return false;
    }
    if (not socket::configure_tap_device_properties(m_fd, device, ip, netmask, mtu)) {
        m_error = EPERM;
        m_fd.close();
        return false;
    }
    m_error = 0;
    return true;
}

bool tap_handler::tx(PayloadT const& data) {
    auto res = ::write(m_fd, data.data(), data.size());
    if (res != static_cast<ssize_t>(data.size())) {
        m_error = errno;
        return false;
    }
    m_error = 0;
    return true;
}

bool tap_handler::rx(PayloadT& data) {
    data.resize(m_mtu);
    auto res = ::read(m_fd, data.data(), data.size());
    if (res < 0) {
        m_error = errno;
        return false;
    }

    data.resize(res);
    m_error = 0;
    return true;
}

int tap_handler::get_fd() const {
    return m_fd;
}

int tap_handler::get_error() const {
    return m_error;
}

} // namespace everest::lib::io::tun_tap
