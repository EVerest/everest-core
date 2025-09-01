// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/crc/crc.hpp>
#include <stdint.h>

uint16_t calculate_xModem_crc16(const std::vector<uint8_t>& message) {
    uint16_t crc = 0;

    if (message.size() < 2) {
        return 0;
    }

    for (auto message_item : message) {
        crc ^= (uint16_t)message_item << 8;
        for (uint16_t i = 0; i < 8; i++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}
