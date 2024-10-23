// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "NxpNfcFrontendTokenProvider.hpp"

namespace module {

void NxpNfcFrontendTokenProvider::init() {
    invoke_init(*p_main);
}

void NxpNfcFrontendTokenProvider::ready() {
    invoke_ready(*p_main);
}

} // namespace module
