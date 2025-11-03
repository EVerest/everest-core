#include "SLAC_openplcutils.hpp"

namespace module {

void SLAC_openplcutils::init() {
    invoke_init(*p_main);
}

void SLAC_openplcutils::ready() {
    invoke_ready(*p_main);
}

} // namespace module
