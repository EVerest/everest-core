// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// Unit tests for the LemDCBMTimeSyncHelper

#include "http_client_interface.hpp"
#include "lem_dcbm_400600_controller.hpp"
#include "lem_dcbm_time_sync_helper.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

namespace module::main {

class HTTPClientMock : public HttpClientInterface {
public:
    MOCK_METHOD(HttpResponse, get, (const std::string& path), (override, const));
    MOCK_METHOD(HttpResponse, post, (const std::string& path, const std::string& body), (override, const));
    MOCK_METHOD(HttpResponse, put, (const std::string& path, const std::string& body), (override, const));
};

// Fixture class providing
//   - a http client mock
//   - a mock of the time sync helper
//   - default responses & request objects
class LemDCBMTimeSyncHelperTest : public ::testing::Test {

protected:
    std::unique_ptr<HTTPClientMock> http_client;

    const std::string put_settings_response_success{R"({
                                                "meterId": "mock_meter_id",
                                                "result": 1
                                                })"};

    const std::string put_settings_response_fail{R"({
                                                "meterId": "mock_meter_id",
                                                "result": 0
                                                })"};

    const std::string expected_system_sync_request_regex{
        R"(\{"time":\{"utc":"[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]+)?Z"\}\})"};

    const std::string expected_tz_sync_request_regex{R"(\{"time": \{"tz":""\}\})"};

    const std::string expected_dst_sync_request_regex{R"(\{"time": \{"dst":\}\})"};

    const std::string expected_ntp_sync_request{
        R"({"ntp":{"servers":[{"ipAddress":"123.123.123.123","port":123},{"ipAddress":"213.213.213.213","port":213}],"syncPeriod":120,"ntpActivated":true}})"};

    const struct ntp_server_spec spec_ntp_disabled {};

    const struct ntp_server_spec spec_ntp_enabled {
        "123.123.123.123", 123, "213.213.213.213", 213
    };

    void SetUp() override {
        this->http_client = std::make_unique<HTTPClientMock>();
    }
};

// Extended fixture for parametrizing tests for invalid response checks
class LemDCBMTimeSyncHelperTestInvalidResponses
    : public LemDCBMTimeSyncHelperTest,
      public ::testing::WithParamInterface<testing::internal::ReturnAction<HttpResponse>> {};

//****************************************************************

/// \brief sync() sends correct HTTP request when in system time mode
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_success_system_time) {
    std::string input_to_put;
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", testing::ContainsRegex(this->expected_dst_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    EXPECT_CALL(*this->http_client, put("/v1/settings", testing::ContainsRegex(this->expected_tz_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    EXPECT_CALL(*this->http_client,
                put("/v1/settings", testing::ContainsRegex(this->expected_system_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    LemDCBMTimeSyncHelper helper(spec_ntp_disabled);
    helper.restart_unsafe_period();

    // Act
    helper.sync(*this->http_client);
}

/// \brief sync() sends correct HTTP request when in NTP mode
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_success_ntp) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", expected_ntp_sync_request))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    LemDCBMTimeSyncHelper helper(spec_ntp_enabled);
    helper.restart_unsafe_period();

    // Act
    helper.sync(*this->http_client);
}

/// \brief sync() throws an exception if it gets a status code other than 200, system time mode
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_if_status_code_not_200_sys_time) {
    // Setup
    EXPECT_CALL(*this->http_client,
                put("/v1/settings", testing::ContainsRegex(this->expected_system_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{400, ""}));
    LemDCBMTimeSyncHelper helper(spec_ntp_disabled);
    helper.restart_unsafe_period();

    // Act
    EXPECT_THROW(helper.sync(*this->http_client), LemDCBM400600Controller::UnexpectedDCBMResponseCode);
}

/// \brief sync() throws an exception if it gets a status code other than 200, NTP mode
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_if_status_code_not_200_ntp) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", expected_ntp_sync_request))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{400, ""}));
    LemDCBMTimeSyncHelper helper(spec_ntp_enabled);
    helper.restart_unsafe_period();

    // Act
    EXPECT_THROW(helper.sync(*this->http_client), LemDCBM400600Controller::UnexpectedDCBMResponseCode);
}

/// \brief sync() throws exception if it gets a response of 200, but a failed write (result=0), system time mode
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_if_200_but_result_is_0_sys_time) {
    // Setup
    EXPECT_CALL(*this->http_client,
                put("/v1/settings", testing::ContainsRegex(this->expected_system_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_fail}));
    LemDCBMTimeSyncHelper helper(spec_ntp_disabled);
    helper.restart_unsafe_period();

    // Act
    EXPECT_THROW(helper.sync(*this->http_client), LemDCBM400600Controller::UnexpectedDCBMResponseBody);
}

/// \brief sync() throws exception if it gets a response of 200, but a failed write (result=0), NTP mode
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_if_200_but_result_is_0_ntp) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", expected_ntp_sync_request))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_fail}));
    LemDCBMTimeSyncHelper helper(spec_ntp_enabled);
    helper.restart_unsafe_period();

    // Act
    EXPECT_THROW(helper.sync(*this->http_client), LemDCBM400600Controller::UnexpectedDCBMResponseBody);
}

/// \brief sync_if_deadline_expired() called twice will not send anything the second time if the first call succeeds
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_if_deadline_expired_twice_when_first_succeeds) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", testing::ContainsRegex(this->expected_dst_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    EXPECT_CALL(*this->http_client, put("/v1/settings", testing::ContainsRegex(this->expected_tz_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    EXPECT_CALL(*this->http_client,
                put("/v1/settings", testing::ContainsRegex(this->expected_system_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));

    // override the timing constants so that we don't need to wait for the time barriers to pass
    // We do set deadline_increment_after_sync though, as we want to ensure it has not passed
    struct timing_config timing_constants {
        std::chrono::seconds(0), std::chrono::seconds(0), std::chrono::seconds(10000000),
    };

    LemDCBMTimeSyncHelper helper(spec_ntp_disabled, timing_constants);
    helper.restart_unsafe_period();

    // Act
    helper.sync_if_deadline_expired(*this->http_client);
    helper.sync_if_deadline_expired(*this->http_client);
}

/// \brief sync_if_deadline_expired() called twice will not send anything the second time, even if the first call fails
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_if_deadline_expired_twice_when_first_fails) {
    // Setup
    EXPECT_CALL(*this->http_client,
                put("/v1/settings", testing::ContainsRegex(this->expected_system_sync_request_regex)))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_fail}));

    // override the timing constants so that we don't need to wait for the time barriers to pass
    // We do set min_time_between_sync_retries though, as we want to ensure it has not passed
    struct timing_config timing_constants {
        std::chrono::seconds(0), std::chrono::seconds(1000000), std::chrono::seconds(0),
    };

    LemDCBMTimeSyncHelper helper(spec_ntp_disabled, timing_constants);
    helper.restart_unsafe_period();

    // Act
    helper.sync_if_deadline_expired(*this->http_client);
    helper.sync_if_deadline_expired(*this->http_client);
}

/// \brief sync() in NTP mode will not send the sync twice if the first sync succeeded
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_twice_if_first_succeeds) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", expected_ntp_sync_request))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));

    // override the timing constants so that we don't need to wait for the time barriers to pass
    struct timing_config timing_constants {
        std::chrono::seconds(0), std::chrono::seconds(0), std::chrono::seconds(0),
    };

    LemDCBMTimeSyncHelper helper(spec_ntp_enabled, timing_constants);
    helper.restart_unsafe_period();

    // Act
    helper.sync(*this->http_client);
    helper.sync(*this->http_client);
}

/// \brief sync() in NTP mode will send the sync twice, even if the first sync succeeded, if it's not safe to save
/// settings yet
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_twice_if_first_succeeds_before_safe) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", expected_ntp_sync_request))
        .Times(2)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}))
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));

    // override the timing constants so that we don't need to wait for the time barriers to pass
    // we do set min_time_before_setting_write_is_safe as we want to ensure it hasn't passed yet though
    struct timing_config timing_constants {
        std::chrono::seconds(1000000), std::chrono::seconds(0), std::chrono::seconds(0),
    };

    LemDCBMTimeSyncHelper helper(spec_ntp_enabled, timing_constants);
    helper.restart_unsafe_period();

    // Act
    helper.sync(*this->http_client);
    helper.sync(*this->http_client);
}

/// \brief sync() in NTP mode will send the sync twice if the first sync failed
TEST_F(LemDCBMTimeSyncHelperTest, test_sync_exception_twice_if_first_fails) {
    // Setup
    EXPECT_CALL(*this->http_client, put("/v1/settings", expected_ntp_sync_request))
        .Times(2)
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_fail}))
        .WillOnce(testing::Return(HttpResponse{200, this->put_settings_response_success}));
    LemDCBMTimeSyncHelper helper(spec_ntp_enabled);
    helper.restart_unsafe_period();

    // Act
    EXPECT_THROW(helper.sync(*this->http_client), LemDCBM400600Controller::UnexpectedDCBMResponseBody);
    helper.sync(*this->http_client);
}

} // namespace module::main