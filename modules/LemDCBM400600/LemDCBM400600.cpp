// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "LemDCBM400600.hpp"

namespace module {

void LemDCBM400600::init() {
    invoke_init(*p_main);
}

void LemDCBM400600::ready() {
    invoke_ready(*p_main);
}

} // namespace module
