// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvSlac.hpp"

namespace module {

void EvSlac::init() {
    invoke_init(*p_main);
}

void EvSlac::ready() {
    invoke_ready(*p_main);
}

} // namespace module
