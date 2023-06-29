// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DPM1000.hpp"

namespace module {

void DPM1000::init() {
    invoke_init(*p_main);
}

void DPM1000::ready() {
    invoke_ready(*p_main);
}

} // namespace module
