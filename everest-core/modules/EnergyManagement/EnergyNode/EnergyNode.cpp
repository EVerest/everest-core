// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyNode.hpp"

namespace module {

void EnergyNode::init() {
    invoke_init(*p_energy_grid);
}

void EnergyNode::ready() {
    invoke_ready(*p_energy_grid);
}

} // namespace module
