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

namespace server {

class WebSocketServer : public TransportInterface
{
public:
    explicit WebSocketServer(bool ssl_enabled, int port);
    ~WebSocketServer() override;

    bool running() const override;

    std::string ssl_configuration() const;

    void send_data(const ClientId &client_id, const Data &data) override;
    void kill_client_connection(const int &client_id, const std::string &kill_reason) override;

    uint connections_count() const override;

private:
    void *m_server = nullptr; 
    bool m_ssl_enabled;
    struct lws_context_creation_info m_info {};
    struct lws_protocols m_lws_protocols[2];
    std::atomic<bool> m_running {false};
    struct lws_context *m_context = nullptr;
    std::thread m_server_thread;
    std::unordered_map<int, struct lws *> m_clients;  // Client-Mapping
    std::mutex m_clients_mutex;

public:
    bool start_server() override;
    bool stop_server() override;

    void on_client_connected();
    void on_client_disconnected(const int &client_id);
    void on_data_received(const int &client_id, const std::vector<uint8_t> &data);
};

}

#endif // WEBSOCKETSERVER_H
