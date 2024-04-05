// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"

namespace module {
namespace powermeter {

static types::powermeter::Powermeter umwc_to_everest(const PowerMeter& p) {
    types::powermeter::Powermeter j;

    j.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    j.meter_id = "UMWC_POWERMETER";

    j.energy_Wh_import.total = 0;

    types::units::Power pwr;
    pwr.total = 0;
    j.power_W = pwr;

    types::units::Voltage volt;
    volt.DC = p.voltage;
    j.voltage_V = volt;

    types::units::Current amp;
    amp.DC = 0;
    j.current_A = amp;

    return j;
}

void powermeterImpl::init() {
    mod->serial.signalPowerMeter.connect([this](const PowerMeter& p) { publish_powermeter(umwc_to_everest(p)); });
}

void powermeterImpl::ready() {
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            {},
            {},
            "MicroMegaWattBSP powermeter does not support the stop_transaction command"};
};

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            "MicroMegaWattBSP powermeter does not support the start_transaction command"};
}

} // namespace powermeter
} // namespace module
