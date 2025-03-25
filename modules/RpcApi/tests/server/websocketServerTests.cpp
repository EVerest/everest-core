#include <gtest/gtest.h>
#include <libwebsockets.h>
#include <thread>
#include <boost/uuid/uuid_io.hpp>
#include <atomic>


#include "../server/websocketServer.hpp"

using namespace server;

class WebSocketServerTest : public ::testing::Test {
protected:
    std::unique_ptr<WebSocketServer> ws_server;
    int test_port = 8080;

    void SetUp() override {
        ws_server = std::make_unique<WebSocketServer>(false, test_port);
        lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

        ws_server->on_client_connected = [](const TransportInterface::ClientId& client_id, const server::TransportInterface::Address& address) {
            // Handle client connected logic here
            std::cerr << "on_client_connected: Client " << boost::uuids::to_string(client_id) << " connected from " << address << std::endl;
        };
    
        ws_server->on_client_disconnected = [](const TransportInterface::ClientId& client_id) {
            // Handle client disconnected logic here
            std::cerr << "on_client_disconnected: Client " << boost::uuids::to_string(client_id) << " disconnected" << std::endl;;
        };
    
        ws_server->on_data_available = [](const TransportInterface::ClientId& client_id, const server::TransportInterface::Data& data) {
            // Handle data available logic here
            std::cerr << "on_data_available: Received data from client " << boost::uuids::to_string(client_id) << " with size " << data.size() << std::endl;;
        };

        ws_server->start_server();
    }

    void TearDown() override {
        ws_server->stop_server();
    }
};

// WebSocket-Client für Tests mit libwebsockets
class WebSocketTestClient {
public:
    WebSocketTestClient(const std::string& address, int port)
        : address(address), port(port), m_context(nullptr), m_wsi(nullptr), connected(false) {
            std::cerr << "connect start" << std::endl;

            struct lws_protocols protocols[] = {
                { "EVerestRpcApi", callback, 0, 0, 0, this, 0 },
                LWS_PROTOCOL_LIST_TERM 
            };
    
            struct lws_context_creation_info info = {};
            info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
            info.port = CONTEXT_PORT_NO_LISTEN; /* client */
            info.protocols = protocols;
            info.gid = -1;
            info.uid = -1;
        
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

//        if (client == nullptr) {
//            std::cerr << "Error: WebSocketTestClient instance not found!" << std::endl;
//            return -1;
//        }
//
//        switch (reason) {
//            case LWS_CALLBACK_CLIENT_ESTABLISHED:
//                client->connected = true;
//                break;
//            case LWS_CALLBACK_CLIENT_RECEIVE:
//                client->received_data.assign(static_cast<char*>(in), len);
//                break;
//            case LWS_CALLBACK_CLOSED:
//                client->connected = false;
//                break;
//            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
//                lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
//                in ? (char *)in : "(null)");
//                break;
//            default:
//                break;
//        }
//{
	struct my_conn *mco = (struct my_conn *)user;
	switch (reason) {
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
			 in ? (char *)in : "(null)");

		break;
	case LWS_CALLBACK_CLIENT_RECEIVE:
		lwsl_hexdump_notice(in, len);
		break;
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		lwsl_user("%s: established\n", __func__);
		break;
	case LWS_CALLBACK_CLIENT_CLOSED:
	default:
		break;
	}


        return 0;
    }

    bool connect() {
        if (m_context == nullptr) {
            std::cerr << "Error: WebSocket m_context not found!" << std::endl;
            return false;
        }

        m_wsi = lws_client_connect_via_info(&m_ccinfo);

        if (m_wsi == nullptr) {
           std::cerr << "Error while connecting to WebSocket server" << std::endl;
        }
        else {
            std::cerr << "Connected to WebSocket server" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for client, otherwise a assertion error will occur, if the client will be directly closed

        return m_wsi != nullptr;
    }

    void send(const std::string& message) {
        if (!connected) return;

        try {
            std::vector<unsigned char> buf(LWS_PRE + message.size());
            memcpy(buf.data() + LWS_PRE, message.c_str(), message.size());
            lws_write(m_wsi, buf.data() + LWS_PRE, message.size(), LWS_WRITE_TEXT);
        } catch (const std::exception& e) {
            std::cerr << "Error while sending message: " << e.what() << std::endl;
        }
    }

    std::string receive() {
        return received_data;
    }

    void close() {
        std::cerr << "close connection" << std::endl;
        if (m_wsi) {
            m_running = false;

            if (m_client_thread.joinable()) {
                m_client_thread.join();  // Wait for client thread to finish
            }

            lws_close_reason(m_wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);

            if (m_context == nullptr) {
                std::cerr << "Error: WebSocket m_context not found!" << std::endl;
                return;
            }

            lws_cancel_service(m_context);
            m_wsi = nullptr;
        }
        if (m_context) {
            lws_context_destroy(m_context);
            m_context = nullptr;
        }
        connected = false;
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
    bool connected;
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
}

// Test: Client can send and receive data
TEST_F(WebSocketServerTest, ClientCanSendAndReceiveData) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());

    std::string test_message = "Hello WebSocket!";
    client.send(test_message);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::string received = client.receive();
    
    EXPECT_EQ(received, test_message);

    client.close();
}

// Test: Client kann sich trennen
TEST_F(WebSocketServerTest, ClientCanDisconnect) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    client.close();
}
