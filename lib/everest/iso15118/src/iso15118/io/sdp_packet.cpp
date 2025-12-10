// SPDX-License-Identifier: Apache-2.0
// Copyright 2023-2025 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/sdp_packet.hpp>

#include <cstdio>
#include <cstring>
#include <limits>

#include <endian.h>

namespace iso15118::io {

v2gtp::PayloadType SdpPacket::get_payload_type() const {
    uint16_t tmp;
    std::memcpy(&tmp, buffer + 2, sizeof(tmp));

    return static_cast<v2gtp::PayloadType>(be16toh(tmp));
}

size_t SdpPacket::get_remaining_bytes_to_read() const {
    switch (state) {
    case State::EMPTY:
        return V2GTP_HEADER_SIZE - bytes_read;
    case State::HEADER_READ:
        return length - bytes_read;
    default:
        return 0;
    }
}

void SdpPacket::update_read_bytes(size_t len) {
    if ((state == State::COMPLETE) or (state == State::INVALID_HEADER) or (state == State::PAYLOAD_TO_LONG)) {
        // nothing to do here - should also not happen, right?
        return;
    }

    bytes_read += len;

    if ((state == State::EMPTY) and (bytes_read == V2GTP_HEADER_SIZE)) {
        parse_header();
    }

    if ((state == State::HEADER_READ) and (bytes_read == length)) {
        state = State::COMPLETE;
    }
}

void SdpPacket::parse_header() {
    if ((buffer[0] != SDP_PROTOCOL_VERSION) or (buffer[1] != SDP_INVERSE_PROTOCOL_VERSION)) {
        state = State::INVALID_HEADER;
        return;
    }

    uint32_t tmp;
    std::memcpy(&tmp, buffer + 4, sizeof(tmp));

    // check if length would overflow
    const auto len_in_buffer = be32toh(tmp);
    if (len_in_buffer > std::numeric_limits<uint32_t>::max() - V2GTP_HEADER_SIZE) {
        state = State::INVALID_HEADER;
        return;
    }
    // FIXME (aw): check for ill-formed header!
    length = len_in_buffer + V2GTP_HEADER_SIZE;

    if (length > sizeof(buffer)) {
        state = State::PAYLOAD_TO_LONG;
        return;
    }

    state = State::HEADER_READ;
}

} // namespace iso15118::io
