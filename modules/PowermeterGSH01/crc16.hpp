// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CRC16_HPP
#define CRC16_HPP
#include <stdint.h>
#include <vector>

uint16_t calculate_xModem_crc16(const std::vector<uint8_t>& message);
#endif