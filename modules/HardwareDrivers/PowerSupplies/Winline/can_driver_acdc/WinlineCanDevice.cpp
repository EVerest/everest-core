/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "WinlineCanDevice.hpp"
#include "CanPackets.hpp"
#include "Conversions.hpp"
#include <everest/logging.hpp>

#include <algorithm>
#include <iomanip>
#include <regex>

static std::vector<std::string> split_by_delimiters(const std::string& s, const std::string& delimiters) {
    std::regex re("[" + delimiters + "]");
    std::sregex_token_iterator first{s.begin(), s.end(), re, -1}, last;
    return {first, last};
}

static std::vector<uint8_t> parse_module_addresses(const std::string& a) {
    std::vector<uint8_t> addresses;
    auto adr = split_by_delimiters(a, ",");
    addresses.reserve(adr.size()); // Pre-allocate memory for efficiency

    for (const auto& ad : adr) {
        try {
            addresses.push_back(std::stoi(ad));
        } catch (const std::exception& e) {
            EVLOG_error << "Winline: Invalid module address '" << ad << "': " << e.what();
        }
    }
    return addresses;
}

WinlineCanDevice::WinlineCanDevice() : CanBus() {
}

WinlineCanDevice::~WinlineCanDevice() {
}

void WinlineCanDevice::initial_ping() {
    if (operating_mode == OperatingMode::GROUP_DISCOVERY) {
        // Winline discovery: Query group information from group broadcast address
        EVLOG_info << "Winline: Starting group discovery on group " << group_address;
        send_read_register(WinlineProtocol::GROUP_BROADCAST_ADDR, WinlineProtocol::Registers::GROUP_INFO, true);
    } else {
        EVLOG_info << "Winline: Operating in FIXED_ADDRESS mode. No need to ping.";
        initialized = true;
        switch_on_off(false);
    }
}

void WinlineCanDevice::set_can_device(const std::string& dev) {
    can_device = dev;
    EVLOG_info << "Winline: Setting config values: CAN device: " << dev;
    open_device(can_device.c_str());
}

void WinlineCanDevice::set_config_values(const std::string& addrs, int group_addr, int timeout,
                                         int controller_address, int power_state_grace_period_ms) {
    this->device_connection_timeout_s = timeout;
    this->group_address = group_addr;
    this->controller_address = controller_address;
    this->power_state_grace_period_ms = power_state_grace_period_ms;

    EVLOG_info << "Winline: Operating with controller address: 0x" << std::hex << controller_address;
    if (!addrs.empty()) {
        operating_mode = OperatingMode::FIXED_ADDRESS;
        active_module_addresses = parse_module_addresses(addrs);
        expected_module_count = active_module_addresses.size();

        // Initialize telemetry entries for configured modules to prevent immediate removal
        // This fixes the chicken-and-egg problem where modules are removed before they can respond
        auto now = std::chrono::steady_clock::now();
        for (const auto& addr : active_module_addresses) {
            auto& telemetry = telemetries[addr];
            telemetry.last_update = now; // Initialize with current time
            EVLOG_debug << "Winline: Initialized telemetry for configured module 0x" << std::hex
                        << static_cast<int>(addr);
        }

        EVLOG_info << "Winline: Operating in FIXED_ADDRESS mode with " << expected_module_count
                   << " addresses: " << addrs;
    } else {
        operating_mode = OperatingMode::GROUP_DISCOVERY;
        EVLOG_info << "Winline: Operating in GROUP_DISCOVERY mode for group address: " << group_address;
    }
    EVLOG_info << "Winline: module communication timeout: " << device_connection_timeout_s << "s";
}

void WinlineCanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
    // if (!(can_id & CAN_EFF_FLAG)) {
    //     return;
    // }

    // Verify this is a Winline protocol message
    uint16_t protocol_number = can_packet_acdc::protocol_number_from_can_id(can_id);
    if (protocol_number != WinlineProtocol::PROTNO) {
        return;
    }

    // Ignore messages not addressed to us (the controller)
    if (can_packet_acdc::destination_address_from_can_id(can_id) != controller_address) {
        return;
    }

    // Discard malformed CAN frames with insufficient data
    if (payload.size() < 8) {
        EVLOG_error << "Winline: Received malformed CAN frame with size " << payload.size()
                    << " (expected 8 bytes). Discarding frame.";
        return;
    }

    const uint8_t source_address = can_packet_acdc::source_address_from_can_id(can_id);

    // Universal Winline response validation helper
    auto validate_response = [&](uint8_t expected_data_type) -> bool {
        uint8_t data_type = payload[0];
        uint8_t error_code = payload[1];

        // Enhanced Winline error code validation
        if (error_code != WinlineProtocol::ERROR_NORMAL) {
            uint16_t register_number = (static_cast<uint16_t>(payload[2]) << 8) | payload[3];

            // Provide specific error code descriptions based on Winline protocol
            std::string error_description;
            switch (error_code) {
            case WinlineProtocol::ERROR_FAULT:
                error_description = "General fault";
                break;
            default:
                error_description = "Unknown error (frame should be discarded per Winline spec)";
                break;
            }

            EVLOG_warning << "Winline: " << error_description << " response from module 0x" << std::hex
                          << static_cast<int>(source_address) << " for register 0x" << register_number
                          << " (error_code=0x" << static_cast<int>(error_code) << ")";

            // Per Winline protocol: "F0: Normal, Others: Fault, discard frame"
            return false;
        }

        if (data_type != expected_data_type) {
            uint16_t register_number = (static_cast<uint16_t>(payload[2]) << 8) | payload[3];
            EVLOG_warning << "Winline: Invalid data type from module 0x" << std::hex << static_cast<int>(source_address)
                          << " for register 0x" << register_number << " (expected=0x"
                          << static_cast<int>(expected_data_type) << ", received=0x" << static_cast<int>(data_type)
                          << ")";
            return false;
        }

        return true;
    };

    // Basic Winline response parsing - extract common fields
    uint8_t data_type = payload[0];
    uint8_t error_code = payload[1];
    uint16_t register_number = (static_cast<uint16_t>(payload[2]) << 8) | payload[3];

    if (error_code != WinlineProtocol::ERROR_NORMAL) {
        EVLOG_warning << "Winline: Received error response from module 0x" << std::hex
                      << static_cast<int>(source_address) << " for register 0x" << register_number << " (error=0x"
                      << static_cast<int>(error_code) << ")";
        return;
    }

    // Comprehensive Winline register response parsing with standardized format validation
    switch (register_number) {
    // Core telemetry registers
    case WinlineProtocol::Registers::VOLTAGE: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadVoltage voltage_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.voltage = voltage_reading.voltage;
            telemetry.last_update = std::chrono::steady_clock::now();
            signalVoltageCurrent(telemetries);
            EVLOG_debug << format_module_id(source_address) << ": Voltage = " << voltage_reading.voltage << "V";
        }
    } break;
    case WinlineProtocol::Registers::CURRENT: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadCurrent current_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.current = current_reading.current;
            telemetry.last_update = std::chrono::steady_clock::now();
            signalVoltageCurrent(telemetries);
            EVLOG_debug << format_module_id(source_address) << ": Current = " << current_reading.current << "A";
        }
    } break;
    case WinlineProtocol::Registers::CURRENT_LIMIT_POINT: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadCurrentLimitPoint limit_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.current_limit_point = limit_reading.limit_point;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_debug << format_module_id(source_address) << ": Current limit = " << limit_reading.limit_point;
        }
    } break;

        // Temperature monitoring registers
    case WinlineProtocol::Registers::DC_BOARD_TEMPERATURE: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadDCBoardTemperature temp_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.dc_board_temperature = temp_reading.temperature;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_debug << format_module_id(source_address) << ": DC board temp = " << temp_reading.temperature << "°C";
        }
    } break;
    case WinlineProtocol::Registers::AMBIENT_TEMPERATURE: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadAmbientTemperature temp_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.ambient_temperature = temp_reading.temperature;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_debug << format_module_id(source_address) << ": Ambient temp = " << temp_reading.temperature << "°C";
        }
    } break;

    // Module capabilities and ratings
    case WinlineProtocol::Registers::RATED_OUTPUT_POWER: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadRatedOutputPower power_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.dc_rated_output_power = power_reading.power;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_info << format_module_id(source_address) << ": Rated power = " << power_reading.power << "W";

            // Check if capabilities are now complete and trigger update
            check_and_update_capabilities(source_address);
        }
    } break;
    case WinlineProtocol::Registers::RATED_OUTPUT_CURRENT: {
        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT) {
            can_packet_acdc::ReadRatedOutputCurrent current_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.dc_max_output_current = current_reading.current;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_info << format_module_id(source_address) << ": Rated current = " << current_reading.current << "A";

            // Check if capabilities are now complete and trigger update
            check_and_update_capabilities(source_address);
        }
    } break;

    // Status and diagnostic information
    case WinlineProtocol::Registers::STATUS: {
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER) {
            can_packet_acdc::PowerModuleStatus status(payload);
            signalModuleStatus(status);
            auto& telemetry = telemetries[source_address];
            check_and_signal_error_status_change(source_address, status, telemetry.status);

            // Enhanced status monitoring - update status and perform analysis
            telemetry.status = status;
            telemetry.last_update = std::chrono::steady_clock::now();

            // Perform trend analysis and maintain status history
            analyze_status_trends(source_address);

            // Enhanced power control verification (Task 12)
            auto& power_tracking = telemetry.power_tracking;
            if (power_tracking.power_commands_sent > 0 && !power_tracking.power_state_verified) {
                bool verification_result = verify_power_state(source_address, power_tracking.expected_power_state);
                if (verification_result) {
                    EVLOG_debug << "Winline: Power state verification successful for module 0x" << std::hex
                                << static_cast<int>(source_address);
                }
            }

            // Update performance metrics for successful status read
            telemetry.status_metrics.status_reads_total++;
            telemetry.status_metrics.last_status_read = std::chrono::steady_clock::now();
            telemetry.status_metrics.status_read_success_rate =
                100.0f * (telemetry.status_metrics.status_reads_total - telemetry.status_metrics.status_errors_total) /
                telemetry.status_metrics.status_reads_total;

            EVLOG_debug << format_module_id(source_address) << ": Status = " << status;
        }
    } break;
    case WinlineProtocol::Registers::GROUP_INFO: {
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER) {
            can_packet_acdc::ReadGroupInfo group_info(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.group_number = group_info.group_number;
            telemetry.dip_address = group_info.dip_address;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_info << format_module_id(source_address) << ": group=" << static_cast<int>(group_info.group_number)
                       << ", DIP=" << static_cast<int>(group_info.dip_address);
            // Enhanced module discovery with group validation
            if (operating_mode == OperatingMode::GROUP_DISCOVERY) {
                // Validate that the discovered module belongs to our target group
                if (group_info.group_number == group_address) {
                    if (std::find(active_module_addresses.begin(), active_module_addresses.end(), source_address) ==
                        active_module_addresses.end()) {
                        active_module_addresses.push_back(source_address);
                        EVLOG_info << "Winline: Discovered new module at address 0x" << std::hex
                                   << static_cast<int>(source_address) << " in group "
                                   << static_cast<int>(group_info.group_number);
                    }
                } else {
                    EVLOG_debug << "Winline: Ignoring module at address 0x" << std::hex
                                << static_cast<int>(source_address) << " (belongs to group "
                                << static_cast<int>(group_info.group_number) << ", expected group " << group_address
                                << ")";
                }
            }
        }
    } break;

        // Module identification
    case WinlineProtocol::Registers::SERIAL_NUMBER_LOW: {
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER) {
            uint32_t serial_low = from_raw<uint32_t>(payload, 4);
            auto& telemetry = telemetries[source_address];
            telemetry.serial_low = serial_low;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_debug << format_module_id(source_address) << ": Serial low = 0x" << std::hex << serial_low;
            // Check if we have both parts of serial number
            if (telemetry.serial_high != 0) {
                can_packet_acdc::ReadSerialNumber serial_number(telemetry.serial_low, telemetry.serial_high);
                telemetry.serial_number = serial_number.serial_number;
                EVLOG_info << format_module_id(source_address) << ": Complete serial = " << telemetry.serial_number;
            }
        }
    } break;
    case WinlineProtocol::Registers::SERIAL_NUMBER_HIGH: {
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER) {
            uint32_t serial_high = from_raw<uint32_t>(payload, 4);
            auto& telemetry = telemetries[source_address];
            telemetry.serial_high = serial_high;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_debug << format_module_id(source_address) << ": Serial high = 0x" << std::hex << serial_high;
            // Check if we have both parts of serial number
            if (telemetry.serial_low != 0) {
                can_packet_acdc::ReadSerialNumber serial_number(telemetry.serial_low, telemetry.serial_high);
                telemetry.serial_number = serial_number.serial_number;
                EVLOG_info << format_module_id(source_address) << ": Complete serial = " << telemetry.serial_number;
            }
        }
    } break;

        // Version information
    case WinlineProtocol::Registers::DCDC_VERSION: {
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER) {
            can_packet_acdc::ReadDCDCVersion version_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.dcdc_version = version_reading.version;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_info << format_module_id(source_address) << ": DCDC version = " << version_reading.version;
        }
    } break;
    case WinlineProtocol::Registers::PFC_VERSION: {
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER) {
            can_packet_acdc::ReadPFCVersion version_reading(payload);
            auto& telemetry = telemetries[source_address];
            telemetry.pfc_version = version_reading.version;
            telemetry.last_update = std::chrono::steady_clock::now();
            EVLOG_info << format_module_id(source_address) << ": PFC version = " << version_reading.version;
        }
    } break;

    // SET operation responses (confirmation of settings)
    case WinlineProtocol::Registers::SET_OUTPUT_VOLTAGE:
    case WinlineProtocol::Registers::SET_OUTPUT_CURRENT:
    case WinlineProtocol::Registers::SET_GROUP_NUMBER:
    case WinlineProtocol::Registers::SET_ALTITUDE:
    case WinlineProtocol::Registers::SET_INPUT_MODE:
    case WinlineProtocol::Registers::SET_ADDRESS_MODE:
    case WinlineProtocol::Registers::SET_CURRENT_LIMIT_POINT:
    case WinlineProtocol::Registers::SET_VOLTAGE_UPPER_LIMIT: {
        // SET operations return success/failure confirmation
        if (error_code == WinlineProtocol::ERROR_NORMAL) {
            EVLOG_debug << format_module_id(source_address) << ": SET operation confirmed for register 0x" << std::hex
                        << register_number;
        } else {
            EVLOG_warning << format_module_id(source_address) << ": SET operation failed for register 0x" << std::hex
                          << register_number << " (error=0x" << static_cast<int>(error_code) << ")";
        }
        auto& telemetry = telemetries[source_address];
        telemetry.last_update = std::chrono::steady_clock::now();
    } break;

    case WinlineProtocol::Registers::POWER_CONTROL: {
        // Enhanced power control response handling (Task 12)
        auto& telemetry = telemetries[source_address];
        auto& power_tracking = telemetry.power_tracking;

        if (error_code == WinlineProtocol::ERROR_NORMAL) {
            EVLOG_info << format_module_id(source_address) << ": Power control command confirmed - "
                       << (power_tracking.expected_power_state ? "ON" : "OFF");

            // Power command was accepted, but we still need to verify actual state change via status
            EVLOG_debug << "Winline: Power control response received. Will verify actual state via status register.";
        } else {
            EVLOG_error << format_module_id(source_address) << ": Power control command FAILED for register 0x"
                        << std::hex << register_number << " (error=0x" << static_cast<int>(error_code) << ")";

            // Reset power tracking on command failure
            power_tracking.power_state_verified = false;
            power_tracking.power_state_mismatches++;
        }

        telemetry.last_update = std::chrono::steady_clock::now();
    } break;

    default: {
        EVLOG_debug << "Winline: Unhandled register response 0x" << std::hex << register_number << " from module 0x"
                    << static_cast<int>(source_address) << " (DataType=0x" << static_cast<int>(data_type)
                    << ", Error=0x" << static_cast<int>(error_code) << ")";
    }
    }
}

size_t WinlineCanDevice::remove_expired_telemetry_entries() {
    auto now = std::chrono::steady_clock::now();
    auto timeout_duration = std::chrono::seconds(device_connection_timeout_s);
    size_t removed_count = 0;

    // Remove expired telemetry entries
    for (auto it = telemetries.begin(); it != telemetries.end();) {
        const auto& [address, telemetry] = *it;
        if (now - telemetry.last_update > timeout_duration) {
            EVLOG_warning << format_module_id(address, telemetry.serial_number)
                          << ": module communication expired (timeout: " << device_connection_timeout_s
                          << "s). Removing from active modules.";
            it = telemetries.erase(it);
            {
                active_module_addresses.erase(
                    std::remove(active_module_addresses.begin(), active_module_addresses.end(), address),
                    active_module_addresses.end());
            }
            ++removed_count;
        } else {
            ++it;
        }
    }

    // Update active_module_addresses to match current telemetries keys
    {
        // Check CommunicationFault state: trigger if no active modules but we expect some, clear otherwise
        if (removed_count != 0 && telemetries.empty()) {
            // No modules responding - trigger CommunicationFault
            signalError(0xFF, Error::CommunicationFault, true); // Use address 0xFF for system-wide fault
        } else if (!telemetries.empty()) {
            // At least one module responding - clear CommunicationFault
            signalError(0xFF, Error::CommunicationFault, false); // Use address 0xFF for system-wide fault
        }
    }

    return removed_count;
}

void WinlineCanDevice::poll_status_handler() {
    // Remove expired telemetry entries
    size_t removed_count = remove_expired_telemetry_entries();

    if (removed_count > 0) {
        EVLOG_info << "Winline: Removed " << removed_count << " expired modules. "
                   << "Active modules remaining: " << active_module_addresses.size();
        // signal the telemetry updates
        signalCapabilitiesUpdate(telemetries);
        signalVoltageCurrent(telemetries);
    }

    // --- Telemetry Polling ---
    // Poll ALL configured modules (not just active ones) to allow offline modules to recover.
    // This enables automatic recovery when temporarily offline modules come back online.

    // For Winline: Enhanced group discovery using register 0x0043 (GROUP_INFO)
    if (operating_mode == OperatingMode::GROUP_DISCOVERY) {
        // Use the enhanced discovery method for more comprehensive group operations
        discover_group_modules();

        // Also query group status for comprehensive group monitoring
        send_read_register(WinlineProtocol::GROUP_BROADCAST_ADDR, WinlineProtocol::Registers::STATUS, true);
    }

    for (const auto& addr : active_module_addresses) {
        // Read essential telemetry using Winline registers
        send_read_register(addr, WinlineProtocol::Registers::VOLTAGE); // Read voltage
        send_read_register(addr, WinlineProtocol::Registers::CURRENT); // Read current

        // Enhanced status monitoring (Task 11) - use comprehensive status check
        perform_comprehensive_status_check(addr);

        send_read_register(addr, WinlineProtocol::Registers::CURRENT_LIMIT_POINT); // Read current limit point

        // Read serial number if we don't have it yet (only poll once to avoid spam)
        auto it = telemetries.find(addr);
        if (it == telemetries.end() || it->second.serial_number.empty()) {
            send_read_register(addr, WinlineProtocol::Registers::RATED_OUTPUT_POWER);   // Read capabilities
            send_read_register(addr, WinlineProtocol::Registers::RATED_OUTPUT_CURRENT); // Read capabilities
            send_read_register(addr, WinlineProtocol::Registers::SERIAL_NUMBER_LOW);    // Read serial number low
            send_read_register(addr, WinlineProtocol::Registers::SERIAL_NUMBER_HIGH);   // Read serial number high
        }

        // Log status summary for modules with issues (every 10th poll to avoid spam)
        static uint32_t poll_counter = 0;
        if (++poll_counter % 10 == 0 && it != telemetries.end()) {
            const auto& status = it->second.status;
            if (status.module_fault || status.module_protection || status.temperature_derating ||
                status.module_power_limiting || status.fan_fault) {
                std::string summary = get_status_summary(addr);
                EVLOG_info << summary;
            }
        }
    }
}

bool WinlineCanDevice::switch_on_off(bool on) {
    EVLOG_info << "Winline: switch_on_off(" << on << ") - active modules: " << active_module_addresses.size();

    if (active_module_addresses.empty()) {
        EVLOG_warning << "Winline: No active modules to send switch_on_off command to.";
        return false;
    }

    // Optimize: Use group operations when we have multiple modules and are in group discovery mode
    if (operating_mode == OperatingMode::GROUP_DISCOVERY && active_module_addresses.size() > 2) {
        EVLOG_info << "Winline: Using optimized group operation for " << active_module_addresses.size() << " modules";
        // Release current lock before calling group function to avoid deadlock
        // The group function will acquire its own lock
        return switch_on_off_group(on);
    }

    // Use individual module commands for fixed address mode or small numbers of modules
    EVLOG_info << "Winline: Using individual module commands with enhanced power tracking";
    uint32_t power_value = on ? WinlineProtocol::POWER_ON : WinlineProtocol::POWER_OFF;
    bool success = true;

    for (const auto& addr : active_module_addresses) {
        bool module_success = send_set_register_integer(addr, WinlineProtocol::Registers::POWER_CONTROL, power_value);
        if (!module_success) {
            EVLOG_warning << "Winline: Failed to send power control to module 0x" << std::hex
                         << static_cast<int>(addr);
            success = false;
        } else {
            // Track power state change for this module
            track_power_state_change(addr, on);
        }
    }

    if (success) {
        EVLOG_info
            << "Winline: Power commands sent successfully. Power state verification will occur in next status poll.";
    }

    return success;
}

bool WinlineCanDevice::set_voltage_current(float voltage, float current) {
    EVLOG_info << "Winline: set_voltage_current(" << voltage << "V, " << current
               << "A) - active modules: " << active_module_addresses.size();

    // Validate that we have active modules before attempting to divide current
    const size_t module_count = active_module_addresses.size();
    if (module_count == 0) {
        EVLOG_warning << "Winline: No active modules to set voltage/current.";
        return false;
    }

    // Optimize: Use group operations when we have multiple modules and are in group discovery mode
    // Note: For group operations, current division should be handled by the modules themselves
    if (operating_mode == OperatingMode::GROUP_DISCOVERY && module_count > 2) {
        EVLOG_info << "Winline: Using optimized group operation for " << module_count << " modules";
        // Calculate current per module for group operation
        const float current_per_module = current / static_cast<float>(module_count);
        // Release current lock before calling group function to avoid deadlock
        // The group function will acquire its own lock
        return set_voltage_current_group(voltage, current_per_module);
    }

    // Use individual module commands for fixed address mode or small numbers of modules
    EVLOG_info << "Winline: Using individual module commands";
    const float current_per_module = current / static_cast<float>(module_count);
    bool success = true;

    for (const auto& addr : active_module_addresses) {
        // Set voltage using register 0x0021 (float)
        bool voltage_result = send_set_register_float(addr, WinlineProtocol::Registers::SET_OUTPUT_VOLTAGE, voltage);

        // Set current using register 0x001B (integer, scaled by 1024)
        uint32_t scaled_current = static_cast<uint32_t>(current_per_module * WinlineProtocol::CURRENT_SCALE_FACTOR);
        bool current_result =
            send_set_register_integer(addr, WinlineProtocol::Registers::SET_OUTPUT_CURRENT, scaled_current);

        success &= (voltage_result && current_result);
    }
    return success;
}

// Enhanced Winline group operations
bool WinlineCanDevice::switch_on_off_group(bool on) {

    EVLOG_info << "Winline: switch_on_off_group(" << on << ") - targeting group " << group_address
               << " with enhanced power tracking";

    // Use Winline group broadcast to send power control to all modules in the group
    uint32_t power_value = on ? WinlineProtocol::POWER_ON : WinlineProtocol::POWER_OFF;
    bool result = send_set_register_integer(WinlineProtocol::GROUP_BROADCAST_ADDR,
                                            WinlineProtocol::Registers::POWER_CONTROL, power_value, true);

    if (!result) {
        EVLOG_warning << "Winline: Group power control command failed";
    } else {
        EVLOG_info << "Winline: Group power control command sent successfully (group " << group_address << ")";

        // Enhanced power tracking (Task 12) - track power state change for all active modules
        for (const auto& addr : active_module_addresses) {
            track_power_state_change(addr, on);
        }

        EVLOG_info << "Winline: Power state tracking updated for " << active_module_addresses.size()
                   << " modules. Verification will occur in next status poll.";
    }

    return result;
}

bool WinlineCanDevice::set_voltage_current_group(float voltage, float current) {
    EVLOG_info << "Winline: set_voltage_current_group(" << voltage << "V, " << current << "A) - targeting group "
               << group_address;

    // For group operations, current is NOT divided - each module in the group should handle current distribution
    // This is different from individual module control where we divide current among modules
    bool voltage_result = send_set_register_float(WinlineProtocol::GROUP_BROADCAST_ADDR,
                                                  WinlineProtocol::Registers::SET_OUTPUT_VOLTAGE, voltage, true);

    // Set current using register 0x001B (integer, scaled by 1024)
    uint32_t scaled_current = static_cast<uint32_t>(current * WinlineProtocol::CURRENT_SCALE_FACTOR);
    bool current_result = send_set_register_integer(
        WinlineProtocol::GROUP_BROADCAST_ADDR, WinlineProtocol::Registers::SET_OUTPUT_CURRENT, scaled_current, true);

    bool success = voltage_result && current_result;

    if (!success) {
        EVLOG_warning << "Winline: Group voltage/current setting failed (voltage=" << voltage_result
                      << ", current=" << current_result << ")";
    } else {
        EVLOG_info << "Winline: Group voltage/current setting sent successfully (group " << group_address << ")";
    }

    return success;
}

bool WinlineCanDevice::send_group_command_to_all_modules(uint16_t register_number, uint32_t value) {
    EVLOG_info << "Winline: send_group_command_to_all_modules(register=0x" << std::hex << register_number
               << ", value=0x" << value << ") - targeting group " << group_address;

    // Send integer command to all modules in the group using group broadcast
    bool result = send_set_register_integer(WinlineProtocol::GROUP_BROADCAST_ADDR, register_number, value, true);

    if (!result) {
        EVLOG_warning << "Winline: Group command failed for register 0x" << std::hex << register_number;
    } else {
        EVLOG_info << "Winline: Group command sent successfully for register 0x" << std::hex << register_number
                   << " (group " << group_address << ")";
    }

    return result;
}

bool WinlineCanDevice::discover_group_modules() {
    EVLOG_info << "Winline: discover_group_modules() - querying group " << group_address;

    // Send group discovery command using register 0x0043 (GROUP_INFO)
    bool result =
        send_read_register(WinlineProtocol::GROUP_BROADCAST_ADDR, WinlineProtocol::Registers::GROUP_INFO, true);

    if (!result) {
        EVLOG_warning << "Winline: Group discovery command failed";
    } else {
        EVLOG_info << "Winline: Group discovery command sent successfully (group " << group_address << ")";
    }

    return result;
}

// Winline error recovery operations
bool WinlineCanDevice::reset_overvoltage_protection(uint8_t module_address) {
    EVLOG_info << "Winline: reset_overvoltage_protection(0x" << std::hex << static_cast<int>(module_address) << ")";

    bool result = send_set_register_integer(module_address, WinlineProtocol::Registers::SET_OVERVOLTAGE_RESET,
                                            WinlineProtocol::RESET_ENABLE);

    if (!result) {
        EVLOG_warning << "Winline: Overvoltage reset command failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << "Winline: Overvoltage reset command sent successfully to module 0x" << std::hex
                   << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::reset_short_circuit_protection(uint8_t module_address) {
    EVLOG_info << "Winline: reset_short_circuit_protection(0x" << std::hex << static_cast<int>(module_address) << ")";

    bool result = send_set_register_integer(module_address, WinlineProtocol::Registers::SET_SHORT_CIRCUIT_RESET,
                                            WinlineProtocol::RESET_ENABLE);

    if (!result) {
        EVLOG_warning << "Winline: Short circuit reset command failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << "Winline: Short circuit reset command sent successfully to module 0x" << std::hex
                   << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::enable_overvoltage_protection(uint8_t module_address, bool enable) {
    EVLOG_info << "Winline: enable_overvoltage_protection(0x" << std::hex << static_cast<int>(module_address) << ", "
               << enable << ")";

    uint32_t protection_value = enable ? WinlineProtocol::RESET_DISABLE : WinlineProtocol::RESET_ENABLE;
    bool result = send_set_register_integer(module_address, WinlineProtocol::Registers::SET_OVERVOLTAGE_PROTECTION,
                                            protection_value);

    if (!result) {
        EVLOG_warning << "Winline: Overvoltage protection command failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << "Winline: Overvoltage protection " << (enable ? "enabled" : "disabled")
                   << " successfully for module 0x" << std::hex << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::reset_all_faults(uint8_t module_address) {
    EVLOG_info << "Winline: reset_all_faults(0x" << std::hex << static_cast<int>(module_address) << ")";

    bool overvoltage_result = reset_overvoltage_protection(module_address);
    bool short_circuit_result = reset_short_circuit_protection(module_address);

    bool all_success = overvoltage_result && short_circuit_result;

    if (!all_success) {
        EVLOG_warning << "Winline: Some fault reset commands failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << "Winline: All fault reset commands sent successfully to module 0x" << std::hex
                   << static_cast<int>(module_address);
    }

    return all_success;
}

// Enhanced Winline precision control methods
bool WinlineCanDevice::set_current_limit_point(uint8_t module_address, float current_limit_point) {
    EVLOG_info << "Winline: set_current_limit_point(0x" << std::hex << static_cast<int>(module_address) << ", "
               << current_limit_point << ")";

    // Validate current limit point (should be between 0.0 and 1.0 for percentage)
    if (current_limit_point < 0.0f || current_limit_point > 1.0f) {
        EVLOG_warning << "Winline: Invalid current limit point " << current_limit_point
                      << " (must be 0.0-1.0). Clamping to valid range.";
        current_limit_point = std::max(0.0f, std::min(current_limit_point, 1.0f));
    }

    bool result = send_set_register_float(module_address, WinlineProtocol::Registers::SET_CURRENT_LIMIT_POINT,
                                          current_limit_point);

    if (!result) {
        EVLOG_warning << "Winline: Current limit point setting failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << "Winline: Current limit point set successfully to " << current_limit_point << " for module 0x"
                   << std::hex << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::set_voltage_upper_limit(uint8_t module_address, float voltage_upper_limit) {
    EVLOG_info << "Winline: set_voltage_upper_limit(0x" << std::hex << static_cast<int>(module_address) << ", "
               << voltage_upper_limit << "V)";

    // Basic validation - voltage should be positive
    if (voltage_upper_limit <= 0.0f) {
        EVLOG_warning << "Winline: Invalid voltage upper limit " << voltage_upper_limit
                      << "V (must be positive). Command not sent.";
        return false;
    }

    bool result = send_set_register_float(module_address, WinlineProtocol::Registers::SET_VOLTAGE_UPPER_LIMIT,
                                          voltage_upper_limit);

    if (!result) {
        EVLOG_warning << "Winline: Voltage upper limit setting failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << "Winline: Voltage upper limit set successfully to " << voltage_upper_limit << "V for module 0x"
                   << std::hex << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::set_voltage_current_with_validation(float voltage, float current, float max_voltage,
                                                           float max_current) {

    EVLOG_info << "Winline: set_voltage_current_with_validation(" << voltage << "V, " << current
               << "A, max_V=" << max_voltage << "V, max_A=" << max_current
               << "A) - active modules: " << active_module_addresses.size();

    // Enhanced validation with limits
    if (max_voltage > 0.0f && voltage > max_voltage) {
        EVLOG_warning << "Winline: Requested voltage " << voltage << "V exceeds maximum " << max_voltage
                      << "V. Clamping.";
        voltage = max_voltage;
    }

    if (max_current > 0.0f && current > max_current) {
        EVLOG_warning << "Winline: Requested current " << current << "A exceeds maximum " << max_current
                      << "A. Clamping.";
        current = max_current;
    }

    if (voltage <= 0.0f || current < 0.0f) {
        EVLOG_error << "Winline: Invalid voltage/current values (V=" << voltage << ", A=" << current << ")";
        return false;
    }

    const size_t module_count = active_module_addresses.size();
    if (module_count == 0) {
        EVLOG_warning << "Winline: No active modules for validated voltage/current setting.";
        return false;
    }

    // Use existing optimized implementation
    return set_voltage_current(voltage, current);
}

bool WinlineCanDevice::set_precision_current_control(uint8_t module_address, float current, float limit_point) {
    EVLOG_info << "Winline: set_precision_current_control(0x" << std::hex << static_cast<int>(module_address) << ", "
               << current << "A, limit=" << limit_point << ")";

    // Set current limit point first
    bool limit_result = set_current_limit_point(module_address, limit_point);

    // Then set the current using the precise scaled method
    uint32_t scaled_current = static_cast<uint32_t>(current * WinlineProtocol::CURRENT_SCALE_FACTOR);
    bool current_result =
        send_set_register_integer(module_address, WinlineProtocol::Registers::SET_OUTPUT_CURRENT, scaled_current);

    bool success = limit_result && current_result;

    if (!success) {
        EVLOG_warning << "Winline: Precision current control failed for module 0x" << std::hex
                      << static_cast<int>(module_address) << " (limit=" << limit_result
                      << ", current=" << current_result << ")";
    } else {
        EVLOG_info << "Winline: Precision current control set successfully for module 0x" << std::hex
                   << static_cast<int>(module_address);
    }

    return success;
}

bool WinlineCanDevice::read_module_telemetry(uint8_t module_address) {
    EVLOG_debug << "Winline: read_module_telemetry(0x" << std::hex << static_cast<int>(module_address) << ")";

    // Read comprehensive telemetry using optimized batch approach
    bool voltage_result = send_read_register(module_address, WinlineProtocol::Registers::VOLTAGE);
    bool current_result = send_read_register(module_address, WinlineProtocol::Registers::CURRENT);
    bool status_result = send_read_register(module_address, WinlineProtocol::Registers::STATUS);
    bool current_limit_result = send_read_register(module_address, WinlineProtocol::Registers::CURRENT_LIMIT_POINT);

    // Optional extended telemetry
    bool dc_temp_result = send_read_register(module_address, WinlineProtocol::Registers::DC_BOARD_TEMPERATURE);
    bool ambient_temp_result = send_read_register(module_address, WinlineProtocol::Registers::AMBIENT_TEMPERATURE);

    bool basic_success = voltage_result && current_result && status_result;
    bool extended_success = current_limit_result && dc_temp_result && ambient_temp_result;

    if (!basic_success) {
        EVLOG_warning << "Winline: Basic telemetry read failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_debug << "Winline: Telemetry read commands sent for module 0x" << std::hex
                    << static_cast<int>(module_address) << " (basic=" << basic_success
                    << ", extended=" << extended_success << ")";
    }

    return basic_success;
}

// Enhanced Winline Status Monitoring Capabilities (Task 11)
bool WinlineCanDevice::perform_comprehensive_status_check(uint8_t module_address) {
    EVLOG_debug << "Winline: perform_comprehensive_status_check(0x" << std::hex << static_cast<int>(module_address)
                << ")";

    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_warning << "Winline: Cannot perform status check for unknown module 0x" << std::hex
                      << static_cast<int>(module_address);
        return false;
    }

    auto& telemetry = it->second;
    telemetry.status_metrics.status_reads_total++;
    telemetry.status_metrics.last_status_read = std::chrono::steady_clock::now();

    // Read comprehensive status information
    bool status_result = send_read_register(module_address, WinlineProtocol::Registers::STATUS);

    if (!status_result) {
        telemetry.status_metrics.status_errors_total++;
        // Update success rate
        telemetry.status_metrics.status_read_success_rate =
            100.0f * (telemetry.status_metrics.status_reads_total - telemetry.status_metrics.status_errors_total) /
            telemetry.status_metrics.status_reads_total;

        EVLOG_warning << "Winline: Comprehensive status check failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
        return false;
    }

    // Update success rate
    telemetry.status_metrics.status_read_success_rate =
        100.0f * (telemetry.status_metrics.status_reads_total - telemetry.status_metrics.status_errors_total) /
        telemetry.status_metrics.status_reads_total;

    // Log status diagnostics if there are any active status flags
    const auto& status = telemetry.status;
    if (status.module_fault || status.module_protection || status.dcdc_overvoltage || status.dcdc_short_circuit ||
        status.dcdc_over_temperature || status.fan_fault) {
        log_status_diagnostics(module_address, status);
    }

    EVLOG_debug << "Winline: Comprehensive status check completed for module 0x" << std::hex
                << static_cast<int>(module_address)
                << " (success rate: " << telemetry.status_metrics.status_read_success_rate << "%)";

    return true;
}

bool WinlineCanDevice::analyze_status_trends(uint8_t module_address) {
    EVLOG_debug << "Winline: analyze_status_trends(0x" << std::hex << static_cast<int>(module_address) << ")";

    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_warning << "Winline: Cannot analyze trends for unknown module 0x" << std::hex
                      << static_cast<int>(module_address);
        return false;
    }

    auto& telemetry = it->second;
    auto& history = telemetry.status_history;

    // Maintain status history (keep last 10 entries)
    constexpr size_t MAX_HISTORY_SIZE = 10;
    history.recent_status.push_back(telemetry.status);
    if (history.recent_status.size() > MAX_HISTORY_SIZE) {
        history.recent_status.pop_front();
    }

    // Analyze trends if we have enough history
    if (history.recent_status.size() >= 3) {
        // Check for persistent faults (fault present in last 3 readings)
        bool persistent_fault = true;
        for (size_t i = history.recent_status.size() - 3; i < history.recent_status.size(); ++i) {
            if (!history.recent_status[i].module_fault && !history.recent_status[i].module_protection) {
                persistent_fault = false;
                break;
            }
        }

        if (persistent_fault) {
            EVLOG_warning << "Winline: Persistent fault detected in module 0x" << std::hex
                          << static_cast<int>(module_address) << " (fault present in last 3 status readings)";
        }

        // Check for frequent temperature derating
        int temp_derating_count = 0;
        for (size_t i = history.recent_status.size() - 5;
             i < history.recent_status.size() && i < history.recent_status.size(); ++i) {
            if (history.recent_status[i].temperature_derating) {
                temp_derating_count++;
            }
        }

        if (temp_derating_count >= 3) {
            EVLOG_warning << "Winline: Frequent temperature derating detected in module 0x" << std::hex
                          << static_cast<int>(module_address) << " (" << temp_derating_count
                          << " occurrences in recent readings)";
        }

        // Check for power limiting patterns
        int power_limiting_count = 0;
        for (size_t i = history.recent_status.size() - 5;
             i < history.recent_status.size() && i < history.recent_status.size(); ++i) {
            if (history.recent_status[i].module_power_limiting || history.recent_status[i].ac_power_limiting) {
                power_limiting_count++;
            }
        }

        if (power_limiting_count >= 3) {
            EVLOG_info << "Winline: Power limiting pattern detected in module 0x" << std::hex
                       << static_cast<int>(module_address) << " (" << power_limiting_count
                       << " occurrences in recent readings) - this may indicate thermal or electrical limits";
        }
    }

    // Update fault statistics
    const auto& current_status = telemetry.status;
    bool has_fault = current_status.module_fault || current_status.module_protection ||
                     current_status.dcdc_overvoltage || current_status.dcdc_short_circuit ||
                     current_status.dcdc_over_temperature || current_status.fan_fault;

    if (has_fault) {
        history.fault_count++;
        history.last_fault_time = std::chrono::steady_clock::now();

        EVLOG_info << "Winline: Module 0x" << std::hex << static_cast<int>(module_address)
                   << " fault count: " << history.fault_count;
    }

    return true;
}

void WinlineCanDevice::log_status_diagnostics(uint8_t module_address,
                                              const can_packet_acdc::PowerModuleStatus& status) {
    std::stringstream diagnostics;
    diagnostics << "Winline: Status diagnostics for module 0x" << std::hex << static_cast<int>(module_address) << ": ";

    // Critical faults
    if (status.module_fault)
        diagnostics << "[CRITICAL:MODULE_FAULT] ";
    if (status.dcdc_overvoltage)
        diagnostics << "[CRITICAL:DCDC_OVERVOLTAGE] ";
    if (status.dcdc_short_circuit)
        diagnostics << "[CRITICAL:SHORT_CIRCUIT] ";
    if (status.dcdc_over_temperature)
        diagnostics << "[CRITICAL:OVER_TEMPERATURE] ";
    if (status.dcdc_output_overvoltage)
        diagnostics << "[CRITICAL:OUTPUT_OVERVOLTAGE] ";

    // Warning conditions
    if (status.module_protection)
        diagnostics << "[WARNING:PROTECTION_ACTIVE] ";
    if (status.fan_fault)
        diagnostics << "[WARNING:FAN_FAULT] ";
    if (status.temperature_derating)
        diagnostics << "[WARNING:TEMP_DERATING] ";
    if (status.module_power_limiting)
        diagnostics << "[WARNING:POWER_LIMITING] ";
    if (status.ac_power_limiting)
        diagnostics << "[WARNING:AC_LIMITING] ";

    // Communication and input issues
    if (status.can_communication_failure)
        diagnostics << "[COMM:CAN_FAILURE] ";
    if (status.sci_communication_failure)
        diagnostics << "[COMM:SCI_FAILURE] ";
    if (status.input_mode_error)
        diagnostics << "[INPUT:MODE_ERROR] ";
    if (status.input_mode_mismatch)
        diagnostics << "[INPUT:MODE_MISMATCH] ";
    if (status.pfc_voltage_abnormal)
        diagnostics << "[INPUT:PFC_ABNORMAL] ";
    if (status.ac_overvoltage)
        diagnostics << "[INPUT:AC_OVERVOLTAGE] ";
    if (status.ac_undervoltage)
        diagnostics << "[INPUT:AC_UNDERVOLTAGE] ";

    // Operational status
    if (status.dcdc_on_off_status)
        diagnostics << "[STATUS:DCDC_OFF] ";
    if (status.module_current_imbalance)
        diagnostics << "[STATUS:CURRENT_IMBALANCE] ";

    std::string diagnostic_str = diagnostics.str();
    if (diagnostic_str.length() > 80) { // If diagnostics string is long, split it
        EVLOG_warning << diagnostic_str;
    } else {
        EVLOG_info << diagnostic_str;
    }
}

std::string WinlineCanDevice::get_status_summary(uint8_t module_address) const {
    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        return "Module not found";
    }

    const auto& telemetry = it->second;
    const auto& status = telemetry.status;
    const auto& history = telemetry.status_history;
    const auto& metrics = telemetry.status_metrics;

    std::stringstream summary;
    summary << "Module[0x" << std::hex << static_cast<int>(module_address) << "] ";

    // Overall health
    bool has_critical_fault = status.module_fault || status.dcdc_overvoltage || status.dcdc_short_circuit ||
                              status.dcdc_over_temperature || status.dcdc_output_overvoltage;

    if (has_critical_fault) {
        summary << "HEALTH:CRITICAL ";
    } else if (status.module_protection || status.fan_fault || status.temperature_derating) {
        summary << "HEALTH:WARNING ";
    } else {
        summary << "HEALTH:NORMAL ";
    }

    // Performance metrics
    summary << "METRICS:(reads:" << metrics.status_reads_total << ",success:" << std::fixed << std::setprecision(1)
            << metrics.status_read_success_rate << "%) ";

    // Fault statistics
    summary << "FAULTS:" << history.fault_count << " ";
    if (history.recovery_count > 0) {
        summary << "RECOVERIES:" << history.recovery_count << " ";
    }

    // Enhanced power control statistics (Task 12)
    const auto& power_tracking = telemetry.power_tracking;
    if (power_tracking.power_commands_sent > 0) {
        summary << "POWER_CMDS:" << power_tracking.power_commands_sent << " ";
        if (power_tracking.power_state_mismatches > 0) {
            summary << "POWER_MISMATCHES:" << power_tracking.power_state_mismatches << " ";
        }
        if (power_tracking.power_state_verified) {
            summary << "POWER_VERIFIED ";
        }
    }

    // Power status (actual vs expected)
    if (status.dcdc_on_off_status) {
        summary << "POWER:OFF ";
    } else {
        summary << "POWER:ON ";
    }

    // Show expected vs actual if they differ
    if (power_tracking.power_commands_sent > 0 && power_tracking.expected_power_state != (!status.dcdc_on_off_status)) {
        summary << "EXPECTED:" << (power_tracking.expected_power_state ? "ON" : "OFF") << " ";
    }

    return summary.str();
}

// Enhanced Winline Power Control Capabilities (Task 12)
bool WinlineCanDevice::set_power_state_with_verification(bool on, uint8_t module_address) {
    EVLOG_info << "Winline: set_power_state_with_verification(" << on << ", 0x" << std::hex
               << static_cast<int>(module_address) << ")";

    // Use all modules if module_address is 0xFF (broadcast)
    std::vector<uint8_t> target_modules;
    if (module_address == 0xFF) {
        target_modules = active_module_addresses;
    } else {
        target_modules.push_back(module_address);
    }

    if (target_modules.empty()) {
        EVLOG_warning << "Winline: No target modules for power state setting";
        return false;
    }

    bool all_success = true;
    uint32_t power_value = on ? WinlineProtocol::POWER_ON : WinlineProtocol::POWER_OFF;

    // Send power commands to target modules
    for (const auto& addr : target_modules) {
        bool cmd_result = send_set_register_integer(addr, WinlineProtocol::Registers::POWER_CONTROL, power_value);

        if (cmd_result) {
            // Update power tracking
            auto it = telemetries.find(addr);
            if (it != telemetries.end()) {
                auto& tracking = it->second.power_tracking;
                tracking.expected_power_state = on;
                tracking.power_commands_sent++;
                tracking.last_power_command = std::chrono::steady_clock::now();
                tracking.power_state_verified = false; // Will be verified later

                EVLOG_debug << "Winline: Power command sent to module 0x" << std::hex << static_cast<int>(addr)
                            << " (command #" << tracking.power_commands_sent << ")";
            }
        } else {
            EVLOG_error << "Winline: Failed to send power command to module 0x" << std::hex << static_cast<int>(addr);
            all_success = false;
        }
    }

    // Schedule verification after a brief delay (power state changes take time)
    if (all_success) {
        EVLOG_info << "Winline: Power commands sent successfully. Will verify state in next status poll.";
    }

    return all_success;
}

bool WinlineCanDevice::verify_power_state(uint8_t module_address, bool expected_on_state) {
    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_warning << "Winline: Cannot verify power state for unknown module 0x" << std::hex
                      << static_cast<int>(module_address);
        return false;
    }

    auto& telemetry = it->second;
    auto& tracking = telemetry.power_tracking;
    const auto& status = telemetry.status;

    // Check if enough time has passed since the last power command
    auto now = std::chrono::steady_clock::now();
    auto time_since_command = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - tracking.last_power_command).count();

    // Give the module time to process the command (configurable grace period)
    if (time_since_command < power_state_grace_period_ms) {
        EVLOG_debug << "Winline: Power state verification skipped for module 0x" << std::hex
                    << static_cast<int>(module_address) << " - Grace period: "
                    << time_since_command << "ms < " << power_state_grace_period_ms << "ms";
        return true; // Don't fail verification during grace period
    }

    // Winline status bit 22: DCDC On/off status (0:On, 1:Off)
    bool module_is_on = !status.dcdc_on_off_status;
    tracking.actual_power_state = module_is_on;
    tracking.last_power_verification = now;

    bool state_matches = (module_is_on == expected_on_state);
    tracking.power_state_verified = state_matches;

    if (!state_matches) {
        tracking.power_state_mismatches++;
        EVLOG_warning << "Winline: Power state mismatch for module 0x" << std::hex << static_cast<int>(module_address)
                      << " - Expected: " << (expected_on_state ? "ON" : "OFF")
                      << ", Actual: " << (module_is_on ? "ON" : "OFF") << " (mismatch #"
                      << tracking.power_state_mismatches << ") - Time since command: "
                      << time_since_command << "ms";
    } else {
        EVLOG_debug << "Winline: Power state verified for module 0x" << std::hex << static_cast<int>(module_address)
                    << " - State: " << (module_is_on ? "ON" : "OFF") << " - Time since command: "
                    << time_since_command << "ms";
    }

    return state_matches;
}

bool WinlineCanDevice::handle_power_transition(bool target_state) {
    EVLOG_info << "Winline: handle_power_transition(" << (target_state ? "ON" : "OFF") << ")";

    // Optimize: Use group operations when appropriate
    bool result;
    {
        if (operating_mode == OperatingMode::GROUP_DISCOVERY && active_module_addresses.size() > 2) {
            EVLOG_info << "Winline: Using group power transition for " << active_module_addresses.size() << " modules";
            result = switch_on_off_group(target_state);
        } else {
            EVLOG_info << "Winline: Using individual power transition for " << active_module_addresses.size()
                       << " modules";
            result = switch_on_off(target_state);
        }
    }

    if (result) {
        // Track the transition for all active modules
        for (const auto& addr : active_module_addresses) {
            track_power_state_change(addr, target_state);
        }

        EVLOG_info << "Winline: Power transition to " << (target_state ? "ON" : "OFF") << " initiated successfully";
    } else {
        EVLOG_error << "Winline: Power transition to " << (target_state ? "ON" : "OFF") << " failed";
    }

    return result;
}

void WinlineCanDevice::track_power_state_change(uint8_t module_address, bool new_power_state) {
    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_debug << "Winline: Creating telemetry entry for module 0x" << std::hex
                    << static_cast<int>(module_address);
        // Create telemetry entry if it doesn't exist
        it = telemetries.emplace(module_address, Telemetry{}).first;
    }

    auto& tracking = it->second.power_tracking;
    bool state_changed = (tracking.expected_power_state != new_power_state);

    tracking.expected_power_state = new_power_state;
    tracking.power_commands_sent++;
    tracking.last_power_command = std::chrono::steady_clock::now();
    tracking.power_state_verified = false; // Will be verified on next status read

    if (state_changed) {
        EVLOG_info << "Winline: Power state change tracked for module 0x" << std::hex
                   << static_cast<int>(module_address) << " - New expected state: " << (new_power_state ? "ON" : "OFF")
                   << " (command #" << tracking.power_commands_sent << ")";
    }
}

bool WinlineCanDevice::send_command_impl(uint8_t destination_address, uint8_t command_number,
                                         const std::vector<uint8_t>& payload, bool group) {
    // Note: This old interface is kept for compatibility but should be replaced
    // For now, we'll adapt it to work with the new system
    EVLOG_warning << "Winline: Using deprecated send_command_impl interface";
    return false; // Disable old interface
}

// New Winline register-based command sending functions
bool WinlineCanDevice::send_read_register(uint8_t destination_address, uint16_t register_number, bool group) {
    uint8_t group_number = group ? group_address : 0;
    uint32_t can_id = can_packet_acdc::encode_can_id(controller_address, destination_address, group_number, !group);
    can_id |= WinlineProtocol::CAN_EXTENDED_FLAG; // Extended frame format

    std::vector<uint8_t> payload = can_packet_acdc::build_read_command(register_number);
    auto result = _tx(can_id, payload);
    if (!result) {
        EVLOG_warning << "Winline: CAN transmission failed for READ register 0x" << std::hex << register_number
                      << " to address 0x" << static_cast<int>(destination_address);
    }
    return result;
}

bool WinlineCanDevice::send_set_register_float(uint8_t destination_address, uint16_t register_number, float value,
                                               bool group) {
    uint8_t group_number = group ? group_address : 0;
    uint32_t can_id = can_packet_acdc::encode_can_id(controller_address, destination_address, group_number, !group);
    can_id |= WinlineProtocol::CAN_EXTENDED_FLAG; // Extended frame format

    std::vector<uint8_t> payload = can_packet_acdc::build_set_command_float(register_number, value);
    auto result = _tx(can_id, payload);
    if (!result) {
        EVLOG_warning << "Winline: CAN transmission failed for SET register 0x" << std::hex << register_number
                      << " (float=" << value << ") to address 0x" << static_cast<int>(destination_address);
    }
    return result;
}

bool WinlineCanDevice::send_set_register_integer(uint8_t destination_address, uint16_t register_number, uint32_t value,
                                                 bool group) {
    uint8_t group_number = group ? group_address : 0;
    uint32_t can_id = can_packet_acdc::encode_can_id(controller_address, destination_address, group_number, !group);
    can_id |= WinlineProtocol::CAN_EXTENDED_FLAG; // Extended frame format

    std::vector<uint8_t> payload = can_packet_acdc::build_set_command_integer(register_number, value);
    auto result = _tx(can_id, payload);
    if (!result) {
        EVLOG_warning << "Winline: CAN transmission failed for SET register 0x" << std::hex << register_number
                      << " (int=0x" << value << ") to address 0x" << static_cast<int>(destination_address);
    }
    return result;
}

void WinlineCanDevice::handle_module_count_packet(const std::vector<uint8_t>& payload) {
    // NOTE: This function is deprecated for Winline protocol
    // Winline uses register 0x0043 (GROUP_INFO) for module discovery instead
    EVLOG_warning << "Winline: handle_module_count_packet called - this is deprecated for Winline protocol";

    // For compatibility during transition, just mark as initialized
    if (!initialized) {
        initialized = true;
        EVLOG_info << "Winline: System initialized via legacy path";
        switch_on_off(false);
    }
}

void WinlineCanDevice::handle_simple_telemetry_update(uint8_t source_address, const std::vector<uint8_t>& payload,
                                                      uint8_t command_number) {
    // NOTE: This function is deprecated for Winline protocol
    // Winline uses register-based responses handled directly in rx_handler
    EVLOG_warning << "Winline: handle_simple_telemetry_update called - this is deprecated for Winline protocol";
}

void WinlineCanDevice::check_and_signal_error_status_change(uint8_t source_address,
                                                            const can_packet_acdc::PowerModuleStatus& new_status,
                                                            const can_packet_acdc::PowerModuleStatus& old_status) {
    // Helper lambda to reduce repetition in error status checking
    auto check_status_change = [this, source_address](bool new_val, bool old_val, Error error_type) {
        if (new_val != old_val) {
            signalError(source_address, error_type, new_val);
        }
    };

    // Enhanced Winline error handling with automatic recovery
    auto check_status_with_recovery = [this, source_address](bool new_val, bool old_val, Error error_type,
                                                             std::function<void()> recovery_action = nullptr) {
        if (new_val != old_val) {
            signalError(source_address, error_type, new_val);

            // Attempt automatic recovery for specific error types when they are activated
            if (new_val && recovery_action) {
                EVLOG_info << "Winline: Attempting automatic recovery for module 0x" << std::hex
                           << static_cast<int>(source_address);
                recovery_action();

                // Update recovery statistics (Task 11 enhancement)
                auto it = telemetries.find(source_address);
                if (it != telemetries.end()) {
                    it->second.status_history.recovery_count++;
                    it->second.status_history.last_recovery_time = std::chrono::steady_clock::now();
                    EVLOG_info << "Winline: Recovery attempt #" << it->second.status_history.recovery_count
                               << " for module 0x" << std::hex << static_cast<int>(source_address);
                }
            }
        }
    };

    // Check all error status changes using Winline status bit names with automatic recovery
    check_status_change(new_status.module_fault, old_status.module_fault, Error::InternalFault);
    check_status_change(new_status.dcdc_over_temperature, old_status.dcdc_over_temperature, Error::OverTemperature);

    // Overvoltage with automatic recovery
    check_status_with_recovery(new_status.dcdc_output_overvoltage, old_status.dcdc_output_overvoltage,
                               Error::OverVoltage,
                               [this, source_address]() { reset_overvoltage_protection(source_address); });

    check_status_change(new_status.fan_fault, old_status.fan_fault, Error::FanFault);
    check_status_change(new_status.can_communication_failure, old_status.can_communication_failure,
                        Error::CommunicationFault);
    check_status_change(new_status.ac_undervoltage, old_status.ac_undervoltage, Error::UnderVoltage);
    check_status_change(new_status.ac_overvoltage, old_status.ac_overvoltage, Error::OverVoltage);
    check_status_change(new_status.input_mode_error, old_status.input_mode_error, Error::InputVoltage);
    check_status_change(new_status.pfc_voltage_abnormal, old_status.pfc_voltage_abnormal, Error::InputVoltage);
    check_status_change(new_status.module_protection, old_status.module_protection, Error::InternalFault);
    check_status_change(new_status.module_current_imbalance, old_status.module_current_imbalance, Error::InternalFault);

    // Short circuit with automatic recovery
    check_status_with_recovery(new_status.dcdc_short_circuit, old_status.dcdc_short_circuit, Error::OverCurrent,
                               [this, source_address]() { reset_short_circuit_protection(source_address); });
}

void WinlineCanDevice::check_and_update_capabilities(uint8_t source_address) {
    auto it = telemetries.find(source_address);
    if (it == telemetries.end()) {
        return;
    }

    auto& telemetry = it->second;

    // Check if we have both power and current data to consider capabilities complete
    bool has_power = (telemetry.dc_rated_output_power > 0.0f);
    bool has_current = (telemetry.dc_max_output_current > 0.0f);

    // Mark capabilities as valid if we have both power and current data
    if (has_power && has_current && !telemetry.valid_caps) {
        telemetry.valid_caps = true;
        EVLOG_info << format_module_id(source_address) << ": Capabilities now complete - Power: "
                   << telemetry.dc_rated_output_power << "W, Current: " << telemetry.dc_max_output_current << "A";

        // Signal capabilities update when a module's capabilities become complete
        signalCapabilitiesUpdate(telemetries);
    }
}

std::string WinlineCanDevice::format_module_id(uint8_t address, const std::string& serial_number) const {
    std::stringstream ss;
    ss << "Winline[0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(address);
    if (!serial_number.empty()) {
        ss << "/" << serial_number;
    }
    ss << "]";
    return ss.str();
}
