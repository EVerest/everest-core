// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/tcp/tcp_client.hpp>
#include <everest/io/tcp/tcp_socket.hpp>
#include <iostream>
#include <sstream>
#include <thread>

using namespace everest::lib::io::tcp;
using namespace everest::lib::io::utilities;
using namespace everest::lib::io::event;
using namespace std::chrono_literals;

using payload_type = everest::lib::io::event::fd_event_client<tcp_socket>::payload;
using generic_tcp = typename tcp_client::interface;

auto to_string_data(const payload_type& d) {
    return std::string(d.begin(), d.end());
}

std::ostream& operator<<(std::ostream& os, payload_type const& p) {
    os << "[TCP ( " << p.size() << " )]: " << to_string_data(p) << " ";
    return os;
}

void rx_handler(payload_type const& payload, generic_tcp& client) {
    std::cout << "MSG: " << payload << std::endl;
    client.tx(payload);
}

payload_type make_message(int val) {
    std::stringstream ss;
    ss << "message #" << std::to_string(val) << std::flush;
    auto tmp = ss.str();
    return {tmp.begin(), tmp.end()};
}

tcp_client::cb_error make_error_cb(tcp_client& client) {
    return [&](int error, std::string const& msg) {
        std::cerr << "ERROR ( " << error << " ): " << msg << std::endl;
        if (error) {
            std::this_thread::sleep_for(1s);
            client.reset();
        }
    };
}

tcp_client::cb_rx make_rx_callback(tcp_client& client) {
    return [&](payload_type const& payload, generic_tcp& repl) { rx_handler(payload, repl); };
}

bool parse_args(int argc, char* argv[], std::string& remote, uint16_t& port) {
    if (argc != 3) {
        std::cout << "\nUSAGE: test_tcp_client {remote} {port}" << std::endl;
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
    std::cout << "tcp echo test client" << std::endl;
    uint16_t port;
    std::string remote;

    if (not parse_args(argc, argv, remote, port)) {
        return 1;
    }

    std::cout << "Connecting to ->  " << remote << ":" << port << std::endl;
    tcp_client client(remote, port);
    client.set_error_handler(make_error_cb(client));
    client.set_rx_handler(make_rx_callback(client));

    std::cout << "TCP socket ok? -> " << not client.on_error() << std::endl;

    fd_event_handler ev_handler;

    ev_handler.register_event_handler(&client);
    while (true) {
        ev_handler.poll();
    }

    return 0;
}
