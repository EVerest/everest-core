// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include <fmt/core.h>
#include <thread>
#include <utils/date.hpp>

namespace module {
namespace main {

void powermeterImpl::init() {
    this->init_default_values();
}

void powermeterImpl::ready() {
    std::thread t([this] {
        while (true) {
            read_powermeter_values();
            sleep(1);
        }
    });
    t.detach();
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    types::powermeter::TransactionStartResponse r;
    r.status = types::powermeter::TransactionRequestStatus::NOT_SUPPORTED;
    return r;
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    types::powermeter::TransactionStopResponse r;
    r.status = types::powermeter::TransactionRequestStatus::NOT_SUPPORTED;
    return r;
}

void powermeterImpl::init_default_values() {
    this->pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    this->pm_last_values.meter_id = std::string(this->mod->info.id);

    this->pm_last_values.energy_Wh_import.total = 0.0f;
    // this->pm_last_values.energy_Wh_export.total = 0.0f;

    types::units::Power pwr;
    pwr.total = 0.0f;
    this->pm_last_values.power_W = pwr;

    types::units::Voltage volt;
    volt.DC = 0.0f;
    this->pm_last_values.voltage_V = volt;

    types::units::Current amp;
    amp.DC = 0.0f;
    this->pm_last_values.current_A = amp;
}

void powermeterImpl::read_powermeter_values() {
    // read power data
    auto power_data_response =
        mod->r_serial_comm_hub->call_modbus_read_holding_registers(config.powermeter_device_id, 0x0000, 20);
    process_power_data_message(power_data_response);

    this->pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    // // read "current rate": "sharp", "peak", "shoulder", "off-peak"
    // types::serial_comm_hub_requests::Result current_rate_response =
    // mod->r_serial_comm_hub->call_modbus_read_holding_registers(config.powermeter_device_id, 0x0021, 4, 0);
    // process_current_rate_message(current_rate_response);

    this->publish_powermeter(this->pm_last_values);
}

void powermeterImpl::process_power_data_message(const types::serial_comm_hub_requests::Result message) {
    if (message.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        types::units::Voltage volt;
        volt.DC = message.value.value()[DC_VOLTAGE] * pow(10.0, (message.value.value()[DC_VOLT_DECIMAL_POINT] - 3));
        this->pm_last_values.voltage_V = volt;

        types::units::Current amp;
        amp.DC = (int16_t)(message.value.value()[DC_CURRENT]) *
                 pow(10.0, (message.value.value()[DC_CURR_DECIMAL_POINT] - 3));
        this->pm_last_values.current_A = amp;

        types::units::Power power;
        power.total =
            (int16_t)(message.value.value()[POWER]) * pow(10.0, (message.value.value()[POWER_DECIMAL_POINT] - 3));
        this->pm_last_values.power_W = power;

        types::units::Energy energy_in;
        energy_in.total = float(uint32_t(message.value.value()[TOTAL_POS_ACT_ENERGY_HIGH] << 16) |
                                uint32_t(message.value.value()[TOTAL_POS_ACT_ENERGY_LOW]));
        this->pm_last_values.energy_Wh_import = energy_in;

        types::units::Energy energy_out;
        energy_out.total = float(uint32_t(message.value.value()[TOTAL_REV_ACT_ENERGY_HIGH] << 16) |
                                 uint32_t(message.value.value()[TOTAL_REV_ACT_ENERGY_LOW]));
        this->pm_last_values.energy_Wh_export = energy_out;
    } else {
        // error: message sending failed
        output_error_with_content(message);
    }
}

// void powermeterImpl::process_current_rate_message(const types::serial_comm_hub_requests::Result message) {
//     if (message.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {

//     } else {
//          // error: message sending failed
//          output_error_with_content(message);
//     }
// }

void powermeterImpl::output_error_with_content(const types::serial_comm_hub_requests::Result& response) {
    std::stringstream ss;

    if (response.value.has_value()) {
        for (size_t i = 0; i < response.value.value().size(); i++) {
            if (i != 0)
                ss << ", ";
            ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << int(response.value.value()[i]);
        }
    }

    EVLOG_debug << "received error response: " << status_code_enum_to_string(response.status_code) << " (" << ss.str()
                << ")\n";
}

} // namespace main
} // namespace module
