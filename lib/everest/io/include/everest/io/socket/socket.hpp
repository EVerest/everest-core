// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <cstdint>
#include <string>

#include <everest/io/event/unique_fd.hpp>

namespace everest::lib::io::socket {

/**
 * @brief Open a UDP socket in server mode
 * @param[in] port The port to listen to
 * @return The managed file descriptor of the socket
 * @throws std::runtime_error if the operation fails.
 */
event::unique_fd open_udp_server_socket(std::uint16_t port);

/**
 * @brief Open a UDP socket in client mode
 * @param[in] host The host to connect to
 * @param[in] port The port to listen to
 * @return The managed file descriptor of the socket
 * @throws std::runtime_error if the operation fails.
 */
event::unique_fd open_udp_client_socket(std::string const& host, std::uint16_t port);

/**
 * @brief Open a TCP socket in client mode
 * @param[in] host The host to connect to
 * @param[in] port The port to listen to
 * @return The managed file descriptor of the socket
 * @throws std::runtime_error if the operation fails.
 */
event::unique_fd open_tcp_socket(const std::string& host, std::uint16_t port);

/**
 * @brief Enable <a href="https://man7.org/linux/man-pages/man7/tcp.7.html">TCP_NODELAY</a> on a socket
 * @details   If set, disable the Nagle algorithm.  This means that
 *            segments are always sent as soon as possible, even if there
 *            is only a small amount of data.  When not set, data is
 *            buffered until there is a sufficient amount to send out,
 *            thereby avoiding the frequent sending of small packets,
 *            which results in poor utilization of the network.  This
 *            option is overridden by TCP_CORK; however, setting this
 *            option forces an explicit flush of pending output, even if
 *            TCP_CORK is currently set.
 * @param[in] fd Filedescriptor of the socket
 * @throws std::runtime_error if the operation fails.
 */
void enable_tcp_no_delay(int fd);

/**
 * @brief Set the port <a href="https://man7.org/linux/man-pages/man3/errno.3.html">non blocking</a>
 * @details Operations on this socket will not block but instead return an error EAGAIN or EWOULDBLOCK
 * @param[in] fd Filedescriptor of the socket
 * @throws std::runtime_error if the operation fails.
 */
void set_non_blocking(int fd);

/**
 * @brief Set the <a href="https://man7.org/linux/man-pages/man7/socket.7.html">keepalive</a> option.
 * @details   Enable sending of keep-alive messages on connection-oriented sockets.
 * @param[in] fd Description
 * @param[in] enable enabled if 'true'
 * @throws std::runtime_error if the operation fails.
 */
void set_keepalive(int fd, bool enable);

/**
 * @brief Set the parameters for <a href="https://man7.org/linux/man-pages/man7/tcp.7.html">TCP keepalive</a>
 * @details This implicitly calls @p set_keepalive
 * @param[in] fd Description
 * @param[in] count  The maximum number of keepalive probes TCP should send
 *                   before dropping the connection.
 * @param[in] idle_s The time (in seconds) the connection needs to remain idle
 *                   before TCP starts sending keepalive probes.
 * @param[in] intval_s The time (in seconds) between individual keepalive probes.
 * @throws std::runtime_error if the operation fails.
 */
void set_tcp_keepalive(int fd, uint32_t count, uint32_t idle_s, uint32_t intval_s);

/**
 * @brief Sets the maximum transmission timeout befor error
 * @details
 * Increasing user timeouts allows a TCP connection to survive
 * extended periods without end-to-end connectivity.
 * Decreasing user timeouts allows applications to "fail
 * fast", if so desired.  Otherwise, failure may take up to 20
 * minutes with the current system defaults in a normal WAN environment.
 * @param[in] fd Description
 * @param[in] to_ms timeout in milliseconds
 * @throws std::runtime_error if the operation fails.
 */
void set_tcp_user_timeout(int fd, uint32_t to_ms);

/**
 * @brief Set socket send buffer to minimum value
 * @details
 * This transforms ENOBUFS to EAGAIN and allows to wait for
 * the socket with select/poll/epoll to be really ready to write
 * This is especially helpfull for
 * <a href="https://rtime.felk.cvut.cz/can/socketcan-qdisc-final.pdf">socket_can</a>.
 * @param[in] fd Filedescriptor of the socket
 * @throws std::runtime_error if the operation fails.
 */
void set_socket_send_buffer_to_min(int fd);

/**
 * @brief Get the pending errors from a socket
 * @details Gets <a href="https://man7.org/linux/man-pages/man7/socket.7.html">SO_ERROR</a>
 * with <a href="https://man7.org/linux/man-pages/man2/getsockopt.2.html">getsockopt</a>
 * @param[in] fd Description
 * @return Description
 */
int get_pending_error(int fd);

/**
 * @brief Checks if a TCP socket is still connected and operational without consuming any incoming data.
 *
 * @details By peeking into the socket's receive buffer the connection status is determinded.
 * The function is guaranteed to be non blocking and not removing any data from the buffer
 * by using `MSG_PEEK` and `MSG_DONTWAIT` flags.
 * @param fd The file descriptor of the TCP socket to check.
 * @return `true` if the socket is considered alive, `false` otherwise.
 */
bool is_tcp_socket_alive(int fd);

/**
 * @brief Brings a network device up (activates it).
 * @param sock_fd The file descriptor of a control socket (AF_INET, SOCK_DGRAM).
 * @param dev_name The name of the network device (e.g., "tap0", "eth0").
 * @throws std::runtime_error if the operation fails.
 */
void bring_device_up(int sock_fd, std::string const& dev_name);

/**
 * @brief Sets the IP address for a network device.
 * @param sock_fd The file descriptor of a control socket (AF_INET, SOCK_DGRAM).
 * @param dev_name The name of the network device.
 * @param ip_address_str The IP address string (e.g., "192.168.1.1").
 * @throws std::runtime_error if the operation fails or IP address is invalid.
 */
void set_ip_address(int sock_fd, std::string const& dev_name, std::string const& ip_address_str);

/**
 * @brief Sets the netmask for a network device.
 * @param sock_fd The file descriptor of a control socket (AF_INET, SOCK_DGRAM).
 * @param dev_name The name of the network device.
 * @param netmask_str The netmask string (e.g., "255.255.255.0").
 * @throws std::runtime_error if the operation fails or netmask is invalid.
 */
void set_netmask(int sock_fd, std::string const& dev_name, std::string const& netmask_str);

/**
 * @brief Sets the MTU (Maximum Transmission Unit) for a network device.
 * @param sock_fd The file descriptor of a control socket (AF_INET, SOCK_DGRAM).
 * @param dev_name The name of the network device.
 * @param mtu The MTU value to set.
 * @throws std::runtime_error if the operation fails.
 */
void set_mtu(int sock_fd, std::string const& dev_name, int mtu);

/**
 * @brief Creates a new TAP device with a *specific* desired name.
 * This function opens /dev/net/tun and performs the TUNSETIFF ioctl.
 * It will fail if the desired_device_name is empty or already in use.
 *
 * @param desired_device_name The exact name for the TAP device (e.g., "mytap0").
 * Must NOT be empty.
 * @return The managed filedescriptor for the device.
 * @throws std::runtime_error if operation fails or if desired_device_name is empty.
 */
event::unique_fd create_tap_device(std::string const& desired_device_name);

/**
 * @brief Opens a control socket for network interface configuration.
 * This socket is used to issue SIOC* ioctl commands.
 * @return An event::unique_fd for the opened control socket.
 * @throws std::runtime_error if the operation fails.
 */
event::unique_fd open_control_socket();

/**
 * @brief Configures a previously created TAP device with IP, netmask, MTU, and non-blocking mode.
 * This function now internally creates and manages the control socket.
 *
 * @param tap_fd The file descriptor of the TAP device.
 * @param dev_name The actual name of the TAP device.
 * @param ip The IP address to assign (e.g., "192.168.1.1").
 * @param netmask The netmask to assign (e.g., "255.255.255.0").
 * @param mtu The MTU value to set (defaults to 1500).
 * @return bool on success, false otherwise
 */
bool configure_tap_device_properties(int tap_fd, std::string const& dev_name, std::string const& ip,
                                     std::string const& netmask, int mtu = 1500);

/**
 * @brief Get the flags of a socket
 * @param[in] fd The file descriptor of a socket
 * @return The flags of the socket
 * @throws std::runtime_error if the flags cannot be read
 */
int get_socket_flags(int fd);

/**
 * @brief Set the flags of a socket
 * @param[in] fd The file descirptor of a socket
 * @param[in] flags The flags to be set
 * @throws std::runtime_error if the flags cannot be set
 */
void set_socket_flags(int fd, int flags);

/**
 * @brief <a href="https://man7.org/linux/man-pages/man2/poll.2.html">poll</a> the socket until timeout
 * @details No special care is taken in the error case to check if poll was interrupted.
 * @param[in] fd The file descirptor of a socket
 * @param[in] timeout_ms The timeout for poll
 */
int poll_for_timeout_once(int fd, uint32_t timeout_ms);

/**
 * @brief <a href="https://man7.org/linux/man-pages/man2/poll.2.html">poll</a> the socket until timeout
 * @details The function checks in the error case if poll was interrupted. If it was, poll is called again with the
 * remaining portion of the timeout
 * @param[in] fd The file descirptor of a socket
 * @param[in] timeout_ms The timeout for poll
 */
int poll_until_timeout(int fd, uint32_t timeout_ms);

/**
 * @brief Connect with timeout
 * @details This function adds a timeout to
 * <a href="https://man7.org/linux/man-pages/man2/connect.2.html">connect</a>.
 * The behavior of the original function is mimicked as closely as possible by
 * forwarding the original 'errno' extended with a possible 'ETIMEDOUT'
 * @param[in] fd The file descriptor of a socket to be connected
 * @param[in] addr Address information
 * @param[in] addrlen The length of addr
 * @param[in] timeout_ms Timeout in milliseconds
 * @return '0' on success, '-1' on error. In the error case, errno is set accordingly
 */
int connect_with_timeout(int fd, const struct sockaddr* addr, uint32_t addrlen, unsigned int timeout_ms);

} // namespace everest::lib::io::socket
