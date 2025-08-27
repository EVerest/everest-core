#ifndef WEBSOCKETTESTCLIENT_HPP
#define WEBSOCKETTESTCLIENT_HPP

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <everest/logging.hpp>
#include <iostream>
#include <libwebsockets.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class WebSocketTestClient {
public:
    WebSocketTestClient(const std::string& address, int port);
    ~WebSocketTestClient();

    bool connect();
    bool is_connected();
    void send(const std::string& message);
    void send_api_hello_req();
    std::string receive();
    void close();
    std::string get_received_data() {
        std::string data;
        {
            std::lock_guard<std::mutex> lock(m_cv_mutex);
            data = m_received_data;
            m_received_data.clear(); // Clear the received data after getting it
        }
        return data;
    }

    std::string wait_for_data(std::chrono::milliseconds timeout, bool is_result = true) {
        std::unique_lock<std::mutex> lock(m_cv_mutex);

        bool received = m_cv.wait_for(lock, timeout, [this, is_result]() {
            if (m_received_data.empty())
                return false;
            if (is_result) {
                // Check string for "result" and "id" keys if is_result is true
                bool has_result = m_received_data.find("\"result\"") != std::string::npos;
                bool has_id = m_received_data.find("\"id\"") != std::string::npos;
                return has_result && has_id;
            }
            return true; // For regular responses, we just check if we have data
        });

        if (!received) {
            return ""; // Timeout
        }

        std::string data = m_received_data;
        m_received_data.clear(); // Clear the received data after getting it
        return data;
    }

    bool wait_for_response(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_cv_mutex);
        return m_cv.wait_for(lock, timeout, [this] { return !m_received_data.empty(); });
    }

    bool wait_until_connected(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_cv_mutex);
        return m_cv.wait_for(lock, timeout, [this] { return m_connected.load(); });
    }

    void start_lws_service_thread();
    void stop_lws_service_thread();

private:
    static int callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

    std::string m_address;
    int m_port;
    struct lws_context* m_context;
    struct lws_client_connect_info m_ccinfo {};
    struct lws* m_wsi;
    std::atomic<bool> m_connected{false};
    std::atomic<bool> m_lws_service_running{false};
    std::thread m_lws_service_thread;
    std::string m_received_data;

public:
    // Condition variable to wait for response
    std::condition_variable m_cv;
    std::mutex m_cv_mutex;
};

#endif // WEBSOCKETTESTCLIENT_HPP
