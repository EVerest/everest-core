// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/detail/io/socket_helper.hpp>

#include <cstring>

#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::io {
bool get_first_sockaddr_in6_for_interface(const std::string& interface_name, sockaddr_in6& address) {
    struct ifaddrs* if_list_head;
    const auto get_if_addrs_result = getifaddrs(&if_list_head);

    if (get_if_addrs_result == -1) {
        log_and_throw("Failed to call getifaddrs");
    }
    auto current_if = if_list_head;

    bool found_interface = false;

    for (auto current_if = if_list_head; current_if != nullptr; current_if = current_if->ifa_next) {
        if (current_if->ifa_addr == nullptr) {
            continue;
        }

        if (current_if->ifa_addr->sa_family != AF_INET6) {
            continue;
        }

        if (interface_name.compare(current_if->ifa_name) != 0) {
            continue;
        }

        // NOTE (aw): because we did the check for AF_INET6, we can assume that ifa_addr is indeed an sockaddr_in6
        memcpy(&address, current_if->ifa_addr, sizeof(address));
        found_interface = true;

        // FIXME (aw): this would work, but JOSEV also puts the scope id aka ifname onto unique link addresses
        // break;
    }

    freeifaddrs(if_list_head);

    return found_interface;
}

std::unique_ptr<char[]> sockaddr_in6_to_name(const sockaddr_in6& address) {
    // 4 chars per 2 bytes, 7 colons, 1 '%' and interface name (note, that IFNAMESIZ already include null termination)
    static constexpr auto MAX_NUMERIC_NAME_LENGTH = 8 * 4 + 7 * 1 + 1 + IFNAMSIZ;
    auto name = std::make_unique<char[]>(MAX_NUMERIC_NAME_LENGTH);

    // FIXME (aw): what about alignment issues here between casting from sockaddr_in6 to sockaddr?
    const auto result = getnameinfo(reinterpret_cast<const sockaddr*>(&address), sizeof(address), name.get(),
                                    MAX_NUMERIC_NAME_LENGTH, nullptr, 0, NI_NUMERICHOST);

    if (result == 0) {
        return name;
    } else {
        return nullptr;
    }
}
} // namespace iso15118::io
