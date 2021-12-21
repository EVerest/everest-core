// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "SLAC_openplcutils.hpp"

namespace module {

void SLAC_openplcutils::init() {
    invoke_init(*p_main);
}

void SLAC_openplcutils::ready() {
    invoke_ready(*p_main);
}

} // namespace module
