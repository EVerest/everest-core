// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <ocpp/common/message_queue.hpp>

namespace ocpp {

using json = nlohmann::json;

/************************************************************************************************
 * Test Message Types
 */

enum class TestMessageType {
    TRANSACTIONAL,
    TRANSACTIONAL_RESPONSE,
    TRANSACTIONAL_UPDATE,
    TRANSACTIONAL_UPDATE_RESPONSE,
    NON_TRANSACTIONAL,
    NON_TRANSACTIONAL_RESPONSE,
    InternalError,
    BootNotification
};

static std::string to_string(TestMessageType m) {
    switch (m) {
    case TestMessageType::TRANSACTIONAL:
        return "transactional";
    case TestMessageType::TRANSACTIONAL_RESPONSE:
        return "transactionalResponse";
    case TestMessageType::TRANSACTIONAL_UPDATE:
        return "transactional_update";
    case TestMessageType::TRANSACTIONAL_UPDATE_RESPONSE:
        return "transactional_updateResponse";
    case TestMessageType::NON_TRANSACTIONAL:
        return "non_transactional";
    case TestMessageType::NON_TRANSACTIONAL_RESPONSE:
        return "non_transactionalResponse";
    case TestMessageType::InternalError:
        return "internal_error";
    case TestMessageType::BootNotification:
        return "boot_notification";
    }
    throw std::out_of_range("unknown TestMessageType");
};

static TestMessageType to_test_message_type(const std::string& s) {
    if (s == "transactional") {
        return TestMessageType::TRANSACTIONAL;
    }
    if (s == "transactionalResponse") {
        return TestMessageType::TRANSACTIONAL_RESPONSE;
    }
    if (s == "transactional_update") {
        return TestMessageType::TRANSACTIONAL_UPDATE;
    }
    if (s == "transactional_updateResponse") {
        return TestMessageType::TRANSACTIONAL_UPDATE_RESPONSE;
    }
    if (s == "non_transactional") {
        return TestMessageType::NON_TRANSACTIONAL;
    }
    if (s == "non_transactionalResponse") {
        return TestMessageType::NON_TRANSACTIONAL_RESPONSE;
    }
    if (s == "internal_error") {
        return TestMessageType::InternalError;
    }
    if (s == "boot_notification") {
        return TestMessageType::BootNotification;
    }
    throw std::out_of_range("unknown string for TestMessageType");
};

struct TestRequest : Message {
    TestMessageType type = TestMessageType::NON_TRANSACTIONAL;
    std::optional<std::string> data;
    std::string get_type() const {
        return to_string(type);
    };
};

void to_json(json& j, const TestRequest& k) {
    j = json{};
    if (k.data) {
        j["data"] = k.data.value();
    }
}

void from_json(const json& j, TestRequest& k) {
    if (j.contains("data")) {
        k.data.emplace(j.at("data"));
    }
}

template <> std::string MessageQueue<TestMessageType>::messagetype_to_string(TestMessageType m) {
    return to_string(m);
}

template <> TestMessageType MessageQueue<TestMessageType>::string_to_messagetype(const std::string& s) {
    return to_test_message_type(s);
}

template <> ControlMessage<TestMessageType>::ControlMessage(const json& message, bool stall_until_accepted) {
    this->message = message.get<json::array_t>();
    EVLOG_info << this->message;
    this->messageType = to_test_message_type(this->message[2]);
    this->message_attempts = 0;
}

std::ostream& operator<<(std::ostream& os, const TestMessageType& message_type) {
    os << to_string(message_type);
    return os;
};

bool is_transaction_message(const TestMessageType message_type) {
    return (message_type == TestMessageType::TRANSACTIONAL) || (message_type == TestMessageType::TRANSACTIONAL_UPDATE);
}

bool is_start_transaction_message(const TestMessageType message_type) {
    return false;
}

template <> bool ControlMessage<TestMessageType>::is_transaction_update_message() const {
    return this->messageType == TestMessageType::TRANSACTIONAL_UPDATE;
}

bool is_boot_notification_message(const TestMessageType message_type) {
    return message_type == TestMessageType::BootNotification;
}

/************************************************************************************************
 * MessageQueueTest
 */

class DatabaseHandlerBaseMock : public common::DatabaseHandlerCommon {
private:
    void init_sql() override {
    }

public:
    DatabaseHandlerBaseMock() : common::DatabaseHandlerCommon(nullptr, "", 1) {
    }

    MOCK_METHOD(std::vector<common::DBTransactionMessage>, get_message_queue_messages, (const QueueType), (override));
    MOCK_METHOD(void, insert_message_queue_message, (const common::DBTransactionMessage&, const QueueType), (override));
    MOCK_METHOD(void, remove_message_queue_message, (const std::string&, const QueueType), (override));
};

class MessageQueueTest : public ::testing::Test {
    int internal_message_count{0};
    int call_count{0};

protected:
    MessageQueueConfig<TestMessageType> config{};
    std::shared_ptr<DatabaseHandlerBaseMock> db;
    std::mutex call_marker_mutex;
    std::condition_variable call_marker_cond_var;
    testing::MockFunction<bool(json message)> send_callback_mock;
    Everest::SteadyTimer reception_timer;
    std::unique_ptr<MessageQueue<TestMessageType>> message_queue;

    int get_call_count() {
        std::lock_guard<std::mutex> lock(call_marker_mutex);
        return call_count;
    }

    /// \brief Increments call_count and notifies the condition variable.
    /// Use this from custom Invoke lambdas instead of accessing call_count directly.
    void mark_call_sent() {
        std::lock_guard<std::mutex> lock(call_marker_mutex);
        this->call_count++;
        this->call_marker_cond_var.notify_one();
    }

    template <typename R> auto MarkAndReturn(R value, bool respond = false) {
        return testing::Invoke([this, value, respond](const json::array_t& s) -> R {
            if (respond) {
                reception_timer.timeout(
                    [this, s]() {
                        this->message_queue->receive(json{3, s[1], ""}.dump());
                    },
                    std::chrono::milliseconds(0));
            }
            std::lock_guard<std::mutex> lock(call_marker_mutex);
            this->call_count++;
            this->call_marker_cond_var.notify_one();
            return value;
        });
    }

    void wait_for_calls(int expected_calls = 1, std::chrono::seconds timeout = std::chrono::seconds(3)) {
        std::unique_lock<std::mutex> lock(call_marker_mutex);
        EXPECT_TRUE(call_marker_cond_var.wait_for(
            lock, timeout, [this, expected_calls] { return this->call_count >= expected_calls; }));
    }

    std::string push_message_call(const TestMessageType& message_type) {
        std::stringstream stream;
        stream << "test_call_" << internal_message_count;
        std::string unique_identifier = stream.str();
        internal_message_count++;
        return push_message_call(message_type, unique_identifier);
    }

    std::string push_message_call(const TestMessageType& message_type, const std::string& identifier) {
        Call<TestRequest> call;
        call.msg.type = message_type;
        call.msg.data = identifier;
        call.uniqueId = identifier;
        message_queue->push_call(call);
        return identifier;
    }

    void init_message_queue() {
        message_queue = std::make_unique<MessageQueue<TestMessageType>>(send_callback_mock.AsStdFunction(), config, db);
        message_queue->start();
        message_queue->set_registration_status_accepted();
        message_queue->resume(std::chrono::seconds(0));
    }

    void restart_message_queue() {
        if (message_queue) {
            message_queue->stop();
        }
        message_queue = std::make_unique<MessageQueue<TestMessageType>>(send_callback_mock.AsStdFunction(), config, db);
        message_queue->start();
        message_queue->set_registration_status_accepted();
        message_queue->resume(std::chrono::seconds(0));
    }

    void SetUp() override {
        call_count = 0;
        config = MessageQueueConfig<TestMessageType>{1, 1, 2, false};
        db = std::make_shared<DatabaseHandlerBaseMock>();
        init_message_queue();
    }

    void TearDown() override {
        message_queue->stop();
    };
};

// \brief Test sending a transactional message
TEST_F(MessageQueueTest, test_transactional_message_is_sent) {

    EXPECT_CALL(send_callback_mock, Call(json{2, "0", "transactional", json{{"data", "test_data"}}}))
        .WillOnce(MarkAndReturn(true));
    EXPECT_CALL(*db, insert_message_queue_message(testing::_, testing::_));

    Call<TestRequest> call;
    call.msg.type = TestMessageType::TRANSACTIONAL;
    call.msg.data = "test_data";
    call.uniqueId = "0";
    message_queue->push_call(call);

    wait_for_calls();
}

// \brief Test sending a non-transactional message
TEST_F(MessageQueueTest, test_non_transactional_message_is_sent) {

    EXPECT_CALL(send_callback_mock, Call(json{2, "0", "non_transactional", json{{"data", "test_data"}}}))
        .WillOnce(MarkAndReturn(true));

    Call<TestRequest> call;
    call.msg.type = TestMessageType::NON_TRANSACTIONAL;
    call.msg.data = "test_data";
    call.uniqueId = "0";
    message_queue->push_call(call);

    wait_for_calls();
}

// \brief Test transactional messages that are sent while being offline are sent afterwards
TEST_F(MessageQueueTest, test_queuing_up_of_transactional_messages) {

    config.transaction_message_attempts = 2;
    restart_message_queue();

    int message_count = config.queues_total_size_threshold + 3;
    testing::Sequence s;

    // Setup: reject the first call ("offline"); after that, accept any call
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(false));
    EXPECT_CALL(send_callback_mock, Call(testing::_))
        .Times(message_count)
        .InSequence(s)
        .WillRepeatedly(MarkAndReturn(true, true));
    EXPECT_CALL(*db, insert_message_queue_message(testing::_, QueueType::Transaction)).Times(message_count);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, QueueType::Transaction)).Times(message_count);

    // Act:
    // push first call and wait for callback; then push all other calls and resume queue
    push_message_call(TestMessageType::TRANSACTIONAL);
    wait_for_calls(1);

    for (int i = 1; i < message_count; i++) {
        push_message_call(TestMessageType::TRANSACTIONAL);
    }

    // expect one repeated and all other calls been made
    wait_for_calls(message_count + 1);
}

// \brief Test that - with default setting -  non-transactional messages that are not sent afterwards
TEST_F(MessageQueueTest, test_non_queuing_up_of_non_transactional_messages) {

    int message_count = config.queues_total_size_threshold + 3;
    testing::Sequence s;

    // Setup: reject the first call ("offline"); after that, accept any call
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(false));
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillRepeatedly(MarkAndReturn(true, true));

    // Act:
    // push first call and wait for callback; then push all other calls and resume queue
    push_message_call(TestMessageType::NON_TRANSACTIONAL);
    wait_for_calls(1);

    // go offline and push all other calls
    message_queue->pause();

    for (int i = 1; i < message_count; i++) {
        push_message_call(TestMessageType::NON_TRANSACTIONAL);
    }

    message_queue->resume(std::chrono::seconds(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // expect calls not repeated
    EXPECT_EQ(1, get_call_count());
}

// \brief Test that if queue_all_messages is set to true, non-transactional messages that are sent when online again
TEST_F(MessageQueueTest, test_queuing_up_of_non_transactional_messages) {
    config.queue_all_messages = true;
    config.transaction_message_attempts = 2;
    restart_message_queue();

    int message_count = config.queues_total_size_threshold;
    testing::Sequence s;

    // Setup: reject the first call ("offline"); after that, accept any call
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(false));
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillRepeatedly(MarkAndReturn(true, true));

    // Act:
    // push first call and wait for callback; then push all other calls and resume queue
    push_message_call(TestMessageType::NON_TRANSACTIONAL);
    wait_for_calls(1);

    for (int i = 1; i < message_count; i++) {
        push_message_call(TestMessageType::NON_TRANSACTIONAL);
    }

    // expect calls _are_ repeated
    wait_for_calls(message_count + 1);
}

// \brief Test that if the max size threshold is exceeded, the non-transactional  messages are dropped
//  Sends both non-transactions and transactional messages while on pause, expects a certain amount of non-transactional
//  to be dropped.
TEST_F(MessageQueueTest, test_clean_up_non_transactional_queue) {

    const int sent_transactional_messages = 10;
    const int sent_non_transactional_messages = 15;
    config.queues_total_size_threshold =
        20; // expect two messages to be dropped each round (3x), end up with 15-6=9 non-transactional remaining
    config.queue_all_messages = true;
    const int expected_skipped_transactional_messages = 6;
    restart_message_queue();

    EXPECT_CALL(*db, insert_message_queue_message(testing::_, testing::_))
        .Times(sent_transactional_messages + sent_non_transactional_messages);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, testing::_))
        .Times(sent_transactional_messages + sent_non_transactional_messages)
        .WillRepeatedly(testing::Return());

    // go offline
    message_queue->pause();

    testing::Sequence s;
    for (int i = 0; i < sent_non_transactional_messages; i++) {
        auto msg_id = push_message_call(TestMessageType::NON_TRANSACTIONAL);

        if (i >= expected_skipped_transactional_messages) {
            EXPECT_CALL(send_callback_mock,
                        Call(json{2, msg_id, to_string(TestMessageType::NON_TRANSACTIONAL), json{{"data", msg_id}}}))
                .InSequence(s)
                .WillOnce(MarkAndReturn(true, true));
        }
    }
    for (int i = 0; i < sent_transactional_messages; i++) {
        auto msg_id = push_message_call(TestMessageType::TRANSACTIONAL);
        EXPECT_CALL(send_callback_mock,
                    Call(json{2, msg_id, to_string(TestMessageType::TRANSACTIONAL), json{{"data", msg_id}}}))
            .InSequence(s)
            .WillOnce(MarkAndReturn(true, true));
    }

    // go online again
    message_queue->resume(std::chrono::seconds(0));

    // expect calls _are_ repeated
    wait_for_calls(sent_transactional_messages + sent_non_transactional_messages -
                   expected_skipped_transactional_messages);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // assert no further calls
    EXPECT_EQ(sent_transactional_messages + sent_non_transactional_messages - expected_skipped_transactional_messages,
              get_call_count());
}

// \brief Test that if the max size threshold is exceeded, intermediate transactional (update) messages are dropped
//  Sends both non-transactions and transactional messages while on pause, expects all non-transactional, and any except
//  every forth transactional to be dropped
TEST_F(MessageQueueTest, test_clean_up_transactional_queue) {

    const int sent_non_transactional_messages = 10;
    const std::vector<int> transaction_update_messages{0, 4, 6,
                                                       2}; // meaning there are 4 transactions, each with a "start" and
                                                           // "stop" message and the provided number of updates;
    // in total 4*2 + 4+ 6 +2 = 20 messages
    config.queues_total_size_threshold = 13;
    /**
     *  Message IDs:
     *   non-transactional:  0 -  9
     *   Transaction I:     10 - 11
     *   Transaction II:    12 - 17
     *   Transaction III:   18 - 25
     *   Transaction IV:    26 - 29
     *
     *   Expected dropping behavior
     *   - adding msg 13-22 -> each drop 1 non-transactional (floored 10% of queue thresholds)
     *   - adding msg 23 (update of third transaction) -> drop 4 messages with ids 13,15,19,21
     *   - adding msg 27 (update of fourth transaction) -> drop 3 message with ids 14,20,23
     */
    const std::set<std::string> expected_dropped_transaction_messages = {
        "test_call_13", "test_call_15", "test_call_19", "test_call_21", "test_call_14", "test_call_20", "test_call_23",
    };
    const int expected_sent_messages = 13;
    config.queue_all_messages = true;
    restart_message_queue();

    EXPECT_CALL(*db, insert_message_queue_message(testing::_, testing::_)).Times(30);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, testing::_)).Times(30).WillRepeatedly(testing::Return());

    // go offline
    message_queue->pause();

    // Send messages / set up expected calls
    testing::Sequence s;
    for (int i = 0; i < sent_non_transactional_messages; i++) {
        push_message_call(TestMessageType::NON_TRANSACTIONAL);
    }

    for (int update_messages : transaction_update_messages) {
        // transaction "start"
        auto start_msg_id = push_message_call(TestMessageType::TRANSACTIONAL);
        EXPECT_CALL(send_callback_mock, Call(json{2, start_msg_id, to_string(TestMessageType::TRANSACTIONAL),
                                                  json{{"data", start_msg_id}}}))
            .InSequence(s)
            .WillOnce(MarkAndReturn(true, true));

        for (int i = 0; i < update_messages; i++) {
            auto update_msg_id = push_message_call(TestMessageType::TRANSACTIONAL_UPDATE);

            if (!expected_dropped_transaction_messages.count(update_msg_id)) {
                EXPECT_CALL(send_callback_mock,
                            Call(json{2, update_msg_id, to_string(TestMessageType::TRANSACTIONAL_UPDATE),
                                      json{{"data", update_msg_id}}}))
                    .InSequence(s)
                    .WillOnce(MarkAndReturn(true, true));
            }
        }

        auto stop_msg_id = push_message_call(TestMessageType::TRANSACTIONAL);
        // transaction "end"
        EXPECT_CALL(send_callback_mock,
                    Call(json{2, stop_msg_id, to_string(TestMessageType::TRANSACTIONAL), json{{"data", stop_msg_id}}}))
            .InSequence(s)
            .WillOnce(MarkAndReturn(true, true));
    }

    // Resume & verify
    message_queue->resume(std::chrono::seconds(0));

    wait_for_calls(expected_sent_messages);
}

// \brief Test that transactional retries and non-transactional messages are all delivered
// when a transactional message continuously fails.
//
// OCPP does not require non-transactional messages to wait for transactional retries.
// The RPC layer is synchronous (one in-flight at a time), but non-transactional messages
// like Heartbeats or StatusNotifications are allowed to be sent between retry attempts.
// This test verifies:
//   - The transactional message is retried the configured number of times
//   - The non-transactional message is still delivered (not starved by retries)
//   - Transactional messages maintain their own ordering
TEST_F(MessageQueueTest, test_transactional_order_strictness) {
    config.queues_total_size_threshold = 20; // Ensure no drops
    config.transaction_message_attempts = 2;
    config.transaction_message_retry_interval = 0;
    restart_message_queue();

    std::vector<std::string> call_order;
    std::mutex order_mutex;

    EXPECT_CALL(send_callback_mock, Call(testing::_))
        .WillRepeatedly(testing::Invoke([&, this](const json::array_t& msg) -> bool {
            // Identify by payload data since UUID changes on retry
            std::string data = msg.at(3).at("data").get<std::string>();

            {
                std::lock_guard<std::mutex> lock(order_mutex);
                call_order.push_back(data);
            }

            this->mark_call_sent();

            if (data == "TX1") {
                return false; // Trigger retry logic
            }

            reception_timer.timeout(
                [this, msg]() {
                    this->message_queue->receive(json{3, msg[1], ""}.dump());
                },
                std::chrono::milliseconds(0));
            return true;
        }));

    // 1. Push TX1 (blocks TX2)
    // 2. Push TX2 (queued behind TX1)
    // 3. Push Heartbeat (Non-transactional)
    push_message_call(TestMessageType::TRANSACTIONAL, "TX1");
    push_message_call(TestMessageType::TRANSACTIONAL, "TX2");
    push_message_call(TestMessageType::NON_TRANSACTIONAL, "Heartbeat");

    // Wait for: TX1 (fail) + TX1 (fail/drop) + TX2 (success) + Heartbeat (success) = 4 calls
    wait_for_calls(4);

    std::lock_guard<std::mutex> lock(order_mutex);

    // TX1 must have been attempted twice
    size_t tx1_count = std::count(call_order.begin(), call_order.end(), "TX1");
    EXPECT_EQ(tx1_count, 2);

    // TX2 must ONLY appear after all TX1 attempts are done.
    // We find the first occurrence of TX2 and ensure no TX1 exists after it.
    auto first_tx2 = std::find(call_order.begin(), call_order.end(), "TX2");
    ASSERT_NE(first_tx2, call_order.end()) << "TX2 was never sent!";

    auto tx1_after_tx2 = std::find(first_tx2, call_order.end(), "TX1");
    EXPECT_EQ(tx1_after_tx2, call_order.end()) << "TX1 was sent after TX2! Ordering violated.";

    // Heartbeat was delivered
    EXPECT_TRUE(std::find(call_order.begin(), call_order.end(), "Heartbeat") != call_order.end());
}

// \brief Test that once retry attempts are exceeded, the message is dropped and the next
// message in the queue is sent successfully.
TEST_F(MessageQueueTest, test_message_dropped_after_max_retries_then_next_message_sent) {

    config.transaction_message_attempts = 2;
    config.transaction_message_retry_interval = 0;
    restart_message_queue();

    std::vector<std::string> observed_actions;
    std::mutex actions_mutex;
    std::atomic<int> local_count{0};

    EXPECT_CALL(send_callback_mock, Call(testing::_))
        .WillRepeatedly(testing::Invoke([&, this](const json::array_t& msg) -> bool {
            std::string action = msg.at(2).get<std::string>();
            int current = ++local_count;
            {
                std::lock_guard<std::mutex> lk(actions_mutex);
                observed_actions.push_back(action);
            }
            this->mark_call_sent();
            if (action == "transactional" && current <= 2) {
                return false; // fail the first transactional message both times
            }
            // Succeed and respond to everything else
            reception_timer.timeout(
                [this, msg]() {
                    this->message_queue->receive(json{3, msg[1], ""}.dump());
                },
                std::chrono::milliseconds(0));
            return true;
        }));

    EXPECT_CALL(*db, insert_message_queue_message(testing::_, testing::_)).Times(2);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, testing::_)).Times(2).WillRepeatedly(testing::Return());

    // Push a transactional message (will be retried and dropped) and a second transactional
    push_message_call(TestMessageType::TRANSACTIONAL);
    push_message_call(TestMessageType::TRANSACTIONAL);

    // 2 failed attempts for first message + 1 successful for second
    wait_for_calls(3);

    std::lock_guard<std::mutex> lk(actions_mutex);
    ASSERT_GE(observed_actions.size(), 3u);
    // First two are the failed transactional, third is the next transactional succeeding
    EXPECT_EQ(observed_actions[0], "transactional");
    EXPECT_EQ(observed_actions[1], "transactional");
    EXPECT_EQ(observed_actions[2], "transactional");
}

// \brief Test handle_timeout_or_callerror when send_callback returns true but no response
// arrives (timeout fires). Verifies that the timeout timer triggers retry.
TEST_F(MessageQueueTest, test_timeout_triggers_retry_after_successful_send) {

    config.transaction_message_attempts = 2;
    config.transaction_message_retry_interval = 0;
    config.message_timeout_seconds = 1; // short timeout so test completes quickly
    restart_message_queue();

    std::mutex removal_mutex;
    std::condition_variable removal_cv;
    bool message_removed = false;

    // send_callback returns true both times, but we never send a CALLRESULT,
    // so the timeout fires and triggers handle_timeout_or_callerror
    EXPECT_CALL(send_callback_mock, Call(testing::_)).Times(2).WillRepeatedly(MarkAndReturn(true));
    EXPECT_CALL(*db, insert_message_queue_message(testing::_, QueueType::Transaction)).Times(1);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, QueueType::Transaction))
        .Times(1)
        .WillOnce(testing::Invoke([&](const std::string&, const QueueType) {
            std::lock_guard<std::mutex> lk(removal_mutex);
            message_removed = true;
            removal_cv.notify_one();
        }));

    push_message_call(TestMessageType::TRANSACTIONAL);

    // Wait for both send attempts (timeouts fire at ~1s each)
    wait_for_calls(2, std::chrono::seconds(5));
    EXPECT_EQ(2, get_call_count());

    // Wait for the DB removal that happens when the final timeout exhausts retries
    {
        std::unique_lock<std::mutex> lk(removal_mutex);
        EXPECT_TRUE(removal_cv.wait_for(lk, std::chrono::seconds(5), [&] { return message_removed; }));
    }
}

// \brief Test handle_timeout_or_callerror when send_callback returns false.
// Verifies that send failure followed by eventual success works correctly.
TEST_F(MessageQueueTest, test_send_failure_then_success) {

    config.transaction_message_attempts = 3;
    config.transaction_message_retry_interval = 0;
    restart_message_queue();

    testing::Sequence s;

    // Fail first two, succeed on third
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(false));
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(false));
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(true, true));

    EXPECT_CALL(*db, insert_message_queue_message(testing::_, QueueType::Transaction)).Times(1);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, QueueType::Transaction)).Times(1);

    push_message_call(TestMessageType::TRANSACTIONAL);
    wait_for_calls(3);

    EXPECT_EQ(3, get_call_count());
}

// \brief Test that non-transactional messages (without queue_all_messages) are dropped
// immediately on send failure and are NOT retried, regardless of transaction_message_attempts.
TEST_F(MessageQueueTest, test_non_transactional_not_retried_on_send_failure) {

    config.queue_all_messages = false;
    config.transaction_message_attempts = 5; // high, to confirm it's irrelevant
    config.transaction_message_retry_interval = 0;
    restart_message_queue();

    // Should only be called once - non-transactional messages without queue_all_messages are not retried
    EXPECT_CALL(send_callback_mock, Call(testing::_)).Times(1).WillOnce(MarkAndReturn(false));

    push_message_call(TestMessageType::NON_TRANSACTIONAL);
    wait_for_calls(1);

    // Wait to confirm no further retry
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(1, get_call_count());
}

// \brief Test that non-transactional messages WITH queue_all_messages=true are retried
// on send failure, just like transactional messages.
TEST_F(MessageQueueTest, test_non_transactional_retried_with_queue_all_messages) {

    config.queue_all_messages = true;
    config.transaction_message_attempts = 3;
    config.transaction_message_retry_interval = 0;
    restart_message_queue();

    // All 3 attempts fail
    EXPECT_CALL(send_callback_mock, Call(testing::_)).Times(3).WillRepeatedly(MarkAndReturn(false));
    EXPECT_CALL(*db, insert_message_queue_message(testing::_, QueueType::Normal)).Times(1);
    EXPECT_CALL(*db, remove_message_queue_message(testing::_, QueueType::Normal)).Times(1);

    push_message_call(TestMessageType::NON_TRANSACTIONAL);
    wait_for_calls(3);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(3, get_call_count());
}

} // namespace ocpp
