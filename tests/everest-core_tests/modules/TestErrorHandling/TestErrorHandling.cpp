// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "TestErrorHandling.hpp"

namespace module {

void TestErrorHandling::init() {
    invoke_init(*p_main);
}

void TestErrorHandling::ready() {
    invoke_ready(*p_main);
}

} // namespace module
