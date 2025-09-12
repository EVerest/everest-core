// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

// clang-format off
#include <everest/io/netlink/vcan_netlink_manager.hpp>
#include <linux/netlink.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <net/if.h>
#include <linux/if_arp.h>
#include <linux/rtnetlink.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
// clang-format on

namespace everest::lib::io::netlink {

struct vcan_netlink_manager::NetlinkMessage {
    struct nlmsghdr header;
    char buffer[4096];
};

void vcan_netlink_manager::add_attribute(nlmsghdr* nlh, int maxlen, int type, const void* data, int len) {
    int attrlen = RTA_LENGTH(len);
    if (NLMSG_ALIGN(nlh->nlmsg_len) + attrlen > static_cast<uint32_t>(maxlen)) {
        throw std::runtime_error("Netlink attribute too long for message buffer.");
    }
    struct rtattr* rta = (struct rtattr*)(((char*)nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
    rta->rta_type = type;
    rta->rta_len = attrlen;
    memcpy(RTA_DATA(rta), data, len);
    nlh->nlmsg_len = NLMSG_ALIGN(nlh->nlmsg_len) + attrlen;
}

vcan_netlink_manager::vcan_netlink_manager() {
    m_nl_socket_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (m_nl_socket_fd < 0) {
        throw std::runtime_error("Failed to create shared Netlink socket: " + std::string(strerror(errno)));
    }

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(m_nl_socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        ::close(m_nl_socket_fd);
        m_nl_socket_fd = -1;
        throw std::runtime_error("Could not set receive timeout on shared Netlink socket: " +
                                 std::string(strerror(errno)));
    }
}

vcan_netlink_manager::~vcan_netlink_manager() {
    if (m_nl_socket_fd != -1) {
        ::close(m_nl_socket_fd);
        m_nl_socket_fd = -1;
    }
}

vcan_netlink_manager& vcan_netlink_manager::Instance() {
    static vcan_netlink_manager obj;
    return obj;
}

void vcan_netlink_manager::send_message(NetlinkMessage const& msg, int flags) {
    iovec iov;
    iov.iov_base = (void*)&msg.header;
    iov.iov_len = msg.header.nlmsg_len;

    msghdr message;
    message.msg_name = nullptr;
    message.msg_namelen = 0;
    message.msg_iov = &iov;
    message.msg_iovlen = 1;
    message.msg_control = nullptr;
    message.msg_controllen = 0;
    message.msg_flags = 0;

    ssize_t bytes_sent = sendmsg(m_nl_socket_fd, &message, flags);
    if (bytes_sent < 0) {
        throw std::runtime_error("Failed to send Netlink message: " + std::string(strerror(errno)));
    }
    if (static_cast<std::uint32_t>(bytes_sent) != msg.header.nlmsg_len) {
        throw std::runtime_error("Partial Netlink message sent. Expected " + std::to_string(msg.header.nlmsg_len) +
                                 " bytes, sent " + std::to_string(bytes_sent));
    }
}

void vcan_netlink_manager::receive_message(NetlinkMessage& msg) {
    iovec iov;
    iov.iov_base = (void*)&msg.header;
    iov.iov_len = sizeof(msg);

    msghdr message;
    message.msg_name = nullptr;
    message.msg_namelen = 0;
    message.msg_iov = &iov;
    message.msg_iovlen = 1;
    message.msg_control = nullptr;
    message.msg_controllen = 0;
    message.msg_flags = 0;

    ssize_t bytes_received = recvmsg(m_nl_socket_fd, &message, 0); // Use shared FD
    if (bytes_received < 0) {
        throw std::runtime_error("Failed to receive Netlink message: " + std::string(strerror(errno)));
    }
    if (bytes_received == 0) {
        throw std::runtime_error("Netlink socket closed during receive.");
    }
    if (bytes_received < static_cast<int>(sizeof(struct nlmsghdr))) {
        throw std::runtime_error("Incomplete Netlink message header.");
    }
}

void vcan_netlink_manager::send_netlink_request_impl(int msg_type, int flags, cb_type const& callback) {
    NetlinkMessage req;
    memset(&req, 0, sizeof(req));

    req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.header.nlmsg_type = msg_type;
    req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | flags;
    req.header.nlmsg_seq = ++s_netlink_seq_counter;
    req.header.nlmsg_pid = getpid();

    struct ifinfomsg* ifi = (struct ifinfomsg*)NLMSG_DATA(&req.header);
    ifi->ifi_family = AF_UNSPEC;
    ifi->ifi_index = 0;
    ifi->ifi_flags = 0;
    ifi->ifi_change = 0xFFFFFFFF;

    callback(&req.header, ifi, sizeof(req.buffer));
    send_message(req);

    NetlinkMessage response;
    bool ack_received = false;
    int max_recv_attempts = 10;

    for (int i = 0; i < max_recv_attempts; ++i) {
        ssize_t bytes_received = 0;
        try {
            memset(&response, 0, sizeof(response));
            receive_message(response);
            bytes_received = response.header.nlmsg_len;
        } catch (const std::runtime_error& e) {
            continue;
        }

        for (struct nlmsghdr* nlh = &response.header; NLMSG_OK(nlh, bytes_received);
             nlh = NLMSG_NEXT(nlh, bytes_received)) {
            if (static_cast<int>(nlh->nlmsg_pid) != getpid() || nlh->nlmsg_seq != req.header.nlmsg_seq) {
                continue;
            }

            if (nlh->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr* err = (struct nlmsgerr*)NLMSG_DATA(nlh);
                if (err->error == 0) {
                    ack_received = true;
                    return;
                } else {
                    throw std::runtime_error(std::string(strerror(-err->error)) +
                                             " (error code: " + std::to_string(err->error) + ")");
                }
            } else if (nlh->nlmsg_type == msg_type) {
                ack_received = true;
                return;
            }
        }
    }

    if (!ack_received) {
        throw std::runtime_error("No valid ACK");
    }
}

bool vcan_netlink_manager::send_netlink_request(int msg_type, int flags, cb_type const& callback,
                                                std::string const& interface_name, std::string const& caller) {
    try {
        send_netlink_request_impl(msg_type, flags, callback);
        return true;
    } catch (std::exception& e) {
        std::cerr << "[VCAN Netlink] (" << interface_name << ") '" << caller << "' -> " << e.what() << std::endl;

    } catch (...) {
        std::cerr << "[VCAN Netlink] (" << interface_name << ") '" << caller << "' -> Unexpected exception"
                  << std::endl;
    }
    return false;
}

bool vcan_netlink_manager::create(std::string const& interface_name) {
    return send_netlink_request(
        RTM_NEWLINK, NLM_F_CREATE | NLM_F_EXCL,
        [&](struct nlmsghdr* nlh, struct ifinfomsg* ifi, int max_buf_len) {
            ifi->ifi_type = AF_NETROM;

            add_attribute(nlh, max_buf_len, IFLA_IFNAME, interface_name.c_str(), interface_name.length() + 1);

            struct rtattr* linkinfo_attr = (struct rtattr*)(((char*)nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
            linkinfo_attr->rta_type = IFLA_LINKINFO;
            linkinfo_attr->rta_len = RTA_LENGTH(0);

            nlh->nlmsg_len = NLMSG_ALIGN(nlh->nlmsg_len) + RTA_ALIGN(RTA_LENGTH(0));

            const char* vcan_kind = "vcan";
            add_attribute(nlh, max_buf_len, IFLA_INFO_KIND, vcan_kind, strlen(vcan_kind) + 1);

            linkinfo_attr->rta_len = (char*)nlh + nlh->nlmsg_len - (char*)linkinfo_attr;
        },
        interface_name, "create");
}

bool vcan_netlink_manager::bring_up(std::string const& interface_name) {
    return send_netlink_request(
        RTM_NEWLINK, 0,
        [&](struct nlmsghdr* nlh, struct ifinfomsg* ifi, int max_buf_len) {
            add_attribute(nlh, max_buf_len, IFLA_IFNAME, interface_name.c_str(), interface_name.length() + 1);
            ifi->ifi_flags = IFF_UP;
            ifi->ifi_change = IFF_UP;
        },
        interface_name, "bringUp");
}

bool vcan_netlink_manager::bring_down(std::string const& interface_name) {
    return send_netlink_request(
        RTM_NEWLINK, 0,
        [&](struct nlmsghdr* nlh, struct ifinfomsg* ifi, int max_buf_len) {
            add_attribute(nlh, max_buf_len, IFLA_IFNAME, interface_name.c_str(), interface_name.length() + 1);
            ifi->ifi_flags = 0;
            ifi->ifi_change = IFF_UP;
        },
        interface_name, "bringDown");
}

bool vcan_netlink_manager::destroy(std::string const& interface_name) {
    return send_netlink_request(
        RTM_DELLINK, 0,
        [&](struct nlmsghdr* nlh, struct ifinfomsg*, int max_buf_len) {
            add_attribute(nlh, max_buf_len, IFLA_IFNAME, interface_name.c_str(), interface_name.length() + 1);
        },
        interface_name, "destroy");
}

} // namespace everest::lib::io::netlink
