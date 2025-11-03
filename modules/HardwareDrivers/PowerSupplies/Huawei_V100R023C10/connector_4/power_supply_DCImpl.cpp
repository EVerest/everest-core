// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"

namespace module {
namespace connector_4 {

void power_supply_DCImpl::init() {
    base.ev_set_config(EverestConnectorConfig::from_everest(config));
    base.ev_set_mod(mod);
    base.ev_init();
}

void power_supply_DCImpl::ready() {
    base.ev_ready();
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& mode,
                                         types::power_supply_DC::ChargingPhase& phase) {
    base.ev_handle_setMode(mode, phase);
}

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    base.ev_handle_setExportVoltageCurrent(voltage, current);
}

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    base.ev_handle_setImportVoltageCurrent(voltage, current);
}

} // namespace connector_4
} // namespace module
