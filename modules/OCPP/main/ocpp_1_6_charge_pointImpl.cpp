// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "ocpp_1_6_charge_pointImpl.hpp"

namespace module {
namespace main {

void ocpp_1_6_charge_pointImpl::init() {
}

void ocpp_1_6_charge_pointImpl::ready() {
}

bool ocpp_1_6_charge_pointImpl::handle_stop() {
    mod->charging_schedules_timer->stop();
    return mod->charge_point->stop();
}
bool ocpp_1_6_charge_pointImpl::handle_restart() {
    mod->charging_schedules_timer->interval(std::chrono::seconds(this->mod->config.PublishChargingScheduleIntervalS));
    return mod->charge_point->restart();
}

} // namespace main
} // namespace module
