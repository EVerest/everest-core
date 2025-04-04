#include <gtest/gtest.h>
#include <thread>
#include <everest/logging.hpp>

#include "../helpers/WebSocketTestClient.hpp"
#include "../rpc/RpcHandler.hpp"
#include "../server/WebsocketServer.hpp"

using namespace server;
using namespace rpc;

class WebSocketServerTest : public ::testing::Test {
protected:
    std::unique_ptr<WebSocketServer> ws_server;
    std::unique_ptr<rpc::RpcHandler> rpc_handler;
    int test_port = 8080;

    void SetUp() override {
        // Start the WebSocket server
        m_websocket_server = std::make_unique<server::WebSocketServer>(false, test_port);
        // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
        std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
        transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
        m_rpc_server = std::make_unique<RpcHandler>(std::move(transport_interfaces));
        m_rpc_server->start_server();
    }

    std::vector<TransportInterface::ClientId>& get_connected_clients() {
        std::lock_guard<std::mutex> lock(cv_mutex);
        return connected_clients;
    }

    std::unique_ptr<server::WebSocketServer> m_websocket_server;
    std::unique_ptr<rpc::RpcHandler> m_rpc_server;

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

// Test: Connect to WebSocket server and check if API.hello timeout occurs
TEST_F(WebSocketServerTest, ClientHelloTimeout) {
    WebSocketTestClient client("localhost", test_port);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(client.connect());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(client.is_connected());

    // Wait for the client hello timeout
    EVLOG_info << "Waiting for client hello timeout...";
    std::this_thread::sleep_for(std::chrono::seconds(CLIENT_HELLO_TIMEOUT) + std::chrono::milliseconds(100));

    // Check if the client is still connected
    ASSERT_FALSE(client.is_connected());
}

