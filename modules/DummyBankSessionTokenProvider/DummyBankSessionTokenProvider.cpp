// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "DummyBankSessionTokenProvider.hpp"

namespace module {

void DummyBankSessionTokenProvider::init() {
    invoke_init(*p_main);
}

void DummyBankSessionTokenProvider::ready() {
    invoke_ready(*p_main);
}

} // namespace module
