#include "everest/io/udp/udp_payload.hpp"
#include "everest/io/udp/udp_socket.hpp"
#include <arpa/inet.h>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/unique_fd.hpp>
#include <everest/io/mdns/mdns.hpp>
#include <everest/io/mdns/mdns_socket.hpp>
#include <everest/io/socket/socket.hpp>
#include <iostream>
#include <net/if.h>
#include <netinet/in.h>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace everest::lib::io::mdns {

/////////////////////////////////////////////////

bool mdns_socket::open(std::string const& interface) {
    auto socket = socket::open_mdns_socket(interface);
    m_owned_udp_fd = std::move(socket);

    target.addr = inet_addr("224.0.0.251");
    target.port = htons(5353);
    target.family = AF_INET;

    return socket::get_pending_error(m_owned_udp_fd) == 0;
}

bool mdns_socket::tx(PayloadT const& payload) {
    return tx_impl(payload.buffer.data(), payload.size(), target);
}

bool mdns_socket::rx(PayloadT& payload) {
    ssize_t msg_size = 0;
    auto result = rx_impl(rx_buffer.data(), rx_buffer.size(), msg_size);
    if (result) {
        payload.set_message(rx_buffer.data(), msg_size);
    }
    return result.has_value();
}

bool mdns_socket::query(std::string const& what) {
    PayloadT payload;
    payload.buffer = everest::lib::io::mdns::create_mdns_query(what);
    return tx(payload);
}

} // namespace everest::lib::io::mdns
