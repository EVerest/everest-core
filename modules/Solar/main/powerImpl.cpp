#include "powerImpl.hpp"

#include <chrono>

namespace module {
namespace main {

void powerImpl::init() {
}

void powerImpl::ready() {
    simulation_thread = std::thread(&powerImpl::simulation, this);
}

void powerImpl::simulation() {
    double step = 0;
    double pi = std::atan(1.0) * 4.0;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        double max_current = std::abs(std::sin((pi / 64.0) * step * 3.20)) + 1.0;
        EVLOG(debug) << "Publishing max_current: " << max_current;
        publish_max_current(max_current);
        step += 1;
    }
}

} // namespace main
} // namespace module
