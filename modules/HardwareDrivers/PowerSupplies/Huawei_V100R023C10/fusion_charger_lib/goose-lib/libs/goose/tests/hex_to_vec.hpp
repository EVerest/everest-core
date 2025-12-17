// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

void hex_to_vec(const char* hex_data, std::uint8_t* data, size_t data_size);
