// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "Bender_isoCHA425HV.hpp"

namespace module {

void Bender_isoCHA425HV::init() {
    invoke_init(*p_main);
}

void Bender_isoCHA425HV::ready() {
    invoke_ready(*p_main);
}

} // namespace module
