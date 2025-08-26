// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ChargerInfo.hpp"

namespace module {

void ChargerInfo::init() {
    invoke_init(*p_main);
}

void ChargerInfo::ready() {
    invoke_ready(*p_main);
}

} // namespace module
