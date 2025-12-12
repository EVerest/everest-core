// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "socket_server.hpp"

SocketServer::SocketServer(int port, void* context, std::function<void(int, void*)> on_client) :
    on_client(std::move(on_client)), context(context), port(port), server_sock(-1) {
    server_thread = std::thread([this]() { main(); });
}

SocketServer::~SocketServer() {
    if (server_sock >= 0) {
        shutdown(server_sock, SHUT_RDWR);
    }
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

void SocketServer::main() {
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    int is_true = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &is_true, sizeof(int));
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    int err = bind(server_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        throw std::runtime_error("Failed to bind");
    }

    err = listen(server_sock, 1);
    if (err < 0) {
        throw std::runtime_error("Failed to listen");
    }

    for (;;) {
        int client_sock = accept(server_sock, nullptr, nullptr);
        if (client_sock < 0) {
            if (errno == EBADF || errno == EINVAL) {
                // Socket was closed, exit gracefully
                break;
            }
            printf("Failed to accept with error: %d", errno);
            close(server_sock);

            throw std::runtime_error("Failed to accept with error: " + std::to_string(errno));
        }

        on_client(client_sock, context);
    }
}
