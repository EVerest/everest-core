// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::io::mdns {

struct mDNS_discovery {
    using txt_field = std::map<std::string, std::string>;

    void add_string(std::string const& key, std::string const& value) {
        txt[key] = value;
    }

    std::string ip;
    std::uint16_t port;
    std::string hostname;
    std::string service_instance;
    txt_field txt;

    static const std::uint16_t txt_string_limit{255};
    static const std::uint16_t txt_record_limit{1500};
};

std::optional<mDNS_discovery> parse_mdns_packet(std::vector<std::uint8_t> const& packet);
std::vector<std::uint8_t> create_mdns_query(std::string const& name);

/// Build an mDNS response packet advertising the given service.
/// Includes PTR, SRV, TXT, and A records.
std::vector<std::uint8_t> create_mdns_response(mDNS_discovery const& service,
                                                std::string const& service_type);

/// Check if an mDNS packet is a query for the given service type.
bool is_query_for(std::vector<std::uint8_t> const& packet, std::string const& service_type);

class mDNS_registry {
public:
    using registry = std::map<std::string, mDNS_discovery>;
    bool update(mDNS_discovery const& update);
    void remove(const std::string& instance);
    void clear();
    registry const& get();

private:
    registry data;
};

} // namespace everest::lib::io::mdns
