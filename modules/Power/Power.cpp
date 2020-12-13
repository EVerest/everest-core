#include "Power.hpp"

namespace module {

void Power::init() {
    invoke_init(*p_main);
}

void Power::ready() {
    invoke_ready(*p_main);
}

} // namespace module
