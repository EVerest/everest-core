// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include <slac/channel.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>

#include "packet_socket.hpp"

namespace slac {

Channel::Channel() : socket(nullptr){};

bool Channel::open(const std::string& interface_name) {
    did_timeout = false;

    auto if_info = ::utils::InterfaceInfo(interface_name);
    if (!if_info.is_valid()) {
        error = if_info.get_error();
        return false;
    }

    memcpy(orig_if_mac, if_info.get_mac(), sizeof(orig_if_mac));

    socket = std::make_unique<::utils::PacketSocket>(if_info, defs::ETH_P_HOMEPLUG_GREENPHY);
    if (!socket->is_valid()) {
        error = socket->get_error();
        socket.reset();
        return false;
    }
    return true;
}

Channel::~Channel() = default;

bool Channel::read(slac::messages::HomeplugMessage& msg, int timeout) {
    did_timeout = false;
    using IOResult = ::utils::PacketSocket::IOResult;
    if (socket) {
        switch (socket->read(reinterpret_cast<uint8_t*>(msg.get_raw_message_ptr()), timeout)) {
        // FIXME (aw): this enum conversion looks ugly
        case IOResult::Failure:
            error = socket->get_error();
            return false;
        case IOResult::Timeout:
            did_timeout = true;
            return false;
        case IOResult::Ok:
            return true;
        }
    }

    error = "No IO socket available\n";
    return false;
}

bool Channel::write(slac::messages::HomeplugMessage& msg, int timeout) {
    using IOResult = ::utils::PacketSocket::IOResult;

    assert(("Homeplug message is not valid\n", msg.is_valid()));

    did_timeout = false;

    if (socket) {
        auto raw_msg_ether_shost = msg.get_src_mac();
        if (!msg.keep_source_mac()) {
            memcpy(raw_msg_ether_shost, orig_if_mac, sizeof(orig_if_mac));
        }
        switch (socket->write(msg.get_raw_message_ptr(), msg.get_raw_msg_len(), timeout)) {
        case IOResult::Failure:
            error = socket->get_error();
            return false;
        case IOResult::Timeout:
            did_timeout = true;
            return false;
        case IOResult::Ok:
            return true;
        }
    }

    error = "No IO socket available\n";
    return false;
}

const uint8_t* Channel::get_mac_addr() {
    return orig_if_mac;
}

} // namespace slac
