// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>

namespace iso15118::io::v2gtp {
enum class Security : uint8_t {
    TLS = 0x00,
    NO_TRANSPORT_SECURITY = 0x10,
};

enum class TransportProtocol : uint8_t {
    TCP = 0x00,
    RESERVED_FOR_UDP = 0x10,
};

static constexpr auto SDP_SERVER_PORT = 15118;

enum class PayloadType : uint16_t {
    SAP = 0x8001,
    Part20Main = 0x8002,
    Part20AC = 0x8003,
    Part20DC = 0x8004,
};

} // namespace iso15118::io::v2gtp
