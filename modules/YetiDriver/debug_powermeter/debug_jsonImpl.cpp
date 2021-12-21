// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
