// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvseSecurity.hpp"

namespace module {

void EvseSecurity::init() {
    invoke_init(*p_main);
}

void EvseSecurity::ready() {
    invoke_ready(*p_main);
}

} // namespace module
