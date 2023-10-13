// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "SLIPCommHub.hpp"

namespace module {

void SLIPCommHub::init() {
    invoke_init(*p_main);
}

void SLIPCommHub::ready() {
    invoke_ready(*p_main);
}

} // namespace module
