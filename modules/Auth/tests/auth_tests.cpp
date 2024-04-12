
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

#include <everest/logging.hpp>
#include <utils/date.hpp>

#include <AuthHandler.hpp>
#include <FakeAuthReceiver.hpp>

using ::testing::_;
using ::testing::Field;
using ::testing::MockFunction;
using ::testing::StrictMock;

namespace types {
namespace authorization {

// Define operator== for types::authorization::IdToken
bool operator==(const types::authorization::IdToken& lhs, const types::authorization::IdToken& rhs) {
    return lhs.value == rhs.value;
}

} // namespace authorization
} // namespace types

namespace module {

const static std::string VALID_TOKEN_1 = "VALID_RFID_1"; // SAME PARENT_ID
const static std::string VALID_TOKEN_2 = "VALID_RFID_2";
const static std::string VALID_TOKEN_3 = "VALID_RFID_3"; // SAME PARENT_ID
const static std::string INVALID_TOKEN = "INVALID_RFID";
const static std::string PARENT_ID_TOKEN = "PARENT_RFID";
const static int32_t CONNECTION_TIMEOUT = 3;

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
    StrictMock<MockFunction<void(const ProvidedIdToken& token, TokenValidationStatus status)>>
        mock_publish_token_validation_status_callback;

    void SetUp() override {
        std::vector<int32_t> evse_indices{0, 1};
        this->auth_receiver = std::make_unique<FakeAuthReceiver>(evse_indices);

        this->auth_handler =
            std::make_unique<AuthHandler>(SelectionAlgorithm::PlugEvents, CONNECTION_TIMEOUT, false, false);

        this->auth_handler->register_notify_evse_callback([this](const int evse_index,
                                                                 const ProvidedIdToken& provided_token,
                                                                 const ValidationResult& validation_result) {
            EVLOG_debug << "Authorize called with evse_index#" << evse_index << " and id_token "
                        << provided_token.id_token.value;
            if (validation_result.authorization_status == AuthorizationStatus::Accepted) {
                this->auth_receiver->authorize(evse_index);
            }
        });
        this->auth_handler->register_withdraw_authorization_callback([this](int32_t evse_index) {
            EVLOG_debug << "DeAuthorize called with evse_index#" << evse_index;
            this->auth_receiver->deauthorize(evse_index);
        });
        this->auth_handler->register_stop_transaction_callback(
            [this](int32_t evse_index, const StopTransactionRequest& request) {
                EVLOG_debug << "Stop transaction called with evse_index#" << evse_index << " and reason "
                            << stop_transaction_reason_to_string(request.reason);
                this->auth_receiver->deauthorize(evse_index);
            });

        this->auth_handler->register_validate_token_callback([](const ProvidedIdToken& provided_token) {
            std::vector<ValidationResult> validation_results;
            const auto id_token = provided_token.id_token.value;

            ValidationResult result_1;
            result_1.authorization_status = AuthorizationStatus::Invalid;

            ValidationResult result_2;
            if (id_token == VALID_TOKEN_1 || id_token == VALID_TOKEN_3) {
                result_2.authorization_status = AuthorizationStatus::Accepted;
                result_2.parent_id_token = {PARENT_ID_TOKEN, types::authorization::IdTokenType::ISO14443};
            } else if (id_token == VALID_TOKEN_2) {
                result_2.authorization_status = AuthorizationStatus::Accepted;
            } else {
                result_2.authorization_status = AuthorizationStatus::Invalid;
            }

            validation_results.push_back(result_1);
            validation_results.push_back(result_2);

            return validation_results;
        });

        this->auth_handler->register_reservation_cancelled_callback([this](const int32_t evse_index) {
            EVLOG_info << "Signaling reservating cancelled to evse#" << evse_index;
        });

        this->auth_handler->register_publish_token_validation_status_callback(
            mock_publish_token_validation_status_callback.AsStdFunction());

        this->auth_handler->init_connector(1, 0);
        this->auth_handler->init_connector(2, 1);
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

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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
    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
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
        .Times(2);
    auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    this->auth_handler->handle_session_event(1, session_event2);
    // wait for state machine to process event
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

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

    TokenHandlingResult result;

    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::Authorized);

    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    t1.join();
    t2.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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

    TokenHandlingResult result1;
    TokenHandlingResult result2;
    TokenHandlingResult result3;
    TokenHandlingResult result4;
    TokenHandlingResult result5;

    std::thread t1([this, provided_token, &result1]() { result1 = this->auth_handler->on_token(provided_token); });

    // wait so that there is no race condition between the threads
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t2([this, provided_token, &result2]() { result2 = this->auth_handler->on_token(provided_token); });
    std::thread t3([this, provided_token, &result3]() { result3 = this->auth_handler->on_token(provided_token); });
    std::thread t4([this, provided_token, &result4]() { result4 = this->auth_handler->on_token(provided_token); });

    // wait for timeout
    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT + 1));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::TIMEOUT);
    ASSERT_TRUE(result2 == TokenHandlingResult::ALREADY_IN_PROCESS);
    ASSERT_TRUE(result3 == TokenHandlingResult::ALREADY_IN_PROCESS);

    ASSERT_TRUE(result4 == TokenHandlingResult::ALREADY_IN_PROCESS);

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    std::thread t5([this, provided_token, &result5]() { result5 = this->auth_handler->on_token(provided_token); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::Authorized);
    std::thread t6([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t5.join();
    t6.join();

    ASSERT_TRUE(result5 == TokenHandlingResult::ACCEPTED);
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

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    std::thread t1([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t2([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::Authorized);
    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t4([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(result2 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if two transactions can be started for two succeeding plugins before two card swipes
TEST_F(AuthTest, test_two_plugins) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(result2 == TokenHandlingResult::ACCEPTED);
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
    std::thread t1(
        [this, session_event_connected]() { this->auth_handler->handle_session_event(1, session_event_connected); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([this, session_event_disconnected]() {
        this->auth_handler->handle_session_event(1, session_event_disconnected);
    });

    // Swipe RFID
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });

    // Plug-in on connector 2, conntector 2 should be authorized
    std::thread t4(
        [this, session_event_connected]() { this->auth_handler->handle_session_event(2, session_event_connected); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::ACCEPTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transactions can be started for two succeeding plugins before one valid and one invalid card swipe
TEST_F(AuthTest, test_two_plugins_with_invalid_rfid) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::EVConnected);

    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Rejected));

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(result2 == TokenHandlingResult::REJECTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if state permanent fault leads to not provide authorization
TEST_F(AuthTest, test_faulted_state) {

    TokenHandlingResult result1;
    TokenHandlingResult result2;

    SessionEvent session_event;
    session_event.event = SessionEventEnum::PermanentFault;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    // wait until state faulted events arrive at auth module
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Rejected));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Rejected));

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([this, session_event1]() { this->auth_handler->handle_session_event(2, session_event1); });

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing))
        .Times(2);

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted))
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted))
        .Times(2);

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    std::thread t5([this, session_event2]() { this->auth_handler->handle_session_event(1, session_event2); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t6([this, session_event3]() { this->auth_handler->handle_session_event(2, session_event3); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(result2 == TokenHandlingResult::ACCEPTED);
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

    std::thread t1([this, session_event1]() { this->auth_handler->handle_session_event(1, session_event1); });

    SessionEvent session_event2 = get_transaction_started_event(provided_token_1);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Processing));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_1.id_token), TokenValidationStatus::Accepted));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted));

    // swipe VALID_TOKEN_1
    std::thread t2([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });

    std::thread t3([this, session_event2]() { this->auth_handler->handle_session_event(1, session_event2); });

    t1.join();
    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3 to finish transaction
    std::thread t4([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });

    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction doenst finished with parent_id when prioritize_authorization_over_stopping_transaction is
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

    // swipe VALID_TOKEN_1
    std::thread t2([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3. This does not finish transaction but will provide authorization to connector#2 after plugin
    std::thread t3([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });
    std::thread t4([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t3.join();
    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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

    SessionEvent session_event_2;
    session_event_2.event = SessionEventEnum::PermanentFault;
    std::thread t2([this, session_event_2]() { this->auth_handler->handle_session_event(2, session_event_2); });

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

    // swipe VALID_TOKEN_1
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);

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

/// \brief Test if a reservation can be placed
TEST_F(AuthTest, test_reservation) {
    Reservation reservation;
    reservation.id_token = VALID_TOKEN_1;
    reservation.reservation_id = 1;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() + std::chrono::hours(1)));

    EVLOG_debug << "1";
    const auto reservation_result = this->auth_handler->handle_reservation(1, reservation);
    EVLOG_debug << "2";

    ASSERT_EQ(reservation_result, ReservationResult::Accepted);
}

/// \brief Test if a reservation cannot be placed if expiry_time is in the past
TEST_F(AuthTest, test_reservation_in_past) {
    Reservation reservation;
    reservation.id_token = VALID_TOKEN_1;
    reservation.reservation_id = 1;
    reservation.expiry_time = Everest::Date::to_rfc3339((date::utc_clock::now() - std::chrono::hours(1)));

    const auto reservation_result = this->auth_handler->handle_reservation(1, reservation);

    ASSERT_EQ(reservation_result, ReservationResult::Rejected);
}

/// \brief Test if a token that is not reserved gets rejected and a token that is reserved gets accepted
TEST_F(AuthTest, test_reservation_with_authorization) {

    TokenHandlingResult result;

    Reservation reservation;
    reservation.id_token = VALID_TOKEN_2;
    reservation.reservation_id = 1;
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(1, reservation);

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
    // this token is not valid for the reservation
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // this token is valid for the reservation
    std::thread t4([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
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

    result = this->auth_handler->on_token(provided_token);

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // wait for state machine to process session finished event
    this->auth_handler->handle_session_event(1, session_event_2);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    this->auth_handler->handle_session_event(1, session_event_3);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    this->auth_handler->handle_session_event(1, session_event_4);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    this->auth_handler->handle_session_event(1, session_event_5);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    this->auth_handler->handle_session_event(1, session_event_6);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    this->auth_receiver->reset();

    this->auth_handler->handle_session_event(1, session_event_1);
    result = this->auth_handler->on_token(provided_token);

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a token that is not reserved gets rejected and a parent_id_token that is reserved gets accepted
TEST_F(AuthTest, test_reservation_with_parent_id_tag) {

    TokenHandlingResult result;

    Reservation reservation;
    reservation.id_token = VALID_TOKEN_1;
    reservation.reservation_id = 1;
    reservation.parent_id_token.emplace(PARENT_ID_TOKEN);
    reservation.expiry_time = Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::hours(1));

    const auto reservation_result = this->auth_handler->handle_reservation(1, reservation);

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
    // this token is not valid for the reservation
    std::thread t3([this, provided_token_1, &result]() { result = this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::REJECTED);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // this token is valid for the reservation because the parent id tags match
    std::thread t4([this, provided_token_2, &result]() { result = this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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

    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    t1.join();
    ASSERT_TRUE(result == TokenHandlingResult::TIMEOUT);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));

    std::thread t2([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });

    SessionEvent session_event = get_session_started_event(types::evse_manager::StartSessionReason::Authorized);

    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    t2.join();
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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

    TokenHandlingResult result;
    std::thread t1([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t1.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    std::thread t2([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t2.join();

    ASSERT_TRUE(result == TokenHandlingResult::ALREADY_IN_PROCESS);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));

    // wait for timeout
    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT + 1));

    std::thread t3([this, provided_token, &result]() { result = this->auth_handler->on_token(provided_token); });
    t3.join();

    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        .Times(2);
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token_2.id_token), TokenValidationStatus::Accepted))
        .Times(2);

    std::thread t3([this, provided_token_1, &result1]() { result1 = this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2, &result2]() { result2 = this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(result1 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(result2 == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));

    std::thread t5([this, session_event2]() { this->auth_handler->handle_session_event(1, session_event2); });
    std::thread t6([this, session_event3]() { this->auth_handler->handle_session_event(2, session_event3); });

    // sleeping because the order of t5,t6 happening before t7,t8 must be preserved
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::thread t7([this, provided_token_2, &result3]() { result3 = this->auth_handler->on_token(provided_token_2); });
    std::thread t8([this, provided_token_1, &result4]() { result4 = this->auth_handler->on_token(provided_token_1); });

    t5.join();
    t6.join();
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

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
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

    const auto result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);
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

    result = this->auth_handler->on_token(
        provided_token); // swipe token that gets accepted without parent id in validation result
    ASSERT_TRUE(result == TokenHandlingResult::ACCEPTED);

    // start transaction
    SessionEvent session_event2 = get_transaction_started_event(provided_token);

    provided_token.id_token = {VALID_TOKEN_1, types::authorization::IdTokenType::ISO14443};

    // check if group id token can stop transactions
    this->auth_handler->handle_session_event(1, session_event2);
    // wait for state machine to process event
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Processing));
    EXPECT_CALL(mock_publish_token_validation_status_callback,
                Call(Field(&ProvidedIdToken::id_token, provided_token.id_token), TokenValidationStatus::Accepted));

    // second swipe to finish transaction
    result = this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(result == TokenHandlingResult::USED_TO_STOP_TRANSACTION);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

} // namespace module
