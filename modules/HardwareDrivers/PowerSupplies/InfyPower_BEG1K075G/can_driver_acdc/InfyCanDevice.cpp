// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "InfyCanDevice.hpp"
#include "CanPackets.hpp"
#include <iostream>
#include <unistd.h>

InfyCanDevice::InfyCanDevice() {
    // spawn thread that requests some data periodically to keep the connection alive
    exitTxThread = false;
    txThreadHandle = std::thread(&InfyCanDevice::txThread, this);
}

InfyCanDevice::~InfyCanDevice() {
    exitTxThread = true;
}

void InfyCanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
    // std::cout << "Infy: CAN frame received. ID: 0x" << std::hex << can_id << std::endl;

    // We only use extended frames here
    if (!(can_id & CAN_EFF_FLAG)) {
        // std::cout << "Infy: Ignoring, not extended protocol." << std::endl;
        return;
    }

    if (can_packet_acdc::command_number_from_can_id(can_id) == can_packet_acdc::CMD_WRITE) {
        // std::cout << "Infy: Write reply, command number: " << std::hex
        //           << (uint16_t)can_packet_acdc::command_number_from_can_id(can_id) << std::endl;
        switch (can_packet_acdc::error_code_from_can_id(can_id)) {
        case 0x02:
            std::cout << "Infy: ERROR: Command invalid." << std::endl;
            break;
        case 0x03:
            std::cout << "Infy: ERROR: Data invalid." << std::endl;
            break;
        case 0x07:
            std::cout << "Infy: ERROR: In start processing." << std::endl;
            break;
        }
        return;
    }

    // is it a reply to a read command?
    if (can_packet_acdc::command_number_from_can_id(can_id) != can_packet_acdc::CMD_READ) {
        // std::cout << "Infy: Not a reply to a read command, command number: " << std::hex
        //           << (uint16_t)can_packet_acdc::command_number_from_can_id(can_id) << std::endl;
        return;
    }

    // is it for us?
    if (can_packet_acdc::destination_address_from_can_id(can_id) != 0xF0) {
        // std::cout << "Infy: Not for us: " << std::hex
        //           << (uint16_t)can_packet_acdc::destination_address_from_can_id(can_id) << std::endl;
        return;
    }

    uint16_t packet_type = payload[0] << 8 | payload[1];

    if (can_packet_acdc::error_code_from_can_id(can_id) > 0) {
        std::cout << "Infy: Parsing CAN packet type: " << std::hex << packet_type << " Error code:" << std::hex
                  << (int)can_packet_acdc::error_code_from_can_id(can_id) << std::endl;
    }

    switch (packet_type) {
    case 0x1001: {
        telemetry.voltage = payload;
        // std::cout << telemetry.voltage << std::endl;
    } break;

    case 0x1002: {
        telemetry.current = payload;
        // std::cout << telemetry.current << std::endl;
        signalVoltageCurrent(telemetry.voltage.volt, telemetry.current.ampere);
    } break;

    case 0x1010: {
        can_packet_acdc::PowerModuleNumber n(payload);
        // std::cout << n << std::endl;
    } break;

    case 0x1110: {
        can_packet_acdc::PowerModuleStatus s(payload);
        telemetry.status = s;
        // std::cout << s << std::endl;
    } break;

    case 0x1111: {
        can_packet_acdc::InverterStatus s(payload);
        signalModuleStatus(telemetry.status, s);
        // std::cout << s << std::endl;
    } break;

    case 0x1103: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_ab_line_voltage = s.value / 1000.;
    } break;

    case 0x1104: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_bc_line_voltage = s.value / 1000.;
    } break;

    case 0x1105: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_ca_line_voltage = s.value / 1000.;
    } break;

    case 0x1106: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ambient_temperature = s.value / 1000.;
    } break;

    case 0x1130: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.dc_max_output_voltage = s.value / 1000.;
    } break;

    case 0x1131: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.dc_min_output_voltage = s.value / 1000.;
    } break;

    case 0x1132: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.dc_max_output_current = s.value / 1000.;
    } break;

    case 0x1133: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.dc_rated_output_power = s.value / 1000.;
    } break;

    case 0x2101: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_a_voltage = s.value / 1000.;
    } break;

    case 0x2102: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_b_voltage = s.value / 1000.;
    } break;

    case 0x2103: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_c_voltage = s.value / 1000.;
    } break;

    case 0x2104: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_a_current = s.value / 1000.;
    } break;

    case 0x2105: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_b_current = s.value / 1000.;
    } break;

    case 0x2106: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_c_current = s.value / 1000.;
    } break;

    case 0x2107: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_frequency = s.value / 1000.;
    } break;

    case 0x2109: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_a_active_power = s.value / 1000.;
    } break;

    case 0x210A: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_b_active_power = s.value / 1000.;
    } break;

    case 0x210B: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_c_active_power = s.value / 1000.;
    } break;

    case 0x2108: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_total_active_ower = s.value / 1000.;
    } break;

    case 0x210D: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_a_reactive_power = s.value / 1000.;
    } break;

    case 0x210E: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_b_reactive_power = s.value / 1000.;
    } break;

    case 0x210F: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_c_reactive_power = s.value / 1000.;
    } break;

    case 0x210C: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_total_reactive_ower = s.value / 1000.;
    } break;

    case 0x2110: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_a_apparent_power = s.value / 1000.;
    } break;

    case 0x2111: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_b_apparent_power = s.value / 1000.;
    } break;

    case 0x2112: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_phase_c_apparent_power = s.value / 1000.;
    } break;

    case 0x2113: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.ac_total_apparent_ower = s.value / 1000.;
    } break;

    case 0x4101: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.dc_high_side_voltage = s.value / 1000.;
    } break;

    case 0x4102: {
        can_packet_acdc::GenericSetting s(payload);
        telemetry.dc_high_side_current = s.value / 1000.;
    } break;

    default: {
        can_packet_acdc::GenericSetting s(payload);
        std::cout << s << std::endl;
    }
    }
}

void InfyCanDevice::txThread() {
    while (!exitTxThread) {
        // request current system DC voltage. Answer will be processed by RX thread.
        request_rx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::SystemDCVoltage());

        // request current system DC current. Answer will be processed by RX thread.
        request_rx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::SystemDCCurrent());

        // request power module number. Answer will be processed by RX thread.
        // request_rx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::PowerModuleNumber());

        // request state. Answer will be processed by RX thread.
        request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::PowerModuleStatus());

        // request inverter state. Answer will be processed by RX thread.
        request_rx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::InverterStatus());

        usleep(100000);
    }
}

bool InfyCanDevice::switch_on_off(bool on) {
    return tx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::OnOff(on));
}

bool InfyCanDevice::set_walkin_enabled(bool on) {
    return tx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::WalkInEnable(on));
}

bool InfyCanDevice::set_inverter_mode(bool i) {
    return tx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::WorkingMode(i));
}

bool InfyCanDevice::adjust_power_factor(float pf) {
    return tx(can_packet_acdc::ADDR_MODULE, can_packet_acdc::PowerFactorAdjust(pf));
}

bool InfyCanDevice::set_voltage_current(float voltage, float current) {
    return tx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::SystemDCVoltage(voltage)) &&
           tx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::SystemDCCurrent(current));
}

bool InfyCanDevice::set_generic_setting(uint8_t byte0, uint8_t byte1, uint32_t value) {
    return tx(can_packet_acdc::ADDR_BROADCAST, can_packet_acdc::GenericSetting(byte0, byte1, value));
}

bool InfyCanDevice::set_output_mode(OutputMode mode) {
    return set_generic_setting(0x11, 0x26, static_cast<std::underlying_type<OutputMode>::type>(mode));
}

bool InfyCanDevice::tx(const uint8_t destination_address, const std::vector<uint8_t>& payload) {
    uint32_t can_id = can_packet_acdc::encode_can_id(destination_address, can_packet_acdc::CMD_WRITE);
    can_id |= 0x80000000U; // Extended frame format
    return _tx(can_id, payload);
}

bool InfyCanDevice::request_rx(const uint8_t destination_address, const std::vector<uint8_t>& payload) {
    uint32_t can_id = can_packet_acdc::encode_can_id(destination_address, can_packet_acdc::CMD_READ);
    can_id |= 0x80000000U; // Extended frame format
    usleep(10000);
    return _tx(can_id, payload);
}

std::ostream& operator<<(std::ostream& out, const InfyCanDevice::Telemetry& self) {
    out << "\n------------------------------------------------\nTelemetry\n---------\n";

    out << "AC line: AB:" << std::to_string(self.ac_ab_line_voltage)
        << " BC: " << std::to_string(self.ac_bc_line_voltage) << " CA: " << std::to_string(self.ac_ca_line_voltage)
        << std::endl;

    out << "AC Phase Voltages: V_A:" << std::to_string(self.ac_phase_a_voltage)
        << " V_B: " << std::to_string(self.ac_phase_b_voltage) << " V_C: " << std::to_string(self.ac_phase_c_voltage)
        << std::endl;

    out << "AC Phase Currents: I_A:" << std::to_string(self.ac_phase_a_current)
        << " I_B: " << std::to_string(self.ac_phase_b_current) << " I_C: " << std::to_string(self.ac_phase_c_current)
        << std::endl;

    out << "AC Active power: Total: " << std::to_string(self.ac_total_active_ower)
        << "P_A:" << std::to_string(self.ac_phase_a_active_power)
        << " P_B: " << std::to_string(self.ac_phase_b_active_power)
        << " P_C: " << std::to_string(self.ac_phase_c_active_power) << std::endl;

    out << "AC Re-Active power: Total: " << std::to_string(self.ac_total_reactive_ower)
        << "P_A:" << std::to_string(self.ac_phase_a_reactive_power)
        << " P_B: " << std::to_string(self.ac_phase_b_reactive_power)
        << " P_C: " << std::to_string(self.ac_phase_c_reactive_power) << std::endl;

    out << "AC Apparent power: Total: " << std::to_string(self.ac_total_apparent_ower)
        << "P_A:" << std::to_string(self.ac_phase_a_apparent_power)
        << " P_B: " << std::to_string(self.ac_phase_b_apparent_power)
        << " P_C: " << std::to_string(self.ac_phase_c_apparent_power) << std::endl;

    out << "AC frequency: " << self.ac_frequency << std::endl;
    out << "Ambient temperature: " << self.ambient_temperature << std::endl;

    out << "DC High Voltage side: Voltage: " << std::to_string(self.dc_high_side_voltage)
        << " Current: " << std::to_string(self.dc_high_side_current) << std::endl;

    out << "Capabilities: dc_min: " << self.dc_min_output_voltage << "V dc_max: " << self.dc_max_output_voltage
        << "V dc_max_current: " << self.dc_max_output_current << "A max_watt: " << self.dc_rated_output_power << "W "
        << std::endl;

    out << self.status << std::endl;
    out << self.voltage << std::endl << self.current << std::endl;

    out << "------------------------------------------------\n";

    return out;
}
