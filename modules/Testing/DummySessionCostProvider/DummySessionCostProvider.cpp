// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DummySessionCostProvider.hpp"

namespace module {

void DummySessionCostProvider::init() {
    invoke_init(*p_main);
}

void DummySessionCostProvider::ready() {
    invoke_ready(*p_main);
}

} // namespace module
