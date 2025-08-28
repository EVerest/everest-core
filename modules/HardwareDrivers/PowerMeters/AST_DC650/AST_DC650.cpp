// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "AST_DC650.hpp"

namespace module {

void AST_DC650::init() {
    invoke_init(*p_main);
}

void AST_DC650::ready() {
    invoke_ready(*p_main);
}

} // namespace module
