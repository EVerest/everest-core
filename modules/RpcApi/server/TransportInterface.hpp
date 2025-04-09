// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef TRANSPORTINTERFACE_H
#define TRANSPORTINTERFACE_H

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <memory>
#include <boost/uuid/uuid.hpp>

namespace std {
    template <>
    struct hash<boost::uuids::uuid> {
        std::size_t operator()(const boost::uuids::uuid& u) const {
            // Combine the two 64-bit parts of the UUID into one 128-bit integer
            uint64_t part1 = *reinterpret_cast<const uint64_t*>(&u.data[0]);
            uint64_t part2 = *reinterpret_cast<const uint64_t*>(&u.data[8]);
            return std::hash<uint64_t>()(part1) ^ (std::hash<uint64_t>()(part2) << 1); // Combine the two parts
        }
    };
}

namespace server {

class TransportInterface {
public:
    using ClientId = boost::uuids::uuid;
    using Address = std::string;
    using Data = std::vector<uint8_t>;

    explicit TransportInterface() = default;
    virtual ~TransportInterface() = default;

    std::string server_name() const;

    virtual void send_data(const ClientId &clientId, const Data &data) = 0;
    inline void send_data(const ClientId &clientId, const std::string &data) {
        send_data(clientId, std::vector<uint8_t>(data.begin(), data.end()));
    };
    virtual void kill_client_connection(const ClientId &clientId, const std::string &killReason) = 0;

    virtual uint32_t connections_count() const = 0;
    
    std::string server_url() const;
    void set_server_url(const std::string &serverUrl);

    virtual bool running() const = 0;

    std::function<void(const ClientId &, const Address &)> on_client_connected;
    std::function<void(const ClientId &)> on_client_disconnected;
    std::function<void(const ClientId &, const Data &)> on_data_available;

protected:
    std::string m_server_url;
    std::string m_server_name;

public:
    virtual bool start_server() = 0;
    virtual bool stop_server() = 0;
};

}

#endif // TRANSPORTINTERFACE_H
