#include "ModbusMeter.hpp"

namespace module {

void ModbusMeter::init() {
    invoke_init(*p_main);
}

void ModbusMeter::ready() {
    invoke_ready(*p_main);
}

} // namespace module
