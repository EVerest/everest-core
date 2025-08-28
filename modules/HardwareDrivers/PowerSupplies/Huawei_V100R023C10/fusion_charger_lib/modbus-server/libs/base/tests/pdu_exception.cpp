// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/frames.hpp>

using namespace modbus_server::pdu;

TEST(PDUExceptionCodes, to_string) {
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::ILLEGAL_FUNCTION), "ILLEGAL_FUNCTION");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::ILLEGAL_DATA_ADDRESS), "ILLEGAL_DATA_ADDRESS");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::ILLEGAL_DATA_VALUE), "ILLEGAL_DATA_VALUE");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::SERVER_DEVICE_FAILURE), "SERVER_DEVICE_FAILURE");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::ACKNOWLEDGE), "ACKNOWLEDGE");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::SERVER_DEVICE_BUSY), "SERVER_DEVICE_BUSY");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::MEMORY_PARITY_ERROR), "MEMORY_PARITY_ERROR");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::GATEWAY_PATH_UNAVAILABLE), "GATEWAY_PATH_UNAVAILABLE");
    EXPECT_EQ(exception_code_to_string(PDUExceptionCode::GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND),
              "GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND");
}

TEST(PDUException, from_pdu_what) {
    GenericPDU pdu;
    pdu.function_code = 0x80 | 0x03;
    pdu.data = {0x01};

    PDUException ex(pdu);
    EXPECT_STREQ(ex.what(), "PDUException: ILLEGAL_FUNCTION");
}

TEST(PDUException, from_code_what) {
    PDUException ex(PDUExceptionCode::ILLEGAL_DATA_ADDRESS);
    EXPECT_STREQ(ex.what(), "PDUException: ILLEGAL_DATA_ADDRESS");
}

TEST(PDUException, get_exception_code) {
    PDUException ex(PDUExceptionCode::ILLEGAL_DATA_ADDRESS);
    EXPECT_EQ(ex.get_exception_code(), (uint8_t)PDUExceptionCode::ILLEGAL_DATA_ADDRESS);
}
