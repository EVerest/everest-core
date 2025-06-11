// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_megawatt_chargingImpl.hpp"

namespace module {
namespace mcs {

void evse_megawatt_chargingImpl::init() {
}

void evse_megawatt_chargingImpl::ready() {
}

void evse_megawatt_chargingImpl::handle_ce_on(double& value) {
    const auto dutycycle = value / 100.0;
    mod->pwm_on(dutycycle);
}

void evse_megawatt_chargingImpl::handle_ce_off() {
    mod->pwm_off();
}

} // namespace mcs
} // namespace module
