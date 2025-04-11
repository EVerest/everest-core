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
#include <condition_variable>
#include <mutex>

class WebSocketTestClient {
public:
    WebSocketTestClient(const std::string& address, int port);
    ~WebSocketTestClient();

    bool connect();
    bool is_connected();
    void send(const std::string& message);
    std::string receive();
    void close();
    std::string get_received_data() const { return m_received_data; }
    void wait_for_response(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_cv_mutex);
        m_cv.wait_for(lock, timeout, [this] { return !m_received_data.empty(); });
    }

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
    std::string m_received_data;

    //Condition variable to wait for response
    std::condition_variable m_cv;
    std::mutex m_cv_mutex;
};

#endif // WEBSOCKETTESTCLIENT_HPP
