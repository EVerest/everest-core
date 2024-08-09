// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

/**
 * \file testing patched version of OpenSSL
 *
 * These tests will only pass on a patched version of OpenSSL.
 * (they should compile and run fine with some test failures)
 *
 * It is recommended to also run tests alongside Wireshark
 * e.g. `./patched_test --gtest_filter=TlsTest.TLS12`
 * to check that the Server Hello record is correctly formed:
 * - no status_request or status_request_v2 then no Certificate Status record
 * - status_request or status_request_v2 then there is a Certificate Status record
 * - never both status_request and status_request_v2
 */

#include "tls_connection_test.hpp"

namespace {

// ----------------------------------------------------------------------------
// The tests - only pass on a patched OpenSSL

TEST_F(TlsTest, TLS12) {
    // test using TLS 1.2
    start();
    connect();
    // no status requested
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_set(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = false;
    client_config.status_request_v2 = true;
    connect();
    // status_request_v2 only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_set(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request and status_request_v2
    // status_request_v2 is preferred over status_request
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_set(flags_t::status_request_v2));
}

} // namespace
