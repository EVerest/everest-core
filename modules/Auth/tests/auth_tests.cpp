
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <thread>

#include <everest/logging.hpp>
#include <utils/date.hpp>

#include <AuthHandler.hpp>
#include <FakeAuthReceiver.hpp>

namespace module {

const static std::string VALID_TOKEN_1 = "VALID_RFID_1"; // SAME PARENT_ID
const static std::string VALID_TOKEN_2 = "VALID_RFID_2";
const static std::string VALID_TOKEN_3 = "VALID_RFID_3"; // SAME PARENT_ID
const static std::string INVALID_TOKEN = "INVALID_RFID";
const static std::string PARENT_ID_TOKEN = "PARENT_RFID";
const static int32_t CONNECTION_TIMEOUT = 2;

static SessionEvent get_session_started_event() {
    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    return session_event;
}

static ProvidedIdToken get_provided_token(const std::string& id_token,
                                          boost::optional<std::vector<int32_t>> connectors = boost::none,
                                          boost::optional<bool> prevalidated = boost::none) {
    ProvidedIdToken provided_token;
    provided_token.id_token = id_token;
    provided_token.type = "ManualProvider";
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

    void SetUp() override {
        std::vector<int32_t> evse_indices{0, 1};
        this->auth_receiver = std::make_unique<FakeAuthReceiver>(evse_indices);

        this->auth_handler = std::make_unique<AuthHandler>(SelectionAlgorithm::PlugEvents, CONNECTION_TIMEOUT, false);

        this->auth_handler->register_authorize_callback([this](int32_t evse_index, const std::string& id_token) {
            EVLOG_debug << "Authorize called with evse_index#" << evse_index << " and id_token " << id_token;
            this->auth_receiver->authorize(evse_index);
        });
        this->auth_handler->register_withdraw_authorization_callback([this](int32_t evse_index) {
            EVLOG_debug << "DeAuthorize called with evse_index#" << evse_index;
            this->auth_receiver->deauthorize(evse_index);
        });
        this->auth_handler->register_stop_transaction_callback(
            [this](int32_t evse_index, const StopTransactionReason& reason) {
                EVLOG_debug << "Stop transaction called with evse_index#" << evse_index << " and reason "
                            << stop_transaction_reason_to_string(reason);
                this->auth_receiver->deauthorize(evse_index);
            });

        this->auth_handler->register_validate_token_callback([](const std::string& id_token) {
            std::vector<ValidationResult> validation_results;

            ValidationResult result_1;
            result_1.authorization_status = AuthorizationStatus::Invalid;

            ValidationResult result_2;
            if (id_token == VALID_TOKEN_1 || id_token == VALID_TOKEN_3) {
                result_2.authorization_status = AuthorizationStatus::Accepted;
                result_2.parent_id_token.emplace(PARENT_ID_TOKEN);
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

        this->auth_handler->init_connector(1, 0);
        this->auth_handler->init_connector(2, 1);
    }

    void TearDown() override {
    }
};

/// \brief Test if a connector receives authorization
TEST_F(AuthTest, test_simple_authorization) {
    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    this->auth_handler->on_token(provided_token);

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if connector that triggered a SessionStarted event receives authorization when two connectors are
/// referenced in the provided token
TEST_F(AuthTest, test_two_referenced_connectors) {

    const SessionEvent session_event = get_session_started_event();
    this->auth_handler->handle_session_event(2, session_event);

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
}

/// \brief Test if a transaction is stopped when an id_token is swiped twice
TEST_F(AuthTest, test_stop_transaction) {
    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    this->auth_handler->handle_session_event(1, session_event);

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // second swipe to finish transaction
    this->auth_handler->on_token(provided_token);
    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Simple test to test if authorize first and plugin provides authorization
TEST_F(AuthTest, test_authorize_first) {

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    std::thread t1([this, provided_token]() { this->auth_handler->on_token(provided_token); });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;

    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    t1.join();
    t2.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if swiping the same card several times and timeout is handled correctly
TEST_F(AuthTest, test_swipe_multiple_times_with_timeout) {
    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    std::thread t1([this, provided_token]() { this->auth_handler->on_token(provided_token); });
    std::thread t2([this, provided_token]() { this->auth_handler->on_token(provided_token); });
    std::thread t3([this, provided_token]() { this->auth_handler->on_token(provided_token); });
    std::thread t4([this, provided_token]() { this->auth_handler->on_token(provided_token); });

    // wait for timeout
    std::this_thread::sleep_for(std::chrono::seconds(CONNECTION_TIMEOUT + 1));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    std::thread t5([this, provided_token]() { this->auth_handler->on_token(provided_token); });

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t6([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t5.join();
    t6.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if swiping two different cars will be processed and used for two seperate transactions
TEST_F(AuthTest, test_two_id_tokens) {
    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    std::thread t1([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    std::thread t2([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t3([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t4([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if two transactions can be started for two succeeding plugins before two card swipes
TEST_F(AuthTest, test_two_plugins) {

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if ne transactions can be started for two succeeding plugins before one valid and one invalid card swipe
TEST_F(AuthTest, test_two_plugins_with_invalid_rfid) {

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(INVALID_TOKEN, connectors);

    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if state permanent fault leads to not provide authorization
TEST_F(AuthTest, test_faulted_state) {

    SessionEvent session_event;
    session_event.event = SessionEventEnum::PermanentFault;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction can be stopped by swiping the card twice
TEST_F(AuthTest, test_transaction_finish) {

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    std::thread t4([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));

    std::thread t5([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    std::thread t6([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t5.join();
    t6.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction can be finished with parent_id when prioritize_authorization_over_stopping_transaction is
/// false
TEST_F(AuthTest, test_parent_id_finish) {
    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    // swipe VALID_TOKEN_1
    std::thread t2([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3 to finish transaction
    std::thread t3([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t3.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction doenst finished with parent_id when prioritize_authorization_over_stopping_transaction is
/// true. Instead: Authorization should be given to connector#2
TEST_F(AuthTest, test_parent_id_no_finish) {

    // this changes the behavior compared to previous test
    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(true);

    SessionEvent session_event;
    session_event.event = SessionEventEnum::SessionStarted;
    std::thread t1([this, session_event]() { this->auth_handler->handle_session_event(1, session_event); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    // swipe VALID_TOKEN_1
    std::thread t2([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3. This does not finish transaction but will provide authorization to connector#2 after plugin
    std::thread t3([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });
    std::thread t4([this, session_event]() { this->auth_handler->handle_session_event(2, session_event); });

    t3.join();
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_TRUE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if transaction doenst finish with parent_id when prioritize_authorization_over_stopping_transaction is
/// true. Instead: Authorization should be given to connector#2
TEST_F(AuthTest, test_parent_id_finish_because_no_available_connector) {

    // this changes the behavior compared to previous test
    this->auth_handler->set_prioritize_authorization_over_stopping_transaction(true);

    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::SessionStarted;
    std::thread t1([this, session_event_1]() { this->auth_handler->handle_session_event(1, session_event_1); });

    SessionEvent session_event_2;
    session_event_2.event = SessionEventEnum::PermanentFault;
    std::thread t2([this, session_event_2]() { this->auth_handler->handle_session_event(2, session_event_2); });

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    // swipe VALID_TOKEN_1
    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });

    t1.join();
    t2.join();
    t3.join();

    SessionEvent session_event_3;
    session_event_3.event = SessionEventEnum::TransactionStarted;
    TransactionStarted transaction_event;
    transaction_event.energy_Wh_import = 0;
    transaction_event.id_tag = provided_token_1.id_token;
    transaction_event.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    std::thread t4([this, session_event_3]() { this->auth_handler->handle_session_event(1, session_event_3); });

    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // swipe VALID_TOKEN_3. This does finish transaction because no connector is available
    std::thread t5([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });

    t5.join();

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

    SessionEvent session_event_2;
    session_event_2.event = SessionEventEnum::SessionStarted;
    std::thread t2([this, session_event_2]() { this->auth_handler->handle_session_event(1, session_event_2); });

    t2.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_1, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_2, connectors);

    // this token is not valid for the reservation
    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // this token is valid for the reservation
    std::thread t4([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test complete happy event flow of a session
TEST_F(AuthTest, test_complete_event_flow) {

    std::vector<int32_t> connectors{1};
    ProvidedIdToken provided_token = get_provided_token(VALID_TOKEN_1, connectors);

    // events
    SessionEvent session_event_1;
    session_event_1.event = SessionEventEnum::SessionStarted;

    SessionEvent session_event_2;
    session_event_2.event = SessionEventEnum::TransactionStarted;
    TransactionStarted transaction_event;
    transaction_event.energy_Wh_import = 0;
    transaction_event.id_tag = provided_token.id_token;
    transaction_event.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    SessionEvent session_event_3;
    session_event_3.event = SessionEventEnum::ChargingPausedEV;

    SessionEvent session_event_4;
    session_event_4.event = SessionEventEnum::ChargingResumed;

    SessionEvent session_event_5;
    session_event_5.event = SessionEventEnum::TransactionFinished;
    TransactionStarted transaction_finished_event;
    transaction_finished_event.energy_Wh_import = 1000;
    transaction_finished_event.id_tag = provided_token.id_token;
    transaction_finished_event.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

    SessionEvent session_event_6;
    session_event_6.event = SessionEventEnum::SessionFinished;

    this->auth_handler->handle_session_event(1, session_event_1);

    this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    this->auth_handler->handle_session_event(1, session_event_2);
    this->auth_handler->handle_session_event(1, session_event_3);
    this->auth_handler->handle_session_event(1, session_event_4);
    this->auth_handler->handle_session_event(1, session_event_5);
    this->auth_handler->handle_session_event(1, session_event_6);

    this->auth_receiver->reset();

    this->auth_handler->handle_session_event(1, session_event_1);
    this->auth_handler->on_token(provided_token);
    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

/// \brief Test if a token that is not reserved gets rejected and a parent_id_token that is reserved gets accepted
TEST_F(AuthTest, test_reservation_with_parent_id_tag) {
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

    SessionEvent session_event_2;
    session_event_2.event = SessionEventEnum::SessionStarted;
    std::thread t2([this, session_event_2]() { this->auth_handler->handle_session_event(1, session_event_2); });

    t2.join();

    std::vector<int32_t> connectors{1, 2};
    ProvidedIdToken provided_token_1 = get_provided_token(VALID_TOKEN_2, connectors);
    ProvidedIdToken provided_token_2 = get_provided_token(VALID_TOKEN_3, connectors);

    // this token is not valid for the reservation
    std::thread t3([this, provided_token_1]() { this->auth_handler->on_token(provided_token_1); });
    t3.join();

    ASSERT_FALSE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));

    // this token is valid for the reservation because the parent id tags match
    std::thread t4([this, provided_token_2]() { this->auth_handler->on_token(provided_token_2); });
    t4.join();

    ASSERT_TRUE(this->auth_receiver->get_authorization(0));
    ASSERT_FALSE(this->auth_receiver->get_authorization(1));
}

} // namespace module
