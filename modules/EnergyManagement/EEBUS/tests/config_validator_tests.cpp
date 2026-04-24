// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <ConfigValidator.hpp>
#include <EEBUS.hpp>

namespace module {

// Readable SKI constants for parse/normalize/dedupe tests.
static const std::string SKI_A(40, 'a');
static const std::string SKI_B(40, 'b');
static const std::string SKI_A_UPPER(40, 'A');

class ConfigValidatorTest : public ::testing::Test {
protected:
    // Build a Conf with only the allowlist + accept_unknown fields varied;
    // other fields are populated with sane defaults so validate() doesn't
    // choke on unrelated rules. manage_eebus_grpc_api_binary = false to
    // skip filesystem setup.
    Conf make_conf(const std::string& allowlist_csv, bool accept_unknown = false) {
        Conf c{};
        c.manage_eebus_grpc_api_binary = false;
        c.eebus_service_port = 4715;
        c.grpc_port = 50051;
        c.eebus_ems_ski_allowlist = allowlist_csv;
        c.accept_unknown_ems = accept_unknown;
        c.certificate_path = "eebus/evse_cert";
        c.private_key_path = "eebus/evse_key";
        c.eebus_grpc_api_binary_path = "eebus_grpc_api";
        c.vendor_code = "VEND";
        c.device_brand = "Brand";
        c.device_model = "Model";
        c.serial_number = "SN0001";
        c.failsafe_control_limit_W = 4200;
        c.max_nominal_power_W = 32000;
        c.restart_delay_s = 5;
        c.reconnect_delay_s = 5;
        return c;
    }

    ConfigValidator make_validator(const Conf& c) {
        return ConfigValidator{c, "/tmp/nonexistent/etc", "/tmp/nonexistent/libexec"};
    }
};

// ---- Valid inputs ----

TEST_F(ConfigValidatorTest, single_valid_allowlist_entry_parses_and_validates) {
    auto c = make_conf(SKI_A);
    auto v = make_validator(c);
    EXPECT_TRUE(v.validate());
    EXPECT_EQ(v.get_effective_ems_ski_allowlist().size(), 1U);
    EXPECT_EQ(*v.get_effective_ems_ski_allowlist().begin(), SKI_A);
}

TEST_F(ConfigValidatorTest, mixed_case_input_normalizes_to_lowercase) {
    auto c = make_conf(SKI_A_UPPER);
    auto v = make_validator(c);
    EXPECT_TRUE(v.validate());
    EXPECT_EQ(v.get_effective_ems_ski_allowlist().size(), 1U);
    EXPECT_EQ(*v.get_effective_ems_ski_allowlist().begin(), SKI_A);
}

TEST_F(ConfigValidatorTest, whitespace_around_csv_entries_is_trimmed) {
    auto c = make_conf("  " + SKI_A + "  ,\t" + SKI_B + "  ");
    auto v = make_validator(c);
    EXPECT_TRUE(v.validate());
    EXPECT_EQ(v.get_effective_ems_ski_allowlist().size(), 2U);
    EXPECT_EQ(v.get_effective_ems_ski_allowlist().count(SKI_A), 1U);
    EXPECT_EQ(v.get_effective_ems_ski_allowlist().count(SKI_B), 1U);
}

TEST_F(ConfigValidatorTest, multiple_entries_dedupe) {
    auto c = make_conf(SKI_A + "," + SKI_A + "," + SKI_A);
    auto v = make_validator(c);
    EXPECT_TRUE(v.validate());
    EXPECT_EQ(v.get_effective_ems_ski_allowlist().size(), 1U);
}

TEST_F(ConfigValidatorTest, empty_allowlist_with_accept_unknown_is_valid) {
    auto c = make_conf("", /*accept_unknown=*/true);
    auto v = make_validator(c);
    EXPECT_TRUE(v.validate());
    EXPECT_TRUE(v.get_effective_ems_ski_allowlist().empty());
    EXPECT_TRUE(v.get_accept_unknown_ems());
}

TEST_F(ConfigValidatorTest, empty_allowlist_without_accept_unknown_is_valid_but_inert) {
    auto c = make_conf("", /*accept_unknown=*/false);
    auto v = make_validator(c);
    // validate() returns true — the validator logs a warning but does not reject.
    EXPECT_TRUE(v.validate());
    EXPECT_TRUE(v.get_effective_ems_ski_allowlist().empty());
    EXPECT_FALSE(v.get_accept_unknown_ems());
}

// ---- Invalid inputs ----

TEST_F(ConfigValidatorTest, entry_with_non_hex_char_rejects) {
    std::string bad(40, 'a');
    bad[10] = 'g'; // 'g' is not hex
    auto c = make_conf(bad);
    auto v = make_validator(c);
    EXPECT_FALSE(v.validate());
}

TEST_F(ConfigValidatorTest, entry_too_short_rejects) {
    const std::string short39(39, 'a');
    auto c = make_conf(short39);
    auto v = make_validator(c);
    EXPECT_FALSE(v.validate());
}

TEST_F(ConfigValidatorTest, entry_too_long_rejects) {
    const std::string long41(41, 'a');
    auto c = make_conf(long41);
    auto v = make_validator(c);
    EXPECT_FALSE(v.validate());
}

} // namespace module
