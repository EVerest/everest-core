// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "SerialCommHub.hpp"

namespace module {

void SerialCommHub::init() {
    invoke_init(*p_main);
}

void SerialCommHub::ready() {
    invoke_ready(*p_main);
}

} // namespace module
