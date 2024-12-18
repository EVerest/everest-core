// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "sdp.hpp"
#include "log.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <inttypes.h>
#include <net/if.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG 1

/* defines for V2G SDP implementation */
#define SDP_SRV_PORT 15118

#define SDP_VERSION         0x01
#define SDP_INVERSE_VERSION 0xfe

#define SDP_HEADER_LEN           8
#define SDP_REQUEST_PAYLOAD_LEN  2
#define SDP_RESPONSE_PAYLOAD_LEN 20

#define SDP_REQUEST_TYPE  0x9000
#define SDP_RESPONSE_TYPE 0x9001

#define POLL_TIMEOUT 20

enum sdp_security {
    SDP_SECURITY_TLS = 0x00,
    SDP_SECURITY_NONE = 0x10,
};

enum sdp_transport_protocol {
    SDP_TRANSPORT_PROTOCOL_TCP = 0x00,
    SDP_TRANSPORT_PROTOCOL_UDP = 0x10,
};

/* link-local multicast address ff02::1 aka ip6-allnodes */
#define IN6ADDR_ALLNODES                                                                                               \
    { 0xff, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01 }

/* bundles various aspects of a SDP query */
struct sdp_query {
    struct v2g_context* v2g_ctx;

    struct sockaddr_in6 remote_addr;

    enum sdp_security security_requested;
    enum sdp_transport_protocol proto_requested;
};

/*
 * Fills the SDP header into a given buffer
 */
static int sdp_write_header(uint8_t* buffer, uint16_t payload_type, uint32_t payload_len) {
    int offset = 0;

    buffer[offset++] = SDP_VERSION;
    buffer[offset++] = SDP_INVERSE_VERSION;

    /* payload is network byte order */
    buffer[offset++] = (payload_type >> 8) & 0xff;
    buffer[offset++] = payload_type & 0xff;

    /* payload_length is network byte order */
    buffer[offset++] = (payload_len >> 24) & 0xff;
    buffer[offset++] = (payload_len >> 16) & 0xff;
    buffer[offset++] = (payload_len >> 8) & 0xff;
    buffer[offset++] = payload_len & 0xff;

    return offset;
}

static int sdp_validate_header(uint8_t* buffer, uint16_t expected_payload_type, uint32_t expected_payload_len) {
    uint16_t payload_type;
    uint32_t payload_len;

    if (buffer[0] != SDP_VERSION) {
        dlog(DLOG_LEVEL_ERROR, "Invalid SDP version");
        return -1;
    }

    if (buffer[1] != SDP_INVERSE_VERSION) {
        dlog(DLOG_LEVEL_ERROR, "Invalid SDP inverse version");
        return -1;
    }

    payload_type = (buffer[2] << 8) + buffer[3];
    if (payload_type != expected_payload_type) {
        dlog(DLOG_LEVEL_ERROR, "Invalid payload type: expected %" PRIu16 ", received %" PRIu16, expected_payload_type,
             payload_type);
        return -1;
    }

    payload_len = (buffer[4] << 24) + (buffer[5] << 16) + (buffer[6] << 8) + buffer[7];
    if (payload_len != expected_payload_len) {
        dlog(DLOG_LEVEL_ERROR, "Invalid payload length: expected %" PRIu32 ", received %" PRIu32, expected_payload_len,
             payload_len);
        return -1;
    }

    return 0;
}

int sdp_create_response(uint8_t* buffer, struct sockaddr_in6* addr, enum sdp_security security,
                        enum sdp_transport_protocol proto) {
    int offset = SDP_HEADER_LEN;

    /* fill in first the payload */

    /* address is already network byte order */
    memcpy(&buffer[offset], &addr->sin6_addr, sizeof(addr->sin6_addr));
    offset += sizeof(addr->sin6_addr);

    memcpy(&buffer[offset], &addr->sin6_port, sizeof(addr->sin6_port));
    offset += sizeof(addr->sin6_port);

    buffer[offset++] = security;
    buffer[offset++] = proto;

    /* now fill in the header with payload length */
    sdp_write_header(buffer, SDP_RESPONSE_TYPE, offset - SDP_HEADER_LEN);

    return offset;
}

/*
 * Sends a SDP response packet
 */
int sdp_send_response(int sdp_socket, struct sdp_query* sdp_query) {
    uint8_t buffer[SDP_HEADER_LEN + SDP_RESPONSE_PAYLOAD_LEN];
    int rv = 0;

    /* at the moment we only understand TCP protocol */
    if (sdp_query->proto_requested != SDP_TRANSPORT_PROTOCOL_TCP) {
        dlog(DLOG_LEVEL_ERROR, "SDP requested unsupported protocol 0x%02x, announcing nothing",
             sdp_query->proto_requested);
        return 1;
    }

    switch (sdp_query->security_requested) {
    case SDP_SECURITY_TLS:
        if (sdp_query->v2g_ctx->local_tls_addr) {
            dlog(DLOG_LEVEL_INFO, "SDP requested TLS, announcing TLS");
            sdp_create_response(buffer, sdp_query->v2g_ctx->local_tls_addr, SDP_SECURITY_TLS,
                                SDP_TRANSPORT_PROTOCOL_TCP);
            break;
        }
        if (sdp_query->v2g_ctx->local_tcp_addr) {
            dlog(DLOG_LEVEL_INFO, "SDP requested TLS, announcing NO-TLS");
            sdp_create_response(buffer, sdp_query->v2g_ctx->local_tcp_addr, SDP_SECURITY_NONE,
                                SDP_TRANSPORT_PROTOCOL_TCP);
            break;
        }
        dlog(DLOG_LEVEL_ERROR, "SDP requested TLS, announcing nothing");
        return 1;

    case SDP_SECURITY_NONE:
        if (sdp_query->v2g_ctx->local_tcp_addr) {
            dlog(DLOG_LEVEL_INFO, "SDP requested NO-TLS, announcing NO-TLS");
            sdp_create_response(buffer, sdp_query->v2g_ctx->local_tcp_addr, SDP_SECURITY_NONE,
                                SDP_TRANSPORT_PROTOCOL_TCP);
            break;
        }
        if (sdp_query->v2g_ctx->local_tls_addr) {
            dlog(DLOG_LEVEL_INFO, "SDP requested NO-TLS, announcing TLS");
            sdp_create_response(buffer, sdp_query->v2g_ctx->local_tls_addr, SDP_SECURITY_TLS,
                                SDP_TRANSPORT_PROTOCOL_TCP);
            break;
        }
        dlog(DLOG_LEVEL_ERROR, "SDP requested NO-TLS, announcing nothing");
        return 1;

    default:
        dlog(DLOG_LEVEL_ERROR, "SDP requested unsupported security 0x%02x, announcing nothing",
             sdp_query->security_requested);
        return 1;
    }

    if (sendto(sdp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&sdp_query->remote_addr,
               sizeof(struct sockaddr_in6)) != sizeof(buffer)) {
        rv = -1;
    }
    if (DEBUG) {
        char addrbuf[INET6_ADDRSTRLEN] = {0};
        const char* addr;
        int saved_errno = errno;

        addr = inet_ntop(AF_INET6, &sdp_query->remote_addr.sin6_addr, addrbuf, sizeof(addrbuf));
        if (rv == 0) {
            dlog(DLOG_LEVEL_INFO, "sendto([%s]:%" PRIu16 ") succeeded", addr, ntohs(sdp_query->remote_addr.sin6_port));
        } else {
            dlog(DLOG_LEVEL_ERROR, "sendto([%s]:%" PRIu16 ") failed: %s", addr, ntohs(sdp_query->remote_addr.sin6_port),
                 strerror(saved_errno));
        }
    }

    return rv;
}

int sdp_init(struct v2g_context* v2g_ctx) {
    struct sockaddr_in6 sdp_addr = {AF_INET6, htons(SDP_SRV_PORT)};
    struct ipv6_mreq mreq = {{IN6ADDR_ALLNODES}, 0};
    int enable = 1;

    mreq.ipv6mr_interface = if_nametoindex(v2g_ctx->if_name);
    if (!mreq.ipv6mr_interface) {
        dlog(DLOG_LEVEL_ERROR, "No such interface: %s", v2g_ctx->if_name);
        return -1;
    }

    /* create receiving socket */
    v2g_ctx->sdp_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (v2g_ctx->sdp_socket == -1) {
        dlog(DLOG_LEVEL_ERROR, "socket() failed: %s", strerror(errno));
        return -1;
    }

    if (setsockopt(v2g_ctx->sdp_socket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) == -1) {
        dlog(DLOG_LEVEL_ERROR, "setsockopt(SO_REUSEPORT) failed: %s", strerror(errno));
        close(v2g_ctx->sdp_socket);
        return -1;
    }

    sdp_addr.sin6_addr = in6addr_any;

    if (bind(v2g_ctx->sdp_socket, (struct sockaddr*)&sdp_addr, sizeof(sdp_addr)) == -1) {
        dlog(DLOG_LEVEL_ERROR, "bind() failed: %s", strerror(errno));
        close(v2g_ctx->sdp_socket);
        return -1;
    }

    dlog(DLOG_LEVEL_INFO, "SDP socket setup succeeded");

    /* bind only to specified device */
    if (setsockopt(v2g_ctx->sdp_socket, SOL_SOCKET, SO_BINDTODEVICE, v2g_ctx->if_name, strlen(v2g_ctx->if_name)) ==
        -1) {
        dlog(DLOG_LEVEL_ERROR, "setsockopt(SO_BINDTODEVICE) failed: %s", strerror(errno));
        close(v2g_ctx->sdp_socket);
        return -1;
    }

    dlog(DLOG_LEVEL_TRACE, "bind only to specified device");

    /* join multicast group */
    if (setsockopt(v2g_ctx->sdp_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) == -1) {
        dlog(DLOG_LEVEL_ERROR, "setsockopt(IPV6_JOIN_GROUP) failed: %s", strerror(errno));
        close(v2g_ctx->sdp_socket);
        return -1;
    }

    dlog(DLOG_LEVEL_TRACE, "joined multicast group");

    return 0;
}

int sdp_listen(struct v2g_context* v2g_ctx) {
    /* Init pollfd struct */
    struct pollfd pollfd = {v2g_ctx->sdp_socket, POLLIN, 0};

    while (!v2g_ctx->shutdown) {
        uint8_t buffer[SDP_HEADER_LEN + SDP_REQUEST_PAYLOAD_LEN];
        char addrbuf[INET6_ADDRSTRLEN] = {0};
        const char* addr = addrbuf;
        struct sdp_query sdp_query = {
            .v2g_ctx = v2g_ctx,
        };
        socklen_t addrlen = sizeof(sdp_query.remote_addr);

        /* Check if data was received on socket */
        signed status = poll(&pollfd, 1, POLL_TIMEOUT);

        if (status == -1) {
            if (errno == EINTR) { // If the call did not succeed because it was interrupted
                continue;
            } else {
                dlog(DLOG_LEVEL_ERROR, "poll() failed: %s", strerror(errno));
                continue;
            }
        }
        /* If new data was received, handle sdp request */
        if (status > 0) {
            ssize_t len = recvfrom(v2g_ctx->sdp_socket, buffer, sizeof(buffer), 0,
                                   (struct sockaddr*)&sdp_query.remote_addr, &addrlen);
            if (len == -1) {
                if (errno != EINTR)
                    dlog(DLOG_LEVEL_ERROR, "recvfrom() failed: %s", strerror(errno));
                continue;
            }

            addr = inet_ntop(AF_INET6, &sdp_query.remote_addr.sin6_addr, addrbuf, sizeof(addrbuf));

            if (len != sizeof(buffer)) {
                dlog(DLOG_LEVEL_WARNING, "Discarded packet from [%s]:%" PRIu16 " due to unexpected length %zd", addr,
                     ntohs(sdp_query.remote_addr.sin6_port), len);
                continue;
            }

            if (sdp_validate_header(buffer, SDP_REQUEST_TYPE, SDP_REQUEST_PAYLOAD_LEN)) {
                dlog(DLOG_LEVEL_WARNING, "Packet with invalid SDP header received from [%s]:%" PRIu16, addr,
                     ntohs(sdp_query.remote_addr.sin6_port));
                continue;
            }

            sdp_query.security_requested = (sdp_security)buffer[SDP_HEADER_LEN + 0];
            sdp_query.proto_requested = (sdp_transport_protocol)buffer[SDP_HEADER_LEN + 1];

            dlog(DLOG_LEVEL_INFO, "Received packet from [%s]:%" PRIu16 " with security 0x%02x and protocol 0x%02x",
                 addr, ntohs(sdp_query.remote_addr.sin6_port), sdp_query.security_requested, sdp_query.proto_requested);

            sdp_send_response(v2g_ctx->sdp_socket, &sdp_query);
        }
    }

    if (close(v2g_ctx->sdp_socket) == -1) {
        dlog(DLOG_LEVEL_ERROR, "close() failed: %s", strerror(errno));
    }

    return 0;
}
