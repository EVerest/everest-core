// SPDX-License-Identifier: Apache-2.0
// Copyright Frickly Systems GmbH
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

    EVLOG_info << "Starting main loop";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(MAIN_LOOP_INTERVAL_S));

        if (self_test_triggered or self_test_running) {
            // If the self test takes too long to start or to run, we time out here
            if (std::chrono::steady_clock::now() > self_test_deadline) {
                EVLOG_error << "Self test timed out";
                self_test_running = false;
                self_test_triggered = false;
                publish_self_test_result(false);
            }

            // If some communication error occurs during the self test, we consider the self test failed
            if (error_state_monitor->is_error_active("isolation_monitor/CommunicationFault", "")) {
                EVLOG_error << "Self test failed due to communication error";
                self_test_running = false;
                self_test_triggered = false;
                publish_self_test_result(false);
            }
        }

        auto device_fault_and_state = read_device_fault_and_state();
        if (not device_fault_and_state.has_value()) {
            EVLOG_error << "Failed to read device fault and state";
            continue;
        }

        const auto [device_fault, device_state] = device_fault_and_state.value();

        EVLOG_debug << "Device status: " << to_string(device_state);
        EVLOG_debug << "Device error: " << to_string(device_fault);

        raise_or_clear_device_fault(device_fault);

        if (device_fault == DeviceFault_30001::CommunicationFault_Modbus) {
            EVLOG_info << "Device modbus timeout detected, trying to reset device and reconfigure";

            if (not update_control_word1(ControlWord1Action::ResetFaults)) {
                EVLOG_error << "Failed to reset device faults";
                continue;
            }

            if (not update_control_word1()) {
                EVLOG_error << "Failed to update control word 1";
                continue;
            }
        }

        // update Timeout register if enabled
        if (not write_timeout_registers()) {
            EVLOG_error << "Failed to write timeout register";
            continue;
        }

        // When we trigger a self test, the device can publish a normal status before switching to self test mode, this
        // is handled here
        if (self_test_triggered and not self_test_running and device_state == DeviceState_30002::SelfTesting) {
            EVLOG_info << "Device has started selftesting";
            self_test_running = true;
        }

        // Once the device has entered self test mode AND then left it again (either device state is reset or some fault
        // is raised)
        if (self_test_triggered and self_test_running and
            (device_state != DeviceState_30002::SelfTesting or device_fault != DeviceFault_30001::NoFailure)) {
            // the self test is now complete, reset flags
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

        // if device is initializing, raise an error as device is not ready
        if (device_state == DeviceState_30002::Initializing) {
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
        bool should_publish_isolation_measurement = device_state == DeviceState_30002::Measuring or
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
            if (not isolation_measurement.has_value()) {
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
    (void)test_voltage_V; // Unused parameter
    // Note that we only check if a self test has been triggered by us, not if the device is currently in self test
    // mode. If the device is already in self test mode, we can use the running self test to populate our self test
    // result

    if (self_test_running or self_test_triggered) {
        EVLOG_warning << "Self test already running or triggered";
        return;
    }

    EVLOG_info << "Starting self test";

    if (not update_control_word1(ControlWord1Action::StartSelfTest)) {
        publish_self_test_result(false);
        EVLOG_error << "Failed to start self test";
        return;
    }

    self_test_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(mod->config.self_test_timeout_s);

    self_test_triggered = true;
    // device might take some time to actually start the self test; this is set in the main
    // loop when we see the device state change to self testing
    self_test_running = false;
}

bool isolation_monitorImpl::configure_device() {
    // disable timeout if not enabled. Note that is enabled in the write_timeout_registers function after Timeout is
    // written
    if (not mod->config.timeout_release) {
        // Timeout release register. Write 0 to disable timeout
        const auto success = write_holding_register(1, 0);
        if (not success) {
            EVLOG_error << "Failed to disable device timeout";
            return false;
        }
    }

    // Read current settings from device to prevent unnecessary writes (the datasheet specifies that unnecessary writes
    // should be avoided to prevent wearing out the EEPROM).
    // There are 11 configuration registers starting at address 2000, we read all for convenience
    const auto present_settings = read_holding_registers(2000, 11);
    if (not present_settings.has_value()) {
        EVLOG_error << "Failed to read current configuration from device";
        return false;
    }

    // prepare new settings based on current settings
    std::vector<uint16_t> new_settings;
    for (const auto& value : *present_settings) {
        new_settings.push_back(static_cast<uint16_t>(value));
    }

    uint16_t broken_wire_detect = 0; // see datasheet
    if (mod->config.broken_wire_detect == "ON") {
        broken_wire_detect = 1;
    } else if (mod->config.broken_wire_detect == "OFF") {
        broken_wire_detect = 2;
    } else if (mod->config.broken_wire_detect == "ONLY_DURING_SELF_TEST") {
        broken_wire_detect = 4;
    } else {
        EVLOG_error << "Invalid connection monitoring configuration: " << mod->config.broken_wire_detect;
        return false;
    }

    new_settings[0] = broken_wire_detect; // 2000

    new_settings[1] = mod->config.storing_insulation_fault ? 1 : 0; // 2001

    uint16_t switching_mode_indicator_relay = 0; // see datasheet
    if (mod->config.switching_mode_indicator_relay == "DE_ENERGIZED_ON_TRIP") {
        switching_mode_indicator_relay = 0;
    } else if (mod->config.switching_mode_indicator_relay == "ENERGIZED_ON_TRIP") {
        switching_mode_indicator_relay = 1;
    } else {
        EVLOG_error << "Invalid indicator relay switching mode configuration: "
                    << mod->config.switching_mode_indicator_relay;
        return false;
    }
    new_settings[2] = switching_mode_indicator_relay; // 2002

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

    new_settings[5] = mod->config.response_value_alarm_kohm;     // 2005
    new_settings[6] = mod->config.response_value_pre_alarm_kohm; // 2006

    uint16_t coupling_device = 1; // see datasheet; 1 = Off
    if (mod->config.coupling_device == "RP5898") {
        coupling_device = 2;
    }
    new_settings[7] = coupling_device; // 2007

    uint16_t indicator_relay_k1_function = 8; // see datasheet
    if (mod->config.indicator_relay_k1_function == "INSULATION_FAULT_ALARM") {
        indicator_relay_k1_function = (1U << 0);
    } else if (mod->config.indicator_relay_k1_function == "INSULATION_FAULT_PREALARM") {
        indicator_relay_k1_function = (1U << 1);
    } else if (mod->config.indicator_relay_k1_function == "DEVICE_FAULT") {
        indicator_relay_k1_function = (1U << 2);
    } else if (mod->config.indicator_relay_k1_function == "INSULATION_FAULT_ALARM_OR_DEVICE_FAULT") {
        indicator_relay_k1_function = (1U << 3);
    } else if (mod->config.indicator_relay_k1_function == "INSULATION_FAULT_ON_DC+") {
        indicator_relay_k1_function = (1U << 4);
    } else if (mod->config.indicator_relay_k1_function == "INSULATION_FAULT_ON_DC+_OR_DEVICE_FAULT") {
        indicator_relay_k1_function = (1U << 5);
    } else {
        EVLOG_error << "Invalid indicator relay K1 function configuration: " << mod->config.indicator_relay_k1_function;
        return false;
    }

    uint16_t indicator_relay_k2_function = 8; // see datasheet
    if (mod->config.indicator_relay_k2_function == "INSULATION_FAULT_ALARM") {
        indicator_relay_k2_function = (1U << 0);
    } else if (mod->config.indicator_relay_k2_function == "INSULATION_FAULT_PRE_ALARM") {
        indicator_relay_k2_function = (1U << 1);
    } else if (mod->config.indicator_relay_k2_function == "DEVICE_FAULT") {
        indicator_relay_k2_function = (1U << 2);
    } else if (mod->config.indicator_relay_k2_function == "INSULATION_FAULT_PRE_ALARM_OR_DEVICE_FAULT") {
        indicator_relay_k2_function = (1U << 3);
    } else if (mod->config.indicator_relay_k2_function == "INSULATION_FAULT_ON_DC-") {
        indicator_relay_k2_function = (1U << 4);
    } else if (mod->config.indicator_relay_k2_function == "INSULATION_FAULT_ON_DC-_OR_DEVICE_FAULT") {
        indicator_relay_k2_function = (1U << 5);
    } else {
        EVLOG_error << "Invalid indicator relay K2 function configuration: " << mod->config.indicator_relay_k2_function;
        return false;
    }

    new_settings[8] = indicator_relay_k1_function; // 2008
    new_settings[9] = indicator_relay_k2_function; // 2009

    new_settings[10] = mod->config.automatic_self_test ? 1 : 0; // 2010

    if (not std::equal(new_settings.begin(), new_settings.end(), present_settings->begin())) {
        const auto write_success = write_holding_registers(2000, new_settings);
        if (not write_success) {
            EVLOG_error << "Failed to write configuration to device";
            return false;
        }
    } else {
        EVLOG_debug << "Device configuration is already up to date, no need to write";
    }

    if (not update_control_word1(ControlWord1Action::ResetFaults)) {
        EVLOG_error << "Failed to reset device faults after configuration";
        return false;
    }

    if (not update_control_word1()) {
        EVLOG_error << "Failed to set control word 1 after configuration";
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

bool isolation_monitorImpl::write_holding_registers(uint16_t protocol_address, const std::vector<uint16_t>& values) {
    types::serial_comm_hub_requests::VectorUint16 values_converted;
    for (const auto& v : values) {
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

bool isolation_monitorImpl::update_control_word1(ControlWord1Action action) {
    if (action == ControlWord1Action::ResetFaults) {
        return write_holding_register(0, 1U << 0); // Bit 0: reset
    }

    if (action == ControlWord1Action::StartSelfTest) {
        // Note that the self test only works when measurement is not disabled, so we only set bit 4 here
        return write_holding_register(0, 1U << 4); // Bit 4: Start self test
    }

    if (self_test_triggered or self_test_running) {
        // if a self test was triggered, we don't want to change the control word until the self test is complete.
        // One should call update_control_word1 when the self test is complete!
        return true;
    }

    uint16_t control_word1 = 0;

    // if we should not measure, set bit 8 to disable measurements
    if (not publish_enabled and not mod->config.keep_measurement_active) {
        control_word1 |= 1U << 8; // bit 8: Measurement off
    }

    return write_holding_register(0, control_word1);
}

std::optional<types::isolation_monitor::IsolationMeasurement> isolation_monitorImpl::read_isolation_measurement() {
    types::isolation_monitor::IsolationMeasurement isolation_measurement;

    const auto input_registers = read_input_registers(2000, 3);
    if (not input_registers.has_value()) {
        return std::nullopt;
    }

    const uint16_t raw_insulation_resistance = static_cast<uint16_t>(input_registers->at(0)); // 2000
    const uint16_t raw_voltage = static_cast<uint16_t>(input_registers->at(2));               // 2002

    isolation_measurement.resistance_F_Ohm =
        static_cast<float>(insulation_resistance_to_ohm(raw_insulation_resistance));

    // 0xFFFF means voltage out of range
    if (raw_voltage != 0xFFFF) {
        isolation_measurement.voltage_V = static_cast<float>(raw_voltage);
    }

    return isolation_measurement;
}

std::optional<std::tuple<DeviceFault_30001, DeviceState_30002>> isolation_monitorImpl::read_device_fault_and_state() {
    const auto raw_registers = read_input_registers(0x0000, 2);
    if (not raw_registers.has_value()) {
        return std::nullopt;
    }

    const auto device_fault = static_cast<DeviceFault_30001>(raw_registers.value()[0]);
    const auto device_state = static_cast<DeviceState_30002>(raw_registers.value()[1]);

    return std::make_tuple(device_fault, device_state);
}

bool isolation_monitorImpl::write_timeout_registers() {
    if (not mod->config.timeout_release) {
        return true;
    }

    // write timeout register
    if (not write_holding_register(2, mod->config.timeout_s * 1000)) {
        EVLOG_error << "Failed to write Timeout register";
        return false;
    }

    //  enable timeout (note that disabling the timeout is only done in the configure_device function to reduce
    //  unnecessary writes)
    if (not write_holding_register(1, 1)) {
        EVLOG_error << "Failed to enable device timeout";
        return false;
    }

    return true;
}

void isolation_monitorImpl::raise_or_clear_device_fault(const DeviceFault_30001& device_fault) {
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
}

} // namespace main
} // namespace module
