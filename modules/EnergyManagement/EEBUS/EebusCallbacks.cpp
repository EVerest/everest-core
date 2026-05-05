#include <EebusCallbacks.hpp>

#include <everest/logging.hpp>

namespace module::eebus {

bool EEBusCallbacks::all_callbacks_valid() const {
    return this->update_limits_callback != nullptr;
}

} // namespace module::eebus
