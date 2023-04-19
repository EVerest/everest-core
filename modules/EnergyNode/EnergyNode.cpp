// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyNode.hpp"

namespace module {

void EnergyNode::init() {
    invoke_init(*p_energy_grid);
    invoke_init(*p_external_limits);
}

void EnergyNode::ready() {
    invoke_ready(*p_energy_grid);
    invoke_ready(*p_external_limits);
}

void EnergyNode::shutdown() {
    invoke_shutdown(*p_energy_grid);
    invoke_shutdown(*p_external_limits);
}

} // namespace module
