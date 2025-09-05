// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "InfyPower_BEG1K075G.hpp"

namespace module {

void InfyPower_BEG1K075G::init() {
    // open DCDC CAN device
    if (!acdc.open_device(config.can_device.c_str())) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open CAN interface ", config.can_device));
    }

    invoke_init(*p_main);
}

void InfyPower_BEG1K075G::ready() {
    invoke_ready(*p_main);
}

} // namespace module
