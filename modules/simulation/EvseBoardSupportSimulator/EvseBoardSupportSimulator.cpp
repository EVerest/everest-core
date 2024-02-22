// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvseBoardSupportSimulator.hpp"

namespace module {

void EvseBoardSupportSimulator::init() {
    invoke_init(*p_board_support);
}

void EvseBoardSupportSimulator::ready() {
    invoke_ready(*p_board_support);
}

} // namespace module
