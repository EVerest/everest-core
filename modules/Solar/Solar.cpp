// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "Solar.hpp"

namespace module {

void Solar::init() {
    invoke_init(*p_main);
}

void Solar::ready() {
    invoke_ready(*p_main);
}

} // namespace module
