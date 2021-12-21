// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "Power.hpp"

namespace module {

void Power::init() {
    invoke_init(*p_main);
}

void Power::ready() {
    invoke_ready(*p_main);
}

} // namespace module
