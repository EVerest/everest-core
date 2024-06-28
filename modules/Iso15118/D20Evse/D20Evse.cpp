// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "D20Evse.hpp"

namespace module {

void D20Evse::init() {
    invoke_init(*p_charger);
}

void D20Evse::ready() {
    invoke_ready(*p_charger);
}

} // namespace module
