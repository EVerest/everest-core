// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "PersistentStore.hpp"

namespace module {

void PersistentStore::init() {
    invoke_init(*p_main);
}

void PersistentStore::ready() {
    invoke_ready(*p_main);
}

} // namespace module
