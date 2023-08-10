// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ImdBenderIsoCha.hpp"

namespace module {

void ImdBenderIsoCha::init() {
    invoke_init(*p_main);
}

void ImdBenderIsoCha::ready() {
    invoke_ready(*p_main);
}

} // namespace module
