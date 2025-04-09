// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "YamlStore.hpp"

namespace module {

void YamlStore::init() {
    invoke_init(*p_main);
}

void YamlStore::ready() {
    invoke_ready(*p_main);
}

} // namespace module
