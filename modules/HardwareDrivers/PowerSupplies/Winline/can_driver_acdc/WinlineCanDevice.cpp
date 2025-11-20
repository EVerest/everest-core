// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

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
            EVLOG_error << " Invalid module address '" << ad << "': " << e.what();
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
        EVLOG_info << " Starting group discovery on group " << group_address;
        send_read_register(WinlineProtocol::GROUP_BROADCAST_ADDR, WinlineProtocol::Registers::GROUP_INFO, true);
    } else {
        EVLOG_info << " Operating in FIXED_ADDRESS mode. No need to ping.";
        initialized = true;
        switch_on_off(false);
    }
}

void WinlineCanDevice::set_can_device(const std::string& dev) {
    can_device = dev;
    EVLOG_info << " Setting config values: CAN device: " << dev;
    open_device(can_device.c_str());
}

void WinlineCanDevice::set_config_values(const std::string& addrs, int group_addr, int timeout, int controller_address,
                                         int power_state_grace_period_ms, int altitude_setting_m,
                                         const std::string& input_mode, double module_current_limit_point) {
    this->device_connection_timeout_s = timeout;
    this->group_address = group_addr;
    this->controller_address = controller_address;
    this->power_state_grace_period_ms = power_state_grace_period_ms;
    this->altitude_setting_m = altitude_setting_m;
    this->input_mode = input_mode;
    this->module_current_limit_point = module_current_limit_point;

    EVLOG_info << " Operating with controller address: 0x" << std::hex << controller_address;
    EVLOG_info << " Altitude setting: " << altitude_setting_m << "m";
    EVLOG_info << " Input mode: " << input_mode;
    if (!addrs.empty()) {
        operating_mode = OperatingMode::FIXED_ADDRESS;
        configured_module_addresses = parse_module_addresses(addrs); // Store original configured addresses
        active_module_addresses = configured_module_addresses;       // Initialize active list with configured addresses
        expected_module_count = active_module_addresses.size();

        // Initialize telemetry entries for configured modules to prevent immediate removal
        // This fixes the chicken-and-egg problem where modules are removed before they can respond
        auto now = std::chrono::steady_clock::now();
        for (const auto& addr : active_module_addresses) {
            auto& telemetry = telemetries[addr];
            telemetry.last_update = now; // Initialize with current time
            EVLOG_debug << " Initialized telemetry for configured module 0x" << std::hex << static_cast<int>(addr);
        }

        EVLOG_info << " Operating in FIXED_ADDRESS mode with " << expected_module_count << " addresses: " << addrs;
    } else {
        operating_mode = OperatingMode::GROUP_DISCOVERY;
        EVLOG_info << " Operating in GROUP_DISCOVERY mode for group address: " << group_address;
    }
    EVLOG_info << " module communication timeout: " << device_connection_timeout_s << "s";
}

void WinlineCanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
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
        EVLOG_error << " Received malformed CAN frame with size " << payload.size()
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

            EVLOG_warning << " " << error_description << " response from module 0x" << std::hex
                          << static_cast<int>(source_address) << " for register 0x" << register_number
                          << " (error_code=0x" << static_cast<int>(error_code) << ")";

            // Per Winline protocol: "F0: Normal, Others: Fault, discard frame"
            return false;
        }

        if (data_type != expected_data_type) {
            uint16_t register_number = (static_cast<uint16_t>(payload[2]) << 8) | payload[3];
            EVLOG_warning << " Invalid data type from module 0x" << std::hex << static_cast<int>(source_address)
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
        EVLOG_warning << " Received error response from module 0x" << std::hex << static_cast<int>(source_address)
                      << " for register 0x" << register_number << " (error=0x" << static_cast<int>(error_code) << ")";
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
            //
            // Strangely the rated output current is not the max output current, it is the basis calculation for the max
            // output current The max output current is the rated output current * module_current_limit_point
            //
            telemetry.dc_max_output_current = current_reading.current * module_current_limit_point;
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

            // Enhanced power control verification
            auto& power_tracking = telemetry.power_tracking;
            if (power_tracking.power_commands_sent > 0 && !power_tracking.power_state_verified) {
                bool verification_result = verify_power_state(source_address, power_tracking.expected_power_state);
                if (verification_result) {
                    EVLOG_debug << " Power state verification successful for module 0x" << std::hex
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
                    // Add to configured_module_addresses (persistent discovery list)
                    if (std::find(configured_module_addresses.begin(), configured_module_addresses.end(),
                                  source_address) == configured_module_addresses.end()) {
                        configured_module_addresses.push_back(source_address);
                        EVLOG_info << " Added discovered module 0x" << std::hex << static_cast<int>(source_address)
                                   << " to configured list (group " << static_cast<int>(group_info.group_number) << ")";
                    }

                    // Also add to active_module_addresses for online tracking
                    if (std::find(active_module_addresses.begin(), active_module_addresses.end(), source_address) ==
                        active_module_addresses.end()) {
                        active_module_addresses.push_back(source_address);
                        EVLOG_info << " Discovered new module at address 0x" << std::hex
                                   << static_cast<int>(source_address) << " in group "
                                   << static_cast<int>(group_info.group_number);

                        // Configure newly discovered module
                        set_altitude_all_modules();
                        set_current_limit_point_all_modules();
                    }
                } else {
                    EVLOG_debug << " Ignoring module at address 0x" << std::hex << static_cast<int>(source_address)
                                << " (belongs to group " << static_cast<int>(group_info.group_number)
                                << ", expected group " << group_address << ")";
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

    // SET operation responses (confirmation of settings)
    case WinlineProtocol::Registers::SET_OUTPUT_VOLTAGE:
    case WinlineProtocol::Registers::SET_OUTPUT_CURRENT:
    case WinlineProtocol::Registers::SET_ALTITUDE:
    case WinlineProtocol::Registers::SET_INPUT_MODE: {
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
        // Enhanced power control response handling
        auto& telemetry = telemetries[source_address];
        auto& power_tracking = telemetry.power_tracking;

        if (error_code == WinlineProtocol::ERROR_NORMAL) {
            EVLOG_info << format_module_id(source_address) << ": Power control command confirmed - "
                       << (power_tracking.expected_power_state ? "ON" : "OFF");

            // Power command was accepted, but we still need to verify actual state change via status
            EVLOG_debug << " Power control response received. Will verify actual state via status register.";
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
        EVLOG_debug << " Unhandled register response 0x" << std::hex << register_number << " from module 0x"
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

            // In FIXED_ADDRESS mode, ensure all responding modules are in active list
            if (operating_mode == OperatingMode::FIXED_ADDRESS) {
                bool modules_re_added = false;
                for (const auto& [addr, telemetry] : telemetries) {
                    if (std::find(active_module_addresses.begin(), active_module_addresses.end(), addr) ==
                        active_module_addresses.end()) {
                        active_module_addresses.push_back(addr);
                        EVLOG_info << " Re-added module 0x" << std::hex << static_cast<int>(addr)
                                   << " to active list during cleanup";
                        modules_re_added = true;
                    }
                }

                // Signal capabilities update if modules were re-added
                if (modules_re_added) {
                    signalCapabilitiesUpdate(telemetries);

                    // Configure reconnected modules
                    set_altitude_all_modules();
                    set_current_limit_point_all_modules();
                }
            }
        }
    }

    return removed_count;
}

void WinlineCanDevice::poll_status_handler() {
    // Remove expired telemetry entries
    size_t removed_count = remove_expired_telemetry_entries();

    if (removed_count > 0) {
        EVLOG_info << " Removed " << removed_count << " expired modules. "
                   << "Active modules remaining: " << active_module_addresses.size();
        // signal the telemetry updates
        signalCapabilitiesUpdate(telemetries);
        signalVoltageCurrent(telemetries);
    }

    // --- Telemetry Polling ---
    // Poll ALL configured modules (not just active ones) to allow offline modules to recover.
    // This enables automatic recovery when temporarily offline modules come back online.

    // Unified polling approach for both modes
    if (operating_mode == OperatingMode::GROUP_DISCOVERY) {
        // First, try to discover new modules
        discover_group_modules();
    }

    // Poll ALL configured modules (not just active ones) to allow offline modules to recover.
    // This enables automatic recovery when temporarily offline modules come back online.
    for (const auto& addr : configured_module_addresses) {
        // Send basic telemetry requests to check if module is responding
        send_read_register(addr, WinlineProtocol::Registers::VOLTAGE);
        send_read_register(addr, WinlineProtocol::Registers::CURRENT);

        // If module responds and is not in active list, re-add it
        if (telemetries.find(addr) != telemetries.end() &&
            std::find(active_module_addresses.begin(), active_module_addresses.end(), addr) ==
                active_module_addresses.end()) {
            active_module_addresses.push_back(addr);
            EVLOG_info << " Module 0x" << std::hex << static_cast<int>(addr)
                       << " reconnected and added back to active list";

            // Signal capabilities update when module is re-added
            signalCapabilitiesUpdate(telemetries);

            // Configure reconnected module
            set_altitude_all_modules();
            set_current_limit_point_all_modules();
        }
    }

    // Poll active modules for detailed telemetry
    for (const auto& addr : active_module_addresses) {
        // Read essential telemetry using Winline registers
        send_read_register(addr, WinlineProtocol::Registers::VOLTAGE); // Read voltage
        send_read_register(addr, WinlineProtocol::Registers::CURRENT); // Read current

        // Enhanced status monitoring - use comprehensive status check
        perform_comprehensive_status_check(addr);

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
    EVLOG_info << " switch_on_off(" << on << ") - active modules: " << active_module_addresses.size();

    if (active_module_addresses.empty()) {
        EVLOG_warning << " No active modules to send switch_on_off command to.";
        return false;
    }

    // Use individual module commands (unified approach for both modes)
    EVLOG_info << " Using individual module commands with enhanced power tracking";
    uint32_t power_value = on ? WinlineProtocol::POWER_ON : WinlineProtocol::POWER_OFF;
    bool success = true;

    for (const auto& addr : active_module_addresses) {
        bool module_success = send_set_register_integer(addr, WinlineProtocol::Registers::POWER_CONTROL, power_value);
        if (!module_success) {
            EVLOG_warning << " Failed to send power control to module 0x" << std::hex << static_cast<int>(addr);
            success = false;
        } else {
            // Track power state change for this module
            track_power_state_change(addr, on);
        }
    }

    if (success) {
        EVLOG_info << " Power commands sent successfully. Power state verification will occur in next status poll.";
    }

    return success;
}

bool WinlineCanDevice::set_voltage_current(float voltage, float current) {
    EVLOG_info << " set_voltage_current(" << voltage << "V, " << current
               << "A) - active modules: " << active_module_addresses.size();

    // Validate that we have active modules before attempting to divide current
    const size_t module_count = active_module_addresses.size();
    if (module_count == 0) {
        EVLOG_warning << " No active modules to set voltage/current.";
        return false;
    }

    // Use individual module commands (unified approach for both modes)
    EVLOG_info << " Using individual module commands";
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

bool WinlineCanDevice::discover_group_modules() {
    EVLOG_info << " discover_group_modules() - querying group " << group_address;

    // Send group discovery command using register 0x0043 (GROUP_INFO)
    bool result =
        send_read_register(WinlineProtocol::GROUP_BROADCAST_ADDR, WinlineProtocol::Registers::GROUP_INFO, true);

    if (!result) {
        EVLOG_warning << " Group discovery command failed";
    } else {
        EVLOG_info << " Group discovery command sent successfully (group " << group_address << ")";
    }

    return result;
}

// Winline error recovery operations
bool WinlineCanDevice::reset_overvoltage_protection(uint8_t module_address) {
    EVLOG_info << " reset_overvoltage_protection(0x" << std::hex << static_cast<int>(module_address) << ")";

    bool result = send_set_register_integer(module_address, WinlineProtocol::Registers::SET_OVERVOLTAGE_RESET,
                                            WinlineProtocol::RESET_ENABLE);

    if (!result) {
        EVLOG_warning << " Overvoltage reset command failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << " Overvoltage reset command sent successfully to module 0x" << std::hex
                   << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::reset_short_circuit_protection(uint8_t module_address) {
    EVLOG_info << " reset_short_circuit_protection(0x" << std::hex << static_cast<int>(module_address) << ")";

    bool result = send_set_register_integer(module_address, WinlineProtocol::Registers::SET_SHORT_CIRCUIT_RESET,
                                            WinlineProtocol::RESET_ENABLE);

    if (!result) {
        EVLOG_warning << " Short circuit reset command failed for module 0x" << std::hex
                      << static_cast<int>(module_address);
    } else {
        EVLOG_info << " Short circuit reset command sent successfully to module 0x" << std::hex
                   << static_cast<int>(module_address);
    }

    return result;
}

bool WinlineCanDevice::set_altitude_all_modules() {
    EVLOG_info << " Setting altitude to " << altitude_setting_m << "m on all modules";

    // Validate altitude setting
    if (altitude_setting_m < WinlineProtocol::ALTITUDE_MIN || altitude_setting_m > WinlineProtocol::ALTITUDE_MAX) {
        EVLOG_error << " Invalid altitude setting " << altitude_setting_m
                    << "m. Valid range: " << WinlineProtocol::ALTITUDE_MIN << "-" << WinlineProtocol::ALTITUDE_MAX
                    << "m";
        return false;
    }

    bool all_success = true;

    // Send to each configured module individually (unified approach for both modes)
    for (const auto& addr : configured_module_addresses) {
        EVLOG_info << " Setting altitude on module 0x" << std::hex << static_cast<int>(addr);
        bool result =
            send_set_register_integer(addr, WinlineProtocol::Registers::SET_ALTITUDE, altitude_setting_m, false);
        if (!result) {
            EVLOG_warning << " Failed to send altitude setting to module 0x" << std::hex << static_cast<int>(addr);
            all_success = false;
        } else {
            EVLOG_info << " Altitude setting sent successfully to module 0x" << std::hex << static_cast<int>(addr);
        }
    }

    if (all_success) {
        EVLOG_info << " Altitude setting " << altitude_setting_m << "m sent to all modules successfully";
    } else {
        EVLOG_warning << " Some altitude setting commands failed";
    }

    return all_success;
}

bool WinlineCanDevice::set_current_limit_point_all_modules() {
    EVLOG_info << " Setting current limit point to " << module_current_limit_point << " on all modules";

    bool all_success = true;

    // Send to each configured module individually (unified approach for both modes)
    for (const auto& addr : configured_module_addresses) {
        EVLOG_info << " Setting current limit point on module 0x" << std::hex << static_cast<int>(addr);
        bool result = send_set_register_integer(addr, WinlineProtocol::Registers::SET_CURRENT_LIMIT_POINT,
                                                module_current_limit_point, false);
        if (!result) {
            EVLOG_warning << " Failed to send current limit point setting to module 0x" << std::hex
                          << static_cast<int>(addr);
            all_success = false;
        } else {
            EVLOG_info << " Current limit point setting sent successfully to module 0x" << std::hex
                       << static_cast<int>(addr);
        }
    }

    if (all_success) {
        EVLOG_info << " Current limit point setting " << module_current_limit_point
                   << " sent to all modules successfully";
    } else {
        EVLOG_warning << " Some current limit point setting commands failed";
    }

    return all_success;
}

bool WinlineCanDevice::set_input_mode_all_modules() {
    EVLOG_info << " Setting input mode to " << input_mode << " on all modules";

    // Convert input mode string to Winline protocol value
    uint32_t input_mode_value;
    if (input_mode == "AC") {
        input_mode_value = 1; // AC mode
    } else if (input_mode == "DC") {
        input_mode_value = 2; // DC mode
    } else {
        EVLOG_error << " Invalid input mode '" << input_mode << "'. Valid values: AC, DC";
        return false;
    }

    bool all_success = true;

    // Send to each configured module individually (unified approach for both modes)
    for (const auto& addr : configured_module_addresses) {
        EVLOG_info << " Setting input mode on module 0x" << std::hex << static_cast<int>(addr);
        bool result =
            send_set_register_integer(addr, WinlineProtocol::Registers::SET_INPUT_MODE, input_mode_value, false);
        if (!result) {
            EVLOG_warning << " Failed to send input mode setting to module 0x" << std::hex << static_cast<int>(addr);
            all_success = false;
        } else {
            EVLOG_info << " Input mode setting sent successfully to module 0x" << std::hex << static_cast<int>(addr);
        }
    }

    if (all_success) {
        EVLOG_info << " Input mode setting " << input_mode << " sent to all modules successfully";
    } else {
        EVLOG_warning << " Some input mode setting commands failed";
    }

    return all_success;
}

// Enhanced Winline Status Monitoring Capabilities
bool WinlineCanDevice::perform_comprehensive_status_check(uint8_t module_address) {
    EVLOG_debug << " perform_comprehensive_status_check(0x" << std::hex << static_cast<int>(module_address) << ")";

    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_warning << " Cannot perform status check for unknown module 0x" << std::hex
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

        EVLOG_warning << " Comprehensive status check failed for module 0x" << std::hex
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

    EVLOG_debug << " Comprehensive status check completed for module 0x" << std::hex << static_cast<int>(module_address)
                << " (success rate: " << telemetry.status_metrics.status_read_success_rate << "%)";

    return true;
}

bool WinlineCanDevice::analyze_status_trends(uint8_t module_address) {
    EVLOG_debug << " analyze_status_trends(0x" << std::hex << static_cast<int>(module_address) << ")";

    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_warning << " Cannot analyze trends for unknown module 0x" << std::hex << static_cast<int>(module_address);
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
            EVLOG_warning << " Persistent fault detected in module 0x" << std::hex << static_cast<int>(module_address)
                          << " (fault present in last 3 status readings)";
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
            EVLOG_warning << " Frequent temperature derating detected in module 0x" << std::hex
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
            EVLOG_info << " Power limiting pattern detected in module 0x" << std::hex
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

        EVLOG_info << " Module 0x" << std::hex << static_cast<int>(module_address)
                   << " fault count: " << history.fault_count;
    }

    return true;
}

void WinlineCanDevice::log_status_diagnostics(uint8_t module_address,
                                              const can_packet_acdc::PowerModuleStatus& status) {
    std::stringstream diagnostics;
    diagnostics << " Status diagnostics for module 0x" << std::hex << static_cast<int>(module_address) << ": ";

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

    // Enhanced power control statistics
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

bool WinlineCanDevice::verify_power_state(uint8_t module_address, bool expected_on_state) {
    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_warning << " Cannot verify power state for unknown module 0x" << std::hex
                      << static_cast<int>(module_address);
        return false;
    }

    auto& telemetry = it->second;
    auto& tracking = telemetry.power_tracking;
    const auto& status = telemetry.status;

    // Check if enough time has passed since the last power command
    auto now = std::chrono::steady_clock::now();
    auto time_since_command =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - tracking.last_power_command).count();

    // Give the module time to process the command (configurable grace period)
    if (time_since_command < power_state_grace_period_ms) {
        EVLOG_debug << " Power state verification skipped for module 0x" << std::hex << static_cast<int>(module_address)
                    << " - Grace period: " << time_since_command << "ms < " << power_state_grace_period_ms << "ms";
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
        EVLOG_warning << " Power state mismatch for module 0x" << std::hex << static_cast<int>(module_address)
                      << " - Expected: " << (expected_on_state ? "ON" : "OFF")
                      << ", Actual: " << (module_is_on ? "ON" : "OFF") << " (mismatch #"
                      << tracking.power_state_mismatches << ") - Time since command: " << time_since_command << "ms";
    } else {
        EVLOG_debug << " Power state verified for module 0x" << std::hex << static_cast<int>(module_address)
                    << " - State: " << (module_is_on ? "ON" : "OFF") << " - Time since command: " << time_since_command
                    << "ms";
    }

    return state_matches;
}

bool WinlineCanDevice::handle_power_transition(bool target_state) {
    EVLOG_info << " handle_power_transition(" << (target_state ? "ON" : "OFF") << ")";

    // Optimize: Use group operations when appropriate
    bool result;
    EVLOG_info << " Using individual power transition for " << active_module_addresses.size() << " modules";
    result = switch_on_off(target_state);

    if (result) {
        // Track the transition for all active modules
        for (const auto& addr : active_module_addresses) {
            track_power_state_change(addr, target_state);
        }

        EVLOG_info << " Power transition to " << (target_state ? "ON" : "OFF") << " initiated successfully";
    } else {
        EVLOG_error << " Power transition to " << (target_state ? "ON" : "OFF") << " failed";
    }

    return result;
}

void WinlineCanDevice::track_power_state_change(uint8_t module_address, bool new_power_state) {
    auto it = telemetries.find(module_address);
    if (it == telemetries.end()) {
        EVLOG_debug << " Creating telemetry entry for module 0x" << std::hex << static_cast<int>(module_address);
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
        EVLOG_info << " Power state change tracked for module 0x" << std::hex << static_cast<int>(module_address)
                   << " - New expected state: " << (new_power_state ? "ON" : "OFF") << " (command #"
                   << tracking.power_commands_sent << ")";
    }
}

bool WinlineCanDevice::send_command_impl(uint8_t destination_address, uint8_t command_number,
                                         const std::vector<uint8_t>& payload, bool group) {
    // Note: This old interface is kept for compatibility but should be replaced
    // For now, we'll adapt it to work with the new system
    EVLOG_warning << " Using deprecated send_command_impl interface";
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
        EVLOG_warning << " CAN transmission failed for READ register 0x" << std::hex << register_number
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
        EVLOG_warning << " CAN transmission failed for SET register 0x" << std::hex << register_number
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
        EVLOG_warning << " CAN transmission failed for SET register 0x" << std::hex << register_number << " (int=0x"
                      << value << ") to address 0x" << static_cast<int>(destination_address);
    }
    return result;
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
                EVLOG_info << " Attempting automatic recovery for module 0x" << std::hex
                           << static_cast<int>(source_address);
                recovery_action();

                // Update recovery statistics
                auto it = telemetries.find(source_address);
                if (it != telemetries.end()) {
                    it->second.status_history.recovery_count++;
                    it->second.status_history.last_recovery_time = std::chrono::steady_clock::now();
                    EVLOG_info << " Recovery attempt #" << it->second.status_history.recovery_count << " for module 0x"
                               << std::hex << static_cast<int>(source_address);
                }
            }
        }
    };

    // Check all error status changes using Winline status bit names with automatic recovery
    check_status_change(new_status.module_fault, old_status.module_fault, Error::VendorError);
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
    check_status_change(new_status.input_mode_error, old_status.input_mode_error, Error::VendorError);
    check_status_change(new_status.pfc_voltage_abnormal, old_status.pfc_voltage_abnormal, Error::VendorError);
    check_status_change(new_status.module_protection, old_status.module_protection, Error::VendorError);
    check_status_change(new_status.module_current_imbalance, old_status.module_current_imbalance, Error::VendorWarning);

    // Short circuit with automatic recovery
    check_status_with_recovery(new_status.dcdc_short_circuit, old_status.dcdc_short_circuit, Error::OverCurrent,
                               [this, source_address]() { reset_short_circuit_protection(source_address); });

    // Additional status bits that were missing
    check_status_change(new_status.sci_communication_failure, old_status.sci_communication_failure, Error::VendorError);
    check_status_change(new_status.input_mode_mismatch, old_status.input_mode_mismatch, Error::VendorError);
    check_status_change(new_status.dcdc_overvoltage, old_status.dcdc_overvoltage, Error::OverVoltage);
    check_status_change(new_status.temperature_derating, old_status.temperature_derating, Error::VendorWarning);
    check_status_change(new_status.module_power_limiting, old_status.module_power_limiting, Error::InternalFault);
    check_status_change(new_status.ac_power_limiting, old_status.ac_power_limiting, Error::VendorWarning);
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
        EVLOG_info << format_module_id(source_address)
                   << ": Capabilities now complete - Power: " << telemetry.dc_rated_output_power
                   << "W, Current: " << telemetry.dc_max_output_current << "A";

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
