// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/udp/udp_socket.hpp>
#include <iostream>
#include <thread>

using namespace everest::lib::io::udp;
using namespace std::chrono_literals;

auto to_string_data(const udp_payload& d) {
    return std::string(d.buffer.begin(), d.buffer.end());
}

std::ostream& operator<<(std::ostream& os, udp_payload const& p) {
    os << "[UDP ( " << p.size() << " )]: " << to_string_data(p) << " ";
    return os;
}

int main() {
    std::cout << "udp server" << std::endl;
    udp_client_socket socket;

    socket.open_as_server(7766);

    std::cout << "UDP socket open? -> " << socket.is_open() << std::endl;

    while (socket.is_open()) {
        udp_payload payload;
        auto source = socket.rx(payload);

        if (source) {
            std::cout << payload << std::endl;
            socket.tx(payload);
        } else {
            std::cout << "no value" << std::endl;
        }
        std::this_thread::sleep_for(500ms);
    }

    return 0;
}
