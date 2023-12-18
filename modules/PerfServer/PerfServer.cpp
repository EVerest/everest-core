// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PerfServer.hpp"

namespace module {

void PerfServer::init() {
    invoke_init(*p_main);
}

void PerfServer::ready() {
    invoke_ready(*p_main);
}

} // namespace module
