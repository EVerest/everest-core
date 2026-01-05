// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <modbus-server/frames.hpp>
#include <modbus-server/modbus_basic_server.hpp>
#include <modbus-server/server_exception.hpp>
#include <modbus-server/transport.hpp>
#include <modbus-server/transport_protocol.hpp>
#include <type_traits>

using namespace modbus_server;

int main() {
    printf("Hello wold\n");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(502);
    int err = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        throw std::runtime_error("Failed to bind");
    }

    err = listen(sock, 1);
    if (err < 0) {
        throw std::runtime_error("Failed to listen");
    }

    while (true) {
        int client_sock = accept(sock, NULL, NULL);

        auto transport = std::make_shared<ModbusSocketTransport>(client_sock);
        auto protocol = std::make_shared<modbus_server::ModbusTCPProtocol>(transport);
        auto pas = std::make_shared<modbus_server::PDUCorrelationLayer>(protocol);

        ModbusBasicServer server(pas);

        server.set_read_holding_registers_request_cb([](const pdu::ReadHoldingRegistersRequest& req) {
            std::vector<std::uint8_t> data;

            if (req.register_start > 0x1000) {
                throw ApplicationServerError(pdu::PDUExceptionCode::ILLEGAL_DATA_ADDRESS);
            }

            for (int i = 0; i < req.register_count; i++) {
                data.push_back(0x00);
                data.push_back(0x01);
            }

            pdu::ReadHoldingRegistersResponse resp(req, data);
            return resp;
        });

        try {
            while (1) {
                pas->blocking_poll();
            }
        } catch (transport_exceptions::ConnectionClosedException e) {
            printf("Transport exception: %s\n", e.what());
            printf("Accepting new connection\n");
        }

        close(client_sock);
    }
}
