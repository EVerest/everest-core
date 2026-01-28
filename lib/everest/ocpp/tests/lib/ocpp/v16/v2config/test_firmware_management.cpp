// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <optional>

#include "configuration_stub.hpp"
#include "ocpp/v16/types.hpp"

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, SupportedFileTransferProtocols) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getSupportedFileTransferProtocols(), "FTP");
    auto kv = get()->getSupportedFileTransferProtocolsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SupportedFileTransferProtocols");
    EXPECT_EQ(kv.value().value, "FTP");
    EXPECT_TRUE(kv.value().readonly);
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, SetSupportedFileTransferProtocolsV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("FirmwareManagement", "SupportedFileTransferProtocols", "HTTP,HTTPS");

    EXPECT_EQ(v2_config->getSupportedFileTransferProtocols(), "HTTP,HTTPS");
    auto kv = v2_config->getSupportedFileTransferProtocolsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SupportedFileTransferProtocols");
    EXPECT_EQ(kv.value().value, "HTTP,HTTPS");
    EXPECT_TRUE(kv.value().readonly);
}

} // namespace
