// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_MESSAGE_QUEUE_HPP
#define OCPP_COMMON_MESSAGE_QUEUE_HPP

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

#include <everest/timer.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/types.hpp>
#include <ocpp/v201/types.hpp>

namespace ocpp {

const auto STANDARD_MESSAGE_TIMEOUT = std::chrono::seconds(30);

/// \brief Contains a OCPP message in json form with additional information
template <typename M> struct EnhancedMessage {
    json message;                     ///< The OCPP message as json
    MessageId uniqueId;               ///< The unique ID of the json message
    M messageType = M::InternalError; ///< The OCPP message type
    MessageTypeId messageTypeId;      ///< The OCPP message type ID (CALL/CALLRESULT/CALLERROR)
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
template <typename M> struct ControlMessage {
    json::array_t message;    ///< The OCPP message as a json array
    M messageType;            ///< The OCPP message type
    int32_t message_attempts; ///< The number of times this message has been rejected by the central system
    std::promise<EnhancedMessage<M>> promise; ///< A promise used by the async send interface
    DateTime timestamp;                       ///< A timestamp that shows when this message can be sent

    /// \brief Creates a new ControlMessage object from the provided \p message
    explicit ControlMessage(json message);

    /// \brief Provides the unique message ID stored in the message
    /// \returns the unique ID of the contained message
    MessageId uniqueId() {
        return this->message[MESSAGE_ID];
    }
};

/// \brief contains a message queue that makes sure that OCPPs synchronicity requirements are met
template <typename M> class MessageQueue {
private:
    int transaction_message_attempts;
    int transaction_message_retry_interval; // seconds
    std::thread worker_thread;
    /// message deque for transaction related messages
    std::deque<ControlMessage<M>*> transaction_message_queue;
    /// message queue for non-transaction related messages
    std::queue<ControlMessage<M>*> normal_message_queue;
    ControlMessage<M>* in_flight;
    std::mutex message_mutex;
    std::condition_variable cv;
    std::function<bool(json message)> send_callback;
    bool paused;
    bool running;
    bool new_message;
    boost::uuids::random_generator uuid_generator;
    std::vector<M> external_notify;

    Everest::SteadyTimer in_flight_timeout_timer;
    Everest::SteadyTimer notify_queue_timer;

    // key is the message id of the stop transaction and the value is the transaction id
    // this map is used for StopTransaction.req that have been put on the message queue without having received a
    // transactionId from the backend (e.g. when offline) it is used to replace the transactionId in the
    // StopTransaction.req
    std::map<std::string, int32_t> message_id_transaction_id_map;

    MessageId getMessageId(const json::array_t& json_message) {
        return MessageId(json_message.at(MESSAGE_ID).get<std::string>());
    }
    MessageTypeId getMessageTypeId(const json::array_t& json_message) {
        if (json_message.size() > 0) {
            auto messageTypeId = json_message.at(MESSAGE_TYPE_ID);
            if (messageTypeId == MessageTypeId::CALL) {
                return MessageTypeId::CALL;
            }
            if (messageTypeId == MessageTypeId::CALLRESULT) {
                return MessageTypeId::CALLRESULT;
            }
            if (messageTypeId == MessageTypeId::CALLERROR) {
                return MessageTypeId::CALLERROR;
            }
        }

        return MessageTypeId::UNKNOWN;
    }
    bool isValidMessageType(const json::array_t& json_message) {
        if (this->getMessageTypeId(json_message) != MessageTypeId::UNKNOWN) {
            return true;
        }
        return false;
    }

    bool isTransactionMessage(ControlMessage<M>* message);

    void add_to_normal_message_queue(ControlMessage<M>* message) {
        EVLOG_debug << "Adding message to normal message queue";
        {
            std::lock_guard<std::mutex> lk(this->message_mutex);
            this->normal_message_queue.push(message);
            this->new_message = true;
        }
        this->cv.notify_all();
        EVLOG_debug << "Notified message queue worker";
    }
    void add_to_transaction_message_queue(ControlMessage<M>* message) {
        EVLOG_debug << "Adding message to transaction message queue";
        {
            std::lock_guard<std::mutex> lk(this->message_mutex);
            this->transaction_message_queue.push_back(message);
            this->new_message = true;
        }
        this->cv.notify_all();
        EVLOG_debug << "Notified message queue worker";
    }

public:
    /// \brief Creates a new MessageQueue object with the provided \p configuration and \p send_callback
    MessageQueue(const std::function<bool(json message)>& send_callback, const int transaction_message_attempts,
                 const int transaction_message_retry_interval, const std::vector<M>& external_notify) :
        transaction_message_attempts(transaction_message_attempts),
        transaction_message_retry_interval(transaction_message_retry_interval),
        external_notify(external_notify),
        paused(true),
        running(true),
        new_message(false),
        uuid_generator(boost::uuids::random_generator()) {
        this->send_callback = send_callback;
        this->in_flight = nullptr;
        this->worker_thread = std::thread([this]() {
            // TODO(kai): implement message timeout
            while (this->running) {
                EVLOG_debug << "Waiting for a message from the message queue";

                std::unique_lock<std::mutex> lk(this->message_mutex);
                using namespace std::chrono_literals;
                this->cv.wait(lk, [this]() {
                    return !this->running || (!this->paused && this->new_message && this->in_flight == nullptr);
                });
                if (this->transaction_message_queue.empty() && this->normal_message_queue.empty()) {
                    // There is nothing in the message queue, not progressing further
                    continue;
                }
                EVLOG_debug << "There are " << this->normal_message_queue.size()
                            << " messages in the normal message queue.";
                EVLOG_debug << "There are " << this->transaction_message_queue.size()
                            << " messages in the transaction message queue.";

                if (this->paused) {
                    // Message queue is paused, not progressing further
                    continue;
                }

                if (this->in_flight != nullptr) {
                    // There already is a message in flight, not progressing further
                    continue;
                } else {
                    EVLOG_debug << "There is no message in flight, checking message queue for a new message.";
                }

                // prioritize the message with the oldest timestamp
                auto now = DateTime();
                ControlMessage<M>* message = nullptr;
                QueueType queue_type = QueueType::None;

                if (!this->normal_message_queue.empty()) {
                    auto& normal_message = this->normal_message_queue.front();
                    EVLOG_debug << "normal msg timestamp: " << normal_message->timestamp;
                    if (normal_message->timestamp <= now) {
                        EVLOG_debug << "normal message timestamp <= now";
                        message = normal_message;
                        queue_type = QueueType::Normal;
                    } else {
                        EVLOG_error << "A normal message should not have a timestamp in the future: "
                                    << normal_message->timestamp << " now: " << now;
                    }
                }

                if (!this->transaction_message_queue.empty()) {
                    auto& transaction_message = this->transaction_message_queue.front();
                    EVLOG_debug << "transaction msg timestamp: " << transaction_message->timestamp;
                    if (message == nullptr) {
                        if (transaction_message->timestamp <= now) {
                            EVLOG_debug << "transaction message timestamp <= now";
                            message = transaction_message;
                            queue_type = QueueType::Transaction;
                        }
                    } else {
                        if (transaction_message->timestamp <= message->timestamp) {
                            EVLOG_debug << "transaction message timestamp <= normal message timestamp";
                            message = transaction_message;
                            queue_type = QueueType::Transaction;
                        } else {
                            EVLOG_debug << "Prioritizing newer normal message over older transaction message";
                        }
                    }
                }

                if (message == nullptr) {
                    EVLOG_debug << "No message in queue ready to be sent yet";
                    continue;
                }

                EVLOG_debug << "Attempting to send message to central system. UID: " << message->uniqueId()
                            << " attempt#: " << message->message_attempts;
                this->in_flight = message;
                this->in_flight->message_attempts += 1;

                if (this->message_id_transaction_id_map.count(this->in_flight->message.at(1))) {
                    EVLOG_debug << "Replacing transaction id";
                    this->in_flight->message.at(3)["transactionId"] =
                        this->message_id_transaction_id_map.at(this->in_flight->message.at(1));
                    this->message_id_transaction_id_map.erase(this->in_flight->message.at(1));
                }

                if (!this->send_callback(this->in_flight->message)) {
                    this->paused = true;
                    EVLOG_error << "Could not send message, this is most likely because the charge point is offline.";
                    if (this->isTransactionMessage(this->in_flight)) {
                        EVLOG_info << "The message in flight is transaction related and will be sent again once the "
                                      "connection can be established again.";
                    } else {
                        EVLOG_info << "The message in flight is not transaction related and will be dropped";
                        if (queue_type == QueueType::Normal) {
                            EnhancedMessage<M> enhanced_message;
                            enhanced_message.offline = true;
                            this->in_flight->promise.set_value(enhanced_message);
                            this->normal_message_queue.pop();
                        }
                    }
                    this->reset_in_flight();
                } else {
                    EVLOG_debug << "Successfully sent message. UID: " << this->in_flight->uniqueId();
                    this->in_flight_timeout_timer.timeout([this]() { this->handle_timeout_or_callerror(boost::none); },
                                                          STANDARD_MESSAGE_TIMEOUT);
                    switch (queue_type) {
                    case QueueType::Normal:
                        this->normal_message_queue.pop();
                        break;
                    case QueueType::Transaction:
                        this->transaction_message_queue.pop_front();
                        break;

                    default:
                        break;
                    }
                }
                if (this->transaction_message_queue.empty() && this->normal_message_queue.empty()) {
                    this->new_message = false;
                }
                lk.unlock();
                cv.notify_one();
            }
            EVLOG_info << "Message queue stopped processing messages";
        });
    }

    MessageQueue(const std::function<bool(json message)>& send_callback, const int transaction_message_attempts,
                 const int transaction_message_retry_interval) :
        MessageQueue(send_callback, transaction_message_attempts, transaction_message_retry_interval, {}){};

    /// \brief pushes a new \p call message onto the message queue
    template <class T> void push(Call<T> call) {
        auto* message = new ControlMessage<M>(call);
        if (this->isTransactionMessage(message)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order

            // intentionally break this message for testing...
            // message->message[CALL_PAYLOAD]["broken"] = this->createMessageId();
            this->add_to_transaction_message_queue(message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            if (!this->paused || message->messageType == M::BootNotification) {
                this->add_to_normal_message_queue(message);
            }
        }
        this->cv.notify_all();
    }

    /// \brief pushes a new \p call message onto the message queue
    /// \returns a future from which the CallResult can be extracted
    template <class T> std::future<EnhancedMessage<M>> push_async(Call<T> call) {
        auto* message = new ControlMessage<M>(call);
        if (this->isTransactionMessage(message)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order
            this->add_to_transaction_message_queue(message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            if (this->paused && message->messageType != M::BootNotification) {
                // do not add a normal message to the queue if the queue is paused/offline
                auto enhanced_message = EnhancedMessage<M>();
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
    EnhancedMessage<M> receive(const std::string& message) {
        EnhancedMessage<M> enhanced_message;

        try {
            enhanced_message.message = json::parse(message);
            enhanced_message.uniqueId = this->getMessageId(enhanced_message.message);
            enhanced_message.messageTypeId = this->getMessageTypeId(enhanced_message.message);

            if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
                enhanced_message.messageType = this->string_to_messagetype(enhanced_message.message.at(CALL_ACTION));
                enhanced_message.call_message = enhanced_message.message;
            }

            // TODO(kai): what happens if we receive a CallResult or CallError out of order?
            if (enhanced_message.messageTypeId == MessageTypeId::CALLRESULT ||
                enhanced_message.messageTypeId == MessageTypeId::CALLERROR) {
                // we need to remove Call messages from in_flight if we receive a CallResult OR a CallError

                // TODO(kai): we need to do some error handling in the CallError case
                std::unique_lock<std::mutex> lk(this->message_mutex);
                if (this->in_flight == nullptr) {
                    EVLOG_error
                        << "Received a CALLRESULT OR CALLERROR without a message in flight, this should not happen";
                    return enhanced_message;
                }
                if (this->in_flight->uniqueId() != enhanced_message.uniqueId) {
                    EVLOG_error << "Received a CALLRESULT OR CALLERROR with mismatching uid: "
                                << this->in_flight->uniqueId() << " != " << enhanced_message.uniqueId;
                    return enhanced_message;
                }
                if (enhanced_message.messageTypeId == MessageTypeId::CALLERROR) {
                    EVLOG_error << "Received a CALLERROR for message with UID: " << enhanced_message.uniqueId;
                    lk.unlock();
                    this->handle_timeout_or_callerror(enhanced_message);
                } else {
                    this->handle_call_result(enhanced_message);
                }
            }

        } catch (const std::exception& e) {
            EVLOG_error << "json parse failed because: "
                        << "(" << e.what() << ")";
        }

        return enhanced_message;
    }

    void reset_in_flight() {
        this->in_flight = nullptr;
        this->in_flight_timeout_timer.stop();
    }

    void handle_call_result(EnhancedMessage<M>& enhanced_message) {
        if (this->in_flight->uniqueId() == enhanced_message.uniqueId) {
            enhanced_message.call_message = this->in_flight->message;
            enhanced_message.messageType = this->string_to_messagetype(
                this->in_flight->message.at(CALL_ACTION).template get<std::string>() + std::string("Response"));
            this->in_flight->promise.set_value(enhanced_message);
            this->reset_in_flight();

            // we want the start transaction response handler to be executed before the next message will be
            // send in order to be able to replace the transaction id if necessary
            // start transaction response handler will notify
            if (std::find(this->external_notify.begin(), this->external_notify.end(), enhanced_message.messageType) ==
                this->external_notify.end()) {
                this->cv.notify_one();
            }
        }
    }

    /// \brief Handles a message timeout or a CALLERROR. \p enhanced_message_opt is set only in case of CALLERROR
    void handle_timeout_or_callerror(const boost::optional<EnhancedMessage<M>>& enhanced_message_opt) {
        std::lock_guard<std::mutex> lk(this->message_mutex);
        EVLOG_warning << "Message timeout or CALLERROR for: " << this->in_flight->messageType << " (" << this->in_flight->uniqueId()
                      << ")";
        if (this->isTransactionMessage(this->in_flight)) {
            if (this->in_flight->message_attempts < this->transaction_message_attempts) {
                EVLOG_warning << "Message is transaction related and will therefore be sent again";
                this->in_flight->message[MESSAGE_ID] = this->createMessageId();
                if (this->transaction_message_retry_interval > 0) {
                    // exponential backoff
                    this->in_flight->timestamp =
                        DateTime(this->in_flight->timestamp.to_time_point() +
                                 std::chrono::seconds(this->transaction_message_retry_interval) *
                                     this->in_flight->message_attempts);
                    EVLOG_debug << "Retry interval > 0: " << this->transaction_message_retry_interval
                                << " attempting to retry message at: " << this->in_flight->timestamp;
                } else {
                    // immediate retry
                    this->in_flight->timestamp = DateTime();
                    EVLOG_debug << "Retry interval of 0 means immediate retry";
                }

                EVLOG_warning << "Attempt: " << this->in_flight->message_attempts + 1 << "/"
                              << this->transaction_message_attempts << " will be sent at "
                              << this->in_flight->timestamp;

                this->transaction_message_queue.push_front(this->in_flight);
                this->notify_queue_timer.at(
                    [this]() {
                        this->new_message = true;
                        this->cv.notify_all();
                    },
                    this->in_flight->timestamp.to_time_point());
            } else {
                EVLOG_error << "Could not deliver message within the configured amount of attempts, "
                               "dropping message";
                if (enhanced_message_opt) {
                    this->in_flight->promise.set_value(enhanced_message_opt.value());
                } else {
                    EnhancedMessage<M> enhanced_message;
                    enhanced_message.offline = true;
                    this->in_flight->promise.set_value(enhanced_message);
                }
            }
        } else {
            EVLOG_warning << "Message is not transaction related, dropping it";
            if (enhanced_message_opt) {
                this->in_flight->promise.set_value(enhanced_message_opt.value());
            } else {
                EnhancedMessage<M> enhanced_message;
                enhanced_message.offline = true;
                this->in_flight->promise.set_value(enhanced_message);
            }
        }
        this->reset_in_flight();
        this->cv.notify_all();
    }

    /// \brief Stops the message queue
    void stop() {
        EVLOG_debug << "stop()";
        // stop the running thread
        this->running = false;
        this->cv.notify_one();
        this->worker_thread.join();
        EVLOG_debug << "stop() notified message queue";
    }

    /// \brief Pauses the message queue
    void pause() {
        EVLOG_debug << "pause()";
        std::lock_guard<std::mutex> lk(this->message_mutex);
        this->paused = true;
        this->cv.notify_one();
        EVLOG_debug << "pause() notified message queue";
    }

    /// \brief Resumes the message queue
    void resume() {
        EVLOG_debug << "resume()";
        std::lock_guard<std::mutex> lk(this->message_mutex);
        this->paused = false;
        this->cv.notify_one();
        EVLOG_debug << "resume() notified message queue";
    }

    /// \brief Set transaction_message_attempts to given \p transaction_message_attempts
    void update_transaction_message_attempts(const int transaction_message_attempts) {
        this->transaction_message_attempts = transaction_message_attempts;
    }

    /// \brief Set transaction_message_retry_interval to given \p transaction_message_retry_interval in seconds
    void update_transaction_message_retry_interval(const int transaction_message_retry_interval) {
        this->transaction_message_retry_interval = transaction_message_retry_interval;
    }

    /// \brief Creates a unique message ID
    /// \returns the unique message ID
    MessageId createMessageId() {
        std::stringstream s;
        s << this->uuid_generator();
        return MessageId(s.str());
    }

    /// \brief Adds the given \p transaction_id to the message_id_transaction_id_map using the key \p
    /// stop_transaction_message_id
    void add_stopped_transaction_id(std::string stop_transaction_message_id, int32_t transaction_id) {
        EVLOG_debug << "adding " << stop_transaction_message_id << " for transaction " << transaction_id;
        this->message_id_transaction_id_map[stop_transaction_message_id] = transaction_id;
    }
    void notify_start_transaction_handled() {
        this->cv.notify_one();
    }

    M string_to_messagetype(const std::string& s);
};

} // namespace ocpp
#endif // OCPP_COMMON_MESSAGE_QUEUE_HPP
