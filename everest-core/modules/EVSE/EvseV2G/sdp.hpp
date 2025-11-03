// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#ifndef SDP_H
#define SDP_H

#include "v2g.hpp"

enum sdp_security {
    SDP_SECURITY_TLS = 0x00,
    SDP_SECURITY_NONE = 0x10,
};

enum sdp_transport_protocol {
    SDP_TRANSPORT_PROTOCOL_TCP = 0x00,
    SDP_TRANSPORT_PROTOCOL_UDP = 0x10,
};

int sdp_write_header(uint8_t* buffer, uint16_t payload_type, uint32_t payload_len);
int sdp_validate_header(uint8_t* buffer, uint16_t expected_payload_type, uint32_t expected_payload_len);
int sdp_create_response(uint8_t* buffer, struct sockaddr_in6* addr, enum sdp_security security,
                        enum sdp_transport_protocol proto);
int sdp_init(struct v2g_context* v2g_ctx);
int sdp_listen(struct v2g_context* v2g_ctx);

#endif /* SDP_H */
