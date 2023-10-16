// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "isolation_monitorImpl.hpp"
#include <fmt/core.h>
#include <thread>
#include <utils/date.hpp>

namespace module {
namespace main {

void isolation_monitorImpl::init() {
    this->init_registers();
}

void isolation_monitorImpl::init_registers() {
    add_register_to_query(ImdRegisters::RESISTANCE_R_F_OHM);
    add_register_to_query(ImdRegisters::VOLTAGE_U_N_V);
    // add_register_to_query(ImdRegisters::VOLTAGE_U_L1E_V);
    // add_register_to_query(ImdRegisters::VOLTAGE_U_L2E_V);
    // add_register_to_query(ImdRegisters::RESISTANCE_R_FU_OHM);
}

void isolation_monitorImpl::init_device() {
    send_to_imd(3005, config.threshold_resistance_kohm); // set "pre-alarm" resistor R1 (default = 600k)
    send_to_imd(3007,
                (config.threshold_resistance_kohm - 10)); // set "alarm" resistor R2 to slightly lower than R1 (= -10k)
    send_to_imd(3008, 0);                                 // disable low-voltage alarm
    send_to_imd(3010, 0);                                 // disable overvoltage alarm
    send_to_imd(3023, 0);                                 // set mode to "dc"
    send_to_imd(3021, 0);                                 // disable automatic test
    send_to_imd(3024, 0);                                 // disable line voltage test
    send_to_imd(3025, 0);                                 // disable self-test on power-up
    if (config.threshold_resistance_kohm == 600) { // only disable device, if it is in its default threshold setting
        disable_device();                          // (otherwise it is assumed that the internal relays are being used
                                                   //  and that automatic switching is desired)
    } else {
        enable_device();
    }
}

void isolation_monitorImpl::enable_device() {
    send_to_imd(3026, 1);
    EVLOG_debug << "IMD beginning measurements.";
}

void isolation_monitorImpl::disable_device() {
    if (config.threshold_resistance_kohm == 600) { // only disable device, if it is in its default threshold setting
        send_to_imd(3026, 0);                      // (otherwise it is assumed that the internal relays are being used
                                                   //  and that automatic switching is desired)
        EVLOG_debug << "IMD has stopped measurements.";
    }
}

void isolation_monitorImpl::send_to_imd(const uint16_t& command, const uint16_t& value) {
    if (mod->r_serial_comm_hub->call_modbus_write_multiple_registers(
            config.imd_device_id, static_cast<int>(command),
            (types::serial_comm_hub_requests::VectorUint16){{value}}) !=
        types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        EVLOG_AND_THROW(std::runtime_error("Sending configuration data failed!"));
    }
}

void isolation_monitorImpl::ready() {
    this->init_device();
    std::thread([this] {
        while (42) {
            if (this->enable_publishing) {
                read_imd_values();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();
    EVLOG_info << "IMD ready for measurements.";
}

void isolation_monitorImpl::dead_time_wait() {
    std::this_thread::sleep_for(std::chrono::seconds(DEAD_TIME_S));
    this->dead_time_flag = false;
}

void isolation_monitorImpl::handle_start() {
    if (this->dead_time_flag != true) {
        request_start();
    } else { // if busy first time, try again later
        std::this_thread::sleep_for(std::chrono::seconds(DEAD_TIME_S));
        request_start();
    }
}

void isolation_monitorImpl::handle_stop() {
    if (this->dead_time_flag != true) {
        request_stop();
    } else { // if busy first time, try again later
        std::this_thread::sleep_for(std::chrono::seconds(DEAD_TIME_S));
        request_stop();
    }
}

void isolation_monitorImpl::request_start() {
    if (this->dead_time_flag != true) {
        enable_device();
        this->enable_publishing = true;
        this->dead_time_flag = true;
        std::thread([this]() { dead_time_wait(); }).detach();
    }
}

void isolation_monitorImpl::request_stop() {
    if (this->dead_time_flag != true) {
        this->enable_publishing = false;
        disable_device();
        this->dead_time_flag = true;
        std::thread([this]() { dead_time_wait(); }).detach();
    }
}

void isolation_monitorImpl::add_register_to_query(const ImdRegisters register_type) {
    RegisterData data = {};
    data.type = register_type;
    data.start_register = static_cast<uint16_t>(register_type);
    data.register_function = ModbusFunctionType::READ_HOLDING_REGISTER;
    data.num_registers = 4;
    data.multiplier = 1;
    this->imd_read_configuration.push_back(data);
}

void isolation_monitorImpl::read_imd_values() {
    for (auto& register_data : this->imd_read_configuration) {
        readRegister(register_data);
    }
    this->publish_IsolationMeasurement(this->isolation_measurement_data);
}

void isolation_monitorImpl::readRegister(const RegisterData& register_config) {

    types::serial_comm_hub_requests::Result register_response{};

    if (register_config.register_function == ModbusFunctionType::READ_HOLDING_REGISTER) {
        register_response = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
            config.imd_device_id, register_config.start_register, register_config.num_registers);
    }
    process_response(register_config, register_response);
    update_IsolationMeasurement();
}

void isolation_monitorImpl::process_response(const RegisterData& register_data,
                                             const types::serial_comm_hub_requests::Result& register_message) {

    if (register_message.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        if (register_data.type == ImdRegisters::RESISTANCE_R_F_OHM) {
            this->imd_last_values.resistance_R_F_ohm = extract_register_values(register_data, register_message);
        } else if (register_data.type == ImdRegisters::RESISTANCE_R_FU_OHM) {
            this->imd_last_values.resistance_R_FU_ohm = extract_register_values(register_data, register_message);
        } else if (register_data.type == ImdRegisters::VOLTAGE_U_N_V) {
            this->imd_last_values.voltage_U_N_V = extract_register_values(register_data, register_message);
        } else if (register_data.type == ImdRegisters::VOLTAGE_U_L1E_V) {
            this->imd_last_values.voltage_U_L1e_V = extract_register_values(register_data, register_message);
        } else if (register_data.type == ImdRegisters::VOLTAGE_U_L2E_V) {
            this->imd_last_values.voltage_U_L2e_V = extract_register_values(register_data, register_message);
        } else {
        }
    } else {
        // error: message sending failed
        output_error_with_content(register_message);
    }
}

double isolation_monitorImpl::extract_register_values(const RegisterData& reg_data,
                                                      const types::serial_comm_hub_requests::Result& reg_message) {
    if (reg_message.value.has_value()) {
        uint32_t value{0};
        auto& reg_value = reg_message.value.value();
        if (reg_data.num_registers >= 2 && reg_value.size() >= 2) {
            value += reg_value.at(0) << 16;
            value += reg_value.at(1);
        } else {
            EVLOG_AND_THROW(std::runtime_error("Values of less than 2 registers in size are not supported!"));
        }
        uint8_t alarm_status = static_cast<uint8_t>((reg_value.at(2) >> 8) & 0x00FF);
        uint8_t value_status_u8 = static_cast<uint8_t>(reg_value.at(2) & 0x00FF);
        ValueStatus value_status = get_value_status(value_status_u8);
        uint16_t channel_description = reg_value.at(3);

        auto val = *reinterpret_cast<float*>(&value);
        auto val_scaled = double(val * reg_data.multiplier);

        if (value_status == ValueStatus::VALUE_INVALID) {
            EVLOG_error << "(=/=) measurement invalid";
            val_scaled = 0.0;
        } else if (value_status == ValueStatus::TRUE_VALUE_LARGER) {
            EVLOG_debug << "(>) real value larger than measurement result";
        } else if (value_status == ValueStatus::TRUE_VALUE_SMALLER) {
            EVLOG_debug << "WARNING! Real value SMALLER than measurement result!";
            val_scaled = 0.0;
        }

        EVLOG_debug << "value: " << val << "\nalarm_status: " << get_alarm_status(alarm_status)
                    << "\nchannel_description: 0x" << std::hex << channel_description;

        return val_scaled;
    } else {
        EVLOG_error << "Error! Received message is empty!\n";
    }
    return 0.0;
}

std::string isolation_monitorImpl::get_alarm_status(const uint8_t& alarm_reg) {
    std::string status{};
    if (!((alarm_reg >> 6) ^ EXTERNAL_TEST_STATE)) {
        status = "external test";
    } else if (!((alarm_reg >> 6) ^ INTERNAL_TEST_STATE)) {
        status = "internal test";
    } else if (!((alarm_reg & ALARM_BITS_MASK) ^ PRE_WARNING_STATE)) {
        status = "pre-warning";
    } else if (!((alarm_reg & ALARM_BITS_MASK) ^ DEVICE_ERROR_STATE)) {
        status = "device error";
    } else if (!((alarm_reg & ALARM_BITS_MASK) ^ WARNING_STATE)) {
        status = "warning";
    } else if (!((alarm_reg & ALARM_BITS_MASK) ^ ALARM_STATE)) {
        status = "alarm";
    } else if ((alarm_reg & ALARM_AND_TEST_BITS_MASK) == 0) {
        status = "no error";
    } else {
        status = "communication error: reserved state received";
    }
    return status;
}

isolation_monitorImpl::ValueStatus isolation_monitorImpl::get_value_status(const uint8_t& unit_reg) {
    ValueStatus status{};
    if (!((unit_reg >> 6) ^ static_cast<uint8_t>(ValueStatus::TRUE_VALUE_SMALLER))) {
        status = ValueStatus::TRUE_VALUE_SMALLER;
    } else if (!((unit_reg >> 6) ^ static_cast<uint8_t>(ValueStatus::TRUE_VALUE_LARGER))) {
        status = ValueStatus::TRUE_VALUE_LARGER;
    } else if (!((unit_reg >> 6) ^ static_cast<uint8_t>(ValueStatus::VALUE_INVALID))) {
        status = ValueStatus::VALUE_INVALID;
    } else {
        status = ValueStatus::VALUE_IS_TRUE;
    }
    return status;
}

void isolation_monitorImpl::output_error_with_content(const types::serial_comm_hub_requests::Result& response) {
    std::stringstream ss;

    if (response.value) {
        for (size_t i = 0; i < response.value->size(); i++) {
            if (i != 0)
                ss << ", ";
            ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << int(response.value.value()[i]);
        }
    }
    EVLOG_debug << "received error response: " << status_code_enum_to_string(response.status_code) << " (" << ss.str()
                << ")";
}

void isolation_monitorImpl::update_IsolationMeasurement() {
    this->isolation_measurement_data.resistance_F_Ohm = this->imd_last_values.resistance_R_F_ohm;
    this->isolation_measurement_data.voltage_V = this->imd_last_values.voltage_U_N_V;
}

} // namespace main
} // namespace module
