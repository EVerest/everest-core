// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvseD20.hpp"

namespace module {

void EvseD20::init() {
    invoke_init(*p_charger);
}

void EvseD20::ready() {
    invoke_ready(*p_charger);
}

} // namespace module
