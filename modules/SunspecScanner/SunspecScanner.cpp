#include "SunspecScanner.hpp"

namespace module {

void SunspecScanner::init() {
    invoke_init(*p_main);
}

void SunspecScanner::ready() {
    invoke_ready(*p_main);
}

void SunspecScanner::shutdown() {
    invoke_shutdown(*p_main);
}

} // namespace module
