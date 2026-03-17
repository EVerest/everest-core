// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <array>
#include <cstdint>
#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/event/unique_fd.hpp>
#include <functional>
#include <slac/slac.hpp>
#include <string>

namespace everest::lib::slac {

class slac_socket {
public:
    using PayloadT = ::slac::messages::HomeplugMessage;
    using MacAddress = std::array<std::uint8_t, 6>;

    slac_socket() = default;
    ~slac_socket() = default;

    bool open(std::string const& if_name);
    void close();

    bool tx(PayloadT const& payload);
    bool rx(PayloadT& buffer);

    int get_fd() const;
    int get_error() const;
    bool is_open() const;
    MacAddress get_mac_address() const;
private:
    io::event::unique_fd m_fd;
    MacAddress m_mac;
};

using slac_client = io::event::fd_event_client<slac_socket>::type;

} // namespace everest::lib::slac
