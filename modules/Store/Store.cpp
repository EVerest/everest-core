#include "Store.hpp"

namespace module {

void Store::init() {
    invoke_init(*p_main);
}

void Store::ready() {
    invoke_ready(*p_main);
}

} // namespace module
