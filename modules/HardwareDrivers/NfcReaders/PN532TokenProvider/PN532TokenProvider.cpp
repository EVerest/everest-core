// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "PN532TokenProvider.hpp"

namespace module {

void PN532TokenProvider::init() {
    invoke_init(*p_main);
}

void PN532TokenProvider::ready() {
    invoke_ready(*p_main);
}

} // namespace module
