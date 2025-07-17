// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "Example.hpp"

namespace module {

void Example::init() {
    invoke_init(*p_example);
    invoke_init(*p_example_multiple);
    invoke_init(*p_store);
}

void Example::ready() {
    invoke_ready(*p_example);
    invoke_ready(*p_example_multiple);
    invoke_ready(*p_store);

    mqtt.publish("external/topic", "data");
}

} // namespace module
