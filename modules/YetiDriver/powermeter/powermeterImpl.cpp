// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "powermeterImpl.hpp"
#include <utils/date.hpp>

namespace module {
namespace powermeter {

static types::powermeter::Powermeter yeti_to_everest(const PowerMeter& p) {
    types::powermeter::Powermeter j;

    j.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    j.meter_id = "YETI_POWERMETER";
    j.phase_seq_error = p.phaseSeqError;

    j.energy_Wh_import.total = p.totalWattHr;
    j.energy_Wh_import.L1 = p.wattHrL1;
    j.energy_Wh_import.L2 = p.wattHrL2;
    j.energy_Wh_import.L3 = p.wattHrL3;

    types::units::Power pwr;
    pwr.total = p.wattL1 + p.wattL2 + p.wattL3;
    pwr.L1 = p.wattL1;
    pwr.L2 = p.wattL2;
    pwr.L3 = p.wattL3;
    j.power_W = pwr;

    types::units::Voltage volt;
    volt.L1 = p.vrmsL1;
    volt.L2 = p.vrmsL2;
    volt.L3 = p.vrmsL3;
    j.voltage_V = volt;

    types::units::Current amp;
    amp.L1 = p.irmsL1;
    amp.L2 = p.irmsL2;
    amp.L3 = p.irmsL3;
    amp.N = p.irmsN;
    j.current_A = amp;

    types::units::Frequency freq;
    freq.L1 = p.freqL1;
    freq.L2 = p.freqL2;
    freq.L3 = p.freqL3;
    j.frequency_Hz = freq;

    return j;
}

void powermeterImpl::init() {
    mod->serial.signalPowerMeter.connect([this](const PowerMeter& p) { publish_powermeter(yeti_to_everest(p)); });
}

void powermeterImpl::ready() {
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            {},
            {},
            "YetiDriver powermeter does not support the stop_transaction command"};
};

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            "YetiDriver powermeter does not support the start_transaction command"};
}

} // namespace powermeter
} // namespace module
