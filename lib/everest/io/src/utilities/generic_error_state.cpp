// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/utilities/generic_error_state.hpp>
#include <string.h>

namespace everest::lib::io::utilities {

bool generic_error_state::set_error_status(int error_code) {
    m_current_error = error_code;
    auto on_error = error_code != 0;
    m_clear_error_pending = (not on_error) and m_on_error;
    m_on_error = on_error;
    return not m_on_error;
}

bool generic_error_state::clear_error_pending() const {
    return m_clear_error_pending;
}

bool generic_error_state::on_error() const {
    return m_on_error;
}

int generic_error_state::current_error() const {
    return m_current_error;
}

void generic_error_state::call_error_handler(cb_error& handler) const {
    if (handler) {
        handler(m_current_error, strerror(m_current_error));
    }
}

void generic_error_state::clear_error_handler(cb_error& handler) {
    if (handler) {
        handler(0, strerror(0));
    }
    set_error_cleared();
}

void generic_error_state::set_error_cleared() {
    m_clear_error_pending = false;
}

} // namespace everest::lib::io::utilities
