// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <ifaddrs.h>
#include <iostream>
#include <linux/if_packet.h>
#include <linux/if_tun.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <everest/io/event/unique_fd.hpp>
#include <everest/io/socket/socket.hpp>

namespace {
// a simple raii wrapper, which will call the given c-like deleter
// function on its supplied pointer at the end of its lifetime
template <typename T, void (*deleter)(T*)> class handle_disposer {
public:
    handle_disposer(T* pointer) : m_pointer(pointer){};

    handle_disposer(const handle_disposer&) = delete;
    handle_disposer(handle_disposer&&) = delete;
    handle_disposer& operator=(const handle_disposer&) = delete;
    handle_disposer& operator=(handle_disposer&&) = delete;
    ~handle_disposer() {
        deleter(m_pointer);
    }

private:
    T* m_pointer;
};

long get_remaining_time_ms(std::chrono::steady_clock::time_point const& init_time, uint32_t timeout_ms) {
    auto time_delta = std::chrono::steady_clock::now() - init_time;
    auto time_delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_delta).count();
    auto remaining_ms = timeout_ms - time_delta_ms;
    return remaining_ms;
}

void set_ifr_name(struct ifreq& ifr, std::string const& dev_name) {
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, dev_name.c_str(),
            std::min(dev_name.size(), static_cast<std::string::size_type>(IFNAMSIZ - 1)));
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
}

std::string build_errno_string(std::string const& msg) {
    std::stringstream str;
    str << msg << ": " << strerror(errno) << " (" << errno << ")";
    return str.str();
}

} // namespace

namespace everest::lib::io {
namespace socket {

//
// socket.hpp implementations
//

event::unique_fd open_udp_server_socket(std::uint16_t port) {
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    addrinfo* servinfo;

    const auto err = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &servinfo);
    if (err) {
        throw std::runtime_error("Failed to resolve endpoint (getaddrinfo): " + std::string(gai_strerror(err)));
    };

    handle_disposer<addrinfo, freeaddrinfo> addrinfo_disposer(servinfo);

    // open the first possible socket
    for (auto* p = servinfo; p != NULL; p = p->ai_next) {
        const auto socket_fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        set_reuse_address(socket_fd);
        if (socket_fd == -1) {
            continue;
        }

        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            continue;
        }

        return event::unique_fd{socket_fd};
    }
    throw std::runtime_error(std::string("Could not open a socket for localhost:") + std::to_string(port));
}

event::unique_fd open_udp_client_socket(const std::string& host, std::uint16_t port) {
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    addrinfo* servinfo;

    const auto err = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &servinfo);
    if (err) {
        throw std::runtime_error("Failed to resolve endpoint (getaddrinfo): " + std::string(gai_strerror(err)));
    };

    handle_disposer<addrinfo, freeaddrinfo> addrinfo_disposer(servinfo);

    // open the first possible socket
    for (auto* p = servinfo; p != NULL; p = p->ai_next) {
        const auto socket_fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (socket_fd == -1)
            continue;

        if (-1 == ::connect(socket_fd, p->ai_addr, p->ai_addrlen)) {
            close(socket_fd);
            continue;
        }

        return event::unique_fd{socket_fd};
    }

    throw std::runtime_error(std::string("Could not open a socket for ") + host + ":" + std::to_string(port));
}

event::unique_fd open_tcp_socket_with_timeout(const std::string& host, std::uint16_t port, unsigned int timeout_ms) {
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* servinfo;

    const auto err = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &servinfo);
    if (err) {
        throw std::runtime_error("Failed to resolve endpoint (getaddrinfo): " + std::string(gai_strerror(err)));
    };

    handle_disposer<addrinfo, freeaddrinfo> addrinfo_disposer(servinfo);

    // open the first possible socket
    for (auto* p = servinfo; p != NULL; p = p->ai_next) {
        const auto socket_fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (socket_fd == -1)
            continue;

        if (-1 == connect_with_timeout(socket_fd, p->ai_addr, p->ai_addrlen, timeout_ms)) {
            //        if (-1 == ::connect(socket_fd, p->ai_addr, p->ai_addrlen)) {
            close(socket_fd);
            continue;
        }

        return event::unique_fd{socket_fd};
    }

    throw std::runtime_error(std::string("Could not open a socket for ") + host + ":" + std::to_string(port));
}

event::unique_fd open_tcp_socket(const std::string& host, std::uint16_t port) {
    struct addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* servinfo;

    const auto err = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &servinfo);
    if (err) {
        throw std::runtime_error("Failed to resolve endpoint (getaddrinfo): " + std::string(gai_strerror(err)));
    };

    handle_disposer<addrinfo, freeaddrinfo> addrinfo_disposer(servinfo);

    // open the first possible socket
    for (auto* p = servinfo; p != NULL; p = p->ai_next) {
        const auto socket_fd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (socket_fd == -1)
            continue;

        if (-1 == connect(socket_fd, p->ai_addr, p->ai_addrlen)) {
            close(socket_fd);
            continue;
        }

        return event::unique_fd{socket_fd};
    }

    throw std::runtime_error(std::string("Could not open a socket for ") + host + ":" + std::to_string(port));
}

event::unique_fd open_raw_promiscuous_socket(std::string const& if_name) {
    auto const socket_fd = ::socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (socket_fd == -1) {
        auto msg = build_errno_string("Could not open raw socket");
        throw std::runtime_error(msg);
    }

    int const if_index = if_nametoindex(if_name.c_str());
    if (if_index == 0) {
        close(socket_fd);
        throw std::runtime_error("Invalid interface name: " + if_name);
    }

    int ignore_out = 1;
    if (setsockopt(socket_fd, SOL_PACKET, PACKET_IGNORE_OUTGOING, &ignore_out, sizeof(ignore_out)) == -1) {
        auto msg = build_errno_string("Could not set PACKET_IGNORE_OUTGOIG for " + if_name);
        close(socket_fd);
        throw std::runtime_error(msg);
    }

    struct sockaddr_ll sll;
    std::memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_index;
    sll.sll_protocol = htons(ETH_P_ALL);

    if (::bind(socket_fd, (struct sockaddr*)&sll, sizeof(sll)) == -1) {
        auto msg = build_errno_string("Could not bind socket to " + if_name);
        close(socket_fd);
        throw std::runtime_error(msg);
    }

    struct packet_mreq mreq;
    std::memset(&mreq, 0, sizeof(mreq));
    mreq.mr_ifindex = if_nametoindex(if_name.c_str());
    mreq.mr_type = PACKET_MR_PROMISC;
    auto error = setsockopt(socket_fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (error) {
        auto msg = build_errno_string("Cannot set 'PROMISCUOUS MODE' for '" + if_name + "'");
        close(socket_fd);
        throw std::runtime_error(msg);
    }
    return event::unique_fd(socket_fd);
}

void enable_tcp_no_delay(int fd) {
    socklen_t enable = 1;
    auto err = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));

    if (err) {
        throw std::runtime_error("Failed to setsockopt(TCP_NODELAY)");
    }
}

void set_non_blocking(int fd) {
    const auto err = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    if (err) {
        throw std::runtime_error("Failed to fcntl(O_NONBLOCK)");
    }
}

void set_keepalive(int fd, bool enable) {
    socklen_t value = static_cast<int>(enable);
    auto err = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(value));

    if (err) {
        throw std::runtime_error("Failed to setsockopt(SO_KEEPALIVE)");
    }
}

void set_tcp_keepalive(int fd, uint32_t count, uint32_t idle_s, uint32_t intval_s) {
    set_keepalive(fd, true);
    auto err = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(TCP_KEEPCNT)");
    }
    err = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle_s, sizeof(idle_s));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(TCP_KEEPIDLE)");
    }
    err = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &intval_s, sizeof(intval_s));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(TCP_INTVL)");
    }
}

void set_tcp_user_timeout(int fd, uint32_t to_ms) {
    auto err = setsockopt(fd, IPPROTO_TCP, TCP_USER_TIMEOUT, &to_ms, sizeof(to_ms));

    if (err) {
        throw std::runtime_error("Failed to setsockopt(TCP_USER_TIMEOUT)");
    }
}

void set_socket_send_buffer_to_min(int fd) {
    int sndbuf = 0;
    auto err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));

    if (err) {
        throw std::runtime_error("Failed to setsockopt(SO_SNDBUF)");
    }
}

int get_pending_error(int fd) {
    int error = 0;
    socklen_t errlen = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)&error, &errlen)) {
        return errno;
    }
    return error;
}

bool is_tcp_socket_alive(int sock_fd) {
    char buffer[1];
    ssize_t bytes_peeked = recv(sock_fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT);

    if (bytes_peeked == -1) {
        return errno == EAGAIN || errno == EWOULDBLOCK;
    }
    return bytes_peeked not_eq 0;
}

void bring_device_up(int sock_fd, std::string const& dev_name) {
    struct ifreq ifr;
    set_ifr_name(ifr, dev_name);

    if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1) {
        throw std::runtime_error(std::string("Failed to get device flags for ") + dev_name + ": " + strerror(errno));
    }

    ifr.ifr_flags |= IFF_UP;

    if (ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1) {
        throw std::runtime_error(std::string("Failed to bring device UP: ") + dev_name + ": " + strerror(errno));
    }
}

void set_ip_address(int sock_fd, std::string const& dev_name, std::string const& ip_address_str) {
    struct ifreq ifr;
    set_ifr_name(ifr, dev_name);

    struct sockaddr_in* sin = reinterpret_cast<struct sockaddr_in*>(&ifr.ifr_addr);
    sin->sin_family = AF_INET;

    if (inet_pton(AF_INET, ip_address_str.c_str(), &sin->sin_addr) <= 0) {
        throw std::runtime_error(std::string("Invalid IP address format: ") + ip_address_str +
                                 " (Error: " + strerror(errno) + ")");
    }

    if (ioctl(sock_fd, SIOCSIFADDR, &ifr) == -1) {
        throw std::runtime_error(std::string("Failed to set IP address ") + ip_address_str + " on " + dev_name + ": " +
                                 strerror(errno));
    }
}

void set_netmask(int sock_fd, std::string const& dev_name, std::string const& netmask_str) {
    struct ifreq ifr;
    set_ifr_name(ifr, dev_name);

    struct sockaddr_in* sin = reinterpret_cast<struct sockaddr_in*>(&ifr.ifr_netmask);
    sin->sin_family = AF_INET;

    if (inet_pton(AF_INET, netmask_str.c_str(), &sin->sin_addr) <= 0) {
        throw std::runtime_error(std::string("Invalid netmask format: ") + netmask_str + " (Error: " + strerror(errno) +
                                 ")");
    }

    if (ioctl(sock_fd, SIOCSIFNETMASK, &ifr) == -1) {
        throw std::runtime_error(std::string("Failed to set netmask ") + netmask_str + " on " + dev_name + ": " +
                                 strerror(errno));
    }
}

void set_mtu(int sock_fd, std::string const& dev_name, int mtu) {
    struct ifreq ifr;
    set_ifr_name(ifr, dev_name);
    ifr.ifr_mtu = mtu;

    if (ioctl(sock_fd, SIOCSIFMTU, &ifr) == -1) {
        throw std::runtime_error(std::string("Failed to set MTU to ") + std::to_string(mtu) + " on " + dev_name + ": " +
                                 strerror(errno));
    }
}

event::unique_fd create_tap_device(std::string const& desired_device_name) {
    event::unique_fd tap_fd(::open("/dev/net/tun", O_RDWR));
    if (!tap_fd.is_fd()) {
        throw std::runtime_error(std::string("Unable to open /dev/net/tun: ") + strerror(errno));
    }
    if (desired_device_name.empty()) {
        throw std::runtime_error("create_tap_device: No device name provided. ");
    }

    struct ifreq ifr;
    set_ifr_name(ifr, desired_device_name);
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (ioctl(tap_fd, TUNSETIFF, &ifr) == -1) {
        if (errno == EEXIST) {
            throw std::runtime_error(std::string("TAP device name is not available: '") + desired_device_name +
                                     "': " + strerror(errno));

        } else {
            throw std::runtime_error(std::string("Unable to create TAP device '") + desired_device_name +
                                     "': " + strerror(errno));
        }
    }

    return tap_fd;
}

event::unique_fd open_control_socket() {
    event::unique_fd sock_fd(::socket(AF_INET, SOCK_DGRAM, 0));
    if (!sock_fd.is_fd()) {
        throw std::runtime_error(std::string("Unable to open control socket: ") + strerror(errno));
    }
    return sock_fd;
}

bool configure_tap_device_properties(int tap_fd, std::string const& dev_name, std::string const& ip_addr_str,
                                     std::string const& netmask_str, int mtu) {
    event::unique_fd control_sock_fd = open_control_socket();

    try {
        bring_device_up(control_sock_fd, dev_name);
        set_ip_address(control_sock_fd, dev_name, ip_addr_str);
        set_netmask(control_sock_fd, dev_name, netmask_str);
        set_mtu(control_sock_fd, dev_name, mtu);
        set_non_blocking(tap_fd);
    } catch (...) {
        return false;
    }
    return true;
}

int get_socket_flags(int fd) {
    int result = fcntl(fd, F_GETFL, 0);
    if (result < 0) {
        throw std::runtime_error(std::string("Get flags for socket Error: ") + strerror(errno) + ")");
    }
    return result;
}

void set_socket_flags(int fd, int flags) {
    int result = fcntl(fd, F_SETFL, flags);
    if (result < 0) {
        throw std::runtime_error(std::string("Set flags for socket Error: ") + strerror(errno) + ")");
    }
}

int poll_for_timeout_once(int fd, uint32_t timeout_ms) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLOUT;
    struct pollfd pfds[] = {pfd};
    return poll(pfds, 1, timeout_ms);
}

int poll_until_timeout(int fd, uint32_t timeout_ms) {
    int result = 0;
    auto init_time = std::chrono::steady_clock::now();
    do {
        auto remaining_ms = get_remaining_time_ms(init_time, timeout_ms);
        if (remaining_ms < 0) {
            result = 0;
            break;
        }
        result = poll_for_timeout_once(fd, remaining_ms);
        if (result > 0) {
            errno = get_pending_error(fd);
            if (errno != 0) {
                result = -1;
            }
        }
    } while (result == -1 && errno == EINTR);
    if (result == 0) {
        errno = ETIMEDOUT;
        result = -1;
    }
    return result;
}

int connect_with_timeout_impl(int sockfd, const struct sockaddr* addr, socklen_t addrlen, unsigned int timeout_ms) {
    int result = 0;
    int original_flags = get_socket_flags(sockfd);
    set_non_blocking(sockfd);
    if (connect(sockfd, addr, addrlen) < 0) {
        if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS)) {
            result = -1;
        } else {
            result = poll_until_timeout(sockfd, timeout_ms);
        }
    }
    set_socket_flags(sockfd, original_flags);
    return result;
}

int connect_with_timeout(int fd, const struct sockaddr* addr, uint32_t addrlen, unsigned int timeout_ms) {
    auto result = 0;
    try {
        result = connect_with_timeout_impl(fd, addr, addrlen, timeout_ms);
    } catch (...) {
        result = -1;
    }
    return result;
}

std::string get_interface_address(std::string const& name) {
    if (name.empty()) {
        return "";
    }

    struct ifaddrs* ifaddr{nullptr};
    struct ifaddrs* ifa{nullptr};
    if (getifaddrs(&ifaddr) == -1) {
        throw std::runtime_error("Cannot get interfaces: " + std::string(strerror(errno)));
    }

    handle_disposer<ifaddrs, freeifaddrs> ifaddr_disposer(ifaddr);

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET)
            continue;
        if (name == ifa->ifa_name) {
            char host[NI_MAXHOST];
            getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            return host;
            break;
        }
    }

    throw std::runtime_error("Interface: " + name + " not found or has no IPv4");
}

std::vector<if_info> get_all_interaces() {
    struct ifaddrs* ifaddr{nullptr};
    struct ifaddrs* ifa{nullptr};
    if (getifaddrs(&ifaddr) == -1) {
        throw std::runtime_error("Cannot get interfaces: " + std::string(strerror(errno)));
    }

    handle_disposer<ifaddrs, freeifaddrs> ifaddr_disposer(ifaddr);

    std::vector<if_info> interfaces;

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET)
            continue;
        interfaces.push_back({ifa->ifa_name, get_interface_address(ifa->ifa_name)});
    }
    return interfaces;
}

event::unique_fd open_udp_multicast_socket(std::string const& multicast_group, std::uint16_t port,
                                           std::string interface_address, std::string listen_address,
                                           bool reuse_address, bool reuse_port) {
    auto sock = event::unique_fd(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
    set_non_blocking(sock);
    if (reuse_address) {
        set_reuse_address(sock);
    }
    if (reuse_port) {
        set_reuse_port(sock);
    }

    bind_socket_ip4(sock, listen_address, port);
    set_udp_multicast(sock, multicast_group, interface_address);
    return sock;
}

event::unique_fd open_mdns_socket(std::string const& interface_name) {
    auto ip = get_interface_address(interface_name);
    return open_udp_multicast_socket("224.0.0.251", 5353, ip, "0.0.0.0", true, true);
}

void set_reuse_address(int fd) {
    socklen_t enable = 1;
    auto err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(SO_REUSEADDRE)");
    }
}

void set_reuse_port(int fd) {
#ifdef SO_REUSEPORT
    socklen_t enable = 1;
    auto err = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(SO_REUSEADDRE)");
    }
#endif
}

void bind_socket_ip4(int fd, std::string const& ip, std::uint16_t port) {
    sockaddr_in local_bind_addr{};
    int result = inet_pton(AF_INET, ip.c_str(), &local_bind_addr.sin_addr.s_addr);
    if (result not_eq 1) {
        throw std::runtime_error("Invalid IP address: " + ip);
    }

    local_bind_addr.sin_family = AF_INET;
    local_bind_addr.sin_port = htons(port);
    local_bind_addr.sin_addr.s_addr = ip_to_s_addr(ip);
    if (bind(fd, (struct sockaddr*)&local_bind_addr, sizeof(local_bind_addr)) < 0) {
        throw std::runtime_error("Cannot bind socket to " + ip + ":" + std::to_string(port) + " -> " + strerror(errno));
    }
}

void set_udp_multicast(int fd, std::string const& multicast_ip, std::string const& interface_ip) {
    struct in_addr out_addr;
    out_addr.s_addr = inet_addr(interface_ip.c_str());

    auto err = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&out_addr, sizeof(out_addr));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(IP_MULTICAST_IF)");
    }

    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = ip_to_s_addr(multicast_ip);
    mreq.imr_interface.s_addr = ip_to_s_addr(interface_ip);
    err = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err) {
        throw std::runtime_error("Failed to setsockopt(IP_ADD_MEMBERSHIP)");
    }
}

std::uint32_t ip_to_s_addr(std::string const& ip) {
    std::uint32_t s_addr;
    int result = inet_pton(AF_INET, ip.c_str(), &s_addr);
    if (result not_eq 1) {
        throw std::runtime_error("Invalid IP address: " + ip);
    }
    return s_addr;
}

} // namespace socket

} // namespace everest::lib::io
