// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/sdp_server.hpp>

#include <cstring>

#include <endian.h>
#include <netdb.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>

#include <cbv2g/exi_v2gtp.h>

#include <iso15118/detail/helper.hpp>

// FIXME(Sl): Not sure with define
/* link-local multicast address ff02::1 aka ip6-allnodes */
#define IN6ADDR_ALLNODES                                                                                               \
    { 0xff, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01 }

namespace iso15118 {

static void log_peer_hostname(const struct sockaddr_in6& address) {
    char hostname[128];
    socklen_t hostname_len = sizeof(hostname);

    const auto get_if_name_result = getnameinfo(reinterpret_cast<const struct sockaddr*>(&address), sizeof(address),
                                                hostname, hostname_len, nullptr, 0, NI_NUMERICHOST);

    if (0 == get_if_name_result) {
        logf_info("Got SDP request from %s", hostname);
    } else {
        logf_warning("Got SDP request, but failed to get the address");
    }
}

namespace io {

SdpServer::SdpServer(const std::string& interface_name) {
    fd = socket(AF_INET6, SOCK_DGRAM, 0);

    if (fd == -1) {
        log_and_throw("Failed to open socket");
    }

    // initialize socket address, leave scope_id and flowinfo at 0
    struct sockaddr_in6 socket_address;
    bzero(&socket_address, sizeof(socket_address));
    socket_address.sin6_family = AF_INET6;
    socket_address.sin6_port = htobe16(v2gtp::SDP_SERVER_PORT);
    memcpy(&socket_address.sin6_addr, &in6addr_any, sizeof(socket_address.sin6_addr));

    int enable = 1;
    auto result = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));
    if (result == -1) {
        const auto error_msg = adding_err_msg("Setsockopt(SO_REUSEPORT) failed");
        log_and_throw(error_msg.c_str());
    }

    const auto bind_result =
        bind(fd, reinterpret_cast<const struct sockaddr*>(&socket_address), sizeof(socket_address));
    if (bind_result == -1) {
        log_and_throw("Failed to bind to socket");
    }

    // Bind only to specified device
    result = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, interface_name.c_str(), interface_name.length());
    if (result == -1) {
        const auto error_msg = adding_err_msg("Setsockopt(SO_BINDTODEVICE) failed");
        log_and_throw(error_msg.c_str());
    }

    // Join multicast group
    struct ipv6_mreq mreq {};
    mreq.ipv6mr_multiaddr = IN6ADDR_ALLNODES;
    mreq.ipv6mr_interface = if_nametoindex(interface_name.c_str());

    result = setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
    if (result == -1) {
        const auto error_msg = adding_err_msg("Setsockopt(IPV6_JOIN_GROUP) failed");
        log_and_throw(error_msg.c_str());
    }
}

SdpServer::~SdpServer() {
    // FIXME (aw): rather use some RAII class for this!
    logf_info("Shutting down SDP server!");
    if (fd != -1) {
        close(fd);
    }
}

PeerRequestContext SdpServer::get_peer_request() {
    decltype(PeerRequestContext::address) peer_address;
    socklen_t peer_addr_len = sizeof(peer_address);

    const auto read_result = recvfrom(fd, udp_buffer, sizeof(udp_buffer), 0,
                                      reinterpret_cast<struct sockaddr*>(&peer_address), &peer_addr_len);
    if (read_result <= 0) {
        log_and_throw("Read on sdp server socket failed");
    }

    if (peer_addr_len > sizeof(peer_address)) {
        log_and_throw("Unexpected address length during read on sdp server socket");
    }

    log_peer_hostname(peer_address);

    if (read_result == sizeof(udp_buffer)) {
        logf_warning("Read on sdp server socket succeeded, but message is to big for the buffer");
        return PeerRequestContext{false};
    }

    uint32_t sdp_payload_len;
    const auto parse_sdp_result = V2GTP20_ReadHeader(udp_buffer, &sdp_payload_len, V2GTP20_SDP_REQUEST_PAYLOAD_ID);

    if (parse_sdp_result != V2GTP_ERROR__NO_ERROR) {
        // FIXME (aw): we should not die here immediately
        logf_warning("Sdp server received an unexpected payload");
        return PeerRequestContext{false};
    }

    PeerRequestContext peer_request{true};

    // NOTE (aw): this could be moved into a constructor
    const uint8_t sdp_request_byte1 = udp_buffer[8];
    const uint8_t sdp_request_byte2 = udp_buffer[9];
    peer_request.security = static_cast<v2gtp::Security>(sdp_request_byte1);
    peer_request.transport_protocol = static_cast<v2gtp::TransportProtocol>(sdp_request_byte2);
    memcpy(&peer_request.address, &peer_address, sizeof(peer_address));

    return peer_request;
}

void SdpServer::send_response(const PeerRequestContext& request, const Ipv6EndPoint& ipv6_endpoint) {
    // that worked, now response
    uint8_t v2g_packet[28];
    uint8_t* sdp_response = v2g_packet + 8;
    memcpy(sdp_response, ipv6_endpoint.address, sizeof(ipv6_endpoint.address));

    uint16_t port = htobe16(ipv6_endpoint.port);
    memcpy(sdp_response + 16, &port, sizeof(port));

    // FIXME (aw): which values to take here?
    sdp_response[18] = static_cast<std::underlying_type_t<v2gtp::Security>>(request.security);
    sdp_response[19] = static_cast<std::underlying_type_t<v2gtp::TransportProtocol>>(request.transport_protocol);

    V2GTP20_WriteHeader(v2g_packet, 20, V2GTP20_SDP_RESPONSE_PAYLOAD_ID);

    const auto peer_addr_len = sizeof(request.address);

    sendto(fd, v2g_packet, sizeof(v2g_packet), 0, reinterpret_cast<const sockaddr*>(&request.address), peer_addr_len);
}
TlsKeyLoggingServer::TlsKeyLoggingServer(const std::string& interface_name, uint16_t port) {
    static constexpr auto LINK_LOCAL_MULTICAST = "ff02::1";

    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (fd < 0) {
        const auto error_msg = adding_err_msg("Could not create socket");
        log_and_throw(error_msg.c_str()); // FIXME(sl): Find better handling
    }

    // source setup

    // find port between 49152-65535
    auto could_bind = false;
    auto source_port = 49152;
    for (; source_port < 65535; source_port++) {
        sockaddr_in6 source_address = {AF_INET6, htons(source_port), 0, {}, 0};
        if (bind(fd, reinterpret_cast<sockaddr*>(&source_address), sizeof(sockaddr_in6)) == 0) {
            could_bind = true;
            break;
        }
    }

    if (!could_bind) {
        const auto error_msg = adding_err_msg("Could not bind");
        log_and_throw(error_msg.c_str());
    }

    logf_info("UDP socket bound to source port: %u", source_port);

    const auto index = if_nametoindex(interface_name.c_str());
    auto mreq = ipv6_mreq{};
    mreq.ipv6mr_interface = index;
    if (inet_pton(AF_INET6, LINK_LOCAL_MULTICAST, &mreq.ipv6mr_multiaddr) <= 0) {
        const auto error_msg = adding_err_msg("Failed to setup multicast address");
        log_and_throw(error_msg.c_str());
    }
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        const auto error_msg = adding_err_msg("Could not add multicast group membership");
        log_and_throw(error_msg.c_str());
    }

    if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &index, sizeof(index)) < 0) {
        const auto error_msg = adding_err_msg("Could not set interface name:" + std::string(interface_name));
        log_and_throw(error_msg.c_str());
    }

    destination_address = {AF_INET6, htons(port), 0, {}, 0};
    if (inet_pton(AF_INET6, LINK_LOCAL_MULTICAST, &destination_address.sin6_addr) <= 0) {
        const auto error_msg = adding_err_msg("Failed to setup server address, reset key_log_fd");
        log_and_throw(error_msg.c_str());
    }
}

TlsKeyLoggingServer::~TlsKeyLoggingServer() {
    logf_info("Shutting down TlsKeyLoggingServer server!");
    if (fd != -1) {
        close(fd);
    }
}

ssize_t TlsKeyLoggingServer::send(const char* line) {
    return sendto(fd, line, strlen(line), 0, reinterpret_cast<const sockaddr*>(&destination_address),
                  sizeof(destination_address));
}

#if 0

void parse_sdp_request(uint8_t* packet) {
    // check sdp header
    uint32_t sdp_payload_len;
    const auto parse_sdp_result = V2GTP20_ReadHeader(packet, &sdp_payload_len, V2GTP20_SDP_REQUEST_PAYLOAD_ID);

    if (parse_sdp_result != V2GTP_ERROR__NO_ERROR) {
        log_and_throw("Failed to parse sdp header");
    }

    logf_info("Got sdp payload of %d bytes", sdp_payload_len);
    const uint8_t sdp_request_byte1 = packet[8];

    switch (static_cast<v2gtp::Security>(sdp_request_byte1)) {
    case v2gtp::Security::TLS:
        logf_info(" -> TLS requested");
        break;
    case v2gtp::Security::NO_TRANSPORT_SECURITY:
        logf_info(" -> no security");
        break;
    default:
        logf_info(" -> EXCEPTION: reserved value");
        break;
    }

    const uint8_t sdp_request_byte2 = packet[9];
    switch (static_cast<v2gtp::TransportProtocol>(sdp_request_byte2)) {
    case v2gtp::TransportProtocol::TCP:
        logf_info(" -> TCP requested");
        break;
    case v2gtp::TransportProtocol::RESERVED_FOR_UDP:
        logf_info(" -> reserved for UDP");
        break;
    default:
        logf_info(" -> EXCEPTION: reserved value");
        break;
    }
}
#endif
} // namespace io

} // namespace iso15118
