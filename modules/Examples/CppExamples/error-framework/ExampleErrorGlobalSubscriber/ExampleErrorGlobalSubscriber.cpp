// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ExampleErrorGlobalSubscriber.hpp"

namespace module {

void ExampleErrorGlobalSubscriber::init() {
    invoke_init(*p_example_global_subscriber);
}

void ExampleErrorGlobalSubscriber::ready() {
    invoke_ready(*p_example_global_subscriber);
}

} // namespace module
