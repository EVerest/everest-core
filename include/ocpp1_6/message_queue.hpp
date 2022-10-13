// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_MESSAGE_QUEUE_HPP
#define OCPP_MESSAGE_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

class ChargePointConfiguration;

/// \brief Contains a OCPP message in json form with additional information
struct EnhancedMessage {
    json message;                                         ///< The OCPP message as json
    MessageId uniqueId;                                   ///< The unique ID of the json message
    MessageType messageType = MessageType::InternalError; ///< The OCPP message type
    MessageTypeId messageTypeId;                          ///< The OCPP message type ID (CALL/CALLRESULT/CALLERROR)
    json call_message;    ///< If the message is a CALLRESULT or CALLERROR this can contain the original CALL message
    bool offline = false; ///< A flag indicating if the connection to the central system is offline
};

/// \brief This can be used to distinguish the different queue types
enum class QueueType {
    Normal,
    Transaction,
    None,
};

/// \brief This contains an internal control message
struct ControlMessage {
    json::array_t message;                 ///< The OCPP message as a json array
    MessageType messageType;               ///< The OCPP message type
    int32_t message_attempts;              ///< The number of times this message has been rejected by the central system
    std::promise<EnhancedMessage> promise; ///< A promise used by the async send interface
    DateTime timestamp;                    ///< A timestamp that shows when this message can be sent

    /// \brief Creates a new ControlMessage object from the provided \p message
    explicit ControlMessage(json message) {
        this->message = message.get<json::array_t>();
        this->messageType = conversions::string_to_messagetype(message.at(CALL_ACTION));
        this->message_attempts = 0;
    }

    /// \brief Provides the unique message ID stored in the message
    /// \returns the unique ID of the contained message
    MessageId uniqueId() {
        return this->message[MESSAGE_ID];
    }
};

/// \brief contains a message queue that makes sure that OCPPs synchronicity requirements are met
class MessageQueue {
private:
    std::shared_ptr<ChargePointConfiguration> configuration;
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

    // key is the message id of the stop transaction and the value is the transaction id
    // this map is used for StopTransaction.req that have been put on the message queue without having received a
    // transactionId from the backend (e.g. when offline) it is used to replace the transactionId in the
    // StopTransaction.req
    std::map<std::string, int32_t> message_id_transaction_id_map;

    MessageId getMessageId(const json::array_t& json_message);
    MessageTypeId getMessageTypeId(const json::array_t& json_message);
    bool isValidMessageType(const json::array_t& json_message);
    bool isTransactionMessage(ControlMessage* message);
    void add_to_normal_message_queue(ControlMessage* message);
    void add_to_transaction_message_queue(ControlMessage* message);

public:
    /// \brief Creates a new MessageQueue object with the provided \p configuration and \p send_callback
    MessageQueue(std::shared_ptr<ChargePointConfiguration> configuration,
                 const std::function<bool(json message)>& send_callback);

    /// \brief pushes a new \p call message onto the message queue
    template <class T> void push(Call<T> call) {
        auto* message = new ControlMessage(call);
        if (this->isTransactionMessage(message)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order

            // intentionally break this message for testing...
            // message->message[CALL_PAYLOAD]["broken"] = this->createMessageId();
            this->add_to_transaction_message_queue(message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            this->add_to_normal_message_queue(message);
        }
        this->cv.notify_all();
    }

    /// \brief pushes a new \p call message onto the message queue
    /// \returns a future from which the CallResult can be extracted
    template <class T> std::future<EnhancedMessage> push_async(Call<T> call) {
        auto* message = new ControlMessage(call);
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

    /// \brief Enhances a received \p json_message with additional meta information, checks if it is a valid CallResult
    /// with a corresponding Call message on top of the queue
    /// \returns the enhanced message
    EnhancedMessage receive(const std::string& message);

    /// \brief Stops the message queue
    void stop();

    /// \brief Pauses the message queue
    void pause();

    /// \brief Resumes the message queue
    void resume();

    /// \brief Creates a unique message ID
    /// \returns the unique message ID
    MessageId createMessageId();

    /// \brief Adds the given \p transaction_id to the message_id_transaction_id_map using the key \p
    /// stop_transaction_message_id
    void add_stopped_transaction_id(std::string stop_transaction_message_id, int32_t transaction_id);
    void notify_start_transaction_handled();
};

} // namespace ocpp1_6
#endif // OCPP_MESSAGE_QUEUE_HPP
