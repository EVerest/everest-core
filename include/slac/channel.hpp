// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef SLAC_CHANNEL_HPP
#define SLAC_CHANNEL_HPP

#include <memory>
#include <string>

// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <slac/slac.hpp>

namespace utils {
class PacketSocket;
}

namespace slac {

// TODO (aw):
//  - do we need to handle VLAN tags?
//  - probably we need to handle different sessions ...
//  - channel could own the interface handle and pass it to the packet
//    socket

class Channel {
public:
    Channel();
    // Channel(const std::string& interface_name);
    ~Channel();

    bool open(const std::string& interface_name);
    bool read(slac::messages::HomeplugMessage& msg, int timeout);
    bool write(slac::messages::HomeplugMessage& msg, int timeout);

    const std::string& get_error() const {
        return error;
    }

    bool got_timeout() const {
        return did_timeout;
    }

private:
    // for debugging only, should be removed
    std::unique_ptr<::utils::PacketSocket> socket;
    uint8_t orig_if_mac[ETH_ALEN];

    std::string error;
    bool did_timeout{false};
};

} // namespace slac

#endif // SLAC_CHANNEL_HPP
