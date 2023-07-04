#include <cstdio>
#include <cstring>
#include <string>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <slac/slac.hpp>

#include "evse_vs_ev/plc_emu.hpp"

void print_mac(const uint8_t* mac) {
    printf("%2X:%2X:%2X:%2X:%2X:%2X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// FIXME (aw): this helper doesn't really belong here
static void exit_with_error(const char* msg) {
    fprintf(stderr, "%s (%s)\n", msg, strerror(errno));
    exit(-EXIT_FAILURE);
}

struct InterfaceInfo {
    uint8_t mac[ETH_ALEN];
    int interface_index{-1};
};

InterfaceInfo get_interface_info(const std::string& interface_name) {
    InterfaceInfo if_info;

    struct ifaddrs* if_addrs;
    if (-1 == getifaddrs(&if_addrs)) {
        // FIXME (aw): proper error handling?
        return if_info;
    }

    // iterate through them and list them
    struct ifaddrs* cur_if_addr = if_addrs;
    while (cur_if_addr) {
        if (cur_if_addr->ifa_addr && cur_if_addr->ifa_addr->sa_family == AF_PACKET) {
            if (0 == interface_name.compare(cur_if_addr->ifa_name)) {
                const auto* addr_info = reinterpret_cast<struct sockaddr_ll*>(cur_if_addr->ifa_addr);
                memcpy(if_info.mac, addr_info->sll_addr, sizeof(if_info.mac));
                if_info.interface_index = addr_info->sll_ifindex;
                break;
            }
        }

        cur_if_addr = cur_if_addr->ifa_next;
    }

    freeifaddrs(if_addrs);

    return if_info;
}

int create_raw_homeplug_socket(const InterfaceInfo& interface_info) {
    const uint16_t homeplug_protocol = slac::defs::ETH_P_HOMEPLUG_GREENPHY;
    const auto socket_fd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(homeplug_protocol));

    if (socket_fd == -1) {
        exit_with_error("Couldn't create the socket");
    }

    // bind this packet socket to a specific interface
    struct sockaddr_ll sock_addr = {
        AF_PACKET,                                       // sll_family
        htons(homeplug_protocol),                        // sll_protocol
        interface_info.interface_index,                  // sll_ifindex
        0x00,                                            // sll_hatype, set on receiving
        0x00,                                            // sll_pkttype, set on receiving
        ETH_ALEN,                                        // sll_halen
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // sll_addr[8]
    };

    if (-1 == bind(socket_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr))) {
        exit_with_error("Failed to bind the socket");
    }

    return socket_fd;
}

void loop(int ev_fd, int evse_fd) {
    constexpr static auto EV_INDEX = 0;
    constexpr static auto EVSE_INDEX = 1;

    struct pollfd pollfds[] = {
        {ev_fd, POLLIN, 0},
        {evse_fd, POLLIN, 0},
    };

    static constexpr auto num_fds = sizeof(pollfds) / sizeof(struct pollfd);

    while (true) {
        const auto status = poll(pollfds, num_fds, -1);

        if (status == -1) {
            exit_with_error("bridge poll");
        }

        if (pollfds[EV_INDEX].revents & POLLIN) {
            printf("Received ev input\n");
            handle_ev_input(ev_fd, evse_fd);
        }

        if (pollfds[EVSE_INDEX].revents & POLLIN) {
            printf("Received evse input\n");
            handle_evse_input(evse_fd, ev_fd);
        }
    }
}

int main(int argc, char* argv[]) {
    const auto ev_bridge_device_info = get_interface_info("vev-bridge");
    const auto evse_bridge_device_info = get_interface_info("vevse-bridge");
    const auto ev_fd = create_raw_homeplug_socket(ev_bridge_device_info);
    const auto evse_fd = create_raw_homeplug_socket(evse_bridge_device_info);

    loop(ev_fd, evse_fd);

    // printf("mac: ");
    // print_mac(info.mac);
    // printf("index: %d\n", info.interface_index);

    return 0;
}
