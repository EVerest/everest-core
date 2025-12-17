// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "hex_to_vec.hpp"

void hex_to_vec(const char* hex_data, std::uint8_t* data, size_t data_size) {
    for (size_t i = 0; i < data_size; i++) {
        sscanf(&hex_data[i * 2], "%2hhx", &data[i]);
    }
}
