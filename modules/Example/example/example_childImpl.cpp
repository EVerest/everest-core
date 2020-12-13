#include "example_childImpl.hpp"

// initial cpp template for interface example_child
// this file should not be overwritten by the code generator again

namespace module {
namespace example {

void example_childImpl::init() {
    mod->mqtt.subscribe("external/a",
                        [](json data) { EVLOG(error) << "received data from external MQTT handler: " << data.dump(); });
}

void example_childImpl::ready() {
    publish_max_current(config.current);
}

bool example_childImpl::handle_uses_something(std::string& key) {
    if (mod->r_kvs->call_exists(key)) {
        EVLOG(debug) << "IT SHOULD NOT AND DOES NOT EXIST";
    }

    Array test_array = {1, 2, 3};
    mod->r_kvs->call_store(key, test_array);

    bool exi = mod->r_kvs->call_exists(key);

    if (exi) {
        EVLOG(debug) << "IT ACTUALLY EXISTS";
    }

    auto ret = mod->r_kvs->call_load(key);

    Array arr = boost::get<Array>(ret);

    EVLOG(debug) << "loaded array: " << arr << ", original array: " << test_array;

    return exi;
};

} // namespace example
} // namespace module
