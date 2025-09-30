// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "isolation_monitorImpl.hpp"
#include "registers.hpp"

namespace module {
namespace main {

void isolation_monitorImpl::init() {
}

void isolation_monitorImpl::ready() {
    EVLOG_info << "Uploading configuration to device";

    // Note that we continue in this initial setup even if anything fails as the main loop reconfigures the device if
    // necessary

    const auto success = configure_device();
    if (not success) {
        EVLOG_error << "Failed to configure device";
    } else {
        EVLOG_info << "Device configured successfully";
    }

    if (not update_control_word1()) {
        EVLOG_error << "Failed to set control word 1";
    }

    EVLOG_info << "Starting main loop";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // If the self test takes too long to start or to run, we time out here
        if (self_test_triggered or self_test_running) {
            if (std::chrono::steady_clock::now() > self_test_deadline) {
                EVLOG_error << "Self test timed out";
                self_test_running = false;
                self_test_triggered = false;
                publish_self_test_result(false);
            }
        }

        auto device_fault_and_state = read_device_fault_and_state();
        if (!device_fault_and_state) {
            EVLOG_error << "Failed to read input registers 0-1";
            continue;
        }

        const auto [device_fault, device_state] = device_fault_and_state.value();

        EVLOG_debug << "Device status: " << to_string(device_state);
        EVLOG_debug << "Device error: " << to_string(device_fault);

        // When we trigger a self test, the device can publish a normal status before switching to self test mode, this
        // is handled here
        if (self_test_triggered and not self_test_running and device_state == DeviceState_30002::SelfTesting) {
            EVLOG_info << "Device has started selftesting";
            self_test_running = true;
        }

        // Once the device has entered self test mode and then left it again, the self test is complete
        if (self_test_triggered and self_test_running and device_state != DeviceState_30002::SelfTesting) {
            // first, reset the self test state
            self_test_running = false;
            self_test_triggered = false;

            // If no failure was reported, the self test passed
            const auto self_test_result = device_fault == DeviceFault_30001::NoFailure;
            publish_self_test_result(self_test_result);
            EVLOG_info << "Self test completed with result: " << self_test_result;

            // update the control word 1 to potentially disable measurement again (self test only works when measurement
            // is not disabled)
            update_control_word1();
        }

        // raise device fault if any present
        if (device_fault != DeviceFault_30001::NoFailure) {
            if (not error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "DeviceFaultRegister")) {
                EVLOG_error << "Raising device fault: " << to_string(device_fault);
                raise_error(
                    error_factory->create_error("isolation_monitor/DeviceFault", "DeviceFaultRegister",
                                                std::string("Device fault: ") + std::string(to_string(device_fault))));
            }
        } else {
            if (error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "DeviceFaultRegister")) {
                clear_error("isolation_monitor/DeviceFault", "DeviceFaultRegister");
            }
        }

        // if device is initializing, raise an error as device is not ready
        // todo: can state be ErrorMode without device_fault register being an error? If so -> publish another error
        // here
        if (device_state == DeviceState_30002::Initialize) {
            if (not error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "NotReady")) {
                raise_error(
                    error_factory->create_error("isolation_monitor/DeviceFault", "NotReady", "Device not ready"));
            }
        } else {
            if (error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "NotReady")) {
                clear_error("isolation_monitor/DeviceFault", "NotReady");
            }
        }

        // dont publish if not measuring
        bool should_publish_isolation_measurement =
            device_state == DeviceState_30002::Ready_Measuring_NoResponseExceeded or
            device_state == DeviceState_30002::Measuring_PreAlarmExceeded or
            device_state == DeviceState_30002::Measuring_AlarmExceeded;

        // dont publish if device has a fault
        if (device_fault != DeviceFault_30001::NoFailure) {
            should_publish_isolation_measurement = false;
        }

        // publish only when enabled or when always_publish_measurements is set
        if (not mod->config.always_publish_measurements and not publish_enabled) {
            should_publish_isolation_measurement = false;
        }

        if (should_publish_isolation_measurement) {
            const auto isolation_measurement = read_isolation_measurement();
            if (!isolation_measurement) {
                EVLOG_error << "Failed to read isolation measurement";
                continue;
            }
            publish_isolation_measurement(isolation_measurement.value());

            EVLOG_debug << "Insulation resistance: " << isolation_measurement->resistance_F_Ohm << " Ohm";
            if (isolation_measurement->voltage_V) {
                EVLOG_debug << "Voltage: " << *isolation_measurement->voltage_V << " V";
            }
        }

        // If a communication fault was previously raised, we can clear it now, as we were able to read from the device.
        // Also upload the configuration again, as the device might have reset/restarted
        if (error_state_monitor->is_error_active("isolation_monitor/CommunicationFault", "")) {
            // reconfigure device after communication fault, if this fails, keep the communication fault
            if (not configure_device()) {
                EVLOG_error << "Failed to reconfigure device after communication fault";
                continue;
            };
            if (not update_control_word1()) {
                EVLOG_error << "Failed to update control word 1 after communication fault";
                continue;
            }

            clear_error("isolation_monitor/CommunicationFault");
        }
    }
}

void isolation_monitorImpl::handle_start() {
    publish_enabled = true;
    if (not update_control_word1()) {
        EVLOG_error << "Failed to enable measurement";
    }
}

void isolation_monitorImpl::handle_stop() {
    publish_enabled = false;
    if (not update_control_word1()) {
        EVLOG_error << "Failed to disable measurement";
    }
}

void isolation_monitorImpl::handle_start_self_test(double& test_voltage_V) {
    // Note that we only check if a self test has been triggered by us, not if the device is currently in self test
    // mode. If the device is already in self test mode, we can use the self test to populate our self test result

    if (self_test_running or self_test_triggered) {
        EVLOG_warning << "Self test already running or triggered";
        return;
    }

    EVLOG_info << "Starting self test";

    if (not update_control_word1(true)) {
        publish_self_test_result(false);
        EVLOG_error << "Failed to start self test";
        return;
    }

    self_test_deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(static_cast<int>(mod->config.self_test_timeout_s));

    self_test_triggered = true;
    self_test_running = false; // device might take some time to actually start the self test
}

bool isolation_monitorImpl::configure_device() {
    // Read current settings from device to prevent unnecessary writes (the datasheet specifies that unnecessary writes
    // should be avoided to prevent wearing out the EEPROM).
    // There are 11 configuration registers starting at address 2000, we read all for convenience
    const auto present_settings = read_holding_registers(2000, 11);
    if (!present_settings) {
        EVLOG_error << "Failed to read current configuration from device";
        return false;
    }

    // prepare new settings based on current settings
    std::vector<uint16_t> new_settings;
    for (const auto& value : *present_settings) {
        new_settings.push_back(static_cast<uint16_t>(value));
    }

    uint16_t connection_monitoring = 0; // see datasheet
    if (mod->config.connection_monitoring == "ON") {
        connection_monitoring = 1;
    } else if (mod->config.connection_monitoring == "OFF") {
        connection_monitoring = 2;
    } else if (mod->config.connection_monitoring == "ONLY_DURING_SELF_TEST") {
        connection_monitoring = 4;
    } else {
        EVLOG_error << "Invalid connection monitoring configuration: " << mod->config.connection_monitoring;
        return false;
    }

    new_settings[0] = connection_monitoring; // 2000

    // todo: alarmspeicherung

    uint16_t indicator_relay_switching_mode = 0; // see datasheet
    if (mod->config.indicator_relay_switching_mode == "DE_ENERGIZED_ON_TRIP") {
        indicator_relay_switching_mode = 0;
    } else if (mod->config.indicator_relay_switching_mode == "ENERGIZED_ON_TRIP") {
        indicator_relay_switching_mode = 1;
    } else {
        EVLOG_error << "Invalid indicator relay switching mode configuration: "
                    << mod->config.indicator_relay_switching_mode;
        return false;
    }
    new_settings[2] = indicator_relay_switching_mode; // 2002

    uint16_t power_supply_type = 0; // see datasheet
    if (mod->config.power_supply_type == "AC") {
        power_supply_type = 1;
    } else if (mod->config.power_supply_type == "DC") {
        power_supply_type = 2;
    } else if (mod->config.power_supply_type == "3NAC") {
        power_supply_type = 4;
    } else {
        EVLOG_error << "Invalid power supply type configuration: " << mod->config.power_supply_type;
        return false;
    }
    new_settings[3] = power_supply_type; // 2003

    new_settings[5] = static_cast<uint16_t>(mod->config.alarm_threshold_kohm);     // 2005
    new_settings[6] = static_cast<uint16_t>(mod->config.pre_alarm_threshold_kohm); // 2006

    uint16_t coupling_device = 1; // see datasheet; 1 = Off
    if (mod->config.coupling_device == "RP5898") {
        coupling_device = 2;
    }
    new_settings[7] = coupling_device; // 2007

    if (std::equal(new_settings.begin(), new_settings.end(), present_settings->begin())) {
        EVLOG_debug << "Device configuration is already up to date, no need to write";
        return true;
    }

    const auto write_success = write_holding_registers(2000, new_settings);
    if (!write_success) {
        EVLOG_error << "Failed to write configuration to device";
        return false;
    }

    return true;
}

std::optional<std::vector<int>> isolation_monitorImpl::read_input_registers(uint16_t first_protocol_register_address,
                                                                            uint16_t register_quantity) {
    const auto result = mod->r_serial_comm_hub->call_modbus_read_input_registers(
        mod->config.device_id, first_protocol_register_address, register_quantity);
    if (result.status_code != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        raise_communication_fault();
        return std::nullopt;
    }

    return result.value;
}

std::optional<std::vector<int>> isolation_monitorImpl::read_holding_registers(uint16_t first_protocol_register_address,
                                                                              uint16_t register_quantity) {
    const auto result = mod->r_serial_comm_hub->call_modbus_read_holding_registers(
        mod->config.device_id, first_protocol_register_address, register_quantity);

    if (result.status_code != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        raise_communication_fault();
        return std::nullopt;
    }

    return result.value;
}

bool isolation_monitorImpl::write_holding_register(uint16_t protocol_address, uint16_t value) {
    const auto result =
        mod->r_serial_comm_hub->call_modbus_write_single_register(mod->config.device_id, protocol_address, value);
    if (result != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        raise_communication_fault();
        return false;
    }

    return true;
}

bool isolation_monitorImpl::write_holding_registers(uint16_t protocol_address, std::vector<uint16_t> values) {
    types::serial_comm_hub_requests::VectorUint16 values_converted;
    for (const auto v : values) {
        values_converted.data.push_back(v);
    }

    const auto result = mod->r_serial_comm_hub->call_modbus_write_multiple_registers(
        mod->config.device_id, protocol_address, values_converted);
    if (result != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
        raise_communication_fault();
        return false;
    }

    return true;
}

void isolation_monitorImpl::raise_communication_fault() {
    if (not error_state_monitor->is_error_active("isolation_monitor/CommunicationFault", "")) {
        raise_error(error_factory->create_error("isolation_monitor/CommunicationFault", "", "Communication fault"));
    }
}

// todo: what does bit 0 do? Works fine without it but is mentioned...
bool isolation_monitorImpl::update_control_word1(bool start_self_test) {
    if (start_self_test) {
        // Note that the self test only works when measurement is not disabled
        return write_holding_register(0, 1 << 4); // Bit 4: Start self test
    }

    if (self_test_triggered or self_test_running) {
        // if a self test was triggered, we don't want to change the control word until the self test is complete.
        // One should call update_control_word1 when the self test is complete!
        return true;
    }

    uint16_t control_word1 = 0;

    // if we should not measure, set bit 8 to disable measurements
    if (not publish_enabled and not mod->config.keep_measurement_active) {
        control_word1 |= 1 << 8; // bit 8: Measurement off
    }

    return write_holding_register(0, control_word1);
}

std::optional<types::isolation_monitor::IsolationMeasurement> isolation_monitorImpl::read_isolation_measurement() {
    types::isolation_monitor::IsolationMeasurement isolation_measurement;

    const auto input_registers = read_input_registers(2000, 3);
    if (!input_registers) {
        return std::nullopt;
    }

    const uint16_t raw_insulation_resistance = static_cast<uint16_t>(input_registers->at(0)); // 2000
    const uint16_t raw_voltage = static_cast<uint16_t>(input_registers->at(2));               // 2002

    isolation_measurement.resistance_F_Ohm =
        static_cast<float>(insulation_resistance_to_ohm(raw_insulation_resistance));

    // todo: check if this is the best approach
    // 0xFFFF means voltage out of range, 0V means either less than 5V or could not be measured
    // we assume the worst case -> we dont publish 0V
    if (raw_voltage != 0xFFFF and raw_voltage != 0) {
        isolation_measurement.voltage_V = static_cast<float>(raw_voltage);
    }

    return isolation_measurement;
}

std::optional<std::tuple<DeviceFault_30001, DeviceState_30002>> isolation_monitorImpl::read_device_fault_and_state() {
    const auto raw_registers = read_input_registers(0x0000, 2);
    if (!raw_registers) {
        return std::nullopt;
    }

    const auto device_fault = static_cast<DeviceFault_30001>(raw_registers.value()[0]);
    const auto device_state = static_cast<DeviceState_30002>(raw_registers.value()[1]);

    return std::make_tuple(device_fault, device_state);
}

} // namespace main
} // namespace module
