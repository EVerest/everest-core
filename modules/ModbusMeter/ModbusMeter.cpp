// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "ModbusMeter.hpp"
#include <chrono>
#include <date/date.h>
#include <date/tz.h>

namespace module {

void ModbusMeter::init() {
    invoke_init(*p_main);
}

void ModbusMeter::ready() {
    invoke_ready(*p_main);
}

} // namespace module
