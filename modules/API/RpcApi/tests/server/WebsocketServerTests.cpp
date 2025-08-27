#include <everest/logging.hpp>
#include <gtest/gtest.h>
#include <thread>

#include "../helpers/WebSocketTestClient.hpp"
#include "../server/WebsocketServer.hpp"

using namespace server;

class WebSocketServerTest : public ::testing::Test {
protected:
    std::unique_ptr<WebSocketServer> ws_server;
    int test_port = 8080;

    void SetUp() override {
        ws_server = std::make_unique<WebSocketServer>(false, test_port, "");
        lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

        ws_server->on_client_connected = [this](const TransportInterface::ClientId& client_id,
                                                const server::TransportInterface::Address& address) {
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
                connected_clients.erase(std::remove(connected_clients.begin(), connected_clients.end(), client_id),
                                        connected_clients.end());
            } catch (const std::exception& e) {
                EVLOG_error << "Exception occurred while handling client disconnected: " << e.what();
            }
        };

        ws_server->on_data_available = [this](const TransportInterface::ClientId& client_id,
                                              const server::TransportInterface::Data& data) {
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

    // Connected client id's
    std::vector<TransportInterface::ClientId> connected_clients;

    // Condition variable to wait requests
    std::condition_variable cv;
    std::mutex cv_mutex;

    // Received data with client id
    std::unordered_map<TransportInterface::ClientId, std::string> received_data;

    void TearDown() override {
        ws_server->stop_server();
    }
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
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));
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
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    client.send("Hello World!");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait_for(lock, std::chrono::seconds(1), [&] { return !received_data.empty(); });
    lock.unlock();

    ASSERT_EQ(ws_server->connections_count(), 1);
    ASSERT_EQ(received_data[(get_connected_clients()[0])], "Hello World!");
}

// Test: Server can send data to client
TEST_F(WebSocketServerTest, ServerCanSendDataToClient) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    std::string message = "Hello from server!";
    ws_server->send_data(get_connected_clients()[0], std::vector<uint8_t>(message.begin(), message.end()));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::string received_data = client.wait_for_data(std::chrono::seconds(1), false);

    ASSERT_FALSE(received_data.empty());
    ASSERT_EQ(received_data, "Hello from server!");
}

// Test: Server kills client connection
TEST_F(WebSocketServerTest, ServerCanKillClientConnection) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    client.send("Hello World!");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    client.wait_for_data(std::chrono::seconds(1));

    ASSERT_EQ(ws_server->connections_count(), 1);
    ASSERT_EQ(received_data[get_connected_clients()[0]], "Hello World!");

    ws_server->kill_client_connection(get_connected_clients()[0], "Test kill");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_EQ(ws_server->connections_count(), 0);
    ASSERT_FALSE(client.is_connected());
}
