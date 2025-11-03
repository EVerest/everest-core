// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <cstdint>

namespace iso15118::io {

constexpr std::size_t sha_512_hash_size = 64;
using sha512_hash_t = std::array<uint8_t, sha_512_hash_size>;

} // namespace iso15118::io
