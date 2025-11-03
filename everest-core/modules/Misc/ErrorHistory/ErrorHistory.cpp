// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ErrorHistory.hpp"

namespace module {

void ErrorHistory::init() {
    invoke_init(*p_error_history);
}

void ErrorHistory::ready() {
    invoke_ready(*p_error_history);
}

} // namespace module
