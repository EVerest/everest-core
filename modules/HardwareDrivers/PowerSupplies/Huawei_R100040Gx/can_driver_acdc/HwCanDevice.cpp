// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "HwCanDevice.hpp"
#include "CanPackets.hpp"
#include <algorithm>
#include <iostream>
#include <unistd.h>

#include <fmt/core.h>

HwCanDevice::~HwCanDevice() {
    exit_tx_thread = true;
}

bool HwCanDevice::switch_on(bool on) {
    // actual switching on will be handled in tx thread
    requested_on = on;
    return true;
}

bool HwCanDevice::switch_on_nolock(bool on) {
    // Power On/Off Packet
    Huawei::Packet power_on_off(Huawei::MessageId::ControlCommand, Huawei::SignalId::PowerOnOff);
    power_on_off.byte3 = not on;
    send_to_all_modules(power_on_off);
    is_on = on;
    return true;
}

bool HwCanDevice::set_voltage_current(float voltage, float current) {
    requested_set_point_voltage = voltage;
    requested_set_point_current = current;
    return true;
}

bool HwCanDevice::set_voltage_current_nolock(float voltage, float current) {
    Huawei::Packet set_voltage(Huawei::MessageId::ControlCommand, Huawei::SignalId::OutputVoltage);
    set_voltage.data = voltage * 1024;
    send_to_all_modules(set_voltage);

    Huawei::Packet set_current(Huawei::MessageId::ControlCommand, Huawei::SignalId::OutputCurrent);
    set_current.data = current * 1024 / module_addresses.size();
    set_current.byte3 = module_addresses.size();
    send_to_all_modules(set_current);

    set_point_voltage = voltage;
    set_point_current = current;
    return true;
}

static std::string to_serial_number(uint8_t b2, uint8_t b3, uint8_t data) {
    return fmt::format("{}:{}:{}", b2, b3, data);
}

static std::string to_version_string(uint8_t b2, uint8_t b3, uint32_t data) {
    uint16_t hw_version = b2 << 8 | b3;

    int dcdc_versoion_hi = data >> 24;
    int dcdc_versoion_lo = data >> 16 & 0xFF;

    int pfc_versoion_hi = data >> 8 & 0xFF;
    int pfc_versoion_lo = data & 0xFF;
    return fmt::format("HW Version: {} | DCDC: {}.{} | PFC: {}.{}", hw_version, dcdc_versoion_hi, dcdc_versoion_lo,
                       pfc_versoion_hi, pfc_versoion_lo);
}

static bool address_already_in_list(std::vector<uint8_t>& v, uint8_t nv) {
    auto it = std::find(v.begin(), v.end(), nv);
    if (it == v.end()) {
        return false;
    }
    return true;
}

void HwCanDevice::connection_established() {
    switch_on(false);
    request_module_info();
}

void HwCanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
    // We only use extended frames here
    if (!(can_id & CAN_EFF_FLAG)) {
        return;
    }

    // parse packet
    auto packet = Huawei::Packet(can_id, payload);

    if (packet.packet_source_control_unit or packet.protocol_id not_eq 0x0D) {
        return;
    }

    if (packet.error_type not_eq Huawei::ErrorType::Success) {
        std::cout << "Error in CAN packet: " << packet << std::endl;
        return;
    }

    // We received a packet from the PSU, reset timeout timer
    last_communication_received = std::chrono::steady_clock::now();

    uint8_t source_address = packet.module_address;
    bool packet_handled = true;

    if (packet.message_id == Huawei::MessageId::QueryInherentModuleInformation) {
        switch (packet.signal_id) {
        case Huawei::SignalId::SerialNumber:
            if (module_auto_detection) {
                module_serial_numbers[packet.module_address] =
                    "[auto detected] " + to_serial_number(packet.byte2, packet.byte3, packet.data);

                if (not address_already_in_list(module_addresses, source_address)) {
                    // Found a new module? Add it to the list
                    module_addresses.push_back(source_address);
                    // Update capabilities
                    auto caps = get_capabilities();
                    signal_capabilities(caps);
                }

            } else {
                module_serial_numbers[packet.module_address] =
                    to_serial_number(packet.byte2, packet.byte3, packet.data);
            }

            break;

        case Huawei::SignalId::SwHwVersion:
            if (not address_already_in_list(module_addresses_reported, source_address)) {
                signal_serial_number(packet.module_address,
                                     module_serial_numbers[packet.module_address] + " | " +
                                         to_version_string(packet.byte2, packet.byte3, packet.data));
                module_addresses_reported.push_back(source_address);
            }
            break;

        default:
            // Ignore all other Inherent Module Information packages
            packet_handled = true;
            break;
        }
    } else if (packet.message_id == Huawei::MessageId::QueryAllRealtimeData) {
        switch (packet.signal_id) {
        case Huawei::SignalId::OutputVoltageCurrentStatus: {
            const float v = (packet.data & 0x0000FFFF) / 10.;
            const float c = ((packet.data & 0xFFFF0000) >> 16) / 10.;

            // report sum of all currents
            telemetries[source_address].current = c;

            // calculate total current
            total_current = 0.;
            for (const auto& t : telemetries) {
                total_current += t.second.current;
            }

            telemetries[source_address].voltage = v;
            // report average voltage
            float voltage = 0.;
            for (const auto& t : telemetries) {
                voltage += t.second.voltage;
            }
            voltage /= telemetries.size();

            signal_voltage_current(voltage, total_current);
        } break;
        default:
            packet_handled = false;
            break;
        }
    } else if (packet.message_id == Huawei::MessageId::ConfigurationCommand) {
        // Ignore all config command replies for now
        packet_handled = true;
    } else if (packet.message_id == Huawei::MessageId::ControlCommand) {
        // Ignore all control command replies for now
        packet_handled = true;
    } else {
        packet_handled = false;
    }

    if (not packet_handled) {
        std::cout << "UNHANDLED Packet received: " << packet << std::endl;
    }
}

types::power_supply_DC::Capabilities HwCanDevice::get_capabilities() {

    // IMPROVE ME: this could be queried from the power supply instead
    types::power_supply_DC::Capabilities caps;
    caps.bidirectional = false;

    caps.current_regulation_tolerance_A = 1;
    caps.peak_current_ripple_A = 0.5;

    caps.min_export_current_A = 0;
    caps.max_export_current_A = 133 * module_addresses.size();
    caps.min_export_voltage_V = 150;
    caps.max_export_voltage_V = 1000;
    caps.max_export_power_W = 40000 * module_addresses.size();

    caps.max_import_current_A = 0;
    caps.min_import_current_A = 0;
    caps.max_import_power_W = 0;
    caps.min_import_voltage_V = 0;
    caps.max_import_voltage_V = 0;

    return caps;
}

void HwCanDevice::request_module_info() {
    // Request information about modules once
    send_to_broadcast({Huawei::MessageId::QueryInherentModuleInformation, Huawei::SignalId::BatchQuery});
}

void HwCanDevice::set_mode() {
    // Set mode to automatic switching
    Huawei::Packet mode(Huawei::MessageId::ConfigurationCommand, Huawei::SignalId::AutomaticOutputModeSwitch);
    mode.byte3 = 0x01; // switch according to actual voltage on the terminals
    send_to_all_modules(mode);
}

void HwCanDevice::tx_thread() {

    Huawei::Packet query_rt_data(Huawei::MessageId::QueryAllRealtimeData, Huawei::SignalId::BatchQuery);
    query_rt_data.byte3 = 0xAA; // Enable bit configuration of individual messages
    query_rt_data.data = Huawei::RTQ_OUTPUT_VOLTAGE_CURRENT_STATUS;

    uint8_t telemetry_cnt = 0;

    bool comm_timeout_err = false;
    bool comm_bus_err = false;

    bool last_err_present = false;

    set_mode();

    while (!exit_tx_thread) {
        {
            // Is there a communication problem in the CAN stack?
            comm_bus_err = not is_running();

            // Did we receive something from the PSU within timeout?
            // I.e. the CAN stack is working but the hardware does not send anything

            comm_timeout_err = std::chrono::steady_clock::now() - last_communication_received > COMMS_TIMEOUT;

            bool err_present = comm_bus_err or comm_timeout_err;

            if (err_present not_eq last_err_present) {
                signal_communication_error(err_present,
                                           (comm_bus_err ? "CAN Bus error: No such device or wrong settings"
                                                         : "Power supply does not respond on CAN bus"));
            }
            last_err_present = err_present;

            // Request all real time data (telemetry) as configured above
            if (telemetry_cnt++ % 5 == 0) {
                send_to_all_modules(query_rt_data);
            }

            // Configure mode roughly every second
            if (not is_on and telemetry_cnt++ % 20 == 0) {
                set_mode();
            }

            // Read serial numbers every few seconds
            if (telemetry_cnt++ % 40 == 0) {
                request_module_info();
            }

            // Do we need to switch on or off?
            if (last_requested_on not_eq requested_on) {
                last_requested_on = requested_on;
                switch_on_nolock(requested_on);
                signal_on_off(requested_on);
                set_voltage_current_nolock(requested_set_point_voltage, requested_set_point_current);
            }

            // Do we need to set voltage/current limits?
            if (requested_set_point_voltage not_eq set_point_voltage or
                requested_set_point_current not_eq set_point_current) {
                set_voltage_current_nolock(requested_set_point_voltage, requested_set_point_current);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

bool HwCanDevice::tx(Huawei::Packet& packet) {
    uint32_t can_id = packet.get_can_id();
    can_id |= 0x80000000U; // Extended frame format
    return _tx(can_id, packet);
}

std::ostream& operator<<(std::ostream& out, const HwCanDevice::Telemetry& self) {
    out << "DC output: " << std::to_string(self.voltage) << "V / " << std::to_string(self.current) << "A" << std::endl;
    return out;
}
