#include "slac/slac.hpp"
#include <everest/io/socket/socket.hpp>
#include <everest/slac/slac_socket.hpp>

#include <cstring>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {
using namespace everest::lib::io;

std::string build_errno_string(std::string const& msg) {
    std::stringstream str;
    str << msg << ": " << strerror(errno) << " (" << errno << ")";
    return str.str();
}

event::unique_fd open_raw_socket(std::string const& if_name) {
    const auto protocol = htons(slac::defs::ETH_P_HOMEPLUG_GREENPHY);
    auto const socket_fd = ::socket(AF_PACKET, SOCK_RAW, protocol);
    if (socket_fd == -1) {
        auto msg = build_errno_string("Could not open raw socket");
        throw std::runtime_error(msg);
    }

    int const if_index = if_nametoindex(if_name.c_str());
    if (if_index == 0) {
        close(socket_fd);
        throw std::runtime_error("Invalid interface name: " + if_name);
    }

    struct sockaddr_ll sll;
    std::memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_index;
    sll.sll_protocol = protocol;
    sll.sll_halen = ETH_ALEN;

    if (::bind(socket_fd, (struct sockaddr*)&sll, sizeof(sll)) == -1) {
        auto msg = build_errno_string("Could not bind socket to " + if_name);
        close(socket_fd);
        throw std::runtime_error(msg);
    }

    return event::unique_fd(socket_fd);
}

} // namespace

namespace everest::lib::slac {

bool slac_socket::open(std::string const& if_name) {
    try {
        auto socket = open_raw_socket(if_name);
        socket::set_non_blocking(socket);
        m_fd = std::move(socket);
        auto result = socket::get_pending_error(m_fd) == 0;
        m_mac = socket::get_mac_address(m_fd, if_name);
        return result;
    } catch (...) {
    }
    return false;
}

void slac_socket::close() {
    m_fd.close();
}

bool slac_socket::tx(PayloadT const& payload) {
    if (not is_open()) {
        return false;
    }
    auto status = ::send(m_fd, payload.get_raw_message_ptr(), payload.get_raw_msg_len(), 0);
    if (status == -1) {
        return false;
    }
    return true;
}

bool slac_socket::rx(PayloadT& buffer) {
    if (not is_open()) {
        return false;
    }
    auto status = ::recv(m_fd, buffer.get_raw_message_ptr(), ETH_FRAME_LEN, 0);
    if (status <= 0) { // -1 is an error, 0 is a connection closed by the peer
        return false;
    }
    return true;
}

int slac_socket::get_fd() const {
    return m_fd;
}

int slac_socket::get_error() const {
    return socket::get_pending_error(m_fd);
}

bool slac_socket::is_open() const {
    return m_fd.is_fd();
}

slac_socket::MacAddress slac_socket::get_mac_address() const {
    return m_mac;
}
} // namespace everest::lib::slac
