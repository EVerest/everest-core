// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <modbus-ssl/openssl_transport.hpp>
#include <stdexcept>
#include <thread>

using namespace modbus_ssl;

OpenSSLTransport::OpenSSLTransport(SSL* ssl) : ssl(ssl) {
}
OpenSSLTransport::~OpenSSLTransport() {
}

std::vector<uint8_t> OpenSSLTransport::read_bytes(size_t count) {
    std::vector<uint8_t> buffer(count);
    size_t read = 0;

    while (read < count) {
        int ret;
        int err;
        {
            auto lock = std::lock_guard(mutex);
            ret = SSL_read(ssl, buffer.data() + read, count - read);

            if (ret <= 0) {
                err = SSL_get_error(ssl, ret);
            }
        }

        // auto thread_id = std::this_thread::get_id();
        // printf("SSL READ with ID: 0x%X\n", thread_id);

        if (ret <= 0) {
            // int err = SSL_get_error(ssl, ret);

            // "The operation did not complete and can be retried later."
            if (err == SSL_ERROR_WANT_READ) {
                std::this_thread::yield();
                // std::this_thread::sleep_for(std::chrono::microseconds(10));
                continue;
            }

            if (err == SSL_ERROR_WANT_WRITE) {
                std::this_thread::yield();
                // std::this_thread::sleep_for(std::chrono::microseconds(10));
                continue;
            }

            if (err == SSL_ERROR_ZERO_RETURN) {
                throw modbus_server::transport_exceptions::ConnectionClosedException();
            }

            throw OpenSSLTransportException("SSL_read failed with error " + std::string(ERR_error_string(err, NULL)),
                                            err); // todo: generate string from error code
        }
        read += ret;
    }

    return buffer;
}

std::optional<std::vector<uint8_t>> OpenSSLTransport::try_read_bytes(size_t count) {
    std::vector<uint8_t> buffer(count);
    size_t read = 0;

    auto lock = std::lock_guard(mutex);

    // First try to peek the data
    int ret = SSL_peek(ssl, buffer.data(), count);

    if (ret <= 0) {
        int err = SSL_get_error(ssl, ret);

        // "The operation did not complete and can be retried later."
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            return std::nullopt; // let openssl do its thing next time
        }

        if (err == SSL_ERROR_ZERO_RETURN) {
            throw modbus_server::transport_exceptions::ConnectionClosedException();
        }

        throw OpenSSLTransportException("SSL_peek failed with error " + std::string(ERR_error_string(err, NULL)), err);
    }

    if (static_cast<size_t>(ret) < count) {
        return std::nullopt; // not enough data
    }

    // enough data? read it!
    ret = SSL_read(ssl, buffer.data(), count);
    if (ret <= 0) {
        int err = SSL_get_error(ssl, ret);

        // "The operation did not complete and can be retried later."
        if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
            return std::nullopt;
        }

        if (err == SSL_ERROR_ZERO_RETURN) {
            throw modbus_server::transport_exceptions::ConnectionClosedException();
        }

        throw OpenSSLTransportException("SSL_read failed with error " + std::string(ERR_error_string(err, NULL)), err);
    }

    if (ret != count) {
        throw std::runtime_error("SSL_read returned less data than requested");
    }

    return buffer;
}

void OpenSSLTransport::write_bytes(const std::vector<uint8_t>& bytes) {
    size_t written = 0;

    while (written < bytes.size()) {
        auto lock = std::lock_guard(mutex);

        int ret = SSL_write(ssl, bytes.data() + written, bytes.size() - written);
        if (ret <= 0) {
            throw std::runtime_error("SSL_write failed with error " +
                                     std::string(ERR_error_string(SSL_get_error(ssl, ret), NULL)));
        }
        written += ret;
    }
}
