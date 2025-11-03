// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/udp/udp_client.hpp>
#include <everest/io/udp/udp_payload.hpp>
#include <everest/io/udp/udp_socket.hpp>
#include <everest/io/utilities/stop_watch.hpp>
#include <iostream>
#include <sstream>
#include <thread>

using namespace everest::lib::io::udp;
using namespace everest::lib::io::utilities;
using namespace everest::lib::io::event;
using namespace std::chrono_literals;

udp_payload current_message;
stop_watch message_timer;
using generic_udp = typename udp_client::interface;

auto to_string_data(const udp_payload& d) {
    return std::string(d.buffer.begin(), d.buffer.end());
}

std::ostream& operator<<(std::ostream& os, udp_payload const& p) {
    os << "[UDP ( " << p.size() << " )]: " << to_string_data(p) << " ";
    return os;
}

std::ostream& operator<<(std::ostream& os, stop_watch const& sw) {
    os << sw.lap().count() << " us";
    return os;
}

void rx_handler(udp_payload const& payload, generic_udp&) {
    if (payload == current_message) {
        std::cout << "REPLY: " << payload << message_timer << std::endl;
    }
}

udp_payload make_message(int val) {
    std::stringstream ss;
    ss << "message #" << std::to_string(val) << std::flush;
    return ss.str();
}

udp_client::cb_error make_error_cb(udp_client& client) {
    return [&](int error, std::string const& msg) {
        std::cerr << "ERROR ( " << error << " ): " << msg << std::endl;
        if (error) {
            std::this_thread::sleep_for(1s);
            client.reset();
            client.tx(make_message(-1));
        }
    };
}

udp_client::cb_rx make_rx_callback(udp_client& client) {
    return [&](udp_payload const& payload, generic_udp& repl) { rx_handler(payload, repl); };
}

fd_event_handler::event_handler_type make_timer_cb(udp_client& client) {
    return [&](fd_event_handler::event_list const& events) {
        static int counter = 0;
        current_message = make_message(++counter);
        message_timer.reset();
        client.tx(current_message);
    };
}

bool parse_args(int argc, char* argv[], std::string& remote, uint16_t& port) {
    if (argc != 3) {
        std::cout << "\nUSAGE: test_udp_client {remote} {port}" << std::endl;
        return false;
    }

    remote = argv[1];
    std::string port_str = argv[2];

    port = 0;

    try {
        auto tmp = std::stoul(port_str);
        if (tmp > std::numeric_limits<uint16_t>::max()) {
            throw "";
        }
        port = tmp;
    } catch (...) {
        std::cout << "\nERROR '" << port_str << "' is not a valid port" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "udp echo test client" << std::endl;
    uint16_t port;
    std::string remote;

    if (not parse_args(argc, argv, remote, port)) {
        return 1;
    }

    std::cout << "Connecting to ->  " << remote << ":" << port << std::endl;
    udp_client client(remote, port);
    client.set_error_handler(make_error_cb(client));
    client.set_rx_handler(make_rx_callback(client));

    std::cout << "UDP socket ok? -> " << not client.on_error() << std::endl;

    fd_event_handler ev_handler;
    timer_fd msg_timer;
    msg_timer.set_timeout(100ms);

    ev_handler.register_event_handler(client.get_poll_fd(), [&](auto&) { client.sync(); }, {poll_events::read});
    ev_handler.register_event_handler(&msg_timer, make_timer_cb(client));

    while (true) {
        ev_handler.poll();
    }

    return 0;
}
