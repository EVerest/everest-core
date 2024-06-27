// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "phyverso_mcu_temperatureImpl.hpp"

namespace module {
namespace phyverso_mcu_temperature {

namespace {
constexpr auto REFERENCE_VOLTAGE = 3.3;
constexpr auto NUMBER_OF_BITS = 12;
constexpr auto VOLTAGE_TO_TEMPERATURE_SLOPE = -31.0;
constexpr auto VOLTAGE_TO_TEMPERATURE_OFFSET = 92.8;

float get_temp(int raw) {
    float voltage = (static_cast<float>(raw) / ((1 << NUMBER_OF_BITS) - 1)) * REFERENCE_VOLTAGE;
    return VOLTAGE_TO_TEMPERATURE_SLOPE * voltage + VOLTAGE_TO_TEMPERATURE_OFFSET;
}
} // namespace

void phyverso_mcu_temperatureImpl::init() {
    mod->serial.signal_temperature.connect([this](Temperature temperature) {
        types::phyverso_mcu_temperature::MCUTemperatures t;
        t.temperatures = std::vector<float>();
        t.temperatures->reserve(temperature.temp_count);

        for (size_t i = 0; i < temperature.temp_count; ++i) {
            t.temperatures->push_back(get_temp(temperature.temp[i]));
        }
        this->publish_MCUTemperatures(t);
    });
}

void phyverso_mcu_temperatureImpl::ready() {
}

} // namespace phyverso_mcu_temperature
} // namespace module
