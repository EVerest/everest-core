// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <vector>

#include <iso15118/io/stream_view.hpp>

using namespace iso15118;

template <typename Message> std::vector<uint8_t> serialize_helper(const Message& message) {
    uint8_t serialization_buffer[1024];
    io::StreamOutputView out({serialization_buffer, sizeof(serialization_buffer)});

    const auto size = message_20::serialize(message, out);

    return std::vector<uint8_t>(serialization_buffer, serialization_buffer + size);
}