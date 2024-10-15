// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef DUMMY_MODBUS_TRANSPORT_HPP_
#define DUMMY_MODBUS_TRANSPORT_HPP_

#include <modbus-server/transport.hpp>
#include <stdexcept>

// Dummy ModbusTransport for testing purposes
class DummyModbusTransport : public modbus_server::ModbusTransport {
private:
    std::vector<std::uint8_t> incoming_data;
    std::vector<std::uint8_t> outgoing_data;

public:
    std::optional<std::vector<std::uint8_t>> try_read_bytes(size_t count) override {
        return std::nullopt;
    };

    std::vector<std::uint8_t> read_bytes(size_t count) override {
        if (incoming_data.size() < count) {
            throw std::runtime_error("DummyModbusTransport: not enough data to read");
        }
        std::vector<std::uint8_t> bytes(incoming_data.begin(), incoming_data.begin() + count);
        incoming_data.erase(incoming_data.begin(), incoming_data.begin() + count);
        return bytes;
    }

    void write_bytes(const std::vector<std::uint8_t>& bytes) override {
        outgoing_data.insert(outgoing_data.end(), bytes.begin(), bytes.end());
    }

    void add_incoming_data(const std::vector<std::uint8_t>& data) {
        incoming_data.insert(incoming_data.end(), data.begin(), data.end());
    }
    std::vector<std::uint8_t> get_outgoing_data() {
        auto copy = outgoing_data;
        outgoing_data.clear();
        return copy;
    }
};

#endif
