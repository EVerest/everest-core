// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SSL_M_HPP
#define MODBUS_SSL_M_HPP
#include <openssl/ssl.h>

#include <modbus-server/transport.hpp>
#include <mutex>

namespace modbus_ssl {

class OpenSSLTransport : public modbus_server::ModbusTransport {
public:
    OpenSSLTransport(SSL* ssl);
    ~OpenSSLTransport();

    std::vector<uint8_t> read_bytes(size_t count) override;
    std::optional<std::vector<uint8_t>> try_read_bytes(size_t count) override;

    void write_bytes(const std::vector<uint8_t>& bytes) override;

private:
    SSL* ssl;
    std::mutex mutex;
};

class OpenSSLTransportException : public std::runtime_error {
protected:
    int openssl_error;

public:
    OpenSSLTransportException(const std::string& message, int openssl_error) :
        std::runtime_error(message), openssl_error(openssl_error) {
    }

    int get_openssl_error() {
        return openssl_error;
    }
};

}; // namespace modbus_ssl

#endif
