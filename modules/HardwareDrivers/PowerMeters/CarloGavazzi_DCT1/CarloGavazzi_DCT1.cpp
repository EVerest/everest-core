// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "CarloGavazzi_DCT1.hpp"

namespace module {

void CarloGavazzi_DCT1::init() {
    invoke_init(*p_meter);
}

void CarloGavazzi_DCT1::ready() {
    invoke_ready(*p_meter);
}

} // namespace module
