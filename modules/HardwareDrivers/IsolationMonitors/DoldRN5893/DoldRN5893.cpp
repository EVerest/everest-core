// SPDX-License-Identifier: Apache-2.0
// Copyright Frickly Systems GmbH
// Copyright Pionix GmbH and Contributors to EVerest
#include "DoldRN5893.hpp"

namespace module {

void DoldRN5893::init() {
    invoke_init(*p_main);
}

void DoldRN5893::ready() {
    invoke_ready(*p_main);
}

} // namespace module
