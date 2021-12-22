#include "SunspecReader.hpp"

namespace module {

void SunspecReader::init() {
    invoke_init(*p_main);
}

void SunspecReader::ready() {
    invoke_ready(*p_main);
}

} // namespace module
