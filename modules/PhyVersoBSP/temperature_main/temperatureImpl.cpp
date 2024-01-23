// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "temperatureImpl.hpp"
#include <generated/types/temperature.hpp>

namespace module {
namespace temperature_main {


constexpr auto REFERENCE_VOLTAGE = 3.3;
constexpr auto NUMBER_OF_BITS = 12;
constexpr auto VOLTAGE_TO_TEMPERATURE_SLOPE = -31.0;
constexpr auto VOLTAGE_TO_TEMPERATURE_OFFSET = 92.8;

float get_temp(int raw) {
    int voltage = (raw / ((1 << NUMBER_OF_BITS) - 1)) * REFERENCE_VOLTAGE;
    return VOLTAGE_TO_TEMPERATURE_SLOPE * voltage + VOLTAGE_TO_TEMPERATURE_OFFSET;
}

void temperatureImpl::init() {
    mod->serial.signal_temperature.connect([this](Temperature temperature) {
        types::temperature::Temperatures temperatures;
        for(size_t i=0; i < temperature.temp_count; ++i) {
            temperatures.temperatures->push_back(get_temp(temperature.temp[i]));
        }
        publish_temperatures(temperatures);
    });
}

void temperatureImpl::ready() {
}

} // namespace temperature_main
} // namespace module
