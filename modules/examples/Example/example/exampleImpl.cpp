// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "exampleImpl.hpp"

// initial cpp template for interface example_child
// this file should not be overwritten by the code generator again

namespace module {
namespace example {

void exampleImpl::init() {
    mod->mqtt.subscribe("external/a",
                        [](json data) { EVLOG_error << "received data from external MQTT handler: " << data.dump(); });
}

void exampleImpl::ready() {
    publish_max_current(config.current);
    mod->r_kvs->call_store("test", "test");
}

bool exampleImpl::handle_uses_something(std::string& key) {
    if (mod->r_kvs->call_exists(key)) {
        EVLOG_debug << "IT SHOULD NOT AND DOES NOT EXIST";
    }

    Array test_array = {1, 2, 3};
    mod->r_kvs->call_store(key, test_array);

    bool exi = mod->r_kvs->call_exists(key);

    if (exi) {
        EVLOG_debug << "IT ACTUALLY EXISTS";
    }

    auto ret = mod->r_kvs->call_load(key);

    Array arr = std::get<Array>(ret);

    EVLOG_debug << "loaded array: " << arr << ", original array: " << test_array;

    return exi;
};

} // namespace example
} // namespace module
