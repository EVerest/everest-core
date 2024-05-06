// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ExampleErrorRaiser.hpp"

namespace module {

void ExampleErrorRaiser::init() {
    invoke_init(*p_example_raiser);
}

void ExampleErrorRaiser::ready() {
    invoke_ready(*p_example_raiser);
}

} // namespace module
