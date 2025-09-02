// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CRC_HPP
#define CRC_HPP
#include <cstdint>
#include <vector>

std::uint16_t calculate_xModem_crc16(const std::vector<std::uint8_t>& message);
#endif
