// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef SRC_PACKET_SOCKET_HPP
#define SRC_PACKET_SOCKET_HPP

#include <cstdint>
#include <string>

#include <linux/if_ether.h>

namespace utils {
class InterfaceInfo {
public:
    explicit InterfaceInfo(const std::string& interface_name);
    bool is_valid() {
        return valid;
    };
    const std::string& get_error() const {
        return error;
    };

    const int get_index() const {
        return interface_index;
    };
    const uint8_t* get_mac() const {
        return mac;
    };

private:
    bool valid{false};
    int interface_index{0};
    std::string error;
    uint8_t mac[ETH_ALEN];
};

class PacketSocket {
public:
    enum class IOResult {
        Ok,
        Failure,
        Timeout
    };

    PacketSocket(const InterfaceInfo& if_info, int protocol);

    bool is_valid() {
        return valid;
    };

    const std::string& get_error() {
        return error;
    }

    IOResult read(uint8_t* buffer, int timeout);

    int get_last_read_size() const {
        return bytes_read;
    };

    IOResult write(const void* buf, size_t count, int timeout);

    static const int MIN_BUFFER_SIZE = ETH_FRAME_LEN;

private:
    int bytes_read{-1};
    bool valid{false};
    std::string error;
    int socket_fd{-1};
};
} // namespace utils

#endif // SRC_PACKET_SOCKET_HPP
