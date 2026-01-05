// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <functional>
#include <stdexcept>
#include <thread>
#include <vector>

// Simple socket server that accepts (unlimited) connections and calls a
// callback on each accepted client.
class SocketServer {
    void* context;
    std::function<void(int, void*)> on_client; // called when a client connects
    std::thread server_thread;
    int port;
    int server_sock;

public:
    SocketServer(int port, void* context, std::function<void(int, void*)> on_client);
    ~SocketServer();

private:
    void main();
};
