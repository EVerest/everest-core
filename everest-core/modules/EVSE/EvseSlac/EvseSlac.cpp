// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EvseSlac.hpp"

namespace module {

void EvseSlac::init() {
    invoke_init(*p_main);
}

void EvseSlac::ready() {
    invoke_ready(*p_main);
}

} // namespace module
