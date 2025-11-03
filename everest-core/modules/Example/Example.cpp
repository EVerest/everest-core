#include "Example.hpp"

namespace module {

void Example::init() {
    invoke_init(*p_example);
    invoke_init(*p_store);
}

void Example::ready() {
    invoke_ready(*p_example);
    invoke_ready(*p_store);

    mqtt.publish("external/topic", "data");
}

} // namespace module
