// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstddef>
#include <functional>

#include "ipv6_endpoint.hpp"

namespace iso15118::io {

enum class ConnectionEvent {
    ACCEPTED,
    OPEN,
    NEW_DATA,
    CLOSED,
};

using ConnectionEventCallback = std::function<void(ConnectionEvent)>;

struct ReadResult {
    bool would_block{true};
    size_t bytes_read{0};
};

struct IConnection {
    virtual void set_event_callback(const ConnectionEventCallback&) = 0;

    virtual Ipv6EndPoint get_public_endpoint() const = 0;

    virtual void write(const uint8_t* buf, size_t len) = 0;
    virtual ReadResult read(uint8_t* buf, size_t len) = 0;

    virtual void close() = 0;

    virtual ~IConnection() = default;
};
} // namespace iso15118::io
