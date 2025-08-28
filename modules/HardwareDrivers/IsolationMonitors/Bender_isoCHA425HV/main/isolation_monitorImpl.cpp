// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitorImpl.hpp"
#include <fmt/core.h>
#include <thread>
#include <utils/date.hpp>

namespace module {
namespace main {

void isolation_monitorImpl::init() {
}

void isolation_monitorImpl::configure_device() {
    // Query device name and version
    bool successful = true;
    do {
        successful = true;
        successful &= send_to_imd(3005, config.r1_prealarm_kohm);
        successful &= send_to_imd(3007, config.r2_alarm_kohm);
        successful &= send_to_imd(3008, (config.undervoltage_alarm_enable ? 1 : 0));
        successful &= send_to_imd(3009, config.undervoltage_alarm_threshold_V);
        successful &= send_to_imd(3010, (config.overvoltage_alarm_enable ? 1 : 0));
        successful &= send_to_imd(3011, config.overvoltage_alarm_threshold_V);
        successful &= send_to_imd(3012, (config.alarm_memory_enable ? 1 : 0));
        successful &= send_to_imd(3013, (config.relais_r1_mode ? 1 : 0));
        successful &= send_to_imd(3014, (config.relais_r2_mode ? 1 : 0));
        successful &= send_to_imd(3018, (config.delay_startup_device ? 1 : 0));
        successful &= send_to_imd(3019, config.delay_t_on_k1_k2);
        successful &= send_to_imd(3020, config.delay_t_off_k1_k2);
        successful &= send_to_imd(3023, (config.chademo_mode ? 1 : 0));
        successful &= send_to_imd(3024, (config.selftest_enable_gridconnection ? 1 : 0));
        successful &= send_to_imd(3025, (config.selftest_enable_at_start ? 1 : 0));
        // start up
        successful &= send_to_imd(3026, 1);

        if (successful) {
            EVLOG_info << "IMD Device: " << read_device_name() << " (" << read_firmware_version() << ")";
            faster_cable_check_supported = check_for_faster_cablecheck();

            if (faster_cable_check_supported) {
                EVLOG_info << "Supports faster cable check method";
                enable_faster_cable_check_mode();
            } else {
                EVLOG_info
                    << "Does not support faster cable check method, falling back to long self test. This may create "
                       "timeouts with certain cars in CableCheck. Consider upgrading to at least firmware 5.00";
                disable_faster_cable_check_mode();
            }
        } else {
            // allow the system to recover and don't hog the MODBUS
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    } while (not successful);
}

void isolation_monitorImpl::start_self_test() {
    if (last_test != TestType::ExternalTest) {
        if (mod->r_serial_comm_hub->call_modbus_write_multiple_registers(
                config.imd_device_id, static_cast<int>(8005),
                (types::serial_comm_hub_requests::VectorUint16){{0x5445}}) !=
            types::serial_comm_hub_requests::StatusCodeEnum::Success) {
            set_deviceFault("Failed to start self test");
            self_test_started = false;
        }
    } else {
        EVLOG_info << "Self test already running, no need to start again";
        self_test_started = true;
        self_test_timeout = 30;
    }
}

void isolation_monitorImpl::set_deviceFault(const std::string& message) {
    if (!error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
        auto error =
            error_factory->create_error("isolation_monitor/DeviceFault", "", message, Everest::error::Severity::High);
        raise_error(error);
    }
}

void isolation_monitorImpl::clear_deviceFault() {
    if (error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
        clear_error("isolation_monitor/DeviceFault");
    }
}

bool isolation_monitorImpl::enable_faster_cable_check_mode() {
    return send_to_imd(3021, 3);
}

bool isolation_monitorImpl::disable_faster_cable_check_mode() {
    return send_to_imd(3021, 0);
}

bool isolation_monitorImpl::send_to_imd(const uint16_t& command, const uint16_t& value) {
    if (mod->r_serial_comm_hub->call_modbus_write_multiple_registers(
            config.imd_device_id, static_cast<int>(command),
            (types::serial_comm_hub_requests::VectorUint16){{value}}) !=
        types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        EVLOG_error << "ModBus Command failed: " << command << " Value: " << value;
        set_deviceFault("Failed to write registers");
        return false;
    }
    return true;
}

void isolation_monitorImpl::ready() {
    this->configure_device();
    bool self_test_running = false;

    while (true) {
        read_imd_values();

        if (self_test_started) {
            self_test_timeout--;
            if (self_test_timeout <= 0) {
                // a time out happend
                self_test_started = false;
                // publish failed result
                EVLOG_warning << "Self test timed out";
                publish_self_test_result(false);
            }
            if (self_test_running) {
                if (last_test not_eq TestType::ExternalTest) {
                    // Self test is done
                    self_test_running = false;
                    self_test_started = false;
                    // was it successfull? If there is no error, it was...
                    bool result = true;
                    if (last_alarm == AlarmType::DeviceError) {
                        result = false;
                    }
                    EVLOG_info << "Self test completed: " << result;
                    publish_self_test_result(result);
                }
            } else {
                if (last_test == TestType::ExternalTest) {
                    self_test_running = true;
                }
            }
        }
        if (not error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
}

void isolation_monitorImpl::handle_start() {
    enable_publishing = true;
}

void isolation_monitorImpl::handle_stop() {
    enable_publishing = false;
}

void isolation_monitorImpl::handle_start_self_test(double& test_voltage_V) {
    EVLOG_info << "IMD Starting self-test...";
    start_self_test();
}

void isolation_monitorImpl::read_imd_values() {
    // Read rF
    auto rf = read_register(ImdRegisters::RESISTANCE_R_F_OHM);
    EVLOG_debug << to_string(rf);

    last_test = rf.test;
    last_alarm = rf.alarm;

    // Read Voltage
    auto voltage = read_register(ImdRegisters::VOLTAGE_U_N_V);
    EVLOG_debug << to_string(voltage);

    if (last_alarm == AlarmType::DeviceError) {
        set_deviceFault("Device Error");
    }

    bool valid_readings = true;
    if (enable_publishing or config.always_publish_measurements) {
        types::isolation_monitor::IsolationMeasurement m;
        if (voltage.valid not_eq ValidType::Invalid) {
            m.voltage_V = voltage.value;
        } else {
            valid_readings = false;
        }

        if (rf.valid not_eq ValidType::Invalid) {
            m.resistance_F_Ohm = rf.value;
            if (last_test == TestType::NoTest) {
                if (not error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
                    // do not publish values if in error state
                    publish_isolation_measurement(m);
                }
            }
        } else {
            valid_readings = false;
        }
    }
    // let at least one round of values unpublished not to use unstable ones
    if (last_alarm not_eq AlarmType::DeviceError and valid_readings and
        error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
        clear_deviceFault();
        configure_device();
    }
}

std::string isolation_monitorImpl::read_device_name() {
    auto result = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
        config.imd_device_id, static_cast<std::underlying_type_t<ImdRegisters>>(ImdRegisters::DEVICE_NAME), 10);
    std::string device_name;
    if (result.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        for (int character : result.value.value()) {
            device_name.push_back(static_cast<char>(character >> 8));
            device_name.push_back(static_cast<char>(character & 0xFF));
        }
    } else {
        set_deviceFault("Can't read device name");
    }

    // Strip and new lines that may be present in device name
    std::replace(device_name.begin(), device_name.end(), '\n', ' ');
    return device_name;
}

std::string isolation_monitorImpl::read_firmware_version() {
    auto result = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
        config.imd_device_id, static_cast<std::underlying_type_t<ImdRegisters>>(ImdRegisters::DEVICE_FIRMWARE_VERSION),
        6);
    std::string firmware_version;
    if (result.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        firmware_version = fmt::format("Ident: {} | Version {}, Date: {}-{}-{} | Modbus Driver: {}",
                                       result.value.value()[0], result.value.value()[1], result.value.value()[2],
                                       result.value.value()[3], result.value.value()[4], result.value.value()[5]);
    } else {
        set_deviceFault("Can't read the firmware version");
    }
    return firmware_version;
}

bool isolation_monitorImpl::check_for_faster_cablecheck() {
    auto result = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
        config.imd_device_id,
        static_cast<std::underlying_type_t<ImdRegisters>>(ImdRegisters::DEVICE_FIRMWARE_VERSION) + 1, 1);
    std::string firmware_version;
    if (result.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        if (result.value.value()[0] > 500) {
            return true;
        }
    } else {
        set_deviceFault("Could not check if the device supports fast cable check");
    }
    return false;
}

isolation_monitorImpl::MeasurementValue isolation_monitorImpl::read_register(const ImdRegisters start_register) {
    types::serial_comm_hub_requests::Result register_response{};

    register_response = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
        config.imd_device_id, static_cast<std::underlying_type<ImdRegisters>::type>(start_register), 4);

    MeasurementValue m;

    if (register_response.status_code != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        set_deviceFault("Can't read registers");
    }

    if (not register_response.value.has_value() or register_response.value.value().size() not_eq 4) {
        // force an error since we did not received fully the registers
        m.alarm = AlarmType::DeviceError;
        return m;
    }

    const auto& reg_value = register_response.value.value();

    m.alarm = to_alarm_type(reg_value.at(2) >> 8);
    m.test = to_test_type(reg_value.at(2) >> 8);
    m.unit = to_unit_type(reg_value.at(2) & 0xFF);
    m.valid = to_valid_type(reg_value.at(2) & 0xFF);
    m.description = to_channel_description(reg_value.at(3));

    uint32_t value{0};
    value += reg_value.at(0) << 16;
    value += reg_value.at(1);
    auto val = *reinterpret_cast<float*>(&value);
    m.value = val;
    return m;
}

} // namespace main
} // namespace module
