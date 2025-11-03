// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once
#include "connection_abstract.hpp"

#include <memory>
#include <optional>

#include <iso15118/config.hpp>
#include <iso15118/io/poll_manager.hpp>
#include <iso15118/io/sha_hash.hpp>

namespace iso15118::io {

// forward declaration
struct SSLContext;
class ConnectionSSL : public IConnection {
public:
    ConnectionSSL(PollManager&, const std::string& interface_name, const config::SSLConfig&);

    void set_event_callback(const ConnectionEventCallback&) final;
    Ipv6EndPoint get_public_endpoint() const final;

    void write(const uint8_t* buf, size_t len) final;
    ReadResult read(uint8_t* buf, size_t len) final;

    void close() final;

    std::optional<sha512_hash_t> get_vehicle_cert_hash() const final;

    ~ConnectionSSL();

private:
    PollManager& poll_manager;
    std::unique_ptr<SSLContext> ssl;

    Ipv6EndPoint end_point;

    ConnectionEventCallback event_callback{nullptr};

    bool handshake_complete{false};

    void handle_connect();
    void handle_data();
};
} // namespace iso15118::io
