// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "packet_socket.hpp"

#include <cstring>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace utils {
InterfaceInfo::InterfaceInfo(const std::string& interface_name) {
    // fetch all interfaces
    struct ifaddrs* if_addrs;
    int ret = getifaddrs(&if_addrs);
    if (ret == -1) {
        error = "Error while calling getifaddrs(): ";
        error += strerror(errno);
        return;
    }

    // iterate through them and list them
    struct ifaddrs* cur_if_addr = if_addrs;
    while (cur_if_addr) {
        if (cur_if_addr->ifa_addr && cur_if_addr->ifa_addr->sa_family == AF_PACKET) {
            if (0 == interface_name.compare(cur_if_addr->ifa_name)) {
                const auto* addr_info = reinterpret_cast<struct sockaddr_ll*>(cur_if_addr->ifa_addr);
                memcpy(mac, addr_info->sll_addr, 6);
                interface_index = addr_info->sll_ifindex;
                valid = true;
                break;
            }
        }

        cur_if_addr = cur_if_addr->ifa_next;
    }

    freeifaddrs(if_addrs);

    if (!valid) {
        error = "Interface " + interface_name + " not found";
    }
}

PacketSocket::PacketSocket(const InterfaceInfo& if_info, int protocol) {
    // FIXME (aw): do we need to use O_NONBLOCKING?
    socket_fd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(protocol));

    if (socket_fd == -1) {
        error = "Couldn't create the socket: ";
        error += strerror(errno);
        return;
    }

    // bind this packet socket to a specific interface
    struct sockaddr_ll sock_addr = {
        AF_PACKET,                                       // sll_family
        htons(protocol),                                 // sll_protocol
        if_info.get_index(),                             // sll_ifindex
        0x00,                                            // sll_hatype, set on receiving
        0x00,                                            // sll_pkttype, set on receiving
        ETH_ALEN,                                        // sll_halen
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // sll_addr[8]
    };

    if (-1 == bind(socket_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr))) {
        error = "Failed to bind the socket: ";
        error += strerror(errno);
        close(socket_fd);
    }

    // everything should have worked out
    valid = true;
}

PacketSocket::IOResult PacketSocket::read(uint8_t* buffer, int timeout) {
    struct pollfd poll_fd = {
        socket_fd, // file descriptor
        POLLIN,    // requested event
        0          // returned event
    };
    int ret = poll(&poll_fd, 1, timeout);
    if (-1 == ret) {
        error = std::string("poll() failed with: ") + strerror(errno);
        return IOResult::Failure;
    }

    if (0 == ret) {
        return IOResult::Timeout;
    }

    if ((poll_fd.revents & POLLIN) == 0) {
        error = "poll() set other flag than POLLIN";
        return IOResult::Failure;
    }

    bytes_read = ::read(socket_fd, buffer, MIN_BUFFER_SIZE);
    if (bytes_read == -1) {
        error = std::string("read() failed with: ") + strerror(errno);
        return IOResult::Failure;
    }

    return IOResult::Ok;
}

PacketSocket::IOResult PacketSocket::write(const void* buf, size_t size, int timeout) {
    struct pollfd poll_fd = {
        socket_fd, // file descriptor
        POLLOUT,   // requested event
        0          // returned event
    };

    int ret = poll(&poll_fd, 1, timeout);

    if (ret == -1) {
        error = std::string("poll() failed with: ") + strerror(errno);
        return IOResult::Failure;
    }

    if (ret == 0) {
        return IOResult::Timeout;
    }

    if ((poll_fd.revents & POLLOUT) == 0) {
        error = "poll() set other flag than POLLOUT";
        return IOResult::Failure;
    }

    auto bytes_written = ::write(socket_fd, buf, size);

    if (-1 == bytes_written) {
        error = std::string("write() failed with: ") + strerror(errno);
        return IOResult::Failure;
    }

    if (bytes_written != size) {
        error = "write() only send part of the datagram - this should not happen";
        return IOResult::Failure;
    }

    return IOResult::Ok;
}

} // namespace utils
