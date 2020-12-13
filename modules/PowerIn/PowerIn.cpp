#include "PowerIn.hpp"

namespace module {

void PowerIn::init() {
    invoke_init(*p_main);
}

void PowerIn::ready() {
    invoke_ready(*p_main);
}

} // namespace module
