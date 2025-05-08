#include "WebSocketTestClient.hpp"

#include <nlohmann/json.hpp>

#include "JsonRpcUtils.hpp"

using namespace json_rpc_utils;

WebSocketTestClient::WebSocketTestClient(const std::string& address, int port)
    : m_address(address), m_port(port), m_context(nullptr), m_wsi(nullptr), m_connected(false) {

    struct lws_protocols protocols[] = {
        { "EVerestRpcApi", callback, 0, 0, 0, NULL, 0 },
        LWS_PROTOCOL_LIST_TERM
    };

    struct lws_context_creation_info info = {};
    info.port = CONTEXT_PORT_NO_LISTEN; /* client */
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;
    info.user = this;

    m_context = lws_create_context(&info);
    if (!m_context) {
        throw std::runtime_error("Failed to create WebSocket m_context");
    }

    m_ccinfo.context = m_context;
    m_ccinfo.address = m_address.c_str();
    m_ccinfo.port = m_port;
    m_ccinfo.path = "/";
    m_ccinfo.host = m_ccinfo.address;
    m_ccinfo.origin = m_ccinfo.address;
    m_ccinfo.protocol = "EVerestRpcApi";

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // wait to hepl connect not crash
}

WebSocketTestClient::~WebSocketTestClient() {
    close();
}

int WebSocketTestClient::callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
    WebSocketTestClient* client = static_cast<WebSocketTestClient*>(lws_context_user(lws_get_context(wsi)));

    if (client == nullptr) {
        std::cerr << "Error: WebSocketTestClient instance not found!";
        return -1;
    }

    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            client->m_connected = true;
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE: {
            std::lock_guard<std::mutex> lock(client->m_cv_mutex);
            try {
                client->m_received_data.assign(static_cast<char*>(in), len);
                client->m_cv.notify_all();
            } catch (const std::exception& e) {
                EVLOG_error << "Exception occurred while handling data available: " << e.what();
            }
            break;
        }
        case LWS_CALLBACK_CLIENT_CLOSED:
        case LWS_CALLBACK_CLOSED_CLIENT_HTTP: {
            client->m_start_lws_service = false;
            client->m_connected = false;
            EVLOG_info << "Client closed connection: " << (in ? (char*)in : "(null)") << " reason: " << reason;
            break;
        }
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            EVLOG_error << "Client connection error: " << (in ? (char*)in : "(null)");
            break;
        default:
            break;
    }
    return 0;
}

bool WebSocketTestClient::connect() {
    if (m_context == nullptr) {
        EVLOG_error << "Error: WebSocket m_context not found!";
        return false;
    }
    stop_lws_service_thread(); // Stop any existing service thread, otherwise the connect will fail

    m_wsi = lws_client_connect_via_info(&m_ccinfo);

    if (m_wsi == nullptr) {
        EVLOG_error << "Error while connecting to WebSocket server";
    } else {
        EVLOG_info << "Connecting to WebSocket server..";
        start_lws_service_thread();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for client, otherwise an assertion error will occur if the client is directly closed

    return m_wsi != nullptr;
}

void WebSocketTestClient::start_lws_service_thread() {
    if (m_lws_service_running) {
        return;
    }
    m_start_lws_service = true;
    m_lws_service_thread = std::thread([this]() {
        m_lws_service_running = true;
        while (m_lws_service_running) {
            lws_service(m_context, 0);
        }
    });

    // Wait for the service thread to start
    while (!m_lws_service_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for service thread to start
    }
}

void WebSocketTestClient::stop_lws_service_thread() {
    if (!m_lws_service_running) {
        return;
    }
    lws_cancel_service(m_context);
    m_lws_service_running = false;
    if (m_lws_service_thread.joinable()) {
        m_lws_service_thread.join();
    }
}

bool WebSocketTestClient::is_connected() {
    return m_connected;
}

void WebSocketTestClient::send(const std::string& message) {
    if (!m_connected) return;

    try {
        std::vector<unsigned char> buf(LWS_PRE + message.size());
        memcpy(buf.data() + LWS_PRE, message.c_str(), message.size());
        lws_write(m_wsi, buf.data() + LWS_PRE, message.size(), LWS_WRITE_TEXT);
    } catch (const std::exception& e) {
        EVLOG_error << "Error while sending message: " << e.what();
    }
}

std::string WebSocketTestClient::receive() {
    return m_received_data;
}

void WebSocketTestClient::close() {
    if (m_wsi) {
        if (m_connected == true) {
            lws_close_reason(m_wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
        }

        if (m_context == nullptr) {
            EVLOG_error << "Error: WebSocket m_context not found!";
            return;
        }

        stop_lws_service_thread();
        m_wsi = nullptr;

        if (m_lws_service_thread.joinable()) {
            m_lws_service_thread.join();  // Wait for client thread to finish
        }
    }
    if (m_context) {
        lws_context_destroy(m_context);
        m_context = nullptr;
    }
    m_connected = false;

    EVLOG_info << "WebSocket client closed";
}

void WebSocketTestClient::sendApiHelloReq() {
    nlohmann::json apiHelloReq = create_json_rpc_request("API.Hello", {}, 1);

    send(apiHelloReq.dump());
}

