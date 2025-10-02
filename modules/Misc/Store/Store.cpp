// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "Store.hpp"

namespace module {

void Store::init() {
    invoke_init(*p_main);
}

void Store::ready() {
    invoke_ready(*p_main);
}

} // namespace module
