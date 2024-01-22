// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "DZG_GSH01.hpp"

namespace module {

void DZG_GSH01::init() {
    invoke_init(*p_main);
}

void DZG_GSH01::ready() {
    invoke_ready(*p_main);
}

} // namespace module
