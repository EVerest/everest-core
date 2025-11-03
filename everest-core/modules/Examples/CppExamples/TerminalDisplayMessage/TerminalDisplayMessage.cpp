// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "TerminalDisplayMessage.hpp"

namespace module {

void TerminalDisplayMessage::init() {
    invoke_init(*p_display_message);
}

void TerminalDisplayMessage::ready() {
    invoke_ready(*p_display_message);
}

} // namespace module
