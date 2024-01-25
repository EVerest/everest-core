// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#include "DCSupplySimulator.hpp"

namespace module {

void DCSupplySimulator::init() {
    invoke_init(*p_main);
}

void DCSupplySimulator::ready() {
    invoke_ready(*p_main);
}

} // namespace module
