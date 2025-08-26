// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "IsabellenhuetteIemDcr.hpp"

namespace module {

void IsabellenhuetteIemDcr::init() {
    invoke_init(*p_main);
}

void IsabellenhuetteIemDcr::ready() {
    invoke_ready(*p_main);
}

} // namespace module
