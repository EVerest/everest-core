// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <ocpp/common/message_queue.hpp>

namespace ocpp {

using json = nlohmann::json;

enum class TestMessageType {
    TRANSACTIONAL,
    TRANSACTIONAL_RESPONSE,
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

template <> ControlMessage<TestMessageType>::ControlMessage(json message) {
    this->message = message.get<json::array_t>();
    EVLOG_info << this->message;
    this->messageType = to_test_message_type(this->message[2]);
    this->message_attempts = 0;
}

std::ostream& operator<<(std::ostream& os, const TestMessageType& message_type) {
    os << to_string(message_type);
    return os;
};

template <>
bool MessageQueue<TestMessageType>::isTransactionMessage(
    const std::shared_ptr<ControlMessage<TestMessageType>> message) const {
    if (message == nullptr) {
        return false;
    }
    return message->messageType == TestMessageType::TRANSACTIONAL;
}

class DatabaseHandlerBaseMock : public common::DatabaseHandlerBase {
public:
    MOCK_METHOD(std::vector<common::DBTransactionMessage>, get_transaction_messages, (), (override));
    MOCK_METHOD(bool, insert_transaction_message, (const common::DBTransactionMessage&), (override));
    MOCK_METHOD(void, remove_transaction_message, (const std::string&), (override));
};

class MessageQueueTest : public ::testing::Test {
    int internal_message_count{0};
    int call_count{0};

protected:
    MessageQueueConfig config;
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

    template <typename R> auto MarkAndReturn(R value, bool respond = false) {
        return testing::Invoke([this, value, respond](json::array_t s) -> R {
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
    };

    void wait_for_calls(int expected_calls = 1) {
        std::unique_lock<std::mutex> lock(call_marker_mutex);
        EXPECT_TRUE(call_marker_cond_var.wait_for(
            lock, std::chrono::seconds(3), [this, expected_calls] { return this->call_count >= expected_calls; }));
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
        message_queue->push(call);
        return identifier;
    }

    void init_message_queue() {
        if (message_queue) {
            message_queue->stop();
        }
        message_queue = std::make_unique<MessageQueue<TestMessageType>>(send_callback_mock.AsStdFunction(), config, db);
        message_queue->resume();
    }

    void SetUp() override {
        call_count = 0;
        config = MessageQueueConfig{1, 1, 2, false};
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
    EXPECT_CALL(*db, insert_transaction_message(testing::_)).WillOnce(testing::Return(true));

    Call<TestRequest> call;
    call.msg.type = TestMessageType::TRANSACTIONAL;
    call.msg.data = "test_data";
    call.uniqueId = "0";
    message_queue->push(call);

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
    message_queue->push(call);

    wait_for_calls();
}

// \brief Test transactional messages that are sent while being offline are sent afterwards
TEST_F(MessageQueueTest, test_queuing_up_of_transactional_messages) {

    int message_count = config.queues_total_size_threshold + 3;
    testing::Sequence s;

    // Setup: reject the first call ("offline"); after that, accept any call
    EXPECT_CALL(send_callback_mock, Call(testing::_)).InSequence(s).WillOnce(MarkAndReturn(false));
    EXPECT_CALL(send_callback_mock, Call(testing::_))
        .Times(message_count)
        .InSequence(s)
        .WillRepeatedly(MarkAndReturn(true, true));
    EXPECT_CALL(*db, insert_transaction_message(testing::_)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*db, remove_transaction_message(testing::_)).Times(message_count).WillRepeatedly(testing::Return());

    // Act:
    // push first call and wait for callback; then push all other calls and resume queue
    push_message_call(TestMessageType::TRANSACTIONAL);
    wait_for_calls(1);

    for (int i = 1; i < message_count; i++) {
        push_message_call(TestMessageType::TRANSACTIONAL);
    }

    message_queue->resume();

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

    for (int i = 1; i < message_count; i++) {
        push_message_call(TestMessageType::NON_TRANSACTIONAL);
    }

    message_queue->resume();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // expect calls not repeated
    EXPECT_EQ(1, get_call_count());
}

// \brief Test that if queue_all_messages is set to true, non-transactional messages that are sent when online again
TEST_F(MessageQueueTest, test_queuing_up_of_non_transactional_messages) {
    config.queue_all_messages = true;
    init_message_queue();

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

    message_queue->resume();

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
    init_message_queue();

    EXPECT_CALL(*db, insert_transaction_message(testing::_))
        .Times(sent_transactional_messages)
        .WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*db, remove_transaction_message(testing::_))
        .Times(sent_transactional_messages)
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
    message_queue->resume();

    // expect calls _are_ repeated
    wait_for_calls(sent_transactional_messages + sent_non_transactional_messages -
                   expected_skipped_transactional_messages);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // assert no further calls
    EXPECT_EQ(sent_transactional_messages + sent_non_transactional_messages - expected_skipped_transactional_messages,
              get_call_count());
}

} // namespace ocpp
