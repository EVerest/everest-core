// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "System.hpp"

namespace module {

void System::init() {
    invoke_init(*p_main);
}

void System::ready() {
    invoke_ready(*p_main);
}

void System::shutdown() {
    invoke_shutdown(*p_main);
}

} // namespace module
