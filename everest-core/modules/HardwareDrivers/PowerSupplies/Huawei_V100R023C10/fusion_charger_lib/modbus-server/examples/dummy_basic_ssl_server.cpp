// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <modbus-server/modbus_basic_server.hpp>
#include <modbus-ssl/openssl_transport.hpp>

// for wireshark tls decryption set SSLKEYLOGFILE to a file and run the program.
// Concurrently run wireshark and in the preferences>protocols>tls set the
// keylog file to the same file.

static void keylog(const SSL* ssl, const char* line) {
    char* file_name = getenv("SSLKEYLOGFILE");
    if (file_name == NULL) {
        return;
    }

    int file = open(file_name, O_WRONLY | O_APPEND);
    if (file < 0) {
        printf("Could not open keylog %d\n", errno);
        return;
    }

    int err = write(file, line, strlen(line));
    if (err < 0) {
        printf("Could not write keylog %d", errno);
        close(file);
        return;
    }

    write(file, "\n", 1);

    close(file);
}

int main() {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (ctx == NULL) {
        throw std::runtime_error("SSL_CTX_new failed");
    }

    SSL_CTX_set_keylog_callback(ctx, keylog);

    SSL_CTX_use_certificate_file(ctx, "../certs/ca.crt.pem", SSL_FILETYPE_PEM);
    SSL_CTX_load_verify_locations(ctx, "../certs/ca.crt.pem", NULL);
    SSL_CTX_use_certificate_file(ctx, "../certs/server.crt.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "../certs/server.key.pem", SSL_FILETYPE_PEM);

    if (!SSL_CTX_check_private_key(ctx)) {
        throw std::runtime_error("Private key invalid");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT | SSL_VERIFY_CLIENT_ONCE, NULL);
    SSL_CTX_set_verify_depth(ctx, 10);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Socket not opened");
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(802);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("Could not bind");
    }

    if (listen(sock, 5) < 0) {
        throw std::runtime_error("Could not listen");
    }

    while (1) {
        printf("Waiting for client\n");

        int client_sock = accept(sock, 0, 0);
        SSL* ssl = SSL_new(ctx);

        SSL_set_fd(ssl, client_sock);

        int ret = SSL_accept(ssl);
        if (ret != 1) {
            printf("TLS Connection failed\n");
            SSL_free(ssl);
            close(client_sock);
            continue;
        }

        printf("TLS Connected successfully\n");

        auto transport = std::make_shared<modbus_ssl::OpenSSLTransport>(ssl);
        auto protocol = std::make_shared<modbus_server::ModbusTCPProtocol>(transport);
        auto pas = std::make_shared<modbus_server::PDUCorrelationLayer>(protocol);

        modbus_server::ModbusBasicServer client = modbus_server::ModbusBasicServer(pas);

        client.set_read_holding_registers_request_cb([](modbus_server::pdu::ReadHoldingRegistersRequest request) {
            std::vector<uint8_t> data(request.register_count * 2);

            for (size_t i = 0; i < request.register_count * 2; i++) {
                data[i] = i;
            }

            return modbus_server::pdu::ReadHoldingRegistersResponse(request, data);
        });

        printf("Running server\n");
        try {
            while (1) {
                pas->blocking_poll();
            }
        } catch (modbus_server::transport_exceptions::ConnectionClosedException e) {
            printf("Connection closed by peer\n");

            SSL_shutdown(ssl);
        } catch (modbus_ssl::OpenSSLTransportException e) {
            printf("Exception: %s\n", e.what());

            // see man SSL_GET_ERROR(3ssl)
            if (e.get_openssl_error() != SSL_ERROR_SSL) {
                SSL_shutdown(ssl);
            }
        } catch (std::exception e) {
            printf("Other Exception: %s\n", e.what());
            SSL_shutdown(ssl);
        }

        SSL_free(ssl);
        close(client_sock);
    }
}
