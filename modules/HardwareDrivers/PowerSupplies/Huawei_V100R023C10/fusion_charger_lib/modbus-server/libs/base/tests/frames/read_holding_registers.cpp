// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/frames/read_holding_registers.hpp>

TEST(ReadHoldingRegistersRequest, from_generic_splits_correctly) {
    std::vector<std::uint8_t> data = {0x03, 0xca, 0xfe, 0x00, 0x02};

    modbus_server::pdu::GenericPDU generic(data);
    modbus_server::pdu::ReadHoldingRegistersRequest request;

    ASSERT_NO_THROW(request.from_generic(generic));
    ASSERT_EQ(request.register_start, 0xcafe);
    ASSERT_EQ(request.register_count, 0x0002);
}

TEST(ReadHoldingRegistersRequest, from_generic_size_0) {
    std::vector<std::uint8_t> data = {0x04, 0x00, 0x01, 0x00, 0x00};

    modbus_server::pdu::GenericPDU generic(data);
    modbus_server::pdu::ReadHoldingRegistersRequest request;

    ASSERT_THROW(request.from_generic(generic), modbus_server::pdu::DecodingError);
}

TEST(ReadHoldingRegistersRequest, from_generic_invalid_function_code) {
    std::vector<std::uint8_t> data = {0x04, 0x00, 0x01, 0x00, 0x02};

    modbus_server::pdu::GenericPDU generic(data);
    modbus_server::pdu::ReadHoldingRegistersRequest request;

    ASSERT_THROW(request.from_generic(generic), modbus_server::pdu::DecodingError);
}

TEST(ReadHoldingRegistersRequest, from_generic_invalid_data_size_5_bytes) {
    std::vector<std::uint8_t> data = {0x03, 0x00, 0x01, 0x00, 0x02, 0xff};

    modbus_server::pdu::GenericPDU generic(data);
    modbus_server::pdu::ReadHoldingRegistersRequest request;

    ASSERT_THROW(request.from_generic(generic), modbus_server::pdu::DecodingError);
}
TEST(ReadHoldingRegistersRequest, from_generic_invalid_data_size_0_bytes) {
    std::vector<std::uint8_t> data = {0x03};

    modbus_server::pdu::GenericPDU generic(data);
    modbus_server::pdu::ReadHoldingRegistersRequest request;

    ASSERT_THROW(request.from_generic(generic), modbus_server::pdu::DecodingError);
}

TEST(ReadHoldingRegistersRequest, from_generic_invalid_register_count) {
    std::vector<std::uint8_t> data = {0x03, 0x00, 0x01, 0x00, 0x80};

    modbus_server::pdu::GenericPDU generic(data);
    modbus_server::pdu::ReadHoldingRegistersRequest request;

    ASSERT_THROW(request.from_generic(generic), modbus_server::pdu::DecodingError);
}

TEST(ReadHoldingRegistersResponse, constructor_copies_register_count_from_request) {
    modbus_server::pdu::GenericPDU req_generic(0x03, {0xde, 0xad, 0x00, 0x02});
    modbus_server::pdu::ReadHoldingRegistersRequest req;

    ASSERT_NO_THROW(req.from_generic(req_generic));

    modbus_server::pdu::ReadHoldingRegistersResponse resp(req, {0xca, 0xfe, 0xbe, 0xef});

    ASSERT_EQ(resp.register_count, req.register_count);
}

TEST(ReadHoldingRegistersResponse, to_generic_mapping_works_2_registers) {
    modbus_server::pdu::GenericPDU req_generic(0x03, {0xde, 0xad, 0x00, 0x02});
    modbus_server::pdu::ReadHoldingRegistersRequest req;

    ASSERT_NO_THROW(req.from_generic(req_generic));

    modbus_server::pdu::ReadHoldingRegistersResponse resp(req, {0xca, 0xfe, 0xbe, 0xef});

    modbus_server::pdu::GenericPDU gen = resp.to_generic();

    ASSERT_EQ(gen.function_code, 0x03);
    ASSERT_EQ(gen.data.size(), 5);
    ASSERT_EQ(gen.data[0], 0x04);
    ASSERT_EQ(gen.data[1], 0xca);
    ASSERT_EQ(gen.data[2], 0xfe);
    ASSERT_EQ(gen.data[3], 0xbe);
    ASSERT_EQ(gen.data[4], 0xef);
}

TEST(ReadHoldingRegistersResponse, to_generic_mapping_works_3_register) {
    modbus_server::pdu::GenericPDU req_generic(0x03, {0xde, 0xad, 0x00, 0x03});
    modbus_server::pdu::ReadHoldingRegistersRequest req;

    ASSERT_NO_THROW(req.from_generic(req_generic));

    modbus_server::pdu::ReadHoldingRegistersResponse resp(req, {0xca, 0xfe, 0x13, 0x37, 0xaf, 0xfe});

    modbus_server::pdu::GenericPDU gen = resp.to_generic();

    ASSERT_EQ(gen.function_code, 0x03);
    ASSERT_EQ(gen.data.size(), 7);
    ASSERT_EQ(gen.data[0], 0x06);
    ASSERT_EQ(gen.data[1], 0xca);
    ASSERT_EQ(gen.data[2], 0xfe);
    ASSERT_EQ(gen.data[3], 0x13);
    ASSERT_EQ(gen.data[4], 0x37);
    ASSERT_EQ(gen.data[5], 0xaf);
    ASSERT_EQ(gen.data[6], 0xfe);
}

TEST(ReadHoldingRegistersResponse, to_generic_too_much_register_data) {
    modbus_server::pdu::GenericPDU req_generic(0x03, {0xde, 0xad, 0x00, 0x02});
    modbus_server::pdu::ReadHoldingRegistersRequest req;

    ASSERT_NO_THROW(req.from_generic(req_generic));

    modbus_server::pdu::ReadHoldingRegistersResponse resp(req, {0xca, 0xfe, 0xbe, 0xef, 0xca});

    ASSERT_THROW(resp.to_generic(), modbus_server::pdu::EncodingError);
}

TEST(ReadHoldingRegistersResponse, to_generic_too_large_register_count) {
    const std::uint8_t register_count = 0x80;
    modbus_server::pdu::GenericPDU req_generic(0x03, {0xde, 0xad, 0x00, 0x12});
    modbus_server::pdu::ReadHoldingRegistersRequest req;

    ASSERT_NO_THROW(req.from_generic(req_generic));
    req.register_count = register_count;

    std::vector<std::uint8_t> reg_data;
    for (int i = 0; i < register_count; i++) {
        reg_data.push_back(0x00);
        reg_data.push_back(0x01);
    }

    modbus_server::pdu::ReadHoldingRegistersResponse resp(req, reg_data);

    modbus_server::pdu::GenericPDU gen;
    ASSERT_THROW(resp.to_generic(), modbus_server::pdu::EncodingError);
}

TEST(ReadHoldingRegistersResponse, from_generic_exception_contains_original) {
    modbus_server::pdu::GenericPDU req_generic(0x04, {0x00}); // invalid everything
    modbus_server::pdu::ReadHoldingRegistersRequest req;

    ASSERT_THROW(req.from_generic(req_generic), modbus_server::pdu::DecodingError);

    try {
        req.from_generic(req_generic);
    } catch (modbus_server::pdu::DecodingError e) {
        auto original = e.get_original_data();
        ASSERT_EQ(original.function_code, 0x04);
        ASSERT_EQ(original.data.size(), 1);
        ASSERT_EQ(original.data[0], 0x00);
    }
}
