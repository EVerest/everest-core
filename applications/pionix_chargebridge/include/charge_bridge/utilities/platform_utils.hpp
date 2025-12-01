// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace charge_bridge::utilities {

// Converts a struct to raw bytes
template <typename T> static inline void struct_to_vector(const T& data_struct, std::vector<uint8_t>& buffer) {
    static constexpr auto struct_size = sizeof(T);

    buffer.resize(struct_size);
    std::memcpy(buffer.data(), &data_struct, struct_size);
}
} // namespace charge_bridge::utilities
