// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/client.hpp>
#include <modbus-server/frames/write_multiple_registers.hpp>

#include "dummy_pas.hpp"

using namespace modbus_server;
using namespace modbus_server::client;

TEST(ModbusClient, read_holding_registers) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusClient client(correlation_layer);

    correlation_layer->add_next_answer(pdu::GenericPDU(0x03, {0x04, 0x01, 0x02, 0x03, 0x04}));

    auto response = client.read_holding_registers(0x01, 0x02);
    ASSERT_EQ(response.size(), 2);
    ASSERT_EQ(response[0], 0x0102);
    ASSERT_EQ(response[1], 0x0304);
}

TEST(ModbusClient, write_single_register) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusClient client(correlation_layer);

    correlation_layer->add_next_answer(pdu::GenericPDU(0x06, {0x01, 0x02, 0x03, 0x04}));

    client.write_single_register(0x0102, 0x0304);

    auto request = correlation_layer->get_last_request();
    ASSERT_EQ(request.function_code, 0x06);
    ASSERT_EQ(request.data.size(), 4);
    ASSERT_EQ(request.data[0], 0x01);
    ASSERT_EQ(request.data[1], 0x02);
    ASSERT_EQ(request.data[2], 0x03);
    ASSERT_EQ(request.data[3], 0x04);
}

TEST(ModbusClient, write_multiple_registers) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusClient client(correlation_layer);

    ASSERT_NO_THROW(
        correlation_layer->add_next_answer(pdu::WriteMultipleRegistersResponse(0xfeed, 0x0002).to_generic()));

    client.write_multiple_registers(0xfeed, {0xc0de, 0xbeef});

    auto request = correlation_layer->get_last_request();
    auto request_parsed = pdu::WriteMultipleRegistersRequest();
    ASSERT_NO_THROW(request_parsed.from_generic(request));

    ASSERT_EQ(request_parsed.register_start, 0xfeed);
    ASSERT_EQ(request_parsed.register_count, 2);
    ASSERT_EQ(request_parsed.register_data[0], 0xc0);
    ASSERT_EQ(request_parsed.register_data[1], 0xde);
    ASSERT_EQ(request_parsed.register_data[2], 0xbe);
    ASSERT_EQ(request_parsed.register_data[3], 0xef);
}

TEST(ModbusClient, read_holding_registers_error_response) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusClient client(correlation_layer);

    correlation_layer->add_next_answer(pdu::ErrorPDU(0x03, 0x02).to_generic());

    ASSERT_THROW(client.read_holding_registers(0x01, 0x02), pdu::PDUException);
}

TEST(ModbusClient, write_single_register_error_response) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusClient client(correlation_layer);

    correlation_layer->add_next_answer(pdu::ErrorPDU(0x06, 0x02).to_generic());

    ASSERT_THROW(client.write_single_register(0x0102, 0x0304), pdu::PDUException);
}

TEST(ModbusClient, write_multiple_registers_error_response) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusClient client(correlation_layer);

    correlation_layer->add_next_answer(pdu::ErrorPDU(0x10, 0x02).to_generic());

    ASSERT_THROW(client.write_multiple_registers(0xfeed, {0xc0de, 0xbeef}), pdu::PDUException);
}
