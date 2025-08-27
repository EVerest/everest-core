// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "Iso15118InternetVas.hpp"

namespace module {

void Iso15118InternetVas::init() {
    invoke_init(*p_iso15118_vas);
}

void Iso15118InternetVas::ready() {
    invoke_ready(*p_iso15118_vas);
}

} // namespace module
