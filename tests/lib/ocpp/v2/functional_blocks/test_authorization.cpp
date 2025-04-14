// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <ocpp/v2/functional_blocks/authorization.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>

#include "component_state_manager_mock.hpp"
#include "connectivity_manager_mock.hpp"
#include "device_model_test_helper.hpp"
#include "evse_manager_fake.hpp"
#include "evse_security_mock.hpp"
#include "message_dispatcher_mock.hpp"
#include "mocks/database_handler_mock.hpp"

#include <ocpp/v2/messages/Authorize.hpp>
#include <ocpp/v2/messages/ClearCache.hpp>
#include <ocpp/v2/messages/GetLocalListVersion.hpp>
#include <ocpp/v2/messages/SendLocalList.hpp>

using namespace ocpp::v2;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::Throw;

const ocpp::LeafCertificateType DEFAULT_LEAF_CERT_TYPE = ocpp::LeafCertificateType::MO;

class AuthorizationTest : public ::testing::Test {
public:
protected: // Members
    DeviceModelTestHelper device_model_test_helper;
    MockMessageDispatcher mock_dispatcher;
    DeviceModel* device_model;
    ::testing::NiceMock<ConnectivityManagerMock> connectivity_manager;
    ::testing::NiceMock<ocpp::v2::DatabaseHandlerMock> database_handler_mock;
    ocpp::EvseSecurityMock evse_security;
    EvseManagerFake evse_manager;
    ComponentStateManagerMock component_state_manager;
    FunctionalBlockContext functional_block_context;

    std::unique_ptr<Authorization> authorization;

    std::atomic<uint32_t> delete_expired_entries_count = 0;
    std::atomic<uint32_t> get_binary_size_count = 0;
    std::atomic<uint32_t> delete_nr_of_oldest_entries_count = 0;
    std::mutex call_mutex;
    std::condition_variable call_condition_variable;

protected: // Functions
    AuthorizationTest() :
        device_model_test_helper(),
        mock_dispatcher(),
        device_model(device_model_test_helper.get_device_model()),
        connectivity_manager(),
        database_handler_mock(),
        evse_security(),
        evse_manager(2),
        component_state_manager(),
        functional_block_context{this->mock_dispatcher,        *this->device_model,         this->connectivity_manager,
                                 this->evse_manager,           this->database_handler_mock, this->evse_security,
                                 this->component_state_manager},
        authorization(std::make_unique<Authorization>(functional_block_context)) {
    }

    ~AuthorizationTest() {
    }

    auto update_count_and_notify(std::atomic<uint32_t>& variable) {
        return testing::Invoke([this, &variable]() {
            std::unique_lock<std::mutex> lock(this->call_mutex);
            variable++;
            this->call_condition_variable.notify_all();
        });
    }

    auto update_count_and_notify(const size_t return_value, std::atomic<uint32_t>& variable) {
        return testing::Invoke([this, &variable, return_value]() -> size_t {
            std::unique_lock<std::mutex> lock(this->call_mutex);
            variable++;
            this->call_condition_variable.notify_all();
            return return_value;
        });
    }

    void wait_for_calls(const uint32_t expected_delete_expired_entries_count, const uint32_t expected_binary_size_count,
                        const uint32_t expected_delete_nr_of_oldest_entries_count) {
        std::unique_lock<std::mutex> lock(this->call_mutex);
        EXPECT_TRUE(call_condition_variable.wait_for(
            lock, std::chrono::seconds(3),
            [this, expected_delete_expired_entries_count, expected_binary_size_count,
             expected_delete_nr_of_oldest_entries_count] {
                return this->delete_expired_entries_count >= expected_delete_expired_entries_count &&
                       this->get_binary_size_count >= expected_binary_size_count &&
                       this->delete_nr_of_oldest_entries_count >= expected_delete_nr_of_oldest_entries_count;
            }));
    }

    ///
    /// \brief Set value of AuthCachCtrlr variable 'Enabled' in the device model.
    /// \param device_model The device model to set the value in.
    /// \param enabled      True to set to enabled.
    ///
    void set_auth_cache_enabled(DeviceModel* device_model, const bool enabled) {
        const auto& auth_cache_enabled = ControllerComponentVariables::AuthCacheCtrlrEnabled;
        EXPECT_EQ(device_model->set_value(auth_cache_enabled.component, auth_cache_enabled.variable.value(),
                                          AttributeEnum::Actual, enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    ///
    /// \brief Set value of AuthCtrlr variable 'Enabled' in the device model.
    /// \param device_model The device model to set the value in.
    /// \param enabled      True to set to enabled.
    ///
    void set_auth_ctrlr_enabled(DeviceModel* device_model, const bool enabled) {
        const auto& auth_ctrlr_enabled = ControllerComponentVariables::AuthCtrlrEnabled;
        EXPECT_EQ(device_model->set_value(auth_ctrlr_enabled.component, auth_ctrlr_enabled.variable.value(),
                                          AttributeEnum::Actual, enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    ///
    /// \brief Set value of LocalAuthListCtrlr variable 'Enabled' in the device model.
    /// \param device_model The device model to set the value in.
    /// \param enabled      True to set to enabled.
    ///
    void set_local_auth_list_ctrlr_enabled(DeviceModel* device_model, const bool enabled) {
        const auto& local_auth_list_ctrlr_enabled = ControllerComponentVariables::LocalAuthListCtrlrEnabled;
        EXPECT_EQ(device_model->set_value(local_auth_list_ctrlr_enabled.component,
                                          local_auth_list_ctrlr_enabled.variable.value(), AttributeEnum::Actual,
                                          enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void disable_remote_authorization(DeviceModel* device_model, const bool disabled) {
        const auto& disable_remote_authorization = ControllerComponentVariables::DisableRemoteAuthorization;
        EXPECT_EQ(device_model->set_value(disable_remote_authorization.component,
                                          disable_remote_authorization.variable.value(), AttributeEnum::Actual,
                                          disabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_allow_central_contract_validation(DeviceModel* device_model, const bool allow) {
        const auto& allow_central_contract_validation = ControllerComponentVariables::CentralContractValidationAllowed;
        EXPECT_EQ(device_model->set_value(allow_central_contract_validation.component,
                                          allow_central_contract_validation.variable.value(), AttributeEnum::Actual,
                                          allow ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_allow_contract_validation_offline(DeviceModel* device_model, const bool allow) {
        const auto& allow_contract_validation_offline = ControllerComponentVariables::ContractValidationOffline;
        EXPECT_EQ(device_model->set_value(allow_contract_validation_offline.component,
                                          allow_contract_validation_offline.variable.value(), AttributeEnum::Actual,
                                          allow ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_local_authorize_offline(DeviceModel* device_model, const bool value) {
        const auto& local_authorize_offline = ControllerComponentVariables::LocalAuthorizeOffline;
        EXPECT_EQ(device_model->set_value(local_authorize_offline.component, local_authorize_offline.variable.value(),
                                          AttributeEnum::Actual, value ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_auth_cache_lifetime(DeviceModel* device_model, const int value) {
        const auto& auth_cache_lifetime = ControllerComponentVariables::AuthCacheLifeTime;
        EXPECT_EQ(device_model->set_value(auth_cache_lifetime.component, auth_cache_lifetime.variable.value(),
                                          AttributeEnum::Actual, std::to_string(value), "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_local_pre_authorize(DeviceModel* device_model, const bool enable) {
        const auto& local_pre_authorize = ControllerComponentVariables::LocalPreAuthorize;
        EXPECT_EQ(device_model->set_value(local_pre_authorize.component, local_pre_authorize.variable.value(),
                                          AttributeEnum::Actual, enable ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_disable_post_authorize(DeviceModel* device_model, const bool enable) {
        const auto& auth_cache_disable_post_authorize = ControllerComponentVariables::AuthCacheDisablePostAuthorize;
        EXPECT_EQ(device_model->set_value(auth_cache_disable_post_authorize.component,
                                          auth_cache_disable_post_authorize.variable.value(), AttributeEnum::Actual,
                                          enable ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_offline_tx_for_unknown_id_enabled(DeviceModel* device_model, const bool enabled) {
        const auto& offline_tx_for_unknown_id_enabled = ControllerComponentVariables::OfflineTxForUnknownIdEnabled;
        EXPECT_EQ(device_model->set_value(offline_tx_for_unknown_id_enabled.component,
                                          offline_tx_for_unknown_id_enabled.variable.value(), AttributeEnum::Actual,
                                          enabled ? "true" : "false", "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    void set_local_auth_list_ctrlr_entries(DeviceModel* device_model, const int32_t entries) {
        const auto& list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
        EXPECT_EQ(device_model->set_value(list_entries.component, list_entries.variable.value(), AttributeEnum::Actual,
                                          std::to_string(entries), "default", true),
                  SetVariableStatusEnum::Accepted);
    }

    IdToken get_id_token(const std::string& token = "VALID_ID_TOKEN",
                         const ocpp::CiString<20> token_type = IdTokenEnumStringType::ISO14443) {
        IdToken id_token;
        id_token.idToken = token;
        id_token.type = token_type;
        return id_token;
    }

    ocpp::EnhancedMessage<MessageType>
    create_example_authorize_response(const std::optional<AuthorizeCertificateStatusEnum> certificate_status,
                                      const AuthorizationStatusEnum& status) {
        AuthorizeResponse response;
        response.certificateStatus = certificate_status;
        IdTokenInfo id_token_info;
        id_token_info.status = status;
        response.idTokenInfo = id_token_info;
        ocpp::CallResult<AuthorizeResponse> call_result(response, "uniqueId");
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::AuthorizeResponse;
        enhanced_message.message = call_result;
        return enhanced_message;
    }

    ocpp::EnhancedMessage<MessageType> create_example_clear_cache_request() {
        ClearCacheRequest request;
        ocpp::Call<ClearCacheRequest> call(request, "uniqueId");
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::ClearCache;
        enhanced_message.message = call;
        return enhanced_message;
    }

    ocpp::EnhancedMessage<MessageType> create_get_local_list_version_request() {
        GetLocalListVersionRequest request;
        ocpp::Call<GetLocalListVersionRequest> call(request, "uniqueId");
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::GetLocalListVersion;
        enhanced_message.message = call;
        return enhanced_message;
    }

    ocpp::EnhancedMessage<MessageType>
    create_send_local_list_request(const int32_t version_number, const UpdateEnum update_type,
                                   std::optional<std::vector<AuthorizationData>> local_authorization_list) {
        SendLocalListRequest request;
        request.localAuthorizationList = local_authorization_list;
        request.updateType = update_type;
        request.versionNumber = version_number;
        ocpp::Call<SendLocalListRequest> call(request, "uniqueId");
        ocpp::EnhancedMessage<MessageType> enhanced_message;
        enhanced_message.messageType = MessageType::SendLocalList;
        enhanced_message.message = call;
        return enhanced_message;
    }

    AuthorizationCacheEntry create_authorization_cache_entry(const AuthorizationStatusEnum status,
                                                             const bool cache_expiry_has_value,
                                                             const bool cache_expired, const bool lifetime_expired,
                                                             const int cache_lifetime_seconds) {
        AuthorizationCacheEntry authorization_cache_entry;
        IdTokenInfo cache_entry_token_info;
        cache_entry_token_info.status = status;
        if (cache_expiry_has_value) {
            auto cache_timepoint = date::utc_clock::now();
            if (cache_expired) {
                cache_timepoint -= std::chrono::seconds(100);
            } else {
                cache_timepoint += std::chrono::seconds(100);
            }
            cache_entry_token_info.cacheExpiryDateTime = ocpp::DateTime(cache_timepoint);
        }

        auto timepoint_past = date::utc_clock::now();
        int seconds = 0;
        if (lifetime_expired) {
            seconds = cache_lifetime_seconds + 100;
        } else {
            seconds = cache_lifetime_seconds / 2;
        }
        timepoint_past -= std::chrono::seconds(seconds);
        authorization_cache_entry.last_used = ocpp::DateTime(timepoint_past);

        authorization_cache_entry.id_token_info = cache_entry_token_info;
        return authorization_cache_entry;
    }

    ///
    /// \brief Create local auth list with two or three items.
    /// \param include_duplicate    If true, three items are in the list with one duplicate id token. Otherwise there
    ///                             will be two items in the list.
    /// \return The vector with authorization data with two or three items.
    ///
    std::vector<AuthorizationData> create_example_authorization_data_local_list(
        const bool include_duplicate, const bool has_token_info,
        AuthorizationStatusEnum authorization_status = AuthorizationStatusEnum::Accepted) {
        std::vector<AuthorizationData> list;
        AuthorizationData d;
        IdTokenInfo info;
        if (has_token_info) {
            info.status = authorization_status;
            d.idTokenInfo = info;
        }

        d.idToken = get_id_token("TEST_TOKEN_1");
        list.push_back(d);
        d.idToken = get_id_token("TEST_TOKEN_2");
        list.push_back(d);
        if (include_duplicate) {
            list.push_back(d);
        }

        return list;
    }
};

TEST_F(AuthorizationTest, is_auth_cache_ctrlr_enabled) {
    // Check if the auth cache ctrlr is enabled.

    // Set auth cache ctrlr enabled to 'false' in the device model.
    set_auth_cache_enabled(this->device_model, false);
    EXPECT_FALSE(authorization->is_auth_cache_ctrlr_enabled());

    // Set auth cache ctrlr enabled to 'true' in the device model.
    set_auth_cache_enabled(this->device_model, true);
    EXPECT_TRUE(authorization->is_auth_cache_ctrlr_enabled());

    // Remove auth cache ctrlr enabled variable from the device model.
    EXPECT_TRUE(this->device_model_test_helper.remove_variable_from_db(
        ControllerComponentVariables::AuthCacheCtrlrEnabled.component.name, std::nullopt, std::nullopt, std::nullopt,
        ControllerComponentVariables::AuthCacheCtrlrEnabled.variable->name, std::nullopt));
    this->device_model = this->device_model_test_helper.get_device_model();
    FunctionalBlockContext context = {
        this->mock_dispatcher,       *this->device_model, this->connectivity_manager,   this->evse_manager,
        this->database_handler_mock, this->evse_security, this->component_state_manager};
    this->authorization = std::make_unique<Authorization>(context);
    EXPECT_FALSE(authorization->is_auth_cache_ctrlr_enabled());
}

TEST_F(AuthorizationTest, authorize_req_websocket_disconnected) {
    // Try to do an authorize request when the websocket is disconnected.
    ON_CALL(this->connectivity_manager, is_websocket_connected()).WillByDefault(Return(false));
    const AuthorizeResponse response = authorization->authorize_req(get_id_token(), std::nullopt, std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, authorize_req_wrong_future_message_type) {
    // Try to do an authorize request with the websocket connected. The dispatch_call_async returns
    // a wrong message type.
    ON_CALL(this->connectivity_manager, is_websocket_connected()).WillByDefault(Return(true));
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::GetDisplayMessages;
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _))
        .WillOnce(Return(std::async(std::launch::deferred, [enhanced_message]() { return enhanced_message; })));

    const AuthorizeResponse response = authorization->authorize_req(get_id_token(), std::nullopt, std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, authorize_req_accepted) {
    // Try to do an authorize request, which is accepted.
    ON_CALL(this->connectivity_manager, is_websocket_connected()).WillByDefault(Return(true));
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Accepted);
    })));

    const AuthorizeResponse response = authorization->authorize_req(get_id_token(), std::nullopt, std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, authorize_req_exception) {
    // Try to do an authorize request, during which which an exception is thrown.
    ON_CALL(this->connectivity_manager, is_websocket_connected()).WillByDefault(Return(true));
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        // Create authorize response with a wrong enum value, which will throw.
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 static_cast<AuthorizationStatusEnum>(INT32_MAX));
    })));

    const AuthorizeResponse response = authorization->authorize_req(get_id_token(), std::nullopt, std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, authorize_req_exception2) {
    // Try to do an authorization request, an exception is thrown for the authorize response.
    ON_CALL(this->connectivity_manager, is_websocket_connected()).WillByDefault(Return(true));
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        // Create authorize response with a from enum value for the authorize certificute status, which will
        // cause an exception to be thrown.
        return create_example_authorize_response(static_cast<AuthorizeCertificateStatusEnum>(INT32_MAX),
                                                 AuthorizationStatusEnum::Accepted);
    })));

    const AuthorizeResponse response = authorization->authorize_req(get_id_token(), std::nullopt, std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, update_authorization_cache_size) {
    // Test update authorization cache size and check in the device model if the cache size is indeed updated.
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    this->device_model->set_read_only_value(auth_cache_size.component, auth_cache_size.variable.value(),
                                            AttributeEnum::Actual, "42", "test");
    std::optional<int> size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    EXPECT_EQ(size.value(), 42);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size()).WillRepeatedly(Return(35));
    this->authorization->update_authorization_cache_size();

    size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    EXPECT_EQ(size.value(), 35);
}

TEST_F(AuthorizationTest, update_authorization_cache_size_exception) {
    // Test update authorization cache size. When requesting the size from the database handler, it throws a
    // DatabaseException.
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    this->device_model->set_read_only_value(auth_cache_size.component, auth_cache_size.variable.value(),
                                            AttributeEnum::Actual, "42", "test");
    std::optional<int> size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    EXPECT_EQ(size.value(), 42);

    // Throw DatabaseException when requesting the binary size of the authorization cache. Application should not crash!
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("Database exception thrown!!")));

    this->authorization->update_authorization_cache_size();

    size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    // Value is not changed because the exception was thrown.
    EXPECT_EQ(size.value(), 42);
}

TEST_F(AuthorizationTest, update_authorization_cache_size_exception2) {
    // Test update authorization cache size. When requesting the size from the database handler, it throws (something
    // else than DatabaseException).
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    this->device_model->set_read_only_value(auth_cache_size.component, auth_cache_size.variable.value(),
                                            AttributeEnum::Actual, "42", "test");
    std::optional<int> size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    EXPECT_EQ(size.value(), 42);

    // Throw other exception when requesting the binary size of the authorization cache. Application should not crash!
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
        .WillRepeatedly(Throw(std::out_of_range("out of range exception thrown!!")));

    this->authorization->update_authorization_cache_size();

    size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    // Value is not changed because the exception was thrown.
    EXPECT_EQ(size.value(), 42);
}

TEST_F(AuthorizationTest, validate_token_accepted_central_token) {
    // Validate token: central token. For a central token, an authorize request should not be sent.

    // Set AuthCtrlr::Enabled to true
    this->set_auth_ctrlr_enabled(this->device_model, true);
    IdToken id_token;
    // For a central token, an authorize request should not be sent.
    id_token.type = IdTokenEnumStringType::Central;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_accepted_auth_ctrlr_disabled) {
    // Set AuthCtrlr::Enabled to false: no authorize request should be sent, just accept token.
    this->set_auth_ctrlr_enabled(this->device_model, false);
    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_unknown) {
    // Set AuthCtrlr::Enabled to true
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // Local auth list is disabled.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Auth cache is disabled.
    this->set_auth_cache_enabled(this->device_model, false);
    // And remote authorization is also disabled.
    this->disable_remote_authorization(this->device_model, true);
    // But the websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Because almost everything is disabled, authorization can not be done and status is 'unknown'.
    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, validate_token_local_auth_list_enabled_accepted) {
    // Validate token with the local auth list: accepted
    // Set AuthCtrlr::Enabled to true
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // Local auth list is enabled.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    IdTokenInfo id_token_info_result;
    id_token_info_result.status = AuthorizationStatusEnum::Accepted;

    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_entry(_))
        .WillRepeatedly(Return(id_token_info_result));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_local_auth_list_enabled_unknown_no_remote_authorization) {
    // Validate token with the local auth list: unknown because remote authorization is not enabled and token info
    // status is not accepted. Set AuthCtrlr::Enabled to true
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // Local auth list is enabled.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    // Disable remote authorization.
    this->disable_remote_authorization(this->device_model, true);

    IdTokenInfo id_token_info_result;
    id_token_info_result.status = AuthorizationStatusEnum::Invalid;

    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_entry(_))
        .WillRepeatedly(Return(id_token_info_result));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, validate_token_local_auth_list_enabled_unknown_websocket_disconnected) {
    // Validate token with the local auth list: unknown the websocket is not connected and token info
    // status is not accepted. Set AuthCtrlr::Enabled to true
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // Local auth list is enabled.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    // Remote authorization is enabled.
    this->disable_remote_authorization(this->device_model, false);
    // But the websocket is disconnected so it is not possible to authorize the request.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));

    IdTokenInfo id_token_info_result;
    id_token_info_result.status = AuthorizationStatusEnum::Invalid;

    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_entry(_))
        .WillRepeatedly(Return(id_token_info_result));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, validate_token_local_auth_list_enabled_connectivity_manager_connected_accepted) {
    // Validate token with the local auth list: unknown the websocket is not connected and token info
    // status is not accepted. Set AuthCtrlr::Enabled to true
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // Local auth list is enabled.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    // Remote authorization is enabled.
    this->disable_remote_authorization(this->device_model, false);
    // But the websocket is connected so it is possible to authorize the request.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Authorize request returns 'Accepted'.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Accepted);
    })));

    IdTokenInfo id_token_info_result;
    id_token_info_result.status = AuthorizationStatusEnum::Invalid;

    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_entry(_))
        .WillRepeatedly(Return(id_token_info_result));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_emaid_authorize_request_accepted) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Authorize request returns 'Accepted'.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Accepted);
    })));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    std::vector<OCSPRequestData> ocsp_request_data;

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, ocsp_request_data).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_emaid_offline_no_certificate_contract_validation_offline_not_allowed) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is not connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));
    // And offline contract validation is not allowed.
    this->set_allow_contract_validation_offline(this->device_model, false);
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Valid));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::NotAtThisTime);
}

TEST_F(
    AuthorizationTest,
    validate_token_emaid_offline_no_certificate_contract_validation_offline_allowed_certificate_valid_no_authorize_offline) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is not connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));
    // And offline contract validation is not allowed.
    this->set_allow_contract_validation_offline(this->device_model, true);
    this->set_local_authorize_offline(this->device_model, false);
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Valid));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    const AuthorizeResponse response = authorization->validate_token(id_token, "", std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Unknown);
    EXPECT_EQ(response.certificateStatus, AuthorizeCertificateStatusEnum::Accepted);
}

TEST_F(
    AuthorizationTest,
    validate_token_emaid_offline_no_certificate_contract_validation_offline_allowed_certificate_valid_localauthlistctrlr_enabled) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is not connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));
    // And offline contract validation is not allowed.
    this->set_allow_contract_validation_offline(this->device_model, true);
    this->set_local_authorize_offline(this->device_model, true);
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Valid));

    IdTokenInfo id_token_info_result;
    id_token_info_result.status = AuthorizationStatusEnum::Accepted;

    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_entry(_))
        .WillRepeatedly(Return(id_token_info_result));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest,
       validate_token_emaid_offline_no_certificate_contract_validation_offline_allowed_certificate_expired) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is not connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));
    // And offline contract validation is not allowed.
    this->set_allow_contract_validation_offline(this->device_model, true);
    this->set_local_authorize_offline(this->device_model, false);
    ON_CALL(this->evse_security, verify_certificate(_, ocpp::LeafCertificateType::MO))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Expired));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    const AuthorizeResponse response = authorization->validate_token(id_token, "", std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Expired);
    EXPECT_EQ(response.certificateStatus, AuthorizeCertificateStatusEnum::CertificateExpired);
}

TEST_F(AuthorizationTest,
       validate_token_emaid_offline_no_certificate_contract_validation_offline_allowed_certificate_invalid_signature) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is not connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));
    // And offline contract validation is not allowed.
    this->set_allow_contract_validation_offline(this->device_model, true);
    // Local authorize offline is not allowed.
    this->set_local_authorize_offline(this->device_model, false);
    // The certificate has an invalid signature.
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::InvalidSignature));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    const AuthorizeResponse response = authorization->validate_token(id_token, "", std::nullopt);
    EXPECT_EQ(response.idTokenInfo.status, AuthorizationStatusEnum::Unknown);
}

TEST_F(AuthorizationTest, validate_token_emaid_websocket_disconnected_certificate_no_value) {
    // The auth controller is enabled.
    this->set_auth_ctrlr_enabled(this->device_model, true);
    // The websocket is not connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Invalid);
}

TEST_F(AuthorizationTest, validate_token_emaid_no_ocsp_websocket_connected) {
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Certificate is valid according to 'verify_certificate'.
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Valid));
    std::vector<ocpp::OCSPRequestData> ocsp_request_data;
    ocpp::OCSPRequestData d;
    d.hashAlgorithm = ocpp::HashAlgorithmEnumType::SHA256;
    d.issuerKeyHash = "issuerkeyhash";
    d.issuerNameHash = "issuernamehash";
    d.responderUrl = "responderurl";
    d.serialNumber = "serialnumber";
    ocsp_request_data.push_back(d);
    EXPECT_CALL(this->evse_security, get_mo_ocsp_request_data(_)).WillOnce(Return(ocsp_request_data));
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Accepted);
    })));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest,
       validate_token_emaid_no_ocsp_websocket_connected_no_mcsp_request_data_central_contract_validation_not_allowed) {
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Do not allow central contract validation.
    this->set_allow_central_contract_validation(this->device_model, false);
    // Certificate is valid according to 'verify_certificate'.
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Valid));
    std::vector<ocpp::OCSPRequestData> ocsp_request_data;
    EXPECT_CALL(this->evse_security, get_mo_ocsp_request_data(_)).WillOnce(Return(ocsp_request_data));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Invalid);
}

TEST_F(AuthorizationTest,
       validate_token_emaid_no_ocsp_websocket_connected_no_mcsp_request_data_central_contract_validation_allowed) {
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Do not allow central contract validation.
    this->set_allow_central_contract_validation(this->device_model, true);
    // Certificate is valid according to 'verify_certificate'.
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::Valid));
    std::vector<ocpp::OCSPRequestData> ocsp_request_data;
    EXPECT_CALL(this->evse_security, get_mo_ocsp_request_data(_)).WillOnce(Return(ocsp_request_data));
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Blocked);
    })));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Blocked);
}

TEST_F(AuthorizationTest,
       validate_token_emaid_no_ocsp_websocket_connected_issuer_not_found_contract_validation_not_allowed) {
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Certificate is valid according to 'verify_certificate'.
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::IssuerNotFound));
    // Do not allow central contract validation.
    set_allow_central_contract_validation(this->device_model, false);

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Invalid);
}

TEST_F(AuthorizationTest,
       validate_token_emaid_no_ocsp_websocket_connected_issuer_not_found_contract_validation_allowed) {
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Certificate is valid according to 'verify_certificate'.
    ON_CALL(this->evse_security, verify_certificate(_, DEFAULT_LEAF_CERT_TYPE))
        .WillByDefault(Return(ocpp::CertificateValidationResult::IssuerNotFound));
    // Allow central contract validation.
    set_allow_central_contract_validation(this->device_model, true);
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::NoCredit);
    })));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::eMAID;
    id_token.idToken = "test_token";
    EXPECT_EQ(authorization->validate_token(id_token, "", std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::NoCredit);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_accepted) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Allow local pre authorize.
    this->set_local_pre_authorize(this->device_model, true);

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::Accepted, true, false, false, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_auth_local_pre_authorize_disabled) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Allow local pre authorize.
    this->set_local_pre_authorize(this->device_model, false);
    // Disable post authorization of the auth cache
    this->set_disable_post_authorize(this->device_model, true);

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::Accepted, true, false, false, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_blocked_post_authorize_disabled) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Allow local pre authorize.
    this->set_local_pre_authorize(this->device_model, true);
    // Disable post authorization of the auth cache
    this->set_disable_post_authorize(this->device_model, true);

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::Blocked, true, false, false, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Blocked);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_invalid_post_authorize_enabled) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Allow local pre authorize.
    this->set_local_pre_authorize(this->device_model, true);
    // Disable post authorization of the auth cache
    this->set_disable_post_authorize(this->device_model, false);
    // Allow remote authorization
    this->disable_remote_authorization(this->device_model, false);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::NotAtThisLocation, true, false, false, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    // Because the authorization status was not 'Accepted', an authorize request is performed.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Invalid);
    })));

    // Since the auth cache is enabled, after authorizing, the entry is added to the authorization cache.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_insert_entry(_, _));

    IdToken id_token;
    id_token.type = IdTokenEnumStringType::ISO14443;
    id_token.idToken = "test_token";

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Invalid);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_cache_expired_and_status_invalid) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Do not allow local pre authorize.
    this->set_local_pre_authorize(this->device_model, true);
    // Allow remote authorization
    this->disable_remote_authorization(this->device_model, false);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));

    // Since the cache is expired, it will delete the entry from the cache and update the cache size.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_entry(_)).Times(1);
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size());

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::Invalid, true, true, false, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    IdToken id_token = get_id_token("test_token", IdTokenEnumStringType::ISO14443);

    // Because the cache is expired, an authorize request is performed.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::NotAllowedTypeEVSE);
    })));

    // Since the auth cache is enabled, after authorizing, the entry is added to the authorization cache.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_insert_entry(_, _));

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::NotAllowedTypeEVSE);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_lifetime_expired) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Allow remote authorization
    this->disable_remote_authorization(this->device_model, false);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));

    // Since the lifetime is expired, it will delete the entry from the cache and update the cache size.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_entry(_)).Times(1);
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size());

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::Accepted, true, false, true, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    IdToken id_token = get_id_token("test_token", IdTokenEnumStringType::ISO14443);

    // Because the cache is expired, an authorize request is performed.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::ConcurrentTx);
    })));

    // Since the auth cache is enabled, after authorizing, the entry is added to the authorization cache.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_insert_entry(_, _));

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::ConcurrentTx);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_exception) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Allow remote authorization
    this->disable_remote_authorization(this->device_model, false);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));
    // Throw exception when trying to get the cache entry.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("Test exception for the database!")));

    // Because of the database exception, an authorize request is performed
    // Because the cache is expired, an authorize request is performed.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Accepted);
    })));

    // Since the auth cache is enabled, after authorizing, the entry is added to the authorization cache.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_insert_entry(_, _));

    IdToken id_token = get_id_token("test_token", IdTokenEnumStringType::ISO14443);
    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest,
       validate_token_auth_cache_exception_websocket_disconnected_set_offline_tx_for_unknown_id_enabled) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Allow remote authorization
    this->disable_remote_authorization(this->device_model, false);
    // Offline tx for unknown id enabled
    this->set_offline_tx_for_unknown_id_enabled(this->device_model, true);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(false));
    // Throw exception when trying to get the cache entry.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillRepeatedly(Throw(std::out_of_range("Test exception!")));

    // Because of the database exception, and the websocket disabled, and offline tx for unknown id enabled, it will
    // just return 'Accepted'.
    IdToken id_token = get_id_token("test_token", IdTokenEnumStringType::ISO14443);
    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, validate_token_auth_cache_insert_entry_exception) {
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);
    // Disable local auth list.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    // Set auth cache lifetime to some high value
    this->set_auth_cache_lifetime(this->device_model, 5000);
    // Allow remote authorization
    this->disable_remote_authorization(this->device_model, false);
    // The websocket is connected.
    EXPECT_CALL(this->connectivity_manager, is_websocket_connected()).WillRepeatedly(Return(true));

    // The lifetime is expired, it will delete the entry from the cache and update the cache size.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_entry(_)).Times(1);
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size());

    AuthorizationCacheEntry authorization_cache_entry =
        create_authorization_cache_entry(AuthorizationStatusEnum::Accepted, true, false, true, 5000);

    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_entry(_))
        .WillOnce(Return(authorization_cache_entry));

    IdToken id_token = get_id_token("test_token", IdTokenEnumStringType::ISO14443);

    // Because the cache is expired, an authorize request is performed.
    EXPECT_CALL(mock_dispatcher, dispatch_call_async(_, _)).WillOnce(Return(std::async(std::launch::deferred, [this]() {
        return create_example_authorize_response(AuthorizeCertificateStatusEnum::Accepted,
                                                 AuthorizationStatusEnum::Accepted);
    })));

    // Since the auth cache is enabled, after authorizing, the entry is added to the authorization cache. But this will
    // throw an exception. This should not let the application crash but just return an 'Accepted'.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_insert_entry(_, _))
        .WillOnce(Throw(ocpp::common::DatabaseException("Insert entry fails!")));

    EXPECT_EQ(authorization->validate_token(id_token, std::nullopt, std::nullopt).idTokenInfo.status,
              AuthorizationStatusEnum::Accepted);
}

TEST_F(AuthorizationTest, handle_message_not_implemented) {
    ocpp::EnhancedMessage<MessageType> enhanced_message;
    enhanced_message.messageType = MessageType::NotifyDisplayMessages;
    EXPECT_THROW(authorization->handle_message(enhanced_message), MessageTypeNotImplementedException);
}

TEST_F(AuthorizationTest, handle_message_clear_cache) {
    const auto request = create_example_clear_cache_request();
    // Enable auth cache.
    this->set_auth_cache_enabled(this->device_model, true);

    // Expect that the authorization cache is cleared and the cache size is updated.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_clear());
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size()).WillRepeatedly(Return(45));

    // Clear cache is accepted, and the result is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ClearCacheResponse>();
        EXPECT_EQ(response.status, ClearCacheStatusEnum::Accepted);
    }));

    authorization->handle_message(request);

    // Check if cache size is updated
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    std::optional<int> size = device_model->get_optional_value<int>(auth_cache_size, AttributeEnum::Actual);
    ASSERT_TRUE(size.has_value());
    EXPECT_EQ(size.value(), 45);
}

TEST_F(AuthorizationTest, handle_message_clear_cache_auth_cache_ctrlr_disabled) {
    // Disable auth cache.
    this->set_auth_cache_enabled(this->device_model, false);

    const auto request = create_example_clear_cache_request();

    // Clear cache is rejected, and the result is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<ClearCacheResponse>();
        EXPECT_EQ(response.status, ClearCacheStatusEnum::Rejected);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_message_clear_cache_exception) {
    const auto request = create_example_clear_cache_request();

    // The database handler clear cache function throws a database exception.
    EXPECT_CALL(this->database_handler_mock, authorization_cache_clear())
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("Test exception")));

    // Which will dispatch a call error.
    EXPECT_CALL(mock_dispatcher, dispatch_call_error(_)).WillOnce([](const ocpp::CallError& call_error) {
        EXPECT_EQ(call_error.errorCode, "InternalError");
    });
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 4);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // Local authorization list is inserted, therefor the list is first cleared and then the new list is inserted. The
    // list version is also updated.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_));
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(33));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Return(2));

    // Local list is stored, everything is successful, 'Accepted' is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);

    // Number of entries should now be set in the device model, check if the number is correct.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 2);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_get_entries_exception) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 44);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // Local authorization list is inserted, therefor the list is first cleared and then the new list is inserted. The
    // list version is also updated.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_));
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(33));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    // This will throw an exception.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("Oops!")));

    // Local list is stored, only setting the number of entries is the device model failed, but the call is still
    // 'Accepted'.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);

    // Number of entries is not changed in the device model because of the exception.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 44);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_get_entries_exception2) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 51);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // Local authorization list is inserted, therefor the list is first cleared and then the new list is inserted. The
    // list version is also updated.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_));
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(33));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    // This will throw an exception.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Throw(DeviceModelError("Oops! Could not write to device model!!")));

    // Local list is stored, only setting the number of entries is the device model failed, but the call is still
    // 'Accepted'.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);

    // Number of entries is not changed in the device model because of the exception.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 51);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_get_entries_exception3) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 44);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // Local authorization list is inserted, therefor the list is first cleared and then the new list is inserted. The
    // list version is also updated.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_));
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(33));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    // This will throw an exception.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Throw(std::out_of_range("Oops! Something is out of range")));

    // Local list is stored, only setting the number of entries is the device model failed, but the call is still
    // 'Accepted'.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_set_version_exception) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 55);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // Local authorization list is inserted, therefor the list is first cleared and then the new list is inserted.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_));

    // When trying to update the authorization list version, an exception is thrown.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(33))
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("Oh no!")));

    // Local list is stored, but setting the list version throwd an exception. So the response is 'failed' in this case.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));

    authorization->handle_message(request);

    // Number of entries is not changed in the device model because of the exception.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 55);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_disabled) {
    // Local auth list ctrlrl is disabled, handle message fails.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);

    const auto request = create_send_local_list_request(
        33, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // The local auth list ctrlr is not enabled, so the call fails.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_version_number_0) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a request with local auth list version 0.
    const auto request = create_send_local_list_request(
        0, UpdateEnum::Full, this->create_example_authorization_data_local_list(false, true));

    // The version should not be 0, so this call fails.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_version_number_negative) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a request with negative local auth list version.
    const auto request = create_send_local_list_request(
        INT32_MIN, UpdateEnum::Differential, this->create_example_authorization_data_local_list(false, true));

    // The version should not be < 0, so this call fails.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_empty) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a request with an empty list
    const auto request = create_send_local_list_request(4, UpdateEnum::Full, std::vector<AuthorizationData>{});

    // Local authorization list is inserted, therefor the list is first cleared. Nothing is inserted because there is no
    // list. The list version is updated.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(4));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Return(42));

    // The authorization list should now be cleared and is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));
    authorization->handle_message(request);

    // Number of entries should now be set in the device model, check if the number is correct.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 42);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_empty2) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a request with an empty list (optional not set)
    const auto request = create_send_local_list_request(1, UpdateEnum::Full, std::nullopt);

    // Local authorization list is inserted, therefor the list is first cleared. Nothing is inserted because there is no
    // list. The list version is updated.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(1));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Return(6));

    // The authorization list should now be cleared and is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));
    authorization->handle_message(request);

    // Number of entries should now be set in the device model, check if the number is correct.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 6);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_duplicate) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a request with an list with duplicate tokens.
    const auto request =
        create_send_local_list_request(1, UpdateEnum::Full, create_example_authorization_data_local_list(true, true));

    // There are duplicates in the list, so the request has failed. Nothing is inserted.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);

    // The authorization list should now be cleared and is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_no_token_info) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a request with a list without id token info.
    const auto request =
        create_send_local_list_request(1, UpdateEnum::Full, create_example_authorization_data_local_list(false, false));

    // There is at least one token without id token info, so the request has failed. Nothing is inserted.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);

    // The authorization list should now be cleared and is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_clear_list_exception) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a 'valid' request.
    const auto request =
        create_send_local_list_request(1, UpdateEnum::Full, create_example_authorization_data_local_list(false, true));

    // Local authorization list must be inserted, but clearing it throws an exception.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list)
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("exception :(")));

    // The authorization list should now be cleared and is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_empty_clear_list_exception) {
    // Enable auth list ctrlr.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);

    // Make a 'valid' request.
    const auto request = create_send_local_list_request(1, UpdateEnum::Full, std::nullopt);

    // Clearing authorization list throws an exception.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list)
        .WillRepeatedly(Throw(ocpp::common::DatabaseException("exception :(")));

    // The authorization list should now be cleared and is accepted.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));
    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_differential) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 4);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Differential, this->create_example_authorization_data_local_list(false, true));

    // Because this is a differential update, the version must be correct (smaller than the new version number);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version()).WillOnce(Return(21));

    // This is a differential update, so the list is only inserted, not cleared.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_));
    // And the new version is set.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(33));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Return(7));

    // Local list is stored, everything is successful, 'Accepted' is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);

    // Number of entries should now be set in the device model, check if the number is correct.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 7);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_differential_empty) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 4);
    const auto request = create_send_local_list_request(22, UpdateEnum::Differential, std::nullopt);

    // Because this is a differential update, the version must be correct (smaller than the new version number);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version()).WillOnce(Return(21));

    // This is a differential update, so the list is only inserted, not cleared.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list).Times(0);
    // Since there are no entries in the list, nothing is inserted.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    // And the new version is set.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(22));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Return(7));

    // Local list is stored, everything is successful, 'Accepted' is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);

    // Number of entries should now be set in the device model, check if the number is correct.
    auto& auth_list_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
    std::optional<int> entries = device_model->get_optional_value<int>(auth_list_entries, AttributeEnum::Actual);
    ASSERT_TRUE(entries.has_value());
    EXPECT_EQ(entries.value(), 7);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_differential_empty2) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 4);
    const auto request = create_send_local_list_request(22, UpdateEnum::Differential, {});

    // Because this is a differential update, the version must be correct (smaller than the new version number);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version()).WillOnce(Return(12));

    // This is a differential update, so the list is only inserted, not cleared.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list).Times(0);
    // Since there are no entries in the list, nothing is inserted.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    // And the new version is set.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(22));

    // The number of entries is requested from the database after storing the new list, and stored in the device model.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries())
        .WillRepeatedly(Return(7));

    // Local list is stored, everything is successful, 'Accepted' is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Accepted);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_differential_version_mismatch) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    set_local_auth_list_ctrlr_entries(this->device_model, 0);
    const auto request = create_send_local_list_request(
        2, UpdateEnum::Differential, this->create_example_authorization_data_local_list(false, true));

    // Because this is a differential update, the version must be correct (smaller than the new version number);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version()).WillOnce(Return(21));

    // There is a version mismatch, so nothing is inserted or cleared, and no version is set.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries()).Times(0);

    // Local list is not stored, version mismatch.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::VersionMismatch);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_differential_duplicate) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    const auto request = create_send_local_list_request(4, UpdateEnum::Differential,
                                                        this->create_example_authorization_data_local_list(true, true));

    // Because this is a differential update, the version must be correct (smaller than the new version number);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version()).WillOnce(Return(3));

    // There is a duplicate in the list, so nothing is inserted or cleared.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries()).Times(0);

    // Local list is not stored because there is a duplicate, failed is sent..
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_send_local_authorization_list_differential_insert_exception) {
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    const auto request = create_send_local_list_request(
        33, UpdateEnum::Differential, this->create_example_authorization_data_local_list(false, true));

    // Because this is a differential update, the version must be correct (smaller than the new version number);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version()).WillOnce(Return(21));

    // This is a differential update, so the list is only inserted, not cleared.
    EXPECT_CALL(this->database_handler_mock, clear_local_authorization_list).Times(0);
    // Inserting / updating causes an exception.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list(_))
        .WillOnce(Throw(ocpp::common::DatabaseException("This is an exception")));
    // So the update is failed, no new version is set, number of entries is not requested.
    EXPECT_CALL(this->database_handler_mock, insert_or_update_local_authorization_list_version(_)).Times(0);
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_number_of_entries()).Times(0);

    // And 'Failed' is sent.
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<SendLocalListResponse>();
        EXPECT_EQ(response.status, SendLocalListStatusEnum::Failed);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_get_local_authorization_list_version) {
    // Get local authorization list version happy flow.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    const auto request = this->create_get_local_list_version_request();
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version).WillOnce(Return(42));
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<GetLocalListVersionResponse>();
        EXPECT_EQ(response.versionNumber, 42);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_get_local_authorization_list_version_disabled) {
    // Get local authorization list version while the local auth list ctrlr is disabled.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, false);
    const auto request = this->create_get_local_list_version_request();
    // Because the local auth list is not enabled, the get_local_authorization_list_version of the database handler will
    // not even get called.
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version).Times(0);
    EXPECT_CALL(mock_dispatcher, dispatch_call_result(_)).WillOnce(Invoke([](const json& call_result) {
        auto response = call_result[ocpp::CALLRESULT_PAYLOAD].get<GetLocalListVersionResponse>();
        EXPECT_EQ(response.versionNumber, 0);
    }));

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, handle_get_local_authorization_list_version_exception) {
    // Get local authorization list version. The database handler will throw when this is requested. This will dispatch
    // a call error.
    this->set_local_auth_list_ctrlr_enabled(this->device_model, true);
    const auto request = this->create_get_local_list_version_request();
    EXPECT_CALL(this->database_handler_mock, get_local_authorization_list_version)
        .WillOnce(Throw(ocpp::common::DatabaseException("Oops!")));
    EXPECT_CALL(mock_dispatcher, dispatch_call_error(_)).WillOnce([](const ocpp::CallError& call_error) {
        EXPECT_EQ(call_error.errorCode, "InternalError");
    });

    authorization->handle_message(request);
}

TEST_F(AuthorizationTest, cache_cleanup_handler) {
    // Test cache cleanup handler happy flow.
    this->authorization->start_auth_cache_cleanup_thread();
    this->delete_expired_entries_count = 0;
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_expired_entries(_))
        .WillRepeatedly(update_count_and_notify(this->delete_expired_entries_count));
    this->authorization->trigger_authorization_cache_cleanup();
    this->wait_for_calls(1, 0, 0);
}

TEST_F(AuthorizationTest, cache_cleanup_handler_exceeds_max_storage) {
    // Test cleanup handler where the authorization cache exceeds the max storage for a few times. It will then
    // cleanup, the binary size count will be increased by the test (otherwise it will spin forever).
    auto component_variable = ControllerComponentVariables::AuthCacheStorage;

    VariableCharacteristics characteristics;
    characteristics.dataType = DataEnum::integer;
    characteristics.maxLimit = 500.0f;
    characteristics.supportsMonitoring = true;
    EXPECT_TRUE(this->device_model_test_helper.update_variable_characteristics(
        characteristics, component_variable.component.name, std::nullopt, std::nullopt, std::nullopt,
        component_variable.variable->name, std::nullopt));
    this->device_model = device_model_test_helper.get_device_model();
    this->authorization = nullptr;
    FunctionalBlockContext context = {
        this->mock_dispatcher,       *this->device_model, this->connectivity_manager,   this->evse_manager,
        this->database_handler_mock, this->evse_security, this->component_state_manager};
    this->authorization = std::make_unique<Authorization>(context);
    auto meta_data =
        this->device_model->get_variable_meta_data(component_variable.component, component_variable.variable.value());

    ASSERT_TRUE(meta_data.has_value());
    ASSERT_TRUE(meta_data.value().characteristics.maxLimit.has_value());
    EXPECT_EQ(meta_data.value().characteristics.maxLimit.value(), characteristics.maxLimit);

    {
        InSequence seq;
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(0, this->get_binary_size_count))
            .RetiresOnSaturation();
        // Increase size once (which is strange but this is a test...)
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(600, this->get_binary_size_count))
            .RetiresOnSaturation();
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(650, this->get_binary_size_count))
            .RetiresOnSaturation();
        // Decrease size from now on with every get binary size read.
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(550, this->get_binary_size_count))
            .RetiresOnSaturation();
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(300, this->get_binary_size_count))
            .RetiresOnSaturation();
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillRepeatedly(update_count_and_notify(0, this->get_binary_size_count));
    }

    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_expired_entries(_))
        .WillRepeatedly(update_count_and_notify(this->delete_expired_entries_count));
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_nr_of_oldest_entries(1))
        .WillRepeatedly(update_count_and_notify(this->delete_nr_of_oldest_entries_count));

    this->authorization->start_auth_cache_cleanup_thread();

    this->delete_expired_entries_count = 0;
    this->delete_nr_of_oldest_entries_count = 0;
    this->get_binary_size_count = 0;

    this->authorization->trigger_authorization_cache_cleanup();
    this->wait_for_calls(1, 6, 3);
}

TEST_F(AuthorizationTest, cache_cleanup_handler_exceeds_max_storage_database_exception) {
    // Test cleanup handler with an exception thrown when trying to get the binary size from the database handler.
    auto component_variable = ControllerComponentVariables::AuthCacheStorage;

    VariableCharacteristics characteristics;
    characteristics.dataType = DataEnum::integer;
    characteristics.maxLimit = 500.0f;
    characteristics.supportsMonitoring = true;
    EXPECT_TRUE(this->device_model_test_helper.update_variable_characteristics(
        characteristics, component_variable.component.name, std::nullopt, std::nullopt, std::nullopt,
        component_variable.variable->name, std::nullopt));
    this->device_model = device_model_test_helper.get_device_model();
    this->authorization = nullptr;

    FunctionalBlockContext context = {
        this->mock_dispatcher,       *this->device_model, this->connectivity_manager,   this->evse_manager,
        this->database_handler_mock, this->evse_security, this->component_state_manager};
    this->authorization = std::make_unique<Authorization>(context);

    auto meta_data =
        this->device_model->get_variable_meta_data(component_variable.component, component_variable.variable.value());

    ASSERT_TRUE(meta_data.has_value());
    ASSERT_TRUE(meta_data.value().characteristics.maxLimit.has_value());
    EXPECT_EQ(meta_data.value().characteristics.maxLimit.value(), characteristics.maxLimit);

    {
        InSequence seq;
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(0, this->get_binary_size_count))
            .RetiresOnSaturation();
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(600, this->get_binary_size_count))
            .RetiresOnSaturation();
        // One of the calls will throw an exception.
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(Throw(ocpp::common::DatabaseException("Oops!")))
            .RetiresOnSaturation();
        // After that, it is still called once at the end of the function (after catching the exception)
        EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
            .WillOnce(update_count_and_notify(550, this->get_binary_size_count))
            .RetiresOnSaturation();
    }

    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_expired_entries(_))
        .WillRepeatedly(update_count_and_notify(this->delete_expired_entries_count));
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_nr_of_oldest_entries(1))
        .WillRepeatedly(update_count_and_notify(this->delete_nr_of_oldest_entries_count));

    this->delete_expired_entries_count = 0;
    this->delete_nr_of_oldest_entries_count = 0;
    this->get_binary_size_count = 0;

    this->authorization->start_auth_cache_cleanup_thread();

    this->authorization->trigger_authorization_cache_cleanup();
    this->wait_for_calls(1, 3, 1);
}

TEST_F(AuthorizationTest, cache_cleanup_handler_database_exception) {
    // Cache cleanup handler, another exception is thrown at another place (when calling
    // 'authorization_cache_delete_expired_entries')
    EXPECT_CALL(this->database_handler_mock, authorization_cache_get_binary_size())
        .WillRepeatedly(update_count_and_notify(0, this->get_binary_size_count));

    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_expired_entries(_))
        .WillRepeatedly(Throw(std::out_of_range("expired entries out of range! (?)")));
    EXPECT_CALL(this->database_handler_mock, authorization_cache_delete_nr_of_oldest_entries(1))
        .WillRepeatedly(update_count_and_notify(this->delete_nr_of_oldest_entries_count));

    this->delete_expired_entries_count = 0;
    this->delete_nr_of_oldest_entries_count = 0;
    this->get_binary_size_count = 0;

    this->authorization->start_auth_cache_cleanup_thread();

    this->authorization->trigger_authorization_cache_cleanup();
    this->wait_for_calls(0, 2, 0);
}
