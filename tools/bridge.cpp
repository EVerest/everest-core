// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include <string>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <fmt/core.h>

#include <slac/slac.hpp>

// sudo ip link add veth-evse-host type veth peer name veth-evse-plc
// sudo ip link add veth-ev-host type veth peer name veth-ev-plc
// sudo ip link set veth-evse-host up
// sudo ip link set veth-evse-plc up
// sudo ip link set veth-ev-host up
// sudo ip link set veth-ev-plc up

enum class NodeType
{
    EVSE,
    EV
};

struct PLCDeviceInformation {
    std::string veth_device;
    uint8_t plc_mac[ETH_ALEN];
    NodeType id;
    const char* name() {
        return (id == NodeType::EVSE) ? "EVSE" : "EV";
    }
    uint8_t veth_mac[ETH_ALEN];
    int if_index{-1};
    int fd;
};

void fetch_device_information(PLCDeviceInformation& evse, PLCDeviceInformation& ev) {
    struct ifaddrs* if_addrs;
    int ret = getifaddrs(&if_addrs);
    if (ret == -1) {
        fmt::print("Error while calling getifaddrs(): {}\n", strerror(errno));
        exit(-1);
    }

    // iterate through them and list them
    struct ifaddrs* cur_if_addr = if_addrs;
    while (cur_if_addr) {
        if (cur_if_addr->ifa_addr && cur_if_addr->ifa_addr->sa_family == AF_PACKET) {
            if (0 == evse.veth_device.compare(cur_if_addr->ifa_name) && evse.if_index == -1) {
                const auto* addr_info = reinterpret_cast<struct sockaddr_ll*>(cur_if_addr->ifa_addr);
                memcpy(evse.veth_mac, addr_info->sll_addr, 6);
                evse.if_index = addr_info->sll_ifindex;
            }
            if (0 == ev.veth_device.compare(cur_if_addr->ifa_name) && ev.if_index == -1) {
                const auto* addr_info = reinterpret_cast<struct sockaddr_ll*>(cur_if_addr->ifa_addr);
                memcpy(ev.veth_mac, addr_info->sll_addr, 6);
                ev.if_index = addr_info->sll_ifindex;
            }
        }

        cur_if_addr = cur_if_addr->ifa_next;
    }

    freeifaddrs(if_addrs);

    const char common_msg[] = "Could not find device ({}) for {}\n";
    if (ev.if_index == -1) {
        fmt::print(common_msg, ev.veth_device, "EV");
        exit(-1);
    }
    if (evse.if_index == -1) {
        fmt::print(common_msg, evse.veth_device, "EVSE");
        exit(-1);
    }

    fmt::print("Using interface index {} for EV\n", ev.if_index);
    fmt::print("Using interface index {} for EVSE\n", evse.if_index);
}

void open_packet_socket(PLCDeviceInformation& di) {
    auto socket_fd = socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, htons(ETH_P_ALL));

    if (socket_fd == -1) {
        fmt::print("Couldn't create socket on ({}): {}\n", di.veth_device, strerror(errno));
        if (errno == EPERM) {
            fmt::print("You probably want to run this tool with sudo\n");
        }
        exit(-1);
    }

    // bind this packet socket to a specific interface
    struct sockaddr_ll sock_addr = {
        AF_PACKET,                                       // sll_family
        htons(ETH_P_ALL),                                // sll_protocol
        di.if_index,                                     // sll_ifindex
        0x00,                                            // sll_hatype, set on receiving
        0x00,                                            // sll_pkttype, set on receiving
        ETH_ALEN,                                        // sll_halen
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // sll_addr[8]
    };

    if (-1 == bind(socket_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr))) {
        fmt::print("Couldn't bind socket on({}): {}\n", di.veth_device, strerror(errno));
        close(socket_fd);

        exit(-1);
    }

    di.fd = socket_fd;
}

void handle_set_key_req(PLCDeviceInformation& node, const slac::messages::homeplug_message& request,
                        slac::messages::HomeplugMessage& response) {
    slac::messages::cm_set_key_cnf set_key_cnf;
    auto set_key_req = reinterpret_cast<const slac::messages::cm_set_key_req*>(request.mmentry);

    set_key_cnf.result = 0x01; // FIXME (aw): why is this? slac::defs::CM_SET_KEY_CNF_RESULT_SUCCESS;
    set_key_cnf.my_nonce = 0x12345678;
    set_key_cnf.your_nonce = set_key_req->my_nonce;
    set_key_cnf.pid = set_key_req->pid;
    set_key_cnf.prn = 0x0000; // FIXME (aw): what to use here?
    set_key_cnf.cco_capability = slac::defs::CM_SET_KEY_REQ_CCO_CAP_NONE;

    response.setup_payload(&set_key_cnf, sizeof(set_key_cnf),
                           slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_CNF);
    response.setup_ethernet_header(request.ethernet_header.ether_shost, node.plc_mac);
}

void handle_mnbc_sound_ind(PLCDeviceInformation& forward_node, const slac::messages::homeplug_message& indicate,
                           slac::messages::HomeplugMessage& response) {
    slac::messages::cm_atten_profile_ind atten_profile;

    memcpy(atten_profile.pev_mac, indicate.ethernet_header.ether_shost, sizeof(atten_profile.pev_mac));
    atten_profile.num_groups = slac::defs::AAG_LIST_LEN;

    for (int i = 0; i < slac::defs::AAG_LIST_LEN; ++i) {
        atten_profile.aag[i] = std::rand() % 32; // random attenuation
    }

    response.setup_payload(&atten_profile, sizeof(atten_profile),
                           slac::defs::MMTYPE_CM_ATTEN_PROFILE | slac::defs::MMTYPE_MODE_IND);
    response.setup_ethernet_header(slac::defs::BROADCAST_MAC_ADDRESS, forward_node.plc_mac);
}

void process_packet(PLCDeviceInformation& in, PLCDeviceInformation& out) {
    bool forward_packet = true;

    slac::messages::homeplug_message incoming;
    slac::messages::HomeplugMessage outgoing;

    int bytes_read = read(in.fd, &incoming, sizeof(incoming));
    int return_to_fd = -1;

    enum
    {
        IN,
        OUT,
        NONE
    } return_dest = NONE;

    if (incoming.ethernet_header.ether_type == htons(slac::defs::ETH_P_HOMEPLUG_GREENPHY)) {
        // check for cm_set_key
        switch (le16toh(incoming.homeplug_header.mmtype)) {
        case slac::defs::MMTYPE_CM_SET_KEY | slac::defs::MMTYPE_MODE_REQ:
            forward_packet = false;
            handle_set_key_req(in, incoming, outgoing);
            return_dest = IN;
            break;
        case slac::defs::MMTYPE_CM_MNBC_SOUND | slac::defs::MMTYPE_MODE_IND:
            handle_mnbc_sound_ind(out, incoming, outgoing);
            return_dest = OUT;
            break;
        default:
            break;
        }
    }

    if (forward_packet) {
        int bytes_written = write(out.fd, &incoming, bytes_read);
        if (bytes_read != bytes_written) {
            fmt::print("Failed to forward packets from {}\n", in.name());
            exit(-1);
        }
    }

    if (return_dest != NONE) {
        int return_fd = (return_dest == IN) ? in.fd : out.fd;
        int bytes_written = write(return_fd, &outgoing.get_raw_message(), outgoing.get_raw_msg_len());
        if (bytes_written != outgoing.get_raw_msg_len()) {
            const auto name = (return_dest == IN) ? in.name() : out.name();
            fmt::print("Failed return processed message to {}\n", name);
            exit(-1);
        }
    }

    // after forwarding, we can either send a new one like in the sound message
    // or reply a new one
}

int main(int argc, char* argv[]) {
    PLCDeviceInformation evse_di = {"veth-evse-plc", {0x00, 0x01, 0x87, 0x0e, 0xa3, 0x55}, NodeType::EVSE};
    PLCDeviceInformation ev_di = {"veth-ev-plc", {0x00, 0x01, 0x87, 0x0e, 0xa3, 0xaa}, NodeType::EV};

    fetch_device_information(evse_di, ev_di);
    open_packet_socket(evse_di);
    open_packet_socket(ev_di);

    fmt::print("Connecting EVSE on interface {} to EV on interface {}\n", evse_di.veth_device, ev_di.veth_device);

    struct pollfd poll_fds[] = {
        {evse_di.fd, POLLIN, 0},
        {ev_di.fd, POLLIN, 0},
    };

    while (true) {
        int ret = poll(poll_fds, 2, 2000);
        if (ret == -1) {
            fmt::print("poll() failed with: {}\n", strerror(errno));
            exit(-1);
        }

        int bytes_read, bytes_written;

        if (poll_fds[0].revents & POLLIN) {
            process_packet(evse_di, ev_di);
        }

        if (poll_fds[1].revents & POLLIN) {
            process_packet(ev_di, evse_di);
        }
    }

    return 0;
}
