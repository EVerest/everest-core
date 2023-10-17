// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PowermeterGSH01.hpp"

namespace module {

void PowermeterGSH01::init() {
    invoke_init(*p_main);
}

void PowermeterGSH01::ready() {
    invoke_ready(*p_main);
}

} // namespace module
