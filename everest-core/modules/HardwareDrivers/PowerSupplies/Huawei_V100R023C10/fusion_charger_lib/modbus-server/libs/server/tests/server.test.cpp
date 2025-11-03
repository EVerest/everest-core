// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/modbus_basic_server.hpp>
#include <modbus-server/server_exception.hpp>

#include "dummy_pas.hpp"

using namespace modbus_server;

TEST(ModbusBasicServer, read_holding_registers_test) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    server.set_read_holding_registers_request_cb([&callback_called](const pdu::ReadHoldingRegistersRequest& request) {
        callback_called++;
        return pdu::ReadHoldingRegistersResponse(request, {0x01, 0x02, 0x03, 0x04});
    });

    pdu::ReadHoldingRegistersRequest req;
    req.register_start = 0x1234;
    req.register_count = 0x0002;

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req.to_generic());

    ASSERT_TRUE(response.has_value());
    pdu::ReadHoldingRegistersResponse response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));

    ASSERT_EQ(response_parsed.register_count, 2);
    ASSERT_EQ(response_parsed.register_data[0], 0x01);
    ASSERT_EQ(response_parsed.register_data[1], 0x02);
    ASSERT_EQ(response_parsed.register_data[2], 0x03);
    ASSERT_EQ(response_parsed.register_data[3], 0x04);

    ASSERT_EQ(callback_called, 1);
}

TEST(ModbusBasicServer, write_single_register_test) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    server.set_write_single_register_request_cb([&callback_called](const pdu::WriteSingleRegisterRequest& request) {
        callback_called++;
        return pdu::WriteSingleRegisterResponse(request);
    });

    pdu::WriteSingleRegisterRequest req;
    req.register_address = 0x1234;
    req.register_value = 0x5678;

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req.to_generic());

    ASSERT_TRUE(response.has_value());
    pdu::WriteSingleRegisterResponse response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));

    ASSERT_EQ(response_parsed.register_address, 0x1234);
    ASSERT_EQ(response_parsed.register_value, 0x5678);

    ASSERT_EQ(callback_called, 1);
}

TEST(ModbusBasicServer, write_multiple_registers_test) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    server.set_write_multiple_registers_request_cb(
        [&callback_called](const pdu::WriteMultipleRegistersRequest& request) {
            callback_called++;
            return pdu::WriteMultipleRegistersResponse(request);
        });

    pdu::WriteMultipleRegistersRequest req;
    req.register_start = 0x1234;
    req.register_count = 0x0002;
    req.register_data = {0x01, 0x02, 0x03, 0x04};

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req.to_generic());

    ASSERT_TRUE(response.has_value());
    pdu::WriteMultipleRegistersResponse response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));

    ASSERT_EQ(response_parsed.register_start, 0x1234);
    ASSERT_EQ(response_parsed.register_count, 0x0002);

    ASSERT_EQ(callback_called, 1);
}

TEST(ModbusBasicServer, invalid_request_sends_error_pdu) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    server.set_read_holding_registers_request_cb([&callback_called](const pdu::ReadHoldingRegistersRequest& request) {
        callback_called++;
        return pdu::ReadHoldingRegistersResponse(request, {0x01, 0x02, 0x03, 0x04});
    });

    pdu::GenericPDU req;
    req.function_code = 0x03;
    req.data = {0x01, 0x02, 0x03};

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req);

    ASSERT_TRUE(response.has_value());
    pdu::ErrorPDU response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));
    ASSERT_EQ(response_parsed.function_code, 0x03);

    ASSERT_EQ(callback_called, 0);
}

TEST(ModbusBasicServer, not_serializable_response_sends_error_pdu) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    server.set_read_holding_registers_request_cb([&callback_called](const pdu::ReadHoldingRegistersRequest& request) {
        callback_called++;
        return pdu::ReadHoldingRegistersResponse(request,
                                                 {0x01, 0x02, 0x03}); // not serializable because of missing 4th byte
    });

    pdu::ReadHoldingRegistersRequest req;
    req.register_start = 0x1234;
    req.register_count = 0x0002;

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req.to_generic());

    ASSERT_TRUE(response.has_value());
    pdu::ErrorPDU response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));
    ASSERT_EQ(response_parsed.function_code, 0x03);

    ASSERT_EQ(callback_called, 1);
}

TEST(ModbusBasicServer, application_server_error_sends_error_pdu) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    server.set_read_holding_registers_request_cb([&callback_called](const pdu::ReadHoldingRegistersRequest& request) {
        callback_called++;
        throw ApplicationServerError(pdu::PDUExceptionCode::GATEWAY_PATH_UNAVAILABLE);

        return pdu::ReadHoldingRegistersResponse(request, {}); // unreachable
    });

    pdu::ReadHoldingRegistersRequest req;
    req.register_start = 0x1234;
    req.register_count = 0x0002;

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req.to_generic());

    ASSERT_TRUE(response.has_value());
    pdu::ErrorPDU response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));
    ASSERT_EQ(response_parsed.function_code, 0x03);
    ASSERT_EQ(response_parsed.exception_code, (uint8_t)pdu::PDUExceptionCode::GATEWAY_PATH_UNAVAILABLE);

    ASSERT_EQ(callback_called, 1);
}

TEST(ModbusBasicServer, not_registered_cb_sends_illegal_function) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    pdu::ReadHoldingRegistersRequest req;
    req.register_start = 0x1234;
    req.register_count = 0x0002;

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req.to_generic());

    ASSERT_TRUE(response.has_value());
    pdu::ErrorPDU response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));
    ASSERT_EQ(response_parsed.function_code, 0x03);
    ASSERT_EQ(response_parsed.exception_code, (uint8_t)pdu::PDUExceptionCode::ILLEGAL_FUNCTION);
}

TEST(ModbusBasicServer, unknown_function_code_sends_illegal_function) {
    auto correlation_layer = std::make_shared<DummyPDUCorrelationLayer>();
    ModbusBasicServer server(correlation_layer);

    uint8_t callback_called = 0;

    pdu::GenericPDU req(0x7f, {0x00});

    std::optional<pdu::GenericPDU> response = correlation_layer->call_on_pdu(req);

    ASSERT_TRUE(response.has_value());
    pdu::ErrorPDU response_parsed;
    ASSERT_NO_THROW(response_parsed.from_generic(response.value()));
    ASSERT_EQ(response_parsed.function_code, 0x7f);
    ASSERT_EQ(response_parsed.exception_code, (uint8_t)pdu::PDUExceptionCode::ILLEGAL_FUNCTION);
}
