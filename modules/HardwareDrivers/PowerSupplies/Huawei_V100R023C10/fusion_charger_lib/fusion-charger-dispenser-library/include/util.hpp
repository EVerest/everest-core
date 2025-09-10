// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>

inline bool is_valid_mac(const uint8_t* mac) {
    return !(mac[0] == 0 && mac[1] == 1 && mac[2] == 0 && mac[3] == 0);
}
