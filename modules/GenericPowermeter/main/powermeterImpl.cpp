// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include <fmt/core.h>
#include <thread>
#include <utils/date.hpp>
#include <utils/yaml_loader.hpp>

const std::string MODELS_SUB_DIR = "models";

namespace fs = std::filesystem;

namespace module {
namespace main {

void powermeterImpl::init() {

    std::size_t found = config.model.find(".."); // check for invalid path
    if (found != std::string::npos) {
        EVLOG_error << "Error! Substring \"..\" not allowed in model name!\n";
    } else {
        // FIXME (aw): path validation?
        auto model = this->mod->info.paths.share / MODELS_SUB_DIR / fmt::format("{}.yaml", config.model);

        try {
            json powermeter_registers = Everest::load_yaml(model);
            this->init_register_assignments(std::move(powermeter_registers));
            this->init_default_values();
        } catch (const std::exception& e) {
            EVLOG_error << "opening file \"" << config.model << ".yaml\" from path " << model
                        << "\" failed: " << e.what();
            throw std::runtime_error("Module \"GenericPowermeter\" could not be initialized!");
        }
    }
}

void powermeterImpl::ready() {
    if (this->config_loaded_successfully) {
        std::thread t([this] {
            while (true) {
                read_powermeter_values();
                sleep(1);
            }
        });
        t.detach();
    }
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            {},
            {},
            "Generic powermeter does not support the stop_transaction command"};
};

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    return {types::powermeter::TransactionRequestStatus::NOT_SUPPORTED,
            "Generic powermeter does not support the start_transaction command"};
}

void powermeterImpl::init_default_values() {
    this->pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    this->pm_last_values.meter_id = std::string(this->mod->info.id);

    for (auto& register_data : this->pm_configuration) {
        if (register_data.type == ENERGY_WH_IMPORT_TOTAL) {
            this->pm_last_values.energy_Wh_import.total = 0.0f;
        } else if (register_data.type == ENERGY_WH_IMPORT_L1) {
            this->pm_last_values.energy_Wh_import.L1 = 0.0f;
        } else if (register_data.type == ENERGY_WH_IMPORT_L2) {
            this->pm_last_values.energy_Wh_import.L2 = 0.0f;
        } else if (register_data.type == ENERGY_WH_IMPORT_L3) {
            this->pm_last_values.energy_Wh_import.L3 = 0.0f;
        } else if (register_data.type == ENERGY_WH_EXPORT_TOTAL) {
            types::units::Energy energy_out;
            if (this->pm_last_values.energy_Wh_export.has_value()) {
                energy_out = this->pm_last_values.energy_Wh_export.value();
            }
            energy_out.total = 0.0f;
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == ENERGY_WH_EXPORT_L1) {
            types::units::Energy energy_out;
            if (this->pm_last_values.energy_Wh_export.has_value()) {
                energy_out = this->pm_last_values.energy_Wh_export.value();
            }
            energy_out.L1 = 0.0f;
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == ENERGY_WH_EXPORT_L2) {
            types::units::Energy energy_out;
            if (this->pm_last_values.energy_Wh_export.has_value()) {
                energy_out = this->pm_last_values.energy_Wh_export.value();
            }
            energy_out.L2 = 0.0f;
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == ENERGY_WH_EXPORT_L3) {
            types::units::Energy energy_out;
            if (this->pm_last_values.energy_Wh_export.has_value()) {
                energy_out = this->pm_last_values.energy_Wh_export.value();
            }
            energy_out.L3 = 0.0f;
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == POWER_W_TOTAL) {
            types::units::Power pwr;
            if (this->pm_last_values.power_W.has_value()) {
                pwr = this->pm_last_values.power_W.value();
            }
            pwr.total = 0.0f;
            this->pm_last_values.power_W = pwr;
        } else if (register_data.type == POWER_W_L1) {
            types::units::Power pwr;
            if (this->pm_last_values.power_W.has_value()) {
                pwr = this->pm_last_values.power_W.value();
            }
            pwr.L1 = 0.0f;
            this->pm_last_values.power_W = pwr;
        } else if (register_data.type == POWER_W_L2) {
            types::units::Power pwr;
            if (this->pm_last_values.power_W.has_value()) {
                pwr = this->pm_last_values.power_W.value();
            }
            pwr.L2 = 0.0f;
            this->pm_last_values.power_W = pwr;
        } else if (register_data.type == POWER_W_L3) {
            types::units::Power pwr;
            if (this->pm_last_values.power_W.has_value()) {
                pwr = this->pm_last_values.power_W.value();
            }
            pwr.L3 = 0.0f;
            this->pm_last_values.power_W = pwr;
        } else if (register_data.type == VOLTAGE_V_DC) {
            types::units::Voltage volt;
            if (this->pm_last_values.voltage_V.has_value()) {
                volt = this->pm_last_values.voltage_V.value();
            }
            volt.DC = 0.0f;
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == VOLTAGE_V_L1) {
            types::units::Voltage volt;
            if (this->pm_last_values.voltage_V.has_value()) {
                volt = this->pm_last_values.voltage_V.value();
            }
            volt.L1 = 0.0f;
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == VOLTAGE_V_L2) {
            types::units::Voltage volt;
            if (this->pm_last_values.voltage_V.has_value()) {
                volt = this->pm_last_values.voltage_V.value();
            }
            volt.L2 = 0.0f;
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == VOLTAGE_V_L3) {
            types::units::Voltage volt;
            if (this->pm_last_values.voltage_V.has_value()) {
                volt = this->pm_last_values.voltage_V.value();
            }
            volt.L3 = 0.0f;
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == REACTIVE_POWER_VAR_TOTAL) {
            types::units::ReactivePower var;
            if (this->pm_last_values.VAR.has_value()) {
                var = this->pm_last_values.VAR.value();
            }
            var.total = 0.0f;
            this->pm_last_values.VAR = var;
        } else if (register_data.type == REACTIVE_POWER_VAR_L1) {
            types::units::ReactivePower var;
            if (this->pm_last_values.VAR.has_value()) {
                var = this->pm_last_values.VAR.value();
            }
            var.L1 = 0.0f;
            this->pm_last_values.VAR = var;
        } else if (register_data.type == REACTIVE_POWER_VAR_L2) {
            types::units::ReactivePower var;
            if (this->pm_last_values.VAR.has_value()) {
                var = this->pm_last_values.VAR.value();
            }
            var.L2 = 0.0f;
            this->pm_last_values.VAR = var;
        } else if (register_data.type == REACTIVE_POWER_VAR_L3) {
            types::units::ReactivePower var;
            if (this->pm_last_values.VAR.has_value()) {
                var = this->pm_last_values.VAR.value();
            }
            var.L3 = 0.0f;
            this->pm_last_values.VAR = var;
        } else if (register_data.type == CURRENT_A_DC) {
            types::units::Current amp;
            if (this->pm_last_values.current_A.has_value()) {
                amp = this->pm_last_values.current_A.value();
            }
            amp.DC = 0.0f;
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == CURRENT_A_L1) {
            types::units::Current amp;
            if (this->pm_last_values.current_A.has_value()) {
                amp = this->pm_last_values.current_A.value();
            }
            amp.L1 = 0.0f;
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == CURRENT_A_L2) {
            types::units::Current amp;
            if (this->pm_last_values.current_A.has_value()) {
                amp = this->pm_last_values.current_A.value();
            }
            amp.L2 = 0.0f;
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == CURRENT_A_L3) {
            types::units::Current amp;
            if (this->pm_last_values.current_A.has_value()) {
                amp = this->pm_last_values.current_A.value();
            }
            amp.L3 = 0.0f;
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == FREQUENCY_HZ_L1) {
            types::units::Frequency freq;
            if (this->pm_last_values.frequency_Hz.has_value()) {
                freq = this->pm_last_values.frequency_Hz.value();
            }
            freq.L1 = 0.0f;
            this->pm_last_values.frequency_Hz = freq;
        } else if (register_data.type == FREQUENCY_HZ_L2) {
            types::units::Frequency freq;
            if (this->pm_last_values.frequency_Hz.has_value()) {
                freq = this->pm_last_values.frequency_Hz.value();
            }
            freq.L2 = 0.0f;
            this->pm_last_values.frequency_Hz = freq;
        } else if (register_data.type == FREQUENCY_HZ_L3) {
            types::units::Frequency freq;
            if (this->pm_last_values.frequency_Hz.has_value()) {
                freq = this->pm_last_values.frequency_Hz.value();
            }
            freq.L3 = 0.0f;
            this->pm_last_values.frequency_Hz = freq;
        }
    }
}

void powermeterImpl::init_register_assignments(const json& loaded_registers) {
    uint8_t failed = 0;
    failed += assign_register_data(loaded_registers, ENERGY_WH_IMPORT_TOTAL, "energy_Wh_import");
    failed += assign_register_data(loaded_registers, ENERGY_WH_EXPORT_TOTAL, "energy_Wh_export");
    failed += assign_register_data(loaded_registers, POWER_W_TOTAL, "power_W");
    failed += assign_register_data(loaded_registers, VOLTAGE_V_DC, "voltage_V");
    failed += assign_register_data(loaded_registers, REACTIVE_POWER_VAR_TOTAL, "reactive_power_VAR");
    failed += assign_register_data(loaded_registers, CURRENT_A_DC, "current_A");
    failed += assign_register_data(loaded_registers, FREQUENCY_HZ_L1, "frequency_Hz");

    if (failed) {
        EVLOG_error << "Could not load powermeter configuration!\n";
        this->config_loaded_successfully = false;
    } else {
        this->config_loaded_successfully = true;
    }
}

int powermeterImpl::assign_register_data(const json& registers, const PowermeterRegisters register_type,
                                         const std::string& register_selector) {
    try {
        if (registers.contains(register_selector)) {
            if (registers.at(register_selector).at("num_registers") > 0) {
                RegisterData data = {};
                data.type = register_type;
                data.start_register = registers.at(register_selector).at("start_register");
                data.start_register_function = select_modbus_function(
                    (const uint8_t)registers.at(register_selector).at("function_code_start_reg"));
                data.num_registers = registers.at(register_selector).at("num_registers");
                data.exponent_register = registers.at(register_selector).at("exponent_register");
                data.exponent_register_function =
                    select_modbus_function((const uint8_t)registers.at(register_selector).at("function_code_exp_reg"));
                data.multiplier = registers.at(register_selector).at("multiplier");
                pm_configuration.push_back(data);
            }
            if (registers.at(register_selector).contains("L1")) {
                if (registers.at(register_selector).at("L1").at("num_registers") > 0) {
                    assign_register_sublevel_data(registers, register_type, register_selector, "L1", 1);
                }
            }
            if (registers.at(register_selector).contains("L2")) {
                if (registers.at(register_selector).at("L2").at("num_registers") > 0) {
                    assign_register_sublevel_data(registers, register_type, register_selector, "L2", 2);
                }
            }
            if (registers.at(register_selector).contains("L3")) {
                if (registers.at(register_selector).at("L3").at("num_registers") > 0) {
                    assign_register_sublevel_data(registers, register_type, register_selector, "L3", 3);
                }
            }
        }
    } catch (const std::exception& e) {
        EVLOG_error << "assigning configuration data for register \"" << register_selector << "\" failed: " << e.what();
        return -1;
    }

    return 0;
}

void powermeterImpl::assign_register_sublevel_data(const json& registers, const PowermeterRegisters& register_type,
                                                   const std::string& register_selector,
                                                   const std::string& sublevel_selector, const uint8_t offset) {

    RegisterData sublevel_data = {};
    sublevel_data.type = static_cast<PowermeterRegisters>((register_type + offset) % NUM_PM_REGISTERS);
    sublevel_data.start_register = registers.at(register_selector).at(sublevel_selector).at("start_register");
    sublevel_data.start_register_function = select_modbus_function(
        (const uint8_t)registers.at(register_selector).at(sublevel_selector).at("function_code_start_reg"));
    sublevel_data.num_registers = registers.at(register_selector).at(sublevel_selector).at("num_registers");
    sublevel_data.exponent_register = registers.at(register_selector).at(sublevel_selector).at("exponent_register");
    sublevel_data.exponent_register_function = select_modbus_function(
        (const uint8_t)registers.at(register_selector).at(sublevel_selector).at("function_code_exp_reg"));
    sublevel_data.multiplier = registers.at(register_selector).at(sublevel_selector).at("multiplier");
    pm_configuration.push_back(sublevel_data);
}

powermeterImpl::ModbusFunctionType powermeterImpl::select_modbus_function(const uint8_t function_code) {
    switch (function_code) {
    case 0x03:
        return READ_HOLDING_REGISTER;
        break;

    case 0x04:
        return READ_INPUT_REGISTER;
        break;

    default:
        throw std::runtime_error("Incorrect Modbus RTU function code!\n");
        break;
    }
    return REGISTER_TYPE_UNDEFINED;
}

void powermeterImpl::read_powermeter_values() {
    for (auto& register_data : this->pm_configuration) {
        readRegister(register_data);
    }
    this->pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    this->publish_powermeter(this->pm_last_values);
}

void powermeterImpl::readRegister(const RegisterData& register_config) {

    types::serial_comm_hub_requests::Result register_response{};
    types::serial_comm_hub_requests::Result exponent_response{};

    if (register_config.start_register_function == READ_HOLDING_REGISTER) {
        register_response = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
            config.powermeter_device_id, register_config.start_register, register_config.num_registers);
    }

    if (register_config.start_register_function == READ_INPUT_REGISTER) {
        register_response = mod->r_serial_comm_hub->call_modbus_read_input_registers(
            config.powermeter_device_id, register_config.start_register - config.modbus_base_address,
            register_config.num_registers);
    }

    if (register_config.exponent_register != 0) {
        if (register_config.exponent_register_function == READ_HOLDING_REGISTER) {
            exponent_response = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
                config.powermeter_device_id, register_config.exponent_register, register_config.num_registers);
        }

        if (register_config.exponent_register_function == READ_INPUT_REGISTER) {
            exponent_response = mod->r_serial_comm_hub->call_modbus_read_input_registers(
                config.powermeter_device_id, register_config.exponent_register - config.modbus_base_address,
                register_config.num_registers);
        }
    }

    process_response(register_config, std::move(register_response), std::move(exponent_response));
}

void powermeterImpl::process_response(const RegisterData& register_data,
                                      const types::serial_comm_hub_requests::Result register_message,
                                      const types::serial_comm_hub_requests::Result exponent_message) {

    if (register_message.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {

        int16_t exponent{0};
        if (exponent_message.value.has_value()) {
            exponent = exponent_message.value.value()[0];
        } else {
            exponent = 0.0;
        }

        if (register_data.type == ENERGY_WH_IMPORT_TOTAL) {
            types::units::Energy energy_in = this->pm_last_values.energy_Wh_import;
            energy_in.total = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_import = energy_in;
        } else if (register_data.type == ENERGY_WH_IMPORT_L1) {
            types::units::Energy energy_in = this->pm_last_values.energy_Wh_import;
            energy_in.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_import = energy_in;
        } else if (register_data.type == ENERGY_WH_IMPORT_L2) {
            types::units::Energy energy_in = this->pm_last_values.energy_Wh_import;
            energy_in.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_import = energy_in;
        } else if (register_data.type == ENERGY_WH_IMPORT_L3) {
            types::units::Energy energy_in = this->pm_last_values.energy_Wh_import;
            energy_in.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_import = energy_in;
        } else if (register_data.type == ENERGY_WH_EXPORT_TOTAL) {
            types::units::Energy energy_out = this->pm_last_values.energy_Wh_export.value();
            energy_out.total = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == ENERGY_WH_EXPORT_L1) {
            types::units::Energy energy_out = this->pm_last_values.energy_Wh_export.value();
            energy_out.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == ENERGY_WH_EXPORT_L2) {
            types::units::Energy energy_out = this->pm_last_values.energy_Wh_export.value();
            energy_out.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == ENERGY_WH_EXPORT_L3) {
            types::units::Energy energy_out = this->pm_last_values.energy_Wh_export.value();
            energy_out.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.energy_Wh_export = energy_out;
        } else if (register_data.type == POWER_W_TOTAL) {
            types::units::Power power = this->pm_last_values.power_W.value();
            power.total = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.power_W = power;
        } else if (register_data.type == POWER_W_L1) {
            types::units::Power power = this->pm_last_values.power_W.value();
            power.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.power_W = power;
        } else if (register_data.type == POWER_W_L2) {
            types::units::Power power = this->pm_last_values.power_W.value();
            power.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.power_W = power;
        } else if (register_data.type == POWER_W_L3) {
            types::units::Power power = this->pm_last_values.power_W.value();
            power.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.power_W = power;
        } else if (register_data.type == VOLTAGE_V_DC) {
            types::units::Voltage volt = this->pm_last_values.voltage_V.value();
            volt.DC = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == VOLTAGE_V_L1) {
            types::units::Voltage volt = this->pm_last_values.voltage_V.value();
            volt.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == VOLTAGE_V_L2) {
            types::units::Voltage volt = this->pm_last_values.voltage_V.value();
            volt.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == VOLTAGE_V_L3) {
            types::units::Voltage volt = this->pm_last_values.voltage_V.value();
            volt.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.voltage_V = volt;
        } else if (register_data.type == REACTIVE_POWER_VAR_TOTAL) {
            types::units::ReactivePower var = this->pm_last_values.VAR.value();
            var.total = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.VAR = var;
        } else if (register_data.type == REACTIVE_POWER_VAR_L1) {
            types::units::ReactivePower var = this->pm_last_values.VAR.value();
            var.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.VAR = var;
        } else if (register_data.type == REACTIVE_POWER_VAR_L2) {
            types::units::ReactivePower var = this->pm_last_values.VAR.value();
            var.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.VAR = var;
        } else if (register_data.type == REACTIVE_POWER_VAR_L3) {
            types::units::ReactivePower var = this->pm_last_values.VAR.value();
            var.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.VAR = var;
        } else if (register_data.type == CURRENT_A_DC) {
            types::units::Current amp = this->pm_last_values.current_A.value();
            amp.DC = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == CURRENT_A_L1) {
            types::units::Current amp = this->pm_last_values.current_A.value();
            amp.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == CURRENT_A_L2) {
            types::units::Current amp = this->pm_last_values.current_A.value();
            amp.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == CURRENT_A_L3) {
            types::units::Current amp = this->pm_last_values.current_A.value();
            amp.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.current_A = amp;
        } else if (register_data.type == FREQUENCY_HZ_L1) {
            types::units::Frequency freq = this->pm_last_values.frequency_Hz.value();
            freq.L1 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.frequency_Hz = freq;
        } else if (register_data.type == FREQUENCY_HZ_L2) {
            types::units::Frequency freq = this->pm_last_values.frequency_Hz.value();
            freq.L2 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.frequency_Hz = freq;
        } else if (register_data.type == FREQUENCY_HZ_L3) {
            types::units::Frequency freq = this->pm_last_values.frequency_Hz.value();
            freq.L3 = merge_register_values_into_element(register_data, exponent, register_message);
            this->pm_last_values.frequency_Hz = freq;
        } else {
        }
    } else {
        // error: message sending failed
        output_error_with_content(register_message);
    }
}

float powermeterImpl::merge_register_values_into_element(const RegisterData& reg_data, const int16_t exponent,
                                                         const types::serial_comm_hub_requests::Result& reg_message) {
    if (reg_message.value.has_value()) {
        uint32_t value{0};
        auto& reg_value = reg_message.value.value();
        if (reg_data.num_registers == 1 && reg_value.size() == 1) {
            value = reg_value.at(0);
        } else if (reg_data.num_registers == 2 && reg_value.size() == 2) {
            value += reg_value.at(0) << 16;
            value += reg_value.at(1);
        } else {
            throw std::runtime_error("Values of more than 2 registers in size are currently not supported!");
        }

        auto val = *reinterpret_cast<float*>(&value);
        auto val_scaled = float(val * reg_data.multiplier * pow(10.0, exponent));

        return val_scaled;
    } else {
        EVLOG_error << "Error! Received message is empty!\n";
    }
    return 0.0;
}

void powermeterImpl::output_error_with_content(const types::serial_comm_hub_requests::Result& response) {
    std::stringstream ss;

    if (response.value) {
        for (size_t i = 0; i < response.value->size(); i++) {
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
