// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OVMSimulator.hpp"

namespace module {

void OVMSimulator::init() {
    invoke_init(*p_main);
}

void OVMSimulator::ready() {
    invoke_ready(*p_main);
}

} // namespace module
