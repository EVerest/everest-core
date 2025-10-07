// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <cstring>
#include <everest/io/serial/event_pty.hpp>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

namespace everest::lib::io::serial {

void event_pty::set_status_handler(cb_status const& handler) {
    m_status = handler;
}

void event_pty::set_data_handler(cb_rx const& handler) {
    m_data = handler;
    set_rx_handler([this, handler](auto const& pl, auto& obj) {
        if (pl.size() > 0) {
            if (pl[0] == 0) {
                if (m_data) {
                    m_data({pl.begin() + 1, pl.end()}, obj);
                }
            } else {
                if (m_status) {
                    m_status(get_raw_handler()->get_status());
                }
            }
        }
    });
}

std::string event_pty::get_slave_path() {
    return get_raw_handler()->get_slave_path();
}

} // namespace everest::lib::io::serial
