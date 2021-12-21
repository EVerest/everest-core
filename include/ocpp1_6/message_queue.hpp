// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_MESSAGE_QUEUE_HPP
#define OCPP_MESSAGE_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <queue>
#include <thread>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <everest/timer.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

struct EnhancedMessage {
    json message;
    MessageId uniqueId;
    MessageType messageType = MessageType::InternalError;
    MessageTypeId messageTypeId;
    json call_message;
    bool offline = false;
};

enum class QueueType
{
    Normal,
    Transaction,
    None,
};

struct ControlMessage {
    json::array_t message;
    MessageType messageType;
    int32_t message_attempts; /// the number of times this message has been rejected by the central system
    std::promise<EnhancedMessage> promise;
    DateTime timestamp;

    ControlMessage(json message) {
        this->message = message.get<json::array_t>();
        this->messageType = Conversions::string_to_messagetype(message.at(CALL_ACTION));
        this->message_attempts = 0;
    }

    MessageId uniqueId() {
        return this->message[MESSAGE_ID];
    }
};

///
/// \brief contains a message queue that makes sure that OCPPs synchronicity requirements are met
///
class MessageQueue {
private:
    ChargePointConfiguration* configuration;
    std::thread worker_thread;
    /// message deque for transaction related messages
    std::deque<ControlMessage*> transaction_message_queue;
    /// message queue for non-transaction related messages
    std::queue<ControlMessage*> normal_message_queue;
    ControlMessage* in_flight;
    std::mutex message_mutex;
    std::condition_variable cv;
    std::function<bool(json message)> send_callback;
    bool paused;
    bool running;
    bool new_message;
    boost::uuids::random_generator uuid_generator;

    MessageId getMessageId(const json::array_t& json_message);
    MessageTypeId getMessageTypeId(const json::array_t& json_message);
    bool isValidMessageType(const json::array_t& json_message);
    bool isTransactionMessage(ControlMessage* message);
    void add_to_normal_message_queue(ControlMessage* message);
    void add_to_transaction_message_queue(ControlMessage* message);

public:
    MessageQueue(ChargePointConfiguration* configuration, const std::function<bool(json message)>& send_callback);

    ///
    /// \brief pushes a new Call message onto the message queue
    ///
    template <class T> void push(Call<T> call) {
        auto* message = new ControlMessage(call);
        EVLOG(debug) << "Adding Call message " << message->messageType << " with uid " << call.uniqueId << " to queue";
        if (this->isTransactionMessage(message)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order

            // FIXME: intentionally break this message for testing...
            // message->message[CALL_PAYLOAD]["broken"] = this->createMessageId();
            this->add_to_transaction_message_queue(message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            this->add_to_normal_message_queue(message);
        }
        this->cv.notify_all();
    }

    template <class T> std::future<EnhancedMessage> push_async(Call<T> call) {
        auto* message = new ControlMessage(call);
        EVLOG(debug) << "Adding Call message " << message->messageType << " with uid " << call.uniqueId << " to queue";
        if (this->isTransactionMessage(message)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order
            this->add_to_transaction_message_queue(message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            if (this->paused && message->messageType != MessageType::BootNotification) {
                // do not add a normal message to the queue if the queue is paused/offline
                auto enhanced_message = EnhancedMessage();
                enhanced_message.offline = true;
                message->promise.set_value(enhanced_message);
            } else {
                this->add_to_normal_message_queue(message);
            }
        }
        return message->promise.get_future();
    }

    ///
    /// \brief Enhances a received \p json_message with additional meta information, checks if it is a valid CallResult
    /// with a corresponding Call message on top of the queue
    ///
    EnhancedMessage receive(const std::string& message);

    ///
    /// \brief Stops the message queue
    ///
    void stop();

    ///
    /// \brief Pauses the message queue
    ///
    void pause();

    ///
    /// \brief Resumes the message queue
    ///
    void resume();

    ///
    /// \brief returns a unique message id
    ///
    MessageId createMessageId();
};

} // namespace ocpp1_6
#endif // OCPP_MESSAGE_QUEUE_HPP
