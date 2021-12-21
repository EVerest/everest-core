// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "PowerIn.hpp"

namespace module {

void PowerIn::init() {
    invoke_init(*p_main);
}

void PowerIn::ready() {
    invoke_ready(*p_main);
}

} // namespace module
