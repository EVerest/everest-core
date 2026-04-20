// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ocpp/common/websocket/websocket_uri.hpp"

using namespace ocpp;

TEST(WebsocketUriTest, EmptyStrings) {
    EXPECT_THROW(Uri::parse_and_validate("", "cp0001", 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "", 1), std::invalid_argument);
}

// chargepoint_id is interpolated into the HTTP request path and the Basic-Auth header at
// connect time. A CR/LF/NUL/control character would enable header injection — reject up-front.
TEST(WebsocketUriTest, ChargePointIdWithControlCharRejected) {
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "cp\r\nevil:header", 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "cp0001\n", 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", std::string("cp\0001", 6), 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "cp0001\t", 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "cp0001\x7F", 1), std::invalid_argument);
}

TEST(WebsocketUriTest, UriInvalid) {
    EXPECT_THROW(Uri::parse_and_validate("://invalid", "cp0001", 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws:test.uri.com", "cp0001", 1), std::invalid_argument);
}

TEST(WebsocketUriTest, InvalidSecurityLevel) {
    EXPECT_THROW(Uri::parse_and_validate("wss://test.uri.com", "cp0001", 4), std::invalid_argument);
}

TEST(WebsocketUriTest, SecurityLevelMismatch) {
    EXPECT_THROW(Uri::parse_and_validate("wss://test.uri.com", "cp0001", 0), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("wss://test.uri.com", "cp0001", 1), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "cp0001", 2), std::invalid_argument);
    EXPECT_THROW(Uri::parse_and_validate("ws://test.uri.com", "cp0001", 3), std::invalid_argument);
}

TEST(WebsocketUriTest, AppendingIdentity) {
    EXPECT_EQ(Uri::parse_and_validate("ws://test.uri.com/path", "cp0001", 1).string(), "ws://test.uri.com/path/cp0001");
    EXPECT_EQ(Uri::parse_and_validate("ws://test.uri.com/path/", "cp0001", 1).string(),
              "ws://test.uri.com/path/cp0001");
    EXPECT_EQ(Uri::parse_and_validate("ws://test.uri.com/path/cp0001", "cp0001", 1).string(),
              "ws://test.uri.com/path/cp0001");
}

TEST(WebsocketUriTest, SetsCorrectPortForUri) {
    auto uri_temp1 = Uri(Uri::parse_and_validate("test.uri.com/path/", "cp0001", 1));
    EXPECT_EQ(uri_temp1.string(), "ws://test.uri.com/path/cp0001");
    EXPECT_EQ(uri_temp1.get_port(), uri_default_port);

    auto uri_temp2 = Uri(Uri::parse_and_validate("test.uri.com/path/", "cp0001", 2));
    EXPECT_EQ(uri_temp2.string(), "wss://test.uri.com/path/cp0001");
    EXPECT_EQ(uri_temp2.get_port(), uri_default_secure_port);
}
