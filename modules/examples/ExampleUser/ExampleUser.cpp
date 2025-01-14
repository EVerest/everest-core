// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "ExampleUser.hpp"

namespace module {

void ExampleUser::init() {
    invoke_init(*p_example_user);
}

void ExampleUser::ready() {
    invoke_ready(*p_example_user);
}

void ExampleUser::shutdown() {
    EVLOG_info << "shutting down";
}

} // namespace module
