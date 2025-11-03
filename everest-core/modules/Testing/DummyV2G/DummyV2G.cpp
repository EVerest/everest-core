// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DummyV2G.hpp"

namespace module {

void DummyV2G::init() {
    invoke_init(*p_main);
}

void DummyV2G::ready() {
    invoke_ready(*p_main);
}

} // namespace module
