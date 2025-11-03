#include "Solar.hpp"

namespace module {

void Solar::init() {
    invoke_init(*p_main);
}

void Solar::ready() {
    invoke_ready(*p_main);
}

} // namespace module
