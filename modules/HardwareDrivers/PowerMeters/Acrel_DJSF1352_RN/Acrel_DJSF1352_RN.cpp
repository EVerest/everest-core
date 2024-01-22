// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "Acrel_DJSF1352_RN.hpp"

namespace module {

void Acrel_DJSF1352_RN::init() {
    invoke_init(*p_main);
}

void Acrel_DJSF1352_RN::ready() {
    invoke_ready(*p_main);
}

} // namespace module
