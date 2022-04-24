// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "server.hpp"

#include <fstream>
#include <set>
#include <sstream>

#include <fmt/core.h>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <everest/logging.hpp>

class Server::Impl {
public:
    using ConnectionHandle = websocketpp::connection_hdl;
    using WSServer = websocketpp::server<websocketpp::config::asio>;
    using MessagePtr = WSServer::message_ptr;

    Impl() {
        endpoint.init_asio();
        endpoint.set_reuse_addr(true);
        endpoint.set_http_handler([this](ConnectionHandle handle) { on_http(handle); });
        endpoint.set_message_handler(
            [this](ConnectionHandle handle, MessagePtr msg_ptr) { on_message(handle, msg_ptr); });
        endpoint.set_open_handler([this](ConnectionHandle handle) { on_open(handle); });
        endpoint.set_close_handler([this](ConnectionHandle handle) { on_close(handle); });
        endpoint.clear_access_channels(websocketpp::log::alevel::all);
    }

    void run(const Server::IncomingMessageHandler& handler) {
        if (!handler) {
            throw std::runtime_error("Could not run the the server with a null incoming message handler");
        }

        this->message_in_handler = handler;
        uint16_t port = 8849;
        EVLOG_info << fmt::format("Running controller service on port {}\n", port);

        endpoint.listen(port);
        endpoint.start_accept();

        try {
            endpoint.run();
        } catch (websocketpp::exception const& e) {
            EVLOG_error << fmt::format("websocketpp exception: {}", e.what());
        }
    }

    void push(const nlohmann::json& msg) {
        for (auto& connection : connections) {
            endpoint.send(connection, msg.dump(), websocketpp::frame::opcode::text);
        }
    }

private:
    using ConnectionList = std::set<ConnectionHandle, std::owner_less<ConnectionHandle>>;
    using ConnectionPtr = WSServer::connection_ptr;

    void on_message(ConnectionHandle handle, MessagePtr msg) {
        const auto retval = this->message_in_handler(msg->get_payload());
        if (!retval.is_null()) {
            endpoint.send(handle, retval.dump(), websocketpp::frame::opcode::text);
        }
    }

    void on_open(ConnectionHandle handle) {
        connections.insert(handle);
    }

    void on_close(ConnectionHandle handle) {
        connections.erase(handle);
    }

    // FIXME (aw): hack for getting correct content type
    static void add_content_type(ConnectionPtr& conn, const std::string& filename) {
        auto ends_with = [](const std::string& filename, const std::string& ending) -> bool {
            if (ending.size() > filename.size()) {
                return false;
            }
            return std::equal(filename.begin() + filename.size() - ending.size(), filename.end(), ending.begin());
        };

        if (ends_with(filename, ".js")) {
            conn->append_header("Content-Type", "application/javascript; charset=utf-8");
        }
    }

    void on_http(ConnectionHandle handle) {
        auto connection = endpoint.get_con_from_hdl(handle);
        auto resource = connection->get_resource();

        if (resource == "/") {
            resource = "index.html";
        } else {
            resource = resource.substr(1);
        }

        std::ifstream file;
        std::stringstream response;

        auto accept_header = connection->get_request_header("Accept-encoding");
        if (accept_header.compare("gzip, deflate") == 0) {
            // check if we have the file gzipped
            auto gz_resource = resource + ".gz";
            file.open(gz_resource.c_str(), std::ios::in);
        }

        if (file) {
            response << file.rdbuf();
            add_content_type(connection, resource);
            connection->append_header("Content-Encoding", "gzip");
            connection->set_body(response.str());
            connection->set_status(websocketpp::http::status_code::ok);
            return;
        }

        file.open(resource.c_str(), std::ios::in);
        if (!file) {
            connection->set_body(fmt::format("<!doctype html><html><head>"
                                             "<title>Error 404 (Resource not found)</title><body>"
                                             "<h1>Error 404</h1>"
                                             "<p>The requested URL {} was not found on this server.</p>"
                                             "</body></head></html>",
                                             resource));
            connection->set_status(websocketpp::http::status_code::not_found);
            return;
        }

        response << file.rdbuf();
        add_content_type(connection, resource);
        connection->set_body(response.str());
        connection->set_status(websocketpp::http::status_code::ok);
    }

    IncomingMessageHandler message_in_handler;

    WSServer endpoint;
    ConnectionList connections;

    WSServer::timer_ptr timer;
};

Server::Server() : pimpl(std::make_unique<Impl>()) {
}

Server::~Server() = default;

void Server::run(const IncomingMessageHandler& handler) {
    pimpl->run(handler);
}

void Server::push(const nlohmann::json& msg) {
    pimpl->push(msg);
}
