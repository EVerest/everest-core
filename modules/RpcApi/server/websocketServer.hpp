#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <boost/asio.hpp>
//#include <boost/uuid/uuid.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/uuid/uuid_io.hpp>
#include <libwebsockets.h>
#include <atomic>
#include <thread>

#include "transportInterface.hpp"

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

class WebSocketServer : public TransportInterface
{
public:
    // Constructor and Destructor
    explicit WebSocketServer(bool ssl_enabled, int port);
    ~WebSocketServer() override;

    // Methods
    bool running() const override;
    void send_data(const ClientId &client_id, const Data &data) override;
    void kill_client_connection(const ClientId &client_id, const std::string &kill_reason) override;

    uint connections_count() const override;

    bool start_server() override;
    bool stop_server() override;

private:
    // Members
    bool m_ssl_enabled;
    struct lws_context_creation_info m_info {};
    struct lws_protocols m_lws_protocols[2];
    std::atomic<bool> m_running {false};
    struct lws_context *m_context = nullptr;
    std::thread m_server_thread;
    std::unordered_map<ClientId, struct lws *> m_clients;  // Client-Mapping
    mutable std::mutex m_clients_mutex;

    // Methods
    static int callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
};

}

#endif // WEBSOCKETSERVER_H
