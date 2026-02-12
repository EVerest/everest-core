#ifndef EVEREST_CORE_MODULES_EEBUS_INCLUDE_EEBUS_CALLBACKS_HPP
#define EVEREST_CORE_MODULES_EEBUS_INCLUDE_EEBUS_CALLBACKS_HPP

#include <generated/types/energy.hpp>

namespace module::eebus {

struct EEBusCallbacks {
    [[nodiscard]] bool all_callbacks_valid() const;
    std::function<void(const types::energy::ExternalLimits& new_limits)> update_limits_callback;
};

} // namespace module::eebus

#endif // EVEREST_CORE_MODULES_EEBUS_INCLUDE_EEBUS_CALLBACKS_HPP
