// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ManagerLifecycleSimulator.hpp"

namespace module {

void ManagerLifecycleSimulator::init() {
    invoke_init(*p_main);
}

void ManagerLifecycleSimulator::ready() {
    invoke_ready(*p_main);
}

void ManagerLifecycleSimulator::shutdown() {
    invoke_shutdown(*p_main);
}

} // namespace module
