// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <functional>
#include <string>

extern "C" struct nlmsghdr;
extern "C" struct ifinfomsg;

namespace everest::lib::io::netlink {

/**
 * @brief vcan_netlink_manager manages setup of
 * <a href=https://docs.kernel.org/networking/can.html>virtual CAN</a>
 * devices via <a href=https://man7.org/linux/man-pages/man7/netlink.7.html>NETLINK</a>.
 * The class is designed as a singleton.
 */
class vcan_netlink_manager {
public:
    struct NetlinkMessage;

    vcan_netlink_manager(vcan_netlink_manager const&) = delete;
    vcan_netlink_manager& operator=(vcan_netlink_manager const&) = delete;

    /**
     * @brief Creates a new virtual CAN interface.
     * @param interface_name The desired name for the new interface (e.g., "vcan0").
     * @return True on success, false otherwise
     */
    bool create(std::string const& interface_name);

    /**
     * @brief Brings a virtual CAN interface up (activates it).
     * @param interface_name The name of the interface.
     * @return True on success, false otherwise
     */
    bool bring_up(std::string const& interface_name);

    /**
     * @brief Brings a virtual CAN interface down (deactivates it).
     * @param interface_name The name of the interface.
     * @return True on success, false otherwise
     */
    bool bring_down(std::string const& interface_name);

    /**
     * @brief Destroys (deletes) a virtual CAN interface.
     * @param interface_name The name of the interface.
     * @return True on success, false otherwise
     */
    bool destroy(std::string const& interface_name);

    /**
     * @brief Access the single instance of this object.
     * @details The underlying object is created on the first call to this function.
     * @return Reference to the object instance
     */
    static vcan_netlink_manager& Instance();

private:
    using cb_type = std::function<void(nlmsghdr*, ifinfomsg*, int)>;

    vcan_netlink_manager();
    ~vcan_netlink_manager();

    int m_nl_socket_fd{-1};
    uint32_t s_netlink_seq_counter{1};

    /**
     * @brief Adds a Netlink attribute to the message buffer.
     * This helper handles both top-level and nested attributes by always appending
     * to the current end of the message and updating the overall message length.
     * The parent attribute's length must be updated manually after its nested attributes are added.
     * @param nlh The Netlink message header.
     * @param maxlen The maximum allowed length of the message buffer.
     * @param type The attribute type (e.g., IFLA_IFNAME, IFLA_INFO_KIND).
     * @param data Pointer to the attribute data.
     * @param len Length of the attribute data.
     * @throws std::runtime_error if attribute exceeds buffer maxlen.
     */
    void add_attribute(nlmsghdr* nlh, int maxlen, int type, const void* data, int len);

    /**
     * @brief Sends a Netlink message to the kernel.
     * @param msg The Netlink message to send.
     * @param flags Send flags (e.g., MSG_DONTWAIT).
     * @throws std::runtime_error if sendmsg fails.
     */
    void send_message(NetlinkMessage const& msg, int flags = 0);

    /**
     * @brief Receives a Netlink message response from the kernel.
     * @param msg Output parameter to store the received message.
     * @throws std::runtime_error if recvmsg fails or no valid message is received.
     */
    void receive_message(NetlinkMessage& msg);

    /**
     * @brief Sends a Netlink request and waits for an ACK from the kernel.
     * @param msg_type The type of Netlink message (e.g., RTM_NEWLINK).
     * @param flags Netlink message flags (e.g., NLM_F_CREATE | NLM_F_EXCL).
     * @param callback A lambda or function to populate the `ifinfomsg` and add `rtattr`s.
     * @throws std::runtime_error if the operation fails or ACK is not received.
     */
    void send_netlink_request_impl(int msg_type, int flags, cb_type const& callback);

    /**
     * @brief Wraps exception handling for \p send_netlink_request_impl
     * @param msg_type The type of Netlink message (e.g., RTM_NEWLINK).
     * @param flags Netlink message flags (e.g., NLM_F_CREATE | NLM_F_EXCL).
     * @param callback A lambda or function to populate the `ifinfomsg` and add `rtattr`s.
     * @param interface_name For error reporting.
     * @param[in] caller The name of the caller
     * @return True on success, false otherwise
     */
    bool send_netlink_request(int msg_type, int flags, cb_type const& callback, std::string const& interface_name,
                              std::string const& caller);
};

} // namespace everest::lib::io::netlink
