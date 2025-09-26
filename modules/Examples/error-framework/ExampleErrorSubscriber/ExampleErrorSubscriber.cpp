// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "ExampleErrorSubscriber.hpp"

namespace module {

void ExampleErrorSubscriber::init() {
    invoke_init(*p_example_subscriber);
}

void ExampleErrorSubscriber::ready() {
    invoke_ready(*p_example_subscriber);
}

} // namespace module
