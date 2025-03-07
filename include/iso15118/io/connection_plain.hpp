// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "connection_abstract.hpp"

#include <iso15118/config.hpp>
#include <iso15118/io/poll_manager.hpp>

namespace iso15118::io {

class ConnectionPlain : public IConnection {
public:
    ConnectionPlain(PollManager&, const std::string& interface_name);

    void set_event_callback(const ConnectionEventCallback&) final;
    Ipv6EndPoint get_public_endpoint() const final;

    void write(const uint8_t* buf, size_t len) final;
    ReadResult read(uint8_t* buf, size_t len) final;

    void close() final;

    std::optional<sha512_hash_t> get_vehicle_cert_hash() const final {
        return std::nullopt;
    }

    ~ConnectionPlain();

private:
    PollManager& poll_manager;

    Ipv6EndPoint end_point;

    int fd{-1};

    bool connection_open{false};

    ConnectionEventCallback event_callback{nullptr};

    void handle_connect();
    void handle_data();
};
} // namespace iso15118::io
