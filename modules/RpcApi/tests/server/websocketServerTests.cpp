#include <gtest/gtest.h>
#include <libwebsockets.h>
#include <thread>
#include <boost/uuid/uuid_io.hpp>
#include <atomic>
#include <everest/logging.hpp>

#include "../server/websocketServer.hpp"

using namespace server;

class WebSocketServerTest : public ::testing::Test {
protected:
    std::unique_ptr<WebSocketServer> ws_server;
    int test_port = 8080;

    void SetUp() override {
        ws_server = std::make_unique<WebSocketServer>(false, test_port);
        lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

        ws_server->on_client_connected = [this](const TransportInterface::ClientId& client_id, const server::TransportInterface::Address& address) {
            // Handle client connected logic here
            std::lock_guard<std::mutex> lock(cv_mutex);
            try {
                connected_clients.push_back(client_id);
            } catch (const std::exception& e) {
                EVLOG_error << "Exception occurred while handling client connected: " << e.what();
            }
        };
    
        ws_server->on_client_disconnected = [this](const TransportInterface::ClientId& client_id) {
            // Handle client disconnected logic here
            std::lock_guard<std::mutex> lock(cv_mutex);
            try {
                connected_clients.erase(std::remove(connected_clients.begin(), connected_clients.end(), client_id), connected_clients.end());
            } catch (const std::exception& e) {
                EVLOG_error << "Exception occurred while handling client disconnected: " << e.what();
            }
        };
    
        ws_server->on_data_available = [this](const TransportInterface::ClientId& client_id, const server::TransportInterface::Data& data) {
            // Handle data available logic here
            std::lock_guard<std::mutex> lock(cv_mutex);
            try {
            received_data[client_id] = std::string(data.begin(), data.end());
            cv.notify_all();
            } catch (const std::exception& e) {
                EVLOG_error << "Exception occurred while handling data available: " << e.what();
            }
        };

        ws_server->start_server();
    }

    std::vector<TransportInterface::ClientId>& get_connected_clients() {
        std::lock_guard<std::mutex> lock(cv_mutex);
        return connected_clients;
    }

    //Connected client id's
    std::vector<TransportInterface::ClientId> connected_clients;

    //Condition variable to wait for response
    std::condition_variable cv;
    std::mutex cv_mutex;

    //Reveived data with client id
    std::unordered_map<TransportInterface::ClientId, std::string> received_data;

    void TearDown() override {
        ws_server->stop_server();
    }
};

// WebSocket Client for testing
class WebSocketTestClient {
public:
    WebSocketTestClient(const std::string& address, int port)
        : address(address), port(port), m_context(nullptr), m_wsi(nullptr), m_connected(false) {

        struct lws_protocols protocols[] = {
            { "EVerestRpcApi", callback, 0, 0, 0, NULL, 0 },
            LWS_PROTOCOL_LIST_TERM 
        };
    
        struct lws_context_creation_info info = {};
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.port = CONTEXT_PORT_NO_LISTEN; /* client */
        info.protocols = protocols;
        info.gid = -1;
        info.uid = -1;
        info.user = this;
    
        m_context = lws_create_context( &info );
        if (!m_context) {
            throw std::runtime_error("Failed to create WebSocket m_context");
        }

        m_client_thread = std::thread([this]() {
            m_running = true;
            while (m_running) {
                lws_service(m_context, 1000);
            }
        });

        while (!m_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for client to start
        }
    
        m_ccinfo.context = m_context;
        m_ccinfo.address = "localhost";
        m_ccinfo.port = port;
        m_ccinfo.path = "/";
        m_ccinfo.host = m_ccinfo.address;
        m_ccinfo.origin = m_ccinfo.address;
        m_ccinfo.protocol = "EVerestRpcApi";
    }

    static int callback(struct lws* wsi, enum lws_callback_reasons reason,
                        void* user, void* in, size_t len) {
        WebSocketTestClient* client = static_cast<WebSocketTestClient*>(lws_context_user(lws_get_context(wsi)));

        if (client == nullptr) {
            std::cerr << "Error: WebSocketTestClient instance not found!";
            return -1;
        }

        switch (reason) {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                client->m_connected = true;
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE:
                client->received_data.assign(static_cast<char*>(in), len);
                break;
            case LWS_CALLBACK_CLOSED:
                client->m_connected = false;
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                EVLOG_error << "CLIENT_CONNECTION_ERROR: "<< (in ? (char *)in : "(null)");
                break;
            default:
                break;
        }
        return 0;
    }

    bool connect() {
        if (m_context == nullptr) {
            EVLOG_error << "Error: WebSocket m_context not found!";
            return false;
        }

        m_wsi = lws_client_connect_via_info(&m_ccinfo);

        if (m_wsi == nullptr) {
            EVLOG_error << "Error while connecting to WebSocket server";
        }
        else {
            EVLOG_info << "Connected to WebSocket server";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for client, otherwise a assertion error will occur, if the client will be directly closed

        return m_wsi != nullptr;
    }

    bool is_connected() {
        return m_connected;
    }

    void send(const std::string& message) {
        if (!m_connected) return;

        try {
            std::vector<unsigned char> buf(LWS_PRE + message.size());
            memcpy(buf.data() + LWS_PRE, message.c_str(), message.size());
            lws_write(m_wsi, buf.data() + LWS_PRE, message.size(), LWS_WRITE_TEXT);
        } catch (const std::exception& e) {
            EVLOG_error << "Error while sending message: " << e.what();
        }
    }

    std::string receive() {
        return received_data;
    }

    void close() {
        if (m_wsi) {
            m_running = false;
            lws_close_reason(m_wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);

            if (m_context == nullptr) {
                EVLOG_error << "Error: WebSocket m_context not found!";
                return;
            }
            lws_cancel_service(m_context);
            m_wsi = nullptr;

            if (m_client_thread.joinable()) {
                m_client_thread.join();  // Wait for client thread to finish
            }
        }
        if (m_context) {
            lws_context_destroy(m_context);
            m_context = nullptr;
        }
        m_connected = false;
    }

    ~WebSocketTestClient() {
        close();
    }

private:
    std::string address;
    int port;
    struct lws_context* m_context;
    struct lws_client_connect_info m_ccinfo {};
    struct lws* m_wsi;
    std::atomic<bool> m_connected {false};
    std::atomic<bool> m_running {false};
    std::thread m_client_thread;
    std::string received_data;
};

// Test: Start and stop WebSocket server
TEST_F(WebSocketServerTest, WebSocketServerStarts) {
    ASSERT_TRUE(ws_server->running());
    TearDown();
    ASSERT_FALSE(ws_server->running());
}

// Test: Connect WebSocket client to server
TEST_F(WebSocketServerTest, ClientCanConnect) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(client.is_connected());
}

// Test: Connect several WebSocket clients to server
TEST_F(WebSocketServerTest, MultipleClientsCanConnect) {
    WebSocketTestClient client1("localhost", test_port);
    WebSocketTestClient client2("localhost", test_port);
    WebSocketTestClient client3("localhost", test_port);

    ASSERT_TRUE(client1.connect());
    ASSERT_TRUE(client2.connect());
    ASSERT_TRUE(client3.connect());

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ASSERT_TRUE(client1.is_connected());
    ASSERT_TRUE(client2.is_connected());
    ASSERT_TRUE(client3.is_connected());

    ASSERT_TRUE(ws_server->connections_count() == 3);
}

// Test: Client can send data to server
TEST_F(WebSocketServerTest, ClientCanSendAndReceiveData) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(client.is_connected());

    client.send("Hello World!");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait_for(lock, std::chrono::seconds(1), [&] { return !received_data.empty(); });
    lock.unlock();

    ASSERT_EQ(ws_server->connections_count(), 1);
    ASSERT_EQ(received_data[(get_connected_clients()[0])], "Hello World!");
}

// Test: Server kills client connection
TEST_F(WebSocketServerTest, ServerCanKillClientConnection) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(client.is_connected());

    client.send("Hello World!");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait_for(lock, std::chrono::seconds(1), [&] { return !received_data.empty(); });
    lock.unlock();

    ASSERT_EQ(ws_server->connections_count(), 1);
    ASSERT_EQ(received_data[get_connected_clients()[0]], "Hello World!");

    ws_server->kill_client_connection(get_connected_clients()[0], "Test kill");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ASSERT_EQ(ws_server->connections_count(), 0);
}
