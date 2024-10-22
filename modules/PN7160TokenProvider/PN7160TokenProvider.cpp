// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2024 Pionix GmbH and Contributors to EVerest
#include "PN7160TokenProvider.hpp"

namespace module {

void PN7160TokenProvider::init() {
    invoke_init(*p_main);
}

void PN7160TokenProvider::ready() {
    invoke_ready(*p_main);
}

} // namespace module
