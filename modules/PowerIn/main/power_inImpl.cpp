#include "power_inImpl.hpp"

namespace module {
namespace main {

void power_inImpl::init() {
}

void power_inImpl::ready() {
    publish_max_current(4.2);

    publish_phase_count(3);
}

} // namespace main
} // namespace module
