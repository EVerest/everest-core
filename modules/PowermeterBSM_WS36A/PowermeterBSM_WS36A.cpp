// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PowermeterBSM_WS36A.hpp"

namespace module {

void PowermeterBSM_WS36A::init() {
    invoke_init(*p_main);
}

void PowermeterBSM_WS36A::ready() {
    invoke_ready(*p_main);
}

} // namespace module
