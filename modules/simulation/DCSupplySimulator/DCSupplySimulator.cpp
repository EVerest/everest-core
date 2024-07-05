// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH
// Copyright Pionix GmbH and Contributors to EVerest

#include "DCSupplySimulator.hpp"

namespace module {

void DCSupplySimulator::init() {
    invoke_init(*p_main);
    invoke_init(*p_powermeter);
}

void DCSupplySimulator::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_powermeter);
}

} // namespace module
