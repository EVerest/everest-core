// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);
}

void RpcApi::ready() {
    invoke_ready(*p_main);
}

} // namespace module
