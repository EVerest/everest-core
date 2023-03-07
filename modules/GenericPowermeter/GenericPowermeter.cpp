// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "GenericPowermeter.hpp"

namespace module {

void GenericPowermeter::init() {
    invoke_init(*p_main);
}

void GenericPowermeter::ready() {
    invoke_ready(*p_main);
}

} // namespace module
