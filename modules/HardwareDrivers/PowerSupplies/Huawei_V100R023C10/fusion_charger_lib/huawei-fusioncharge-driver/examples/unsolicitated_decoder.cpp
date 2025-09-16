// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fusion_charger/modbus/extensions/unsolicitated_report.hpp>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <data in hex>\n", argv[0]);
        return 1;
    }

    std::string data = std::string(argv[1]);
    std::vector<uint8_t> data_vec;

    for (size_t i = 0; i < data.length(); i += 2) {
        std::string byte_str = data.substr(i, 2);
        data_vec.push_back(std::stoi(byte_str, nullptr, 16));
    }

    modbus_server::pdu::GenericPDU generic(0x41, data_vec);
    fusion_charger::modbus_extensions::UnsolicitatedReportRequest pdu;
    pdu.from_generic(generic);

    printf("Decoded payload:\n");

    for (const fusion_charger::modbus_extensions::UnsolicitatedReportRequest::Device& device : pdu.devices) {
        printf("Device location 0x%04x\n", device.location);
        for (const fusion_charger::modbus_extensions::UnsolicitatedReportRequest::Segment& segment : device.segments) {
            printf("  0x%04x\n", segment.registers_start);
            printf("    Count: 0x%04x\n", segment.registers_count);
            printf("    Data (hex): ");
            for (size_t i = 0; i < segment.registers.size(); i += 2) {
                uint16_t reg = (segment.registers[i] << 8) | segment.registers[i + 1];
                printf("0x%04x ", reg);
            }
            printf("\n");
        }
    }

    return 0;
}
