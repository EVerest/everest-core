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

class mDNS_registry {
public:
    using registry = std::map<std::string, mDNS_discovery>;
    bool update(mDNS_discovery const& update);
    void clear();
    registry const& get();
private:
    registry data;
};

} // namespace everest::lib::io::mdns
