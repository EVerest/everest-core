#include "debug_jsonImpl.hpp"

namespace module {
namespace debug_powermeter {

void debug_jsonImpl::init() {
    mod->serial.signalPowerMeter.connect(
        [this](const PowerMeter& p) { publish_debug_json(power_meter_data_to_json(p)); });
}

void debug_jsonImpl::ready() {
}

} // namespace debug_powermeter
} // namespace module
