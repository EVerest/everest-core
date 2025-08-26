// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DummyTokenProvider.hpp"

namespace module {

void DummyTokenProvider::init() {
    invoke_init(*p_main);
}

void DummyTokenProvider::ready() {
    invoke_ready(*p_main);
}

} // namespace module
