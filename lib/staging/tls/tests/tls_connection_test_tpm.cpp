// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "tls_connection_test.hpp"
#include <openssl_util.hpp>

#include <memory>
#include <mutex>
#include <poll.h>

using namespace std::chrono_literals;

namespace {
using result_t = tls::Connection::result_t;
using tls::status_request::ClientStatusRequestV2;

constexpr auto server_root_CN = "00000000";
constexpr auto alt_server_root_CN = "11111111";
constexpr auto WAIT_FOR_SERVER_START_TIMEOUT = 50ms;

// ----------------------------------------------------------------------------
// The tests

TEST_F(TlsTestTpm, StartConnectDisconnect) {
    start();
    connect();
    // no status requested
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

} // namespace
