// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "StaticISO15118VASProvider.hpp"

namespace module {

void StaticISO15118VASProvider::init() {
    invoke_init(*p_iso15118_vas);
}

void StaticISO15118VASProvider::ready() {
    invoke_ready(*p_iso15118_vas);
}

} // namespace module
