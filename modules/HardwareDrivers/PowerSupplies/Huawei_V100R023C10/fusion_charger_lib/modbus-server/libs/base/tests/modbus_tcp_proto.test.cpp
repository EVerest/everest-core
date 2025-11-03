// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/transport.hpp>
#include <modbus-server/transport_protocol.hpp>

#include "dummy_modbus_transport.hpp"

using namespace modbus_server;

#define ASSERT_EQ_VECTOR(a, b)                                                                                         \
    ASSERT_EQ(a.size(), b.size());                                                                                     \
    for (size_t i = 0; i < a.size(); i++) {                                                                            \
        ASSERT_EQ(a[i], b[i]);                                                                                         \
    }

// Assert that 2 vectors are either different in size or have at least one
// different element
#define ASSERT_NE_VECTOR(a, b)                                                                                         \
    if (a.size() == b.size()) {                                                                                        \
        bool diff = false;                                                                                             \
        for (size_t i = 0; i < a.size(); i++) {                                                                        \
            if (a[i] != b[i]) {                                                                                        \
                diff = true;                                                                                           \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        ASSERT_TRUE(diff);                                                                                             \
    } else {                                                                                                           \
        ASSERT_TRUE(true);                                                                                             \
    }

TEST(ModbusTCPProtocol, client_scenario) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol = ModbusTCPProtocol(transport, 0x01, 0x0000);

    auto send_context = protocol.new_send_context();
    protocol.send_blocking(pdu::GenericPDU(0xab, {0xde, 0xad, 0xca}), send_context);

    auto written = transport->get_outgoing_data();
    ASSERT_EQ(written.size(), 7 + 4); // header + pdu
    // transaction id should be 0
    ASSERT_EQ(written[0], 0);
    ASSERT_EQ(written[1], 0);
    // protocol id should be 0
    ASSERT_EQ(written[2], 0);
    ASSERT_EQ(written[3], 0);
    // length should be 5
    ASSERT_EQ(written[4], 0);
    ASSERT_EQ(written[5], 5);

    ASSERT_EQ(written[7], 0xab);
    ASSERT_EQ(written[8], 0xde);
    ASSERT_EQ(written[9], 0xad);
    ASSERT_EQ(written[10], 0xca);

    std::vector<uint8_t> response = {
        0x00, 0x00, // transaction id
        0x00, 0x00, // protocol id
        0x00, 0x05, // length
        0x01,       // unit id
        0xde, 0xad, 0xbe, 0xef,
    };
    transport->add_incoming_data(response);

    auto [received_context, received_pdu] = protocol.receive_blocking();
    ASSERT_EQ(received_pdu.function_code, 0xde);
    ASSERT_EQ(received_pdu.data.size(), 3);
    ASSERT_EQ(received_pdu.data[0], 0xad);
    ASSERT_EQ(received_pdu.data[1], 0xbe);
    ASSERT_EQ(received_pdu.data[2], 0xef);

    // check that context are the same
    ASSERT_EQ(send_context, received_context);
}

TEST(ModbusTCPProtocol, client_scenario_different_unit_ids) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol = ModbusTCPProtocol(transport, 0xde, 0x0000);

    auto send_context = protocol.new_send_context();
    protocol.send_blocking(pdu::GenericPDU(0xab, {0xde, 0xad, 0xca}), send_context);

    auto written = transport->get_outgoing_data();
    ASSERT_EQ(written.size(), 7 + 4); // header + pdu
    // transaction id should be 0
    ASSERT_EQ(written[0], 0);
    ASSERT_EQ(written[1], 0);
    // unit id should be 0xde
    ASSERT_EQ(written[6], 0xde);

    std::vector<uint8_t> response = {
        0x00, 0x00, // transaction id
        0x00, 0x00, // protocol id
        0x00, 0x05, // length
        0x0f,       // unit id (different)
        0xca, 0xfe, 0xbe, 0xef,
    };
    transport->add_incoming_data(response);

    auto [received_context, received_pdu] = protocol.receive_blocking();
    ASSERT_EQ(received_pdu.function_code, 0xca);
    ASSERT_EQ(received_pdu.data.size(), 3);
    ASSERT_EQ(received_pdu.data[0], 0xfe);
    ASSERT_EQ(received_pdu.data[1], 0xbe);
    ASSERT_EQ(received_pdu.data[2], 0xef);

    // check that context are the same
    ASSERT_NE(send_context.data, received_context.data);
}

TEST(ModbusTCPProtocol, new_send_context_generates_different_contexts) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol = ModbusTCPProtocol(transport);

    auto context1 = protocol.new_send_context();
    auto context2 = protocol.new_send_context();
    auto context3 = protocol.new_send_context();

    ASSERT_NE_VECTOR(context1.data, context2.data);
    ASSERT_NE_VECTOR(context2.data, context3.data);
    ASSERT_NE_VECTOR(context1.data, context3.data);
}

TEST(ModbusTCPProtocol, server_scenario) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol = ModbusTCPProtocol(transport);

    std::vector<uint8_t> request = {
        0xca, 0xfe, // transaction id
        0x00, 0x00, // protocol id
        0x00, 0x07, // length
        0x01,       // unit id
        0xde,       // function code
        0xad, 0xbe, 0xef, 0x00, 0x01,
    };
    transport->add_incoming_data(request);

    auto [received_context, received_pdu] = protocol.receive_blocking();
    ASSERT_EQ(received_pdu.function_code, 0xde);
    ASSERT_EQ(received_pdu.data.size(), 5);
    ASSERT_EQ(received_pdu.data[0], 0xad);
    ASSERT_EQ(received_pdu.data[1], 0xbe);
    ASSERT_EQ(received_pdu.data[2], 0xef);
    ASSERT_EQ(received_pdu.data[3], 0x00);
    ASSERT_EQ(received_pdu.data[4], 0x01);

    pdu::GenericPDU response(0xab, {0xca, 0xfe, 0xba, 0xbe, 0x00});
    protocol.send_blocking(response, received_context);

    auto written = transport->get_outgoing_data();
    ASSERT_EQ(written.size(), 7 + 6); // header + pdu
    // transaction id should be 0xcafe
    ASSERT_EQ(written[0], 0xca);
    ASSERT_EQ(written[1], 0xfe);
    // protocol id should be 0
    ASSERT_EQ(written[2], 0);
    ASSERT_EQ(written[3], 0);
    // length should be 8
    ASSERT_EQ(written[4], 0);
    ASSERT_EQ(written[5], 7);

    ASSERT_EQ(written[7], 0xab);
    ASSERT_EQ(written[8], 0xca);
    ASSERT_EQ(written[9], 0xfe);
    ASSERT_EQ(written[10], 0xba);
    ASSERT_EQ(written[11], 0xbe);
    ASSERT_EQ(written[12], 0x00);
}

TEST(ModbusTCPProtocol, initial_transaction_id) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol = ModbusTCPProtocol(transport, 0xab, 0xc0de);

    auto context = protocol.new_send_context();
    protocol.send_blocking(pdu::GenericPDU(0xde, {0xad, 0xca, 0xfe}), context);

    auto written = transport->get_outgoing_data();
    ASSERT_EQ(written.size(), 7 + 4); // header + pdu
    // transaction id should be 0xc0de
    ASSERT_EQ(written[0], 0xc0);
    ASSERT_EQ(written[1], 0xde);
    // protocol id should be 0
    ASSERT_EQ(written[2], 0);
    ASSERT_EQ(written[3], 0);
    // length should be 5
    ASSERT_EQ(written[4], 0);
    ASSERT_EQ(written[5], 5);
    // unit id should be 0xab
    ASSERT_EQ(written[6], 0xab);
}

TEST(ModbusTCPProtocol, zero_length) {
    auto transport = std::make_shared<DummyModbusTransport>();
    ModbusTCPProtocol protocol = ModbusTCPProtocol(transport, 0xab, 0xc0de);

    transport->add_incoming_data({0xc0, 0xde, 0, 0, 0, 0, 0xab}); // empty pdu
    auto a = protocol.try_receive();

    ASSERT_EQ(a.has_value(), false);
}
