// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "IMDSimulator.hpp"

namespace module {

void IMDSimulator::init() {
    invoke_init(*p_main);
}

void IMDSimulator::ready() {
    invoke_ready(*p_main);
}

} // namespace module
