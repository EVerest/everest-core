// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"

#include <everest_api_types/power_supply_DC/API.hpp>
#include <everest_api_types/power_supply_DC/codec.hpp>
#include <everest_api_types/power_supply_DC/wrapper.hpp>

#include <generated/types/power_supply_DC.hpp>

namespace module {
namespace main {

void power_supply_DCImpl::init() {
}

void power_supply_DCImpl::ready() {
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& mode,
                                         types::power_supply_DC::ChargingPhase& phase) {
    auto topic = mod->get_topics().everest_to_extern("mode");
    auto mode_ext = API_types_ext::to_external_api(mode);
    auto phase_ext = API_types_ext::to_external_api(phase);
    auto data = API_types_ext::ModeRequest{mode_ext, phase_ext};
    mod->mqtt.publish(topic, serialize(data));
}

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    auto topic = mod->get_topics().everest_to_extern("export_voltage_current");
    auto data = API_types_ext::VoltageCurrent{static_cast<float>(voltage), static_cast<float>(current)};
    mod->mqtt.publish(topic, serialize(data));
}

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    auto topic = mod->get_topics().everest_to_extern("import_voltage_current");
    auto data = API_types_ext::VoltageCurrent{static_cast<float>(voltage), static_cast<float>(current)};
    mod->mqtt.publish(topic, serialize(data));
}

} // namespace main
} // namespace module
