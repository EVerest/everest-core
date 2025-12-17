// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <modbus-server/frames.hpp>
#include <modbus-server/modbus_basic_server.hpp>
#include <modbus-server/transport.hpp>
#include <modbus-server/transport_protocol.hpp>
#include <type_traits>

struct CustomPDURequest : public modbus_server::pdu::SpecificPDU {
    std::uint8_t data[2];

public:
    void from_generic(const modbus_server::pdu::GenericPDU& input) override {
        if (input.function_code != 0x41) {
            throw modbus_server::pdu::DecodingError(input, "CustomPDURequest", "Invalid function code");
        }

        if (input.data.size() != 2) {
            throw modbus_server::pdu::DecodingError(input, "CustomPDURequest", "Invalid data size");
        }

        this->data[0] = input.data[0];
        this->data[1] = input.data[1];
    }

    modbus_server::pdu::GenericPDU to_generic() const override {
        modbus_server::pdu::GenericPDU output;
        output.function_code = 0x41;
        output.data.push_back(data[0]);
        output.data.push_back(data[1]);
        return output;
    }
};
struct CustomPDUResponse : public modbus_server::pdu::SpecificPDU {
    std::uint8_t data[3];

public:
    void from_generic(const modbus_server::pdu::GenericPDU& input) override {
        if (input.function_code != 0x41) {
            throw modbus_server::pdu::DecodingError(input, "CustomPDUResponse", "Invalid function code");
        }

        if (input.data.size() != 3) {
            throw modbus_server::pdu::DecodingError(input, "CustomPDUResponse", "Invalid data size");
        }

        this->data[0] = input.data[0];
        this->data[1] = input.data[1];
        this->data[2] = input.data[2];
    }

    modbus_server::pdu::GenericPDU to_generic() const override {
        modbus_server::pdu::GenericPDU output;
        output.function_code = 0x41;
        output.data.push_back(data[0]);
        output.data.push_back(data[1]);
        output.data.push_back(data[2]);
        return output;
    }
};

class CustomServer : public modbus_server::ModbusBasicServer {
protected:
    std::optional<AlwaysRespondingPDUHandler<CustomPDURequest, CustomPDUResponse>> custom_pdu_cb;

public:
    CustomServer(std::shared_ptr<modbus_server::PDUCorrelationLayerIntf> pal) : modbus_server::ModbusBasicServer(pal) {
    }

    void set_custom_pdu_cb(
        modbus_server::ModbusBasicServer::AlwaysRespondingPDUHandler<CustomPDURequest, CustomPDUResponse> fn) {
        custom_pdu_cb = fn;
    }

protected:
    std::optional<modbus_server::pdu::GenericPDU> on_pdu(const modbus_server::pdu::GenericPDU& input) override {
        switch (input.function_code) {
        case 0x41: {
            if (custom_pdu_cb.has_value()) {
                CustomPDURequest req;
                req.from_generic(input);
                auto resp = custom_pdu_cb.value()(req);
                return resp.to_generic();
            }
        }
        }

        return modbus_server::ModbusBasicServer::on_pdu(input);
    }
};

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

        auto transport = std::make_shared<modbus_server::ModbusSocketTransport>(client_sock);
        auto protocol = std::make_shared<modbus_server::ModbusTCPProtocol>(transport);
        auto pas = std::make_shared<modbus_server::PDUCorrelationLayer>(protocol);

        CustomServer server(pas);

        server.set_read_holding_registers_request_cb([](const modbus_server::pdu::ReadHoldingRegistersRequest& req) {
            std::vector<std::uint8_t> data;

            for (int i = 0; i < req.register_count; i++) {
                data.push_back(0x00);
                data.push_back(i);
            }

            modbus_server::pdu::ReadHoldingRegistersResponse resp(req, data);
            return resp;
        });

        server.set_custom_pdu_cb([](const CustomPDURequest& req) {
            CustomPDUResponse resp;
            resp.data[0] = req.data[0];
            resp.data[1] = req.data[1];
            resp.data[2] = 0x42;
            return resp;
        });

        try {
            while (1) {
                pas->blocking_poll();
            }
        } catch (modbus_server::transport_exceptions::ConnectionClosedException e) {
            printf("Transport exception: %s\n", e.what());
            printf("Accepting new connection\n");
        }

        close(client_sock);
    }
}
