// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

#include <everest/helpers/helpers.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

#include <AuthHandler.hpp>
#include <FakeAuthReceiver.hpp>

using ::testing::_;
using ::testing::Field;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::StrictMock;

class kvsIntf;
namespace module {

const static std::string VALID_TOKEN_1 = "VALID_RFID_1"; // SAME PARENT_ID
const static std::string VALID_TOKEN_2 = "VALID_RFID_2";
const static std::string VALID_TOKEN_3 = "VALID_RFID_3"; // SAME PARENT_ID
const static std::string INVALID_TOKEN = "INVALID_RFID";
const static std::string PARENT_ID_TOKEN = "PARENT_RFID";
const static int32_t CONNECTION_TIMEOUT = 1;

static SessionEvent get_session_started_event(const types::evse_manager::StartSessionReason& reason) {
    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    SessionStarted session_started;
    session_started.reason = reason;
    session_event.session_started = session_started;
    return session_event;
}

static SessionEvent get_transaction_started_event(const ProvidedIdToken provided_token) {
    SessionEvent session_event;
    session_event.event = SessionEventEnum::TransactionStarted;
    TransactionStarted transaction_event;
    transaction_event.meter_value.energy_Wh_import.total = 0;
    transaction_event.id_tag = provided_token;
    session_event.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    session_event.transaction_started = transaction_event;
    return session_event;
}

static ProvidedIdToken get_provided_token(const std::string& id_token,
                                          std::optional<std::vector<int32_t>> connectors = std::nullopt,
                                          std::optional<bool> prevalidated = std::nullopt) {
    ProvidedIdToken provided_token;
    provided_token.id_token = {id_token, types::authorization::IdTokenType::ISO14443};
    provided_token.authorization_type = types::authorization::AuthorizationType::RFID;
    if (connectors) {
        provided_token.connectors.emplace(connectors.value());
    }
    if (prevalidated) {
        provided_token.prevalidated.emplace(prevalidated.value());
    }
    return provided_token;
}

class AuthTest : public ::testing::Test {

protected:
    std::unique_ptr<AuthHandler> auth_handler;
    std::unique_ptr<FakeAuthReceiver> auth_receiver;
    testing::MockFunction<bool(json message)> send_callback_mock;
    StrictMock<MockFunction<void(const ProvidedIdToken& token, TokenValidationStatus status)>>
        mock_publish_token_validation_status_callback;
    testing::MockFunction<void(const int evse_index, const StopTransactionRequest& request)>
        mock_stop_transaction_callback;
    testing::MockFunction<void(const int evse_index)> mock_withdraw_authorization_callback_mock;

    void SetUp() override {
        std::vector<int32_t> evse_indices{0, 1};
        this->auth_receiver = std::make_unique<FakeAuthReceiver>(evse_indices);

        const std::string id = "auth_handler_test_id";
        this->auth_handler = std::make_unique<AuthHandler>(SelectionAlgorithm::PlugEvents, CONNECTION_TIMEOUT, false,
                                                           false, id, nullptr);

        this->auth_handler->register_notify_evse_callback([this](const int evse_index,
                                                                 const ProvidedIdToken& provided_token,
                                                                 const ValidationResult& validation_result) {
            EVLOG_debug << "Authorize called with evse_index#" << evse_index << " and id_token "
                        << provided_token.id_token.value;
            if (validation_result.authorization_status == AuthorizationStatus::Accepted) {
                this->auth_receiver->authorize(evse_index);
            } else {
                this->auth_receiver->deauthorize(evse_index);
            }
        });
        this->auth_handler->register_withdraw_authorization_callback([this](int32_t evse_index) {
            EVLOG_debug << "DeAuthorize called with evse_index#" << evse_index;
            this->auth_receiver->deauthorize(evse_index);
            this->mock_withdraw_authorization_callback_mock.Call(evse_index);
        });
        this->auth_handler->register_stop_transaction_callback(
            [this](int32_t evse_index, const StopTransactionRequest& request) {
                EVLOG_debug << "Stop transaction called with evse_index#" << evse_index << " and reason "
                            << stop_transaction_reason_to_string(request.reason);
                this->auth_receiver->deauthorize(evse_index);
                this->mock_stop_transaction_callback.Call(evse_index, request);
            });

        this->auth_handler->register_validate_token_callback([](const ProvidedIdToken& provided_token) {
            std::vector<ValidationResult> validation_results;
            const auto id_token = provided_token.id_token.value;

            ValidationResult result_1;
            result_1.authorization_status = AuthorizationStatus::Invalid;

            ValidationResult result_2;
            if (everest::helpers::is_equal_case_insensitive(id_token, VALID_TOKEN_1) ||
                everest::helpers::is_equal_case_insensitive(id_token, VALID_TOKEN_3)) {
                result_2.authorization_status = AuthorizationStatus::Accepted;
                result_2.parent_id_token = {PARENT_ID_TOKEN, types::authorization::IdTokenType::ISO14443};
            } else if (everest::helpers::is_equal_case_insensitive(id_token, VALID_TOKEN_2)) {
                result_2.authorization_status = AuthorizationStatus::Accepted;
            } else {
                result_2.authorization_status = AuthorizationStatus::Invalid;
            }

            validation_results.push_back(result_1);
            validation_results.push_back(result_2);

            return validation_results;
        });

        this->auth_handler->register_reservation_cancelled_callback(
            [](const std::optional<int32_t> evse_index, const int32_t reservation_id, const ReservationEndReason reason,
               const bool send_reservation_update) {
                EVLOG_info << "Signaling reservating cancelled to evse#"
                           << (evse_index.has_value() ? evse_index.value() : 0);
            });

        this->auth_handler->register_publish_token_validation_status_callback(
            mock_publish_token_validation_status_callback.AsStdFunction());

        this->auth_handler->init_evse(1, 0, {Connector(1, types::evse_manager::ConnectorTypeEnum::cCCS2)});
        this->auth_handler->init_evse(2, 1,
                                      {Connector(1, types::evse_manager::ConnectorTypeEnum::sType2),
                                       Connector(2, types::evse_manager::ConnectorTypeEnum::cCCS2)});
    }

    void TearDown() override {
        SessionEvent event;
        event.event = SessionEventEnum::SessionFinished;
        this->auth_handler->handle_session_event(1, event);
        this->auth_handler->handle_session_event(2, event);
    }
};

/// \brief Test if a connector receives authorization
TEST_F(AuthTest, test_simple_authorization) {
    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken expected = provided_token;
    expected.parent_id_token = {PARENT_ID_TOKEN, types::authorization::IdTokenType::ISO14443};

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback, Call(expected, TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback, Call(expected, TokenValidationStatus::UsedToStart));

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if connector that triggered a SessionStarted event receives authorization when two connectors are
/// referenced in the provided token
TEST_F(AuthTest, test_two_referenced_connectors) {

    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(2, session_event);

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test if connector that triggered a SessionStarted event receives authorization when two connectors are
/// referenced in the provided token
TEST_F(AuthTest, test_multiple_referenced_connectors) {

    this->auth_handler->init_evse(3, 2, {Connector(1, types::evse_manager::ConnectorTypeEnum::sType2)});
    this->auth_receiver->add_evse_index(2);

    std::vector<int32_t> connectors1{1, 2};
    ProvidedIdToken provided_token1 = get_provided_token(VALID_TOKEN_1, connectors1);

    std::vector<int32_t> connectors2 = {3};
    ProvidedIdToken provided_token2 = get_provided_token(VALID_TOKEN_2, connectors2);

    std::mutex mtx;
    std::condition_variable cv;
    bool processing_called = false;

    // Set up expectations for mock_publish_token_validation_status_callback
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Processing))
        .WillOnce(Invoke([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            processing_called = true;
            cv.notify_all();
        }));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::TimedOut));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::UsedToStart));

    TokenHandlingResult result1;
    std::thread t1([this, &result1, provided_token1] { result1 = this->auth_handler->on_token(provided_token1); });

    // Wait for TokenValidationStatus::Processing to be triggered for t1 before starting t2
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return processing_called; });
    }

    TokenHandlingResult result2;
    std::thread t2([this, &result2, provided_token2] { result2 = this->auth_handler->on_token(provided_token2); });

    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(3, session_event);

    t2.join();

    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_START_TRANSACTION);

    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(2));

    SessionEvent session_event2 = get_transaction_started_event(provided_token2);
    this->auth_handler->handle_session_event(3, session_event2);

    t1.join();
    ASSERT_TRUE(result1 == TokenHandlingResult::TIMEOUT);
}

/// \brief Test three authorization requests for different referenced EVSEs with only one EV plugin. Two requests should
/// timeout, one should receive authorization
TEST_F(AuthTest, test_multiple_authorization_requests) {
    std::vector<int32_t> connectors1{1, 2};
    std::vector<int32_t> connectors2{3, 4};
    std::vector<int32_t> connectors3{5, 6};

    this->auth_handler->init_evse(3, 2, {Connector(1, types::evse_manager::ConnectorTypeEnum::sType2)});
    this->auth_handler->init_evse(4, 3, {Connector(1, types::evse_manager::ConnectorTypeEnum::sType2)});
    this->auth_handler->init_evse(5, 4, {Connector(1, types::evse_manager::ConnectorTypeEnum::sType2)});
    this->auth_handler->init_evse(6, 5, {Connector(1, types::evse_manager::ConnectorTypeEnum::sType2)});
    this->auth_receiver->add_evse_index(2);
    this->auth_receiver->add_evse_index(3);
    this->auth_receiver->add_evse_index(4);
    this->auth_receiver->add_evse_index(5);

    ProvidedIdToken provided_token1 = get_provided_token(VALID_TOKEN_1, connectors1);
    ProvidedIdToken provided_token2 = get_provided_token(VALID_TOKEN_2, connectors2);
    ProvidedIdToken provided_token3 = get_provided_token(VALID_TOKEN_3, connectors3);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::TimedOut));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::TimedOut));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token3.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token3.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token3.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token3.id_token), TokenValidationStatus::TimedOut))
        .Times(0);

    TokenHandlingResult result1;
    TokenHandlingResult result2;
    TokenHandlingResult result3;

    std::thread t1([this, provided_token1, &result1]() { result1 = this->auth_handler->on_token(provided_token1); });
    std::thread t2([this, provided_token2, &result2]() { result2 = this->auth_handler->on_token(provided_token2); });
    std::thread t3([this, provided_token3, &result3]() { result3 = this->auth_handler->on_token(provided_token3); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(6, session_event);

    t3.join();

    SessionEvent transaction_started_event = get_transaction_started_event(provided_token3);
    this->auth_handler->handle_session_event(6, transaction_started_event);

    ASSERT_TRUE(this->auth_receiver->get_authorization(5));
    ASSERT_TRUE(result3 == TokenHandlingResult::USED_TO_START_TRANSACTION);

    t1.join();
    t2.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::TIMEOUT);
    ASSERT_TRUE(result2 == TokenHandlingResult::TIMEOUT);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
    ASSERT_FALSE(this->auth_receiver->get_authorization(2));
    ASSERT_FALSE(this->auth_receiver->get_authorization(3));
    ASSERT_FALSE(this->auth_receiver->get_authorization(4));
}

/// \brief Test if a transaction is stopped when an id_token is swiped twice
TEST_F(AuthTest, test_stop_transaction) {
    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    SessionEvent session_event1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    SessionEvent session_event2 = get_transaction_started_event(provided_token);

    this->auth_handler->handle_session_event(1, session_event1);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStop))
        .Times(1);
    auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    this->auth_handler->handle_session_event(1, session_event2);

    // second swipe to finish transaction
    result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Simple test to test if authorize first and plugin provides authorization
TEST_F(AuthTest, test_authorize_first) {

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    TokenHandlingResult result;

    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    t1.join();
    t2.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if swiping the same card several times and timeout is handled correctly
TEST_F(AuthTest, test_swipe_multiple_times_with_timeout) {
    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::TimedOut))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    TokenHandlingResult result1;
    TokenHandlingResult result2;
    TokenHandlingResult result3;
    TokenHandlingResult result4;
    TokenHandlingResult result5;

    std::thread t1([this, provided_token, &result1]() { result1 = this->auth_handler->on_token(provided_token); });
    std::thread t2([this, provided_token, &result2]() { result2 = this->auth_handler->on_token(provided_token); });
    std::thread t3([this, provided_token, &result3]() { result3 = this->auth_handler->on_token(provided_token); });
    std::thread t4([this, provided_token, &result4]() { result4 = this->auth_handler->on_token(provided_token); });

    // wait for timeout
    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT + 1));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::vector<TokenHandlingResult> results = {result1, result2, result3, result4};

    // Count occurrences of TIMEOUT and ALREADY_IN_PROCESS
    int timeout_count = std::count(results.begin(), results.end(), TokenHandlingResult::TIMEOUT);
    int in_process_count = std::count(results.begin(), results.end(), TokenHandlingResult::ALREADY_IN_PROCESS);

    // Assert that exactly one result is TIMEOUT and the others are ALREADY_IN_PROCESS
    ASSERT_EQ(timeout_count, 1);
    ASSERT_EQ(in_process_count, 3);

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    std::thread t5([this, provided_token, &result5]() { result5 = this->auth_handler->on_token(provided_token); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    std::thread t6([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t5.join();
    t6.join();

    ASSERT_TRUE(result5 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if swiping two different cars will be processed and used for two seperate transactions
TEST_F(AuthTest, test_two_id_tokens) {
    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart));
    TokenHandlingResult result1;
    TokenHandlingResult result2;

    std::thread t1([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t2([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::thread t4([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if two transactions can be started for two succeeding plugins before two card swipes
TEST_F(AuthTest, test_two_plugins) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart));

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a connector receives authorization after subsequent plug-in and plug-out events
TEST_F(AuthTest, test_authorization_after_plug_in_and_plug_out) {
    TokenHandlingResult result1;
    TokenHandlingResult result2;
    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);

    // Plug-in and plug-out event on connector 1
    SessionEvent session_event_connected =
        get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    SessionEvent session_event_disconnected;
    session_event_disconnected.event = SessionEventEnum::SessionFinished;
    this->auth_handler->handle_session_event(1, session_event_connected);
    this->auth_handler->handle_session_event(1, session_event_disconnected);

    // Swipe RFID
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });

    // Plug-in on connector 2, conntector 2 should be authorized
    std::thread t4(
        [this, session_event_connected]() { this->auth_handler->handle_session_event(2, session_event_connected); });

    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transactions can be started for two succeeding plugins before one valid and one invalid card swipe
TEST_F(AuthTest, test_two_plugins_with_invalid_rfid) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(INVALID_TOKEN, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Rejected));
    t1.join();
    t2.join();
    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_TRUE(result2 == TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if state permanent fault leads to not provide authorization
TEST_F(AuthTest, test_faulted_state) {
    TokenHandlingResult result1;
    TokenHandlingResult result2;

    std::thread t1([this]() { this->auth_handler->handle_permanent_fault_raised(1, 1); });
    std::thread t2([this]() { this->auth_handler->handle_permanent_fault_raised(2, 1); });
    std::thread t3([this]() { this->auth_handler->handle_permanent_fault_raised(2, 2); });

    t1.join();
    t2.join();
    t3.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Rejected));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Rejected));

    std::thread t4([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t5([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t4.join();
    t5.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::NO_CONNECTOR_AVAILABLE);
    ASSERT_TRUE(result2 == TokenHandlingResult::NO_CONNECTOR_AVAILABLE);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction can be stopped by swiping the card twice
TEST_F(AuthTest, test_transaction_finish) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    SessionEvent session_event1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    SessionEvent session_event2 = get_transaction_started_event(provided_token_1);
    SessionEvent session_event3 = get_transaction_started_event(provided_token_2);

    std::thread t1([this, session_event1]() { this->auth_handler->handle_session_event(1, session_event1); });
    std::thread t2([this, session_event1]() { this->auth_handler->handle_session_event(2, session_event1); });

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing))
        .Times(2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted))
        .Times(1);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStop))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStop))
        .Times(1);

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::thread t5([this, session_event2]() { this->auth_handler->handle_session_event(1, session_event2); });
    std::thread t6([this, session_event3]() { this->auth_handler->handle_session_event(2, session_event3); });

    t5.join();
    t6.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));

    std::thread t7([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t8([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t7.join();
    t8.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction can be finished with parent_id when prioritize_authorization_over_stopping_transaction is
/// false
TEST_F(AuthTest, test_parent_id_finish) {

    TokenHandlingResult result;

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    SessionEvent session_event1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    this->auth_handler->handle_session_event(1, session_event1);

    SessionEvent session_event2 = get_transaction_started_event(provided_token_1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStop));
    // swipe VALID_TOKEN_1
    std::thread t2([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });
    std::thread t3([this, session_event2]() { this->auth_handler->handle_session_event(1, session_event2); });

    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3 to finish transaction
    std::thread t4([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });

    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction doesnt finish with parent_id when prioritize_authorization_over_stopping_transaction is
/// true. Instead: Authorization should be given to connector#2
TEST_F(AuthTest, test_parent_id_no_finish) {

    TokenHandlingResult result;

    // this changes the behavior compared to previous test
    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(true);

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart));
    // swipe VALID_TOKEN_1
    std::thread t2([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3. This does not finish transaction but will provide authorization to connector#2 after plugin
    std::thread t3([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });
    std::thread t4([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t3.join();
    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction doesnt finish with parent_id when prioritize_authorization_over_stopping_transaction is
/// true. Instead: Authorization should be given to connector#2
TEST_F(AuthTest, test_parent_id_finish_because_no_available_connector) {

    TokenHandlingResult result;

    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(true);

    SessionEvent session_event_1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(1, session_event_1); });
    std::thread t2([this]() {
        this->auth_handler->handle_permanent_fault_raised(2, 1);
        this->auth_handler->handle_permanent_fault_raised(2, 2);
    });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStop));

    // swipe VALID_TOKEN_1
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);

    SessionEvent session_event_3 = get_transaction_started_event(provided_token_1);
    std::thread t4([this, session_event_3]() { this->auth_handler->handle_session_event(1, session_event_3); });

    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3. This does finish transaction because no connector is available
    std::thread t5([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });

    t5.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction doesnt finish with parent_id when prioritize_authorization_over_stopping_transaction is
/// true. Instead: Authorization should be given to connector#2
TEST_F(AuthTest, test_parent_id_finish_because_no_available_connector_2) {
    // Same test as above, but now the other evse is set to faulted.
    TokenHandlingResult result;

    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(true);

    SessionEvent session_event_1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(2, session_event_1); });
    std::thread t2([this]() { this->auth_handler->handle_permanent_fault_raised(1, 1); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStop));

    // swipe VALID_TOKEN_1
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);

    SessionEvent session_event_3 = get_transaction_started_event(provided_token_1);
    std::thread t4([this, session_event_3]() { this->auth_handler->handle_session_event(2, session_event_3); });

    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));

    // swipe VALID_TOKEN_3. This does finish transaction because no connector is available
    std::thread t5([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });

    t5.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a reservation can be placed
TEST_F(AuthTest, test_reservation) {
    Reservation reservation;
    reservation.evse_id = 1;
    reservation.id_token = VALID_TOKEN_1;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Accepted);
}

/// \brief Test if a reservation cannot be placed if expiry_time is in the past
TEST_F(AuthTest, test_reservation_in_past) {
    Reservation reservation;
    reservation.evse_id = 1;
    reservation.id_token = VALID_TOKEN_1;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() - std::chrono::hours(1)));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Rejected);
}

/// \brief Test if a token that is not reserved gets rejected and a token that is reserved gets accepted
TEST_F(AuthTest, test_reservation_with_authorization) {

    TokenHandlingResult result;

    Reservation reservation;
    reservation.evse_id = 1;
    reservation.id_token = VALID_TOKEN_2;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Accepted);

    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::ReservationStart;
    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(1, session_event_1); });

    t1.join();

    SessionEvent session_event_2 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t2([this, session_event_2]() { this->auth_handler->handle_session_event(1, session_event_2); });

    t2.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    // In general the token gets accepted but the connector that was picked up by the user is reserved, therefore it's
    // rejected afterwards
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Rejected));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart));
    // this token is not valid for the reservation
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_EQ(result, TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // this token is valid for the reservation
    std::thread t4([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_EQ(result, TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a token that is not reserved gets rejected when it is not possible to charge because of global
///        reservations.
TEST_F(AuthTest, test_reservation_with_authorization_global_reservations) {
    TokenHandlingResult result;

    Reservation reservation;
    reservation.id_token = VALID_TOKEN_2;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::sType2;
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Accepted);

    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::ReservationStart;
    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(2, session_event_1); });

    t1.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);

    // In general the token gets accepted but the connector that was picked up by the user is the only one that has
    // the correct connector for the reservation so it can not be used as it has to be available for the one who
    // reserved it.
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Rejected));

    // this token is not valid for the reservation
    std::thread t2([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });
    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t2.join();
    t3.join();

    ASSERT_EQ(result, TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a token that is not reserved gets rejected when it is not possible to charge because of global
///        reservations.
TEST_F(AuthTest, test_reservation_with_authorization_global_reservations_2) {
    TokenHandlingResult result;

    // Make two global reservations.

    Reservation reservation;
    reservation.id_token = VALID_TOKEN_2;
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Accepted);

    reservation.reservation_id = 2;
    reservation.id_token = VALID_TOKEN_1;
    const auto reservation_result2 = this->auth_handler->handle_reservation(reservation);
    ASSERT_EQ(reservation_result2, ReservationResult::Accepted);

    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::ReservationStart;
    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(2, session_event_1); });

    t1.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_3 = get_provided_token(VALID_TOKEN_3, connectors);

    // There are two global reservations and two evse's, so  no evse is available.
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_3.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_3.id_token), TokenValidationStatus::Rejected));

    // this token is not valid for the reservation
    std::thread t2([this, provided_token_3, &result]() { result = this->auth_handler->on_token(provided_token_3); });
    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::Authorized);
    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t2.join();
    t3.join();

    ASSERT_EQ(result, TokenHandlingResult::NO_CONNECTOR_AVAILABLE);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test complete happy event flow of a session
TEST_F(AuthTest, test_complete_event_flow) {

    TokenHandlingResult result;

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    // events
    SessionEvent session_event_1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    SessionEvent session_event_2 = get_transaction_started_event(provided_token);

    SessionEvent session_event_3;
    session_event_3.event = SessionEventEnum::ChargingPausedEV;

    SessionEvent session_event_4;
    session_event_4.event = SessionEventEnum::ChargingResumed;

    SessionEvent session_event_5;
    session_event_5.event = SessionEventEnum::TransactionFinished;
    TransactionStarted transaction_finished_event;
    transaction_finished_event.meter_value.energy_Wh_import.total = 1000;
    transaction_finished_event.id_tag = provided_token;
    session_event_5.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    SessionEvent session_event_6;
    session_event_6.event = SessionEventEnum::SessionFinished;

    this->auth_handler->handle_session_event(1, session_event_1);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .Times(2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted))
        .Times(2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart))
        .Times(2);

    result = this->auth_handler->on_token(provided_token);

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // wait for state machine to process session finished event
    this->auth_handler->handle_session_event(1, session_event_2);
    this->auth_handler->handle_session_event(1, session_event_3);
    this->auth_handler->handle_session_event(1, session_event_4);
    this->auth_handler->handle_session_event(1, session_event_5);
    this->auth_handler->handle_session_event(1, session_event_6);
    this->auth_receiver->reset();

    this->auth_handler->handle_session_event(1, session_event_1);
    result = this->auth_handler->on_token(provided_token);

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a token that is not reserved gets rejected and a parent_id_token that is reserved gets accepted
TEST_F(AuthTest, test_reservation_with_parent_id_tag) {

    TokenHandlingResult result;

    Reservation reservation;
    reservation.evse_id = 1;
    reservation.id_token = VALID_TOKEN_1;
    reservation.reservation_id = 1;
    reservation.parent_id_token.emplace(PARENT_ID_TOKEN);
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Accepted);

    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::ReservationStart;
    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(1, session_event_1); });

    t1.join();

    SessionEvent session_event_2 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t2([this, session_event_2]() { this->auth_handler->handle_session_event(1, session_event_2); });

    t2.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_2, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    // In general the token gets accepted but the connector that was picked up by the user is reserved, therefore it's
    // rejected afterwards
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Rejected));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart));
    // this token is not valid for the reservation
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // this token is valid for the reservation because the parent id tags match
    std::thread t4([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if authorization is withdrawn after a timeout and new authorization is provided after the next swipe
TEST_F(AuthTest, test_authorization_timeout_and_reswipe) {

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::TimedOut))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    t1.join();
    ASSERT_TRUE(result == TokenHandlingResult::TIMEOUT);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));

    std::thread t2([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
}

/// \brief Test if response is ALREADY_IN_PROCESS with authorization was provided but transaction has not yet been
/// started
TEST_F(AuthTest, test_authorization_without_transaction) {

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart))
        .Times(2);
    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t1.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    std::thread t2([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t2.join();

    ASSERT_TRUE(result == TokenHandlingResult::ALREADY_IN_PROCESS);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::TimedOut));

    // wait for timeout
    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT + 1));

    std::thread t3([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
}

/// \brief Test if two transactions can be started for two succeeding plugins before two card swipes
TEST_F(AuthTest, test_two_transactions_start_stop) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;
    TokenHandlingResult result3;
    TokenHandlingResult result4;

    SessionEvent session_event1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event1]() { this->auth_handler->handle_session_event(1, session_event1); });
    std::thread t2([this, session_event1]() { this->auth_handler->handle_session_event(2, session_event1); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    SessionEvent session_event2 = get_transaction_started_event(provided_token_1);

    SessionEvent session_event3 = get_transaction_started_event(provided_token_2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing))
        .Times(2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted))
        .Times(1);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStop))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStop))
        .Times(1);
    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));

    std::thread t5([this, session_event2]() { this->auth_handler->handle_session_event(1, session_event2); });
    std::thread t6([this, session_event3]() { this->auth_handler->handle_session_event(2, session_event3); });

    t5.join();
    t6.join();

    std::thread t7([this, provided_token_2, &result3]() { result3 = this->auth_handler->on_token(provided_token_2); });
    std::thread t8([this, provided_token_1, &result4]() { result4 = this->auth_handler->on_token(provided_token_1); });

    t7.join();
    t8.join();

    ASSERT_TRUE(result3 == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_TRUE(result4 == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test PlugAndCharge
TEST_F(AuthTest, test_plug_and_charge) {

    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    ProvidedIdToken provided_token;
    provided_token.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::eMAID};
    provided_token.authorization_type = types::authorization::AuthorizationType::PlugAndCharge;
    provided_token.certificate.emplace("TestCertificate");

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test PlugAndCharge
TEST_F(AuthTest, test_plug_and_charge_rejected) {

    // put authorization on evse so that we can check later if it was removed
    this->auth_receiver->authorize(0);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    ProvidedIdToken provided_token;
    provided_token.id_token = {INVALID_TOKEN, types::authorization::IdTokenType::eMAID};
    provided_token.authorization_type = types::authorization::AuthorizationType::PlugAndCharge;
    provided_token.certificate.emplace("TestCertificate");
    provided_token.connectors = {1, 2};

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Rejected));

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test empty intersection of referenced connectors in provided token and in validation result
TEST_F(AuthTest, test_empty_intersection) {

    this->auth_handler->register_validate_token_callback([](const ProvidedIdToken& provided_token) {
        std::vector<ValidationResult> validation_results;
        const auto id_token = provided_token.id_token.value;

        std::vector<int> evse_ids{2};
        std::vector<int> evse_ids2{1, 2};

        ValidationResult result_1;
        result_1.authorization_status = AuthorizationStatus::Accepted;
        result_1.evse_ids.emplace(evse_ids);

        ValidationResult result_2;
        result_1.authorization_status = AuthorizationStatus::Accepted;
        result_1.evse_ids.emplace(evse_ids2);

        validation_results.push_back(result_1);
        validation_results.push_back(result_2);

        return validation_results;
    });

    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    this->auth_handler->handle_session_event(1, session_event);
    this->auth_handler->handle_session_event(2, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token;
    provided_token.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::eMAID};
    provided_token.authorization_type = types::authorization::AuthorizationType::PlugAndCharge;
    provided_token.certificate.emplace("TestCertificate");
    provided_token.connectors.emplace(connectors);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

TEST_F(AuthTest, test_master_pass_group_id) {
    // set master group id to parent id token
    this->auth_handler->set_master_pass_group_id(PARENT_ID_TOKEN);
    // set_prioritize_authorization_over_stopping_transaction=false; otherwise token could be used for authorization of
    // another connector
    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(false);
    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);
    auto provided_token = get_provided_token(PARENT_ID_TOKEN);

    // Test if group id token is not allowed to start transactions
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Rejected));

    auto result = this->auth_handler->on_token(provided_token);

    ASSERT_TRUE(result == TokenHandlingResult::REJECTED);

    provided_token = get_provided_token(VALID_TOKEN_2);
    provided_token.parent_id_token = std::nullopt;

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    result = this->auth_handler->on_token(
        provided_token); // swipe token that gets accepted without parent id in validation result
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);

    // start transaction
    SessionEvent session_event2 = get_transaction_started_event(provided_token);

    provided_token.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::ISO14443};

    // check if group id token can stop transactions
    this->auth_handler->handle_session_event(1, session_event2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStop));

    // second swipe to finish transaction
    result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test TimedOut is published when authorization was provided but transaction has never been started.
TEST_F(AuthTest, test_token_timed_out) {
    // In order to time-out waiting for a plug-in event, we need to get select_connector to wait for a plug-in event
    // in the first place.
    // To get select_connector to wait for a plug-in event, we must provide more then one connector here, since if we
    // provide only 1, select_connector would just return the single connector.
    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::TimedOut));

    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t1.join();

    ASSERT_TRUE(result == TokenHandlingResult::TIMEOUT);

    // wait for timeout, after which TimedOut should be published.
    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT + 1));
}

/// \brief Test that in case of a plug in timeout, no authorization is given to the EVSE afterwards
TEST_F(AuthTest, test_plug_in_time_out) {
    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Rejected));

    // no connector should be available since the plug-in event has timed out
    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t1.join();

    ASSERT_TRUE(result == TokenHandlingResult::NO_CONNECTOR_AVAILABLE);

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a valid id_token is rejected in case a previous token is already assigned to the EVSE
TEST_F(AuthTest, test_subsequent_valid_tokens) {
    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Rejected));

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    std::thread t1([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    t1.join();
    std::thread t2([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });
    t2.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(result2 == TokenHandlingResult::NO_CONNECTOR_AVAILABLE);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a authorization can be withdrawn if an EV was connected and authorization was granted before
TEST_F(AuthTest, test_withdraw_authorization) {
    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_withdraw_authorization_callback_mock, Call(0)).Times(1);
    EXPECT_CALL(mock_stop_transaction_callback, Call(_, _)).Times(0);

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    types::authorization::WithdrawAuthorizationRequest withdraw_request;
    withdraw_request.evse_id = 1;

    auto withdraw_result = this->auth_handler->handle_withdraw_authorization(withdraw_request);

    ASSERT_EQ(withdraw_result, WithdrawAuthorizationResult::Accepted);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));

    SessionEvent deauthorized_event;
    deauthorized_event.event = SessionEventEnum::Deauthorized;
    this->auth_handler->handle_session_event(1, deauthorized_event);

    withdraw_result = this->auth_handler->handle_withdraw_authorization(withdraw_request);
    ASSERT_EQ(withdraw_result, WithdrawAuthorizationResult::AuthorizationNotFound);
}

/// \brief Test if a authorization can be withdrawn while authorization process is still selecting an EVSE
TEST_F(AuthTest, test_withdraw_authorization_while_waiting_for_ev_plugin) {

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token2 = get_provided_token(VALID_TOKEN_2, connectors);

    std::mutex mtx;
    std::condition_variable cv;
    bool processing_called1 = false;
    bool processing_called2 = false;

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Processing))
        .WillOnce(Invoke([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            processing_called1 = true;
            cv.notify_all(); // Notify that the processing call happened
        }));
    ;
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token1.id_token), TokenValidationStatus::Withdrawn));
    EXPECT_CALL(mock_withdraw_authorization_callback_mock, Call(0)).Times(0);
    EXPECT_CALL(mock_stop_transaction_callback, Call(_, _)).Times(0);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Processing))
        .WillOnce(Invoke([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            processing_called2 = true;
            cv.notify_all(); // Notify that the processing call happened
        }));
    ;
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token2.id_token), TokenValidationStatus::Withdrawn));
    EXPECT_CALL(mock_withdraw_authorization_callback_mock, Call(0)).Times(0);
    EXPECT_CALL(mock_stop_transaction_callback, Call(_, _)).Times(0);

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    std::thread t1([this, provided_token1, &result1]() { result1 = this->auth_handler->on_token(provided_token1); });
    std::thread t2([this, provided_token2, &result2]() { result2 = this->auth_handler->on_token(provided_token2); });

    WithdrawAuthorizationResult withdraw_authorization_result1;
    types::authorization::WithdrawAuthorizationRequest withdraw_request1;
    withdraw_request1.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::ISO14443};

    WithdrawAuthorizationResult withdraw_authorization_result2;
    types::authorization::WithdrawAuthorizationRequest withdraw_request2;
    withdraw_request2.id_token = {VALID_TOKEN_2, types::authorization::IdTokenType::ISO14443};

    // Wait for TokenValidationStatus::Processing to be triggered
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return processing_called1 and processing_called2; });
    }

    std::thread t3([this, withdraw_request1, &withdraw_authorization_result1]() {
        withdraw_authorization_result1 = this->auth_handler->handle_withdraw_authorization(withdraw_request1);
    });
    std::thread t4([this, withdraw_request2, &withdraw_authorization_result2]() {
        withdraw_authorization_result2 = this->auth_handler->handle_withdraw_authorization(withdraw_request2);
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_EQ(result1, TokenHandlingResult::WITHDRAWN);
    ASSERT_EQ(withdraw_authorization_result1, WithdrawAuthorizationResult::Accepted);
    ASSERT_EQ(result2, TokenHandlingResult::WITHDRAWN);
    ASSERT_EQ(withdraw_authorization_result2, WithdrawAuthorizationResult::Accepted);

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a authorization is not withdrawn if the token does not match and the authorization requests ends with
/// a timeout
TEST_F(AuthTest, test_withdraw_authorization_while_waiting_for_ev_plugin_timeout) {

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    std::mutex mtx;
    std::condition_variable cv;
    bool processing_called = false;

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .WillOnce(Invoke([&]() {
            std::unique_lock<std::mutex> lock(mtx);
            processing_called = true;
            cv.notify_all(); // Notify that the processing call happened
        }));
    ;
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::TimedOut));
    EXPECT_CALL(mock_withdraw_authorization_callback_mock, Call(_)).Times(0);
    EXPECT_CALL(mock_stop_transaction_callback, Call(_, _)).Times(0);

    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    WithdrawAuthorizationResult withdraw_authorization_result;
    types::authorization::WithdrawAuthorizationRequest withdraw_request;
    withdraw_request.id_token = {VALID_TOKEN_2, types::authorization::IdTokenType::ISO14443};

    // Wait for TokenValidationStatus::Processing to be triggered
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return processing_called; });
    }

    std::thread t2([this, withdraw_request, &withdraw_authorization_result]() {
        withdraw_authorization_result = this->auth_handler->handle_withdraw_authorization(withdraw_request);
    });

    t1.join();
    t2.join();

    ASSERT_EQ(result, TokenHandlingResult::TIMEOUT);
    ASSERT_EQ(withdraw_authorization_result, WithdrawAuthorizationResult::AuthorizationNotFound);

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a authorization can be withdrawn during an active transaction
TEST_F(AuthTest, test_withdraw_authorization_during_transaction) {
    const SessionEvent session_started_event =
        get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_started_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_withdraw_authorization_callback_mock, Call(_)).Times(0);
    EXPECT_CALL(mock_stop_transaction_callback, Call(0, _)).Times(1);

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    const SessionEvent transaction_started_event = get_transaction_started_event(provided_token);
    this->auth_handler->handle_session_event(1, transaction_started_event);

    types::authorization::WithdrawAuthorizationRequest withdraw_request;
    withdraw_request.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::ISO14443};
    this->auth_handler->handle_withdraw_authorization(withdraw_request);
}

/// \brief Test two successive authorization requests and connector selection. The first authorization request targets
/// only connector1, the second authorization request targets connector1 and connector2. Connector2 should be selected,
/// since the transaction at connector1 is already running.
TEST_F(AuthTest, test_two_authorization_plug_events) {

    std::vector<int32_t> connector_1{1};
    std::vector<int32_t> all_connectors{1, 2};

    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connector_1);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, all_connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStart));
    TokenHandlingResult result1;

    std::thread t1([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    t1.join();
    t2.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    SessionEvent session_event2 = get_transaction_started_event(provided_token_1);
    this->auth_handler->handle_session_event(1, session_event2);

    TokenHandlingResult result2;
    std::thread t3([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t3.join();

    ASSERT_TRUE(result2 == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

TEST_F(AuthTest, test_token_swipe_race_with_timeout) {

    auto token_entered_promise = std::make_shared<std::promise<void>>();
    auto timeout_entered_promise = std::make_shared<std::promise<void>>();

    std::shared_future<void> token_entered_future = token_entered_promise->get_future();
    std::shared_future<void> timeout_entered_future = timeout_entered_promise->get_future();

    std::atomic<bool> token_validation_started{false};
    std::atomic<bool> timeout_triggered{false};

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    // Plug-in first
    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    this->auth_handler->register_validate_token_callback(
        [token_entered_promise, timeout_entered_future](const ProvidedIdToken& provided_token) {
            token_entered_promise->set_value();
            timeout_entered_future.wait();

            ValidationResult result;
            result.authorization_status = AuthorizationStatus::Accepted;
            result.parent_id_token = {PARENT_ID_TOKEN, types::authorization::IdTokenType::ISO14443};
            return std::vector<ValidationResult>{ValidationResult{}, result};
        });

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Rejected));

    TokenHandlingResult result;

    std::thread token_thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds((CONNECTION_TIMEOUT * 1000) - 200));
        result = this->auth_handler->on_token(provided_token);
    });

    std::thread timeout_simulation_thread([timeout_entered_promise, token_entered_future, &timeout_triggered]() {
        token_entered_future.wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        timeout_triggered = true;
        timeout_entered_promise->set_value();
    });

    token_thread.join();
    timeout_simulation_thread.join();

    ASSERT_TRUE(timeout_triggered);
    ASSERT_EQ(result, TokenHandlingResult::NO_CONNECTOR_AVAILABLE);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test case-insensitive token comparison for authorization
TEST_F(AuthTest, test_case_insensitive_authorization) {
    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    // Lowercase token for provided token
    ProvidedIdToken provided_token = get_provided_token("valid_rfid_1", connectors);

    // Validation callback returns uppercase VALID_TOKEN_1
    ProvidedIdToken expected = provided_token;
    expected.parent_id_token = {PARENT_ID_TOKEN, types::authorization::IdTokenType::ISO14443};

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback, Call(expected, TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback, Call(expected, TokenValidationStatus::UsedToStart));

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test case-insensitive token comparison for stopping transactions
TEST_F(AuthTest, test_case_insensitive_stop_transaction) {
    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);
    // Create a token with different case for stopping the transaction
    ProvidedIdToken stop_token = get_provided_token("VALID_rfid_1", connectors);

    SessionEvent session_event1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    SessionEvent session_event2 = get_transaction_started_event(provided_token);

    this->auth_handler->handle_session_event(1, session_event1);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, stop_token.id_token), TokenValidationStatus::Processing))
        .Times(1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, stop_token.id_token), TokenValidationStatus::UsedToStop))
        .Times(1);

    auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    this->auth_handler->handle_session_event(1, session_event2);

    result = this->auth_handler->on_token(stop_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test case-insensitive parent ID token comparison for stopping transactions
TEST_F(AuthTest, test_case_insensitive_parent_id_stop_transaction) {
    TokenHandlingResult result;

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    // Use different case for parent ID token
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    SessionEvent session_event1 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event1);

    SessionEvent session_event2 = get_transaction_started_event(provided_token_1);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::UsedToStop));
    // Start transaction with VALID_TOKEN_1
    result = this->auth_handler->on_token(provided_token_1);
    this->auth_handler->handle_session_event(1, session_event2);

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    // Stop transaction with VALID_TOKEN_3 (has same parent ID as VALID_TOKEN_1)
    result = this->auth_handler->on_token(provided_token_2);

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test case-insensitive comparison with master pass group ID
TEST_F(AuthTest, test_case_insensitive_master_pass_group_id) {
    // Set master group id with different case than what will be returned in validation
    this->auth_handler->set_master_pass_group_id("parent_rfid"); // lowercase
    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(false);

    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    // Test with a token that would have master group ID as parent - should be rejected initially
    auto provided_token = get_provided_token("PARENT_RFID"); // uppercase

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Rejected));

    auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::REJECTED);

    // Start a transaction with a valid token
    provided_token = get_provided_token(VALID_TOKEN_2);
    provided_token.parent_id_token = std::nullopt;

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);

    SessionEvent session_event2 = get_transaction_started_event(provided_token);
    this->auth_handler->handle_session_event(1, session_event2);

    // Test if master group id token can stop transactions (case insensitive)
    // Use a token that gets validated and returns a parent_id_token matching master group ID (different case)
    provided_token.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::ISO14443};

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStop));

    result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test case-insensitive withdraw authorization
TEST_F(AuthTest, test_case_insensitive_withdraw_authorization) {
    const SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    EXPECT_CALL(mock_withdraw_authorization_callback_mock, Call(0)).Times(1);

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    // Test withdraw with different case
    types::authorization::WithdrawAuthorizationRequest withdraw_request;
    withdraw_request.evse_id = 1;
    withdraw_request.id_token = {"valid_RFID_1", types::authorization::IdTokenType::ISO14443};

    auto withdraw_result = this->auth_handler->handle_withdraw_authorization(withdraw_request);

    ASSERT_EQ(withdraw_result, WithdrawAuthorizationResult::Accepted);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test case-insensitive reservation matching
TEST_F(AuthTest, test_case_insensitive_reservation_matching) {
    TokenHandlingResult result;

    Reservation reservation;
    reservation.evse_id = 1;
    reservation.id_token = "VALID_rfid_2"; // Different case
    reservation.reservation_id = 1;
    reservation.connector_type = types::evse_manager::ConnectorTypeEnum::cCCS2;
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(reservation);
    ASSERT_EQ(reservation_result, ReservationResult::Accepted);

    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::ReservationStart;
    this->auth_handler->handle_session_event(1, session_event_1);

    SessionEvent session_event_2 = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);
    this->auth_handler->handle_session_event(1, session_event_2);

    std::vector<int32_t> connectors{1, 2};
    // Use different case for token comparison
    ProvidedIdToken provided_token = get_provided_token("valid_RFID_2", connectors);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::UsedToStart));
    result = this->auth_handler->on_token(provided_token);

    ASSERT_EQ(result, TokenHandlingResult::USED_TO_START_TRANSACTION);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

} // namespace module
