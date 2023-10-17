// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DummyTokenValidator.hpp"

namespace module {

void DummyTokenValidator::init() {
    invoke_init(*p_main);
}

void DummyTokenValidator::ready() {
    invoke_ready(*p_main);
}

} // namespace module
