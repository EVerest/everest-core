#include <gtest/gtest.h>
#include <thread>
#include <everest/logging.hpp>

#include "../helpers/WebSocketTestClient.hpp"
#include "../rpc/RpcHandler.hpp"
#include "../server/WebsocketServer.hpp"

using namespace server;
using namespace rpc;

class RpcHandlerTest : public ::testing::Test {
protected:
    int test_port = 8080;
    void SetUp() override {
        // Start the WebSocket server
        m_websocket_server = std::make_unique<server::WebSocketServer>(false, test_port);
        lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

        // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
        std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
        transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
        m_rpc_server = std::make_unique<RpcHandler>(std::move(transport_interfaces));
        m_rpc_server->start_server();
    }

    void TearDown() override {
        m_rpc_server->stop_server();
    }

    std::unique_ptr<server::WebSocketServer> m_websocket_server;
    std::unique_ptr<rpc::RpcHandler> m_rpc_server;

    //Condition variable to wait for response
    std::condition_variable cv;
    std::mutex cv_mutex;
};

// Test: Connect to WebSocket server and check if API.hello timeout occurs
TEST_F(RpcHandlerTest, ClientHelloTimeout) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Wait for the client hello timeout
    EVLOG_info << "Waiting for client hello timeout...";
    std::this_thread::sleep_for(std::chrono::seconds(CLIENT_HELLO_TIMEOUT) + std::chrono::milliseconds(100));

    // Check if the client is still connected
    ASSERT_FALSE(client.is_connected());
}

