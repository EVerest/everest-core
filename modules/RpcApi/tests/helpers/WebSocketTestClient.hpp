#ifndef WEBSOCKETTESTCLIENT_HPP
#define WEBSOCKETTESTCLIENT_HPP

#include <libwebsockets.h>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <cstring>
#include <everest/logging.hpp>

class WebSocketTestClient {
public:
    WebSocketTestClient(const std::string& address, int port);
    ~WebSocketTestClient();

    bool connect();
    bool is_connected();
    void send(const std::string& message);
    std::string receive();
    void close();

private:
    static int callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

    std::string m_address;
    int m_port;
    struct lws_context* m_context;
    struct lws_client_connect_info m_ccinfo {};
    struct lws* m_wsi;
    std::atomic<bool> m_connected {false};
    std::atomic<bool> m_running {false};
    std::thread m_client_thread;
    std::string received_data;
};

#endif // WEBSOCKETTESTCLIENT_HPP
