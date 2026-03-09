#include "everest/slac/slac_event.hpp"
#include "everest/io/socket/socket.hpp"
#include <cstring>
#include <everest/util/misc/bind.hpp>
#include <iostream>
#include <linux/if_ether.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iomanip>
#include <net/if.h>
#include <optional>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

namespace everest::lib::slac {
using util::bind_obj;
using namespace std::chrono_literals;

SlacEvent::SlacEvent(std::string const& if_name) : m_connection(if_name), m_if_name(if_name) {
    m_error_timer.set_timeout(2s);

    m_connection.set_error_handler(bind_obj(&SlacEvent::handle_socket_error, this));
    m_connection.set_rx_handler(bind_obj(&SlacEvent::handle_socket_rx, this));
    try {
        m_mac_address = io::socket::get_mac_address(if_name);
    } catch (...) {
    }
}

void SlacEvent::handle_socket_error(int id, std::string const& msg) {
    m_on_error = id not_eq 0;
    if (not m_on_error) {
        m_mac_address = m_connection.get_raw_handler()->get_mac_address();
    }
}

void SlacEvent::handle_socket_rx(HomeplugMessage const& data, [[maybe_unused]] slac_client::interface& client) {
    if (m_callback) {
        m_callback(data);
    } else {
        std::cout << "\n#################### Something went wrong ##########\n" << std::endl;
    }
}

void SlacEvent::handle_error_timer() {
    if (m_on_error) {
        m_connection.reset();
    }
}

bool SlacEvent::register_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    result = handler.register_event_handler(&m_connection) && result;
    result = handler.register_event_handler(&m_error_timer, bind_obj(&SlacEvent::handle_error_timer, this)) && result;
    return result;
}

bool SlacEvent::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    result = handler.unregister_event_handler(&m_connection) && result;
    result = handler.unregister_event_handler(&m_error_timer) && result;
    return result;
}

void SlacEvent::set_callback(const HomeplugMessageHandler& callback) {
    m_callback = callback;
}

void SlacEvent::send(HomeplugMessage& msg) {
    auto raw_msg_ether_shost = msg.get_src_mac();
    if (!msg.keep_source_mac()) {
        std::memcpy(raw_msg_ether_shost, m_mac_address.data(), sizeof(m_mac_address));
    }
    m_connection.tx(msg);
}

const uint8_t* SlacEvent::get_mac_addr() {
    auto buf = m_mac_address.data();
    return m_mac_address.data();
}

} // namespace everest::lib::slac
