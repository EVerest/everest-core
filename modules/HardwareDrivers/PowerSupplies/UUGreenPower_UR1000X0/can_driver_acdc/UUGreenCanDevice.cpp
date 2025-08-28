// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "UUGreenCanDevice.hpp"
#include "CanPackets.hpp"
#include <iostream>
#include <unistd.h>

UUGreenCanDevice::~UUGreenCanDevice() {
    exit_tx_thread = true;
    exit_cmd_thread = true;
}

bool UUGreenCanDevice::switch_on(bool on) {
    // actual switching on will be handled in tx thread
    requested_on = on;
    return true;
}

bool UUGreenCanDevice::switch_on_nolock(bool on) {

    uint32_t data{0x00};
    if (not on) {
        data = 0x01;
    }

    bool success = true;

    for (auto module_address : module_addresses) {
        if (not tx(module_address, UU::Packet(UU::MessageType::SetData, UU::CommandType::ShutdownDCDC, data))) {
            success = false;
        } else {
            is_on = on;
        }
    }

    if (on) {
        // After switching on, update the voltage/current settings
        set_voltage_current_nolock(set_point_voltage, set_point_current);
    }

    return success;
}

bool UUGreenCanDevice::set_voltage_current(float voltage, float current) {
    requested_set_point_voltage = voltage;
    requested_set_point_current = current;
    return true;
}

bool UUGreenCanDevice::set_voltage_current_nolock(float voltage, float current) {
    bool success = true;

    set_point_voltage = voltage;
    set_point_current = current;

    // check if we need to switch between high and lo voltage mode
    if (hi_mode_config_setting == VoltageMode::Automatic) {
        if (voltage > LO_MODE_MAX_VOLTAGE) {
            hi_mode_commanded = VoltageMode::High;
        } else {
            hi_mode_commanded = VoltageMode::Low;
        }
    }

    internal_update_voltage_mode();

    for (auto module_address : module_addresses) {
        if (not tx(module_address, UU::Packet(UU::MessageType::SetData, UU::CommandType::VoutReference,
                                              static_cast<uint32_t>(voltage * 1000)))) {
            success = false;
        }

        // Split current equally between modules
        if (not tx(module_address, UU::Packet(UU::MessageType::SetData, UU::CommandType::IoutLimit,
                                              static_cast<uint32_t>(current * 1000 / module_addresses.size())))) {
            success = false;
        }
    }
    return success;
}

bool UUGreenCanDevice::internal_update_voltage_mode() {

    bool success = true;
    if (hi_mode_last_commanded not_eq hi_mode_commanded) {
        hi_mode_last_commanded = hi_mode_commanded;

        bool was_on = is_on;

        if (was_on) {
            // Switch it off if it was on before
            switch_on_nolock(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        for (auto module_address : module_addresses) {
            // We need to change hi/lo voltage mode, so power it off, change mode, and power it on again if it was
            // powered on before
            if (not tx(module_address,
                       UU::Packet(UU::MessageType::SetData, UU::CommandType::HiMode_LoMode_Selection,
                                  static_cast<std::underlying_type<VoltageMode>::type>(hi_mode_commanded)))) {
                success = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        if (was_on) {
            // Switch it on if it was on before
            switch_on_nolock(true);
        }
    }

    return success;
}

static std::string to_serial_number(uint16_t a, uint32_t b) {

    std::string ser = "Module type: " + std::to_string((int)(a & 0x01FF));
    ser += " | OV Type code: " + std::to_string((int)(a >> 10));
    // Note: the S/N reported here does not match the sticker on the case
    ser += " | S/N: " + std::to_string(b);
    return ser;
}

void UUGreenCanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
    // We only use extended frames here
    if (!(can_id & CAN_EFF_FLAG)) {
        return;
    }

    // std::cout << "UUGreen: CAN frame received. ID: 0x" << std::hex << can_id << std::endl;

    // is it for us?

    auto monitor_address = UU::monitor_adress_from_can_id(can_id);

    if (not(monitor_address == UU::ADDR_BROADCAST or monitor_address == 0x01)) {
        // std::cout << "UU: Not for us: " << std::endl;
        return;
    }

    // parse packet
    auto packet = UU::Packet(payload);

    uint8_t source_address = UU::module_address_from_can_id(can_id);
    // std::cout << "RX packet: " << packet << std::endl;

    if (packet.message_type == UU::MessageType::ReadDataResponse) {
        switch (packet.command_type) {

        case UU::CommandType::Vout: {
            telemetries[source_address].voltage = packet.data / 1000.;
            // report average voltage
            float voltage = 0.;
            for (const auto& t : telemetries) {
                voltage += t.second.voltage;
            }
            voltage /= telemetries.size();
            signal_voltage_current(voltage, total_current);
        } break;

        case UU::CommandType::Iout_slow: {
            // report sum of all currents
            telemetries[source_address].current = packet.data / 1000.;

            // calculate total current
            total_current = 0.;
            for (const auto& t : telemetries) {
                total_current += t.second.current;
            }

        } break;

        case UU::CommandType::ReadSN: {
            // print serial number information
            signal_serial_number(source_address, to_serial_number(packet.bytes23, packet.data));

        } break;
        }
    }

    if (packet.message_type == UU::MessageType::ReadSerialNumberResponse) {
        // print serial number information
        signal_serial_number(source_address, to_serial_number(packet.bytes23, packet.data));
    }
}

void UUGreenCanDevice::request_module_info() {

    // Request information about modules once
    for (auto module_address : module_addresses) {
        // request serial number
        tx(module_address, UU::Packet(UU::MessageType::ReadData, UU::CommandType::ReadSN));
    }
}

void UUGreenCanDevice::tx_thread() {

    while (!exit_tx_thread) {
        {
            for (auto module_address : module_addresses) {
                // request current system DC voltage. Answer will be processed by RX thread.
                tx(module_address, UU::Packet(UU::MessageType::ReadData, UU::CommandType::Vout));

                // request current system DC current. Answer will be processed by RX thread.
                tx(module_address, UU::Packet(UU::MessageType::ReadData, UU::CommandType::Iout_slow));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void UUGreenCanDevice::cmd_thread() {
    while (!exit_cmd_thread) {

        {

            // Do we need to switch on or off?
            if (last_requested_on not_eq requested_on) {
                last_requested_on = requested_on;
                switch_on_nolock(requested_on);
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

bool UUGreenCanDevice::tx(const uint8_t module_address, const std::vector<uint8_t>& payload) {
    uint32_t can_id = UU::encode_can_id(module_address);
    can_id |= 0x80000000U; // Extended frame format
    return _tx(can_id, payload);
}

std::ostream& operator<<(std::ostream& out, const UUGreenCanDevice::Telemetry& self) {
    out << "\n------------------------------------------------\nTelemetry\n---------\n";

    out << "DC output: " << std::to_string(self.voltage) << "V / " << std::to_string(self.current) << "A" << std::endl;
    out << "------------------------------------------------\n";

    return out;
}
