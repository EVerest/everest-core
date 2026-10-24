// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "isolation_monitorImpl.hpp"
#include "everest/logging.hpp"
#include <array>
#include <chrono>
#include <fmt/format.h>
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
        successful &= send_to_imd(3000, (config.voltage_to_earth_monitoring_alarm_enable ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3005, config.r1_prealarm_kohm);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3007, config.r2_alarm_kohm);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3008, (config.undervoltage_alarm_enable ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3009, config.undervoltage_alarm_threshold_V);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3010, (config.overvoltage_alarm_enable ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3011, config.overvoltage_alarm_threshold_V);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3012, (config.alarm_memory_enable ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3013, (config.relais_r1_mode ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3014, (config.relais_r2_mode ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3018, (config.delay_startup_device ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3019, config.delay_t_on_k1_k2);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3020, config.delay_t_off_k1_k2);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3023, (config.chademo_mode ? 1 : 0));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        // Leaving stop mode below runs a device test in cbl mode, which must not use the stored system connection test
        successful &= send_to_imd_if_changed(3024, SYSTEM_CONNECTION_TEST_OFF);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        // The device's own self test at power-on would run with whatever value of 3024 is stored, so it stays off.
        // The driver runs the self test at start itself (selftest_enable_at_start).
        successful &= send_to_imd_if_changed(3025, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3027, config.relay_k1_alarm_assignment);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        successful &= send_to_imd(3028, config.relay_k2_alarm_assignment);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        // start up enable the device if the configuration option is not set
        if (!config.disable_device_on_stop) {
            successful &= send_to_imd(3026, 1);
        }
        // Give device time to process startup command
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (successful) {
            EVLOG_info << "IMD Device: " << read_device_name() << " (" << read_firmware_version() << ")";
            faster_cable_check_supported = check_for_faster_cablecheck();

            if (faster_cable_check_supported) {
                EVLOG_info << "Supports faster cable check method";
                enable_faster_cable_check_mode();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            } else {
                EVLOG_info << "Does not support faster cable check method, falling "
                              "back to long self test. This may create "
                              "timeouts with certain cars in CableCheck. Consider "
                              "upgrading to at least firmware 5.00";
                disable_faster_cable_check_mode();
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        } else {
            // allow the system to recover and don't hog the MODBUS
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    } while (not successful);
}

bool isolation_monitorImpl::prepare_device_test(uint16_t system_connection_test) {
    // Write 3024 before enabling the device: in cbl mode leaving stop mode already starts the self test.
    // The device stores 3024 permanently, so it keeps the selection until the next test instead of being reset.
    if (not send_to_imd_if_changed(3024, system_connection_test)) {
        EVLOG_error << "Can't set the system connection test for the device test";
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    if (config.disable_device_on_stop) {
        if (not send_to_imd(3026, 1)) {
            EVLOG_error << "Can't enable the device: " << read_device_name();
        } else {
            EVLOG_info << "Device enabled for device test";
        }
    }
    return true;
}

bool isolation_monitorImpl::send_self_test_start() {
    // Wait a bit to ensure device is ready (not processing previous read
    // operations) This prevents conflicts with the regular reading loop
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (not write_imd_register(SELF_TEST_START_REGISTER, SELF_TEST_START_COMMAND)) {
        return false;
    }
    // Give device time to process the self-test command
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return true;
}

uint16_t isolation_monitorImpl::system_connection_test_for(double test_voltage_V) const {
    if (not config.selftest_enable_gridconnection) {
        return SYSTEM_CONNECTION_TEST_OFF;
    }
    // "on" is meant for systems up to DC 100 V, "on U" requires a system voltage above DC 100 V during the test
    return test_voltage_V > SYSTEM_CONNECTION_TEST_ON_U_MIN_VOLTAGE_V ? SYSTEM_CONNECTION_TEST_ON_U
                                                                      : SYSTEM_CONNECTION_TEST_ON;
}

bool isolation_monitorImpl::begin_self_test(uint16_t system_connection_test) {
    if (not prepare_device_test(system_connection_test)) {
        return false;
    }
    // A test the device already runs is tracked instead of started
    if (last_test != TestType::ExternalTest and not send_self_test_start()) {
        set_deviceFault("Failed to start self test");
        return false;
    }
    self_test_started = true;
    self_test_timeout = 30;
    return true;
}

void isolation_monitorImpl::start_requested_self_test(double test_voltage_V) {
    if (recovery_state == RecoveryState::Running or last_test == TestType::ExternalTest or
        last_alarm == AlarmType::DeviceError) {
        // A device test without system connection test must not pass as the cable check self test, and a device
        // error is only cleared by a passing device test
        EVLOG_warning << "IMD self test requested while the device runs a device test or reports a device error";
        publish_self_test_result(false);
        return;
    }
    EVLOG_info << "IMD Starting self-test...";
    if (not begin_self_test(system_connection_test_for(test_voltage_V))) {
        publish_self_test_result(false);
    }
}

void isolation_monitorImpl::start_self_test_at_start() {
    EVLOG_info << "IMD Starting self test at start...";
    // Without HV on the output the system connection test cannot pass
    if (not begin_self_test(SYSTEM_CONNECTION_TEST_OFF)) {
        EVLOG_error << "Could not start the self test at start";
        finish_self_test(false);
    }
}

void isolation_monitorImpl::finish_self_test(bool result) {
    bool at_start{false};
    std::optional<double> pending_test_voltage_V;
    {
        std::scoped_lock lock(self_test_mutex);
        at_start = self_test_at_start_running;
        self_test_at_start_running = false;
        pending_test_voltage_V.swap(pending_self_test_voltage_V);
    }

    if (at_start) {
        // A device error is raised as DeviceFault by read_imd_values()
        if (result) {
            EVLOG_info << "Self test at start passed";
        } else {
            EVLOG_error << "Self test at start failed";
        }
    } else {
        publish_self_test_result(result);
    }

    if (pending_test_voltage_V.has_value()) {
        start_requested_self_test(pending_test_voltage_V.value());
    }
}

void isolation_monitorImpl::request_recovery(std::chrono::seconds delay) {
    recovery_due = std::chrono::steady_clock::now() + delay;
    recovery_start_attempts = 0;
    recovery_state = RecoveryState::Pending;
}

void isolation_monitorImpl::start_recovery_test() {
    EVLOG_info << "Starting a device test without system connection test to clear the device error";
    if (prepare_device_test(SYSTEM_CONNECTION_TEST_OFF) and send_self_test_start()) {
        recovery_started = std::chrono::steady_clock::now();
        recovery_seen_test = false;
        recovery_state = RecoveryState::Running;
        return;
    }
    recovery_start_attempts++;
    if (recovery_start_attempts < RECOVERY_MAX_START_ATTEMPTS) {
        // e.g. "device busy" while the device still runs a test
        EVLOG_warning << "Could not start the device test (attempt " << recovery_start_attempts << "), retrying in "
                      << RECOVERY_START_DELAY.count() << " s";
        recovery_due = std::chrono::steady_clock::now() + RECOVERY_START_DELAY;
    } else {
        EVLOG_warning << "Could not start the device test, retrying in " << RECOVERY_RETRY_INTERVAL.count() << " s";
        recovery_due = std::chrono::steady_clock::now() + RECOVERY_RETRY_INTERVAL;
    }
}

// Returns true once the device test has ended or could not be started, so the device can be disabled again
bool isolation_monitorImpl::run_recovery() {
    const auto now = std::chrono::steady_clock::now();
    switch (recovery_state.load()) {
    case RecoveryState::Idle:
        return false;
    case RecoveryState::Pending:
        if (self_test_started or now < recovery_due) {
            return false;
        }
        if (last_alarm != AlarmType::DeviceError) {
            EVLOG_info << "Device error cleared, no device test needed";
            recovery_state = RecoveryState::Idle;
            return false;
        }
        if (last_test == TestType::ExternalTest and now - recovery_due < RECOVERY_TIMEOUT) {
            // The device still runs a test and would answer "busy"
            return false;
        }
        start_recovery_test();
        return recovery_state != RecoveryState::Running;
    case RecoveryState::Running: {
        if (last_test == TestType::ExternalTest) {
            recovery_seen_test = true;
        }
        const auto elapsed = now - recovery_started;
        // With DeviceFault active the loop runs every 10 s and may not see the test bit of a ~15 s test at all
        const bool test_ended =
            (recovery_seen_test or elapsed > RECOVERY_ASSUME_STARTED) and last_test != TestType::ExternalTest;
        if (test_ended and last_alarm != AlarmType::DeviceError) {
            EVLOG_info << "Device test passed, device error cleared";
            recovery_state = RecoveryState::Idle;
            return true;
        }
        if (test_ended or elapsed > RECOVERY_TIMEOUT) {
            EVLOG_warning << "Device error still present after the device test, retrying in "
                          << RECOVERY_RETRY_INTERVAL.count() << " s";
            request_recovery(RECOVERY_RETRY_INTERVAL);
            return true;
        }
        return false;
    }
    }
    return false;
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

bool isolation_monitorImpl::send_to_imd_if_changed(const uint16_t& command, const uint16_t& value) {
    // The device stores parameters in non-volatile memory, avoid needless writes
    const auto result =
        mod->r_serial_comm_hub->call_modbus_read_holding_registers(config.imd_device_id, static_cast<int>(command), 1);
    if (result.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success and result.value.has_value() and
        result.value.value().size() == 1 and result.value.value()[0] == value) {
        return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    return send_to_imd(command, value);
}

bool isolation_monitorImpl::write_imd_register(uint16_t address, uint16_t value) {
    return mod->r_serial_comm_hub->call_modbus_write_multiple_registers(
               config.imd_device_id, static_cast<int>(address),
               (types::serial_comm_hub_requests::VectorUint16){{value}}) ==
           types::serial_comm_hub_requests::StatusCodeEnum::Success;
}

bool isolation_monitorImpl::send_to_imd(const uint16_t& command, const uint16_t& value) {
    if (not write_imd_register(command, value)) {
        EVLOG_error << "ModBus Command failed: " << command << " Value: " << value;
        set_deviceFault("Failed to write registers");
        return false;
    }
    return true;
}

void isolation_monitorImpl::ready() {
    if (config.selftest_enable_at_start) {
        // A self test requested while the device is configured waits for the self test at start too
        std::scoped_lock lock(self_test_mutex);
        self_test_at_start_running = true;
    }
    configure_device();
    if (config.selftest_enable_at_start) {
        start_self_test_at_start();
    }
    bool self_test_running = false;
    bool need_to_disable_device = false;
    int device_disabled_timeout_s = 10;

    while (true) {
        read_imd_values();

        if (self_test_started) {
            self_test_timeout--;
            if (self_test_timeout <= 0) {
                // a time out happend
                self_test_started = false;
                self_test_running = false;
                need_to_disable_device = true;
                // publish failed result
                EVLOG_warning << "Self test timed out";
                finish_self_test(false);
            } else if (last_alarm == AlarmType::DeviceError) {
                // A failed system connection test (E.02) keeps the device in test mode, the result is known already
                self_test_started = false;
                self_test_running = false;
                need_to_disable_device = true;
                EVLOG_warning << "Self test failed with a device error";
                finish_self_test(false);
            } else if (self_test_running) {
                if (last_test not_eq TestType::ExternalTest) {
                    // Self test is done
                    self_test_running = false;
                    self_test_started = false;
                    need_to_disable_device = true;
                    // was it successfull? If there is no error, it was...
                    bool result = true;
                    if (last_alarm == AlarmType::DeviceError) {
                        result = false;
                    }
                    EVLOG_info << "Self test completed: " << result;
                    finish_self_test(result);
                }
            } else {
                if (last_test == TestType::ExternalTest) {
                    self_test_running = true;
                }
            }
        }
        if (last_alarm == AlarmType::DeviceError and not self_test_started and recovery_state == RecoveryState::Idle) {
            // The device keeps a device error and the alarm relays until a device test passes, 8006 does not clear
            // it, and a self test is refused while the error is active. Whatever raised it, the recovery clears it.
            request_recovery(RECOVERY_START_DELAY);
        }
        if (run_recovery()) {
            need_to_disable_device = true;
        }
        if (not error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (need_to_disable_device) {
                device_disabled_timeout_s--;
                if (device_disabled_timeout_s <= 0) {
                    need_to_disable_device = false;
                    device_disabled_timeout_s = 10;
                    // disable the device if the configuration option is set and we are not publishing
                    // aka we are in a measuring cycle (start called), and no further self test was started meanwhile
                    if (not enable_publishing && not self_test_started && config.disable_device_on_stop) {
                        if (not send_to_imd(3026, 0)) {
                            EVLOG_error << "Can't disable the device: " << read_device_name();
                        } else {
                            EVLOG_info << "Device disabled after self test since we didn't start measuring cycle "
                                          "(timeout 10s)";
                        }
                    }
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
}

void isolation_monitorImpl::handle_start() {
    enable_publishing = true;
    if (config.disable_device_on_stop) {
        if (not send_to_imd(3026, 1)) {
            EVLOG_error << "Can't enable the device: " << read_device_name();
        } else {
            EVLOG_info << "Device enabled for measurements";
        }
    }
}

void isolation_monitorImpl::handle_stop() {
    enable_publishing = false;
    if (config.disable_device_on_stop) {
        if (not send_to_imd(3026, 0)) {
            EVLOG_error << "Can't disable the device: " << read_device_name();
        } else {
            EVLOG_info << "Device disabled after measurements";
        }
    }
}

void isolation_monitorImpl::handle_start_self_test(double& test_voltage_V) {
    {
        std::scoped_lock lock(self_test_mutex);
        if (self_test_at_start_running) {
            // Its result must not be used, run the requested self test once it is done
            EVLOG_info << "IMD self test requested during the self test at start, starting it afterwards";
            pending_self_test_voltage_V = test_voltage_V;
            return;
        }
    }
    start_requested_self_test(test_voltage_V);
}

void isolation_monitorImpl::read_imd_values() {
    // Read rF
    auto rf = read_register(ImdRegisters::RESISTANCE_R_F_OHM);
    EVLOG_debug << "Resistance: " << to_string(rf);

    // A missing or short Modbus reply says nothing about the device: the alarm and test state of the last complete
    // read stays, so a lost reply neither ends a running self test nor counts as a device error
    if (rf.received) {
        last_test = rf.test;
    }

    // Small delay between reads to avoid overwhelming the device
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Read Voltage U_N (always needed)
    auto voltage = read_register(ImdRegisters::VOLTAGE_U_N_V);
    EVLOG_debug << "Voltage: " << to_string(voltage);

    isolation_monitorImpl::MeasurementValue voltage_to_earth_l1e;
    isolation_monitorImpl::MeasurementValue voltage_to_earth_l2e;
    // Read Voltage to Earth L1E and L2E only if the device is not in self test
    // mode and only it we are in a measuring cycle. We have seen that Bender
    // sometimes is overwhelmed
    if ((last_test == TestType::NoTest) and (enable_publishing or config.always_publish_measurements)) {
        // VOLTAGE_U_L1E_V (1016) and VOLTAGE_U_L2E_V (1020) are consecutive (4
        // registers apart) Read 8 registers starting from 1016 to get both
        // measurements in a single operation
        types::serial_comm_hub_requests::Result register_response =
            mod->r_serial_comm_hub->call_modbus_read_holding_registers(
                config.imd_device_id, static_cast<int>(ImdRegisters::VOLTAGE_U_L1E_V), 8);

        if (register_response.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Success and
            register_response.value.has_value() and register_response.value.value().size() == 8) {
            const auto& reg_value_int = register_response.value.value();
            // Convert std::vector<int> to std::vector<uint16_t>
            std::vector<uint16_t> reg_value(reg_value_int.begin(), reg_value_int.end());

            // Parse voltage U_L1E from registers 0-3
            voltage_to_earth_l1e = parse_register_data(reg_value, 0);
            EVLOG_debug << "Voltage to Earth L1E: " << to_string(voltage_to_earth_l1e);

            // Parse voltage U_L2E from registers 4-7
            voltage_to_earth_l2e = parse_register_data(reg_value, 4);
            EVLOG_debug << "Voltage to Earth L2E: " << to_string(voltage_to_earth_l2e);
        } else {
            // Fallback to individual reads
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            voltage_to_earth_l1e = read_register(ImdRegisters::VOLTAGE_U_L1E_V);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            voltage_to_earth_l2e = read_register(ImdRegisters::VOLTAGE_U_L2E_V);
        }
    }

    // A device error is reported in the channel it belongs to (manual table 5.3.1): E.01 (earth connection) in R_F,
    // E.02 (connection to system, i.e. an interrupted L1/+ or L2/- lead) in U_N. Check every channel that was read.
    if (rf.received and voltage.received) {
        last_alarm = rf.alarm;
        std::string device_error_channels;
        const std::array<std::pair<const char*, const MeasurementValue*>, 4> channels{
            {{"R_F", &rf}, {"U_N", &voltage}, {"U_L1e", &voltage_to_earth_l1e}, {"U_L2e", &voltage_to_earth_l2e}}};
        for (const auto& [name, value] : channels) {
            if (value->alarm == AlarmType::DeviceError) {
                last_alarm = AlarmType::DeviceError;
                device_error_channels += (device_error_channels.empty() ? "" : ", ") + std::string(name);
            }
        }

        if (last_alarm == AlarmType::DeviceError) {
            set_deviceFault("Device Error (" + device_error_channels + ")");
        }
    }

    bool valid_readings = true;
    if (enable_publishing or config.always_publish_measurements) {
        types::isolation_monitor::IsolationMeasurement m;
        if (voltage.valid not_eq ValidType::Invalid) {
            m.voltage_V = voltage.value;
        } else {
            valid_readings = false;
        }

        if (voltage_to_earth_l1e.valid not_eq ValidType::Invalid) {
            m.voltage_to_earth_l1e_V = voltage_to_earth_l1e.value;
        } else {
            valid_readings = false;
        }

        if (voltage_to_earth_l2e.valid not_eq ValidType::Invalid) {
            m.voltage_to_earth_l2e_V = voltage_to_earth_l2e.value;
        } else {
            valid_readings = false;
        }

        if (rf.valid not_eq ValidType::Invalid) {
            m.resistance_F_Ohm = rf.value;
        } else {
            valid_readings = false;
        }

        // do not publish values if in error state or the device is in self test
        // mode
        if (last_test == TestType::NoTest) {
            if (not error_state_monitor->is_error_active("isolation_monitor/DeviceFault", "")) {
                publish_isolation_measurement(m);
            }
        }
    }
    // let at least one round of values unpublished not to use unstable ones; configure_device() would reset the
    // system connection test under a running self test
    if (last_alarm not_eq AlarmType::DeviceError and valid_readings and not self_test_started and
        recovery_state != RecoveryState::Running and
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

    if (register_response.status_code != types::serial_comm_hub_requests::StatusCodeEnum::Success or
        not register_response.value.has_value() or register_response.value.value().size() not_eq 4) {
        set_deviceFault("Can't read registers");
        return {};
    }

    // Convert std::vector<int> to std::vector<uint16_t>
    const auto& reg_value_int = register_response.value.value();
    std::vector<uint16_t> reg_value(reg_value_int.begin(), reg_value_int.end());
    return parse_register_data(reg_value, 0);
}

isolation_monitorImpl::MeasurementValue
isolation_monitorImpl::parse_register_data(const std::vector<uint16_t>& reg_value, size_t offset) {
    MeasurementValue m;

    if (reg_value.size() < offset + 4) {
        return m;
    }

    m.received = true;
    m.alarm = to_alarm_type(reg_value.at(offset + 2) >> 8);
    m.test = to_test_type(reg_value.at(offset + 2) >> 8);
    m.unit = to_unit_type(reg_value.at(offset + 2) & 0xFF);
    m.valid = to_valid_type(reg_value.at(offset + 2) & 0xFF);
    m.description = to_channel_description(reg_value.at(offset + 3));

    uint32_t value{0};
    value += reg_value.at(offset + 0) << 16;
    value += reg_value.at(offset + 1);
    auto val = *reinterpret_cast<float*>(&value);
    m.value = val;
    return m;
}

} // namespace main
} // namespace module
