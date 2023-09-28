// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DummyTokenProviderManual.hpp"

namespace module {

void DummyTokenProviderManual::init() {
    invoke_init(*p_main);
}

void DummyTokenProviderManual::ready() {
    invoke_ready(*p_main);
}

} // namespace module
