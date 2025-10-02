// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "LocalAllowlistTokenValidator.hpp"

namespace module {

void LocalAllowlistTokenValidator::init() {
    invoke_init(*p_token_validator);
}

void LocalAllowlistTokenValidator::ready() {
    invoke_ready(*p_token_validator);
}

} // namespace module
