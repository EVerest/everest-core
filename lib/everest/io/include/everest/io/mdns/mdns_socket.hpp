

/** \file */

#pragma once

#include <array>
#include <cstdint>
#include <everest/io/event/unique_fd.hpp>
#include <everest/io/udp/udp_payload.hpp>
#include <everest/io/udp/udp_socket.hpp>
#include <functional>
#include <optional>
#include <string>

namespace everest::lib::io::mdns {

struct udp_info {
    /** Adress */
    uint32_t addr;
    /** Port */
    uint16_t port;
    /** Family */
    uint16_t family;
};

class mdns_socket : public udp::udp_socket_base {
public:
    using PayloadT = udp::udp_payload;

    mdns_socket() = default;
    ~mdns_socket() = default;

    bool open(std::string const& interface);
    bool tx(udp::udp_payload const& payload);
    bool rx(udp::udp_payload& payload);

    bool query(std::string const& what);
private:
    std::string m_remote;
    uint16_t m_port{0};
    int m_timeout_ms{0};

    std::array<uint8_t, udp::udp_payload::max_size> rx_buffer;
    udp::udp_info target;

};

} // namespace everest::lib::io::mdns
