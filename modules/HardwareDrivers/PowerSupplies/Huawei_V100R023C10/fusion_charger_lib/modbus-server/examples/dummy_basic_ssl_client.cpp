// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <modbus-server/client.hpp>
#include <modbus-ssl/openssl_transport.hpp>
#include <thread>

int main() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL) {
        throw std::runtime_error("SSL_CTX_new failed");
    }

    SSL_CTX_use_certificate_file(ctx, "../certs/ca.crt.pem", SSL_FILETYPE_PEM);
    SSL_CTX_load_verify_locations(ctx, "../certs/ca.crt.pem", NULL);
    SSL_CTX_use_certificate_file(ctx, "../certs/client.crt.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "../certs/client.key.pem", SSL_FILETYPE_PEM);

    if (!SSL_CTX_check_private_key(ctx)) {
        throw std::runtime_error("Private key invalid");
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Socket not opened");
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(802);

    printf("Connecting...\n");
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
        throw std::runtime_error("Could not connect");
    };
    printf("TCP Connected\n");

    SSL* ssl = SSL_new(ctx);

    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) != 1) {
        throw std::runtime_error("Could not connect to server");
    }

    printf("TLS Connected\n");

    auto transport = std::make_shared<modbus_ssl::OpenSSLTransport>(ssl);
    auto protocol = std::make_shared<modbus_server::ModbusTCPProtocol>(transport);
    auto pas = std::make_shared<modbus_server::PDUCorrelationLayer>(protocol);

    modbus_server::client::ModbusClient client = modbus_server::client::ModbusClient(pas);

    auto thread = std::thread([&pas]() {
        try {
            while (true) {
                pas->blocking_poll();
            }
        } catch (const std::exception& e) {
        }
    });

    printf("Reading 16 registers...\n");
    auto data = client.read_holding_registers(0x0000, 0x0010);
    printf("...done, result is:\n");
    for (auto& d : data) {
        printf("  0x%04x\n", d);
    }

    printf("Reading another 2 registers...\n");
    data = client.read_holding_registers(0x0010, 0x0002);
    printf("...done, result is:\n");
    for (auto& d : data) {
        printf("  0x%04x\n", d);
    }

    printf("Closing connection\n");

    SSL_shutdown(ssl);
    SSL_free(ssl);

    close(sock);

    thread.join();
}
