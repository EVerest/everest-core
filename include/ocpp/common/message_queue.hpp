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

#include <ocpp/common/call_types.hpp>
#include <ocpp/common/database/database_handler_common.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v16/messages/StopTransaction.hpp>
#include <ocpp/v16/types.hpp>
#include <ocpp/v201/messages/TransactionEvent.hpp>
#include <ocpp/v201/types.hpp>

namespace ocpp {

using QueryExecutionException = common::QueryExecutionException;

struct MessageQueueConfig {
    int transaction_message_attempts;
    int transaction_message_retry_interval; // seconds

    // threshold for the accumulated sizes of the queues; if the queues exceed this limit,
    // messages are potentially dropped in accordance with OCPP 2.0.1. Specification (cf. QueueAllMessages parameter)
    int queues_total_size_threshold;

    bool queue_all_messages; // cf. OCPP 2.0.1. "QueueAllMessages" in OCPPCommCtrlr

    int message_timeout_seconds = 30;
    int boot_notification_retry_interval_seconds =
        60; // interval for BootNotification.req in case response by CSMS is CALLERROR or CSMS does not respond at all
            // (within specified MessageTimeout)
};

/// \brief Contains a OCPP message in json form with additional information
template <typename M> struct EnhancedMessage {
    json message;                     ///< The OCPP message as json
    size_t message_size;              ///< size of the json message in bytes
    MessageId uniqueId;               ///< The unique ID of the json message
    M messageType = M::InternalError; ///< The OCPP message type
    MessageTypeId messageTypeId;      ///< The OCPP message type ID (CALL/CALLRESULT/CALLERROR)
    json call_message;    ///< If the message is a CALLRESULT or CALLERROR this can contain the original CALL message
    bool offline = false; ///< A flag indicating if the connection to the central system is offline
};

/// \brief This contains an internal control message
template <typename M> struct ControlMessage {
    json::array_t message;    ///< The OCPP message as a json array
    M messageType;            ///< The OCPP message type
    int32_t message_attempts; ///< The number of times this message has been rejected by the central system
    std::promise<EnhancedMessage<M>> promise; ///< A promise used by the async send interface
    DateTime timestamp;                       ///< A timestamp that shows when this message can be sent
    MessageId initial_unique_id;
    bool stall_until_accepted; // if true, message shall be sent only if registration status is accepted

    /// \brief Creates a new ControlMessage object from the provided \p message
    explicit ControlMessage(const json& message, const bool stall_until_accepted = false);

    /// \brief Provides the unique message ID stored in the message
    /// \returns the unique ID of the contained message
    [[nodiscard]] MessageId uniqueId() const {
        return this->message[MESSAGE_ID];
    }

    /// \brief True for transactional messages containing updates (measurements) for a transaction
    bool is_transaction_update_message() const;
};

/// \brief Indicates the transmission priority of a message that is being pushed to the message queue
enum class MessageTransmissionPriority {
    SendImmediately,                     // message can be queued and can be send immediately
    SendAfterRegistrationStatusAccepted, // message can be queued and shall be send only if registration status is
                                         // accepted
    Discard                              // message shall be discarded and not be sent
};

/// \brief Helper function to handle messages that shall be send
inline MessageTransmissionPriority get_message_transmission_priority(bool is_boot_notification_message, bool triggered,
                                                                     bool registration_already_accepted,
                                                                     bool is_transaction_related,
                                                                     bool queue_all_message) {
    if (registration_already_accepted || is_boot_notification_message || triggered) {
        return MessageTransmissionPriority::SendImmediately;
    }

    if (is_transaction_related || queue_all_message) {
        return MessageTransmissionPriority::SendAfterRegistrationStatusAccepted;
    }

    return MessageTransmissionPriority::Discard;
};

/// \brief Indicates if the given \p message_type is a transaction message type
/// \param message_type
/// \return true if MessageType is StartTransaction, StopTransaction, MeterValue or SecurityEventNotification
bool is_transaction_message(const ocpp::v16::MessageType message_type);

/// \brief Indicates if the given \p message_type is a transaction message type
/// \param message_type
/// \return true if MessageType is TransactionEvent or SecurityEventNotification
bool is_transaction_message(const ocpp::v201::MessageType message_type);

/// \brief Indicates if the given \p message_type is a StartTransaction message
/// \param message_type
/// \return true if MessageType is a StartTransaction
bool is_start_transaction_message(const ocpp::v16::MessageType message_type);

/// \brief Indicates if the given \p message_type is a StartTransaction message.
/// \param message_type
/// \return Always return false
bool is_start_transaction_message(const ocpp::v201::MessageType message_type);

/// \brief Indicates if the given \p control_message is a start transaction message
template <typename M> auto is_start_transaction_message(const ControlMessage<M>& control_message) {
    return is_start_transaction_message(control_message.messageType);
}

/// \brief Indicates if the given \p control_message is a transaction related message
template <typename M> auto is_transaction_message(const ControlMessage<M>& control_message) {
    return is_transaction_message(control_message.messageType);
}
/// \brief Indicates if the given \p message_type is a BootNotification
/// \param message_type
/// \return true if MessageType is BootNotification
bool is_boot_notification_message(const ocpp::v16::MessageType message_type);

/// \brief Indicates if the given \p message_type is a BootNotification
/// \param message_type
/// \return true if MessageType is BootNotification
bool is_boot_notification_message(const ocpp::v201::MessageType message_type);

template <typename M>
bool allowed_to_send_message(const ControlMessage<M>& message, const DateTime& time,
                             const bool is_registration_status_accepted) {
    if (message.stall_until_accepted and !is_registration_status_accepted) {
        return false;
    }

    if (message.timestamp > time) {
        return false;
    }
    return true;
}

/// \brief contains a message queue that makes sure that OCPPs synchronicity requirements are met
template <typename M> class MessageQueue {
private:
    MessageQueueConfig config;
    std::shared_ptr<ocpp::common::DatabaseHandlerCommon> database_handler;

    std::thread worker_thread;
    /// message deque for transaction related messages
    std::deque<std::shared_ptr<ControlMessage<M>>> transaction_message_queue;
    /// message queue for non-transaction related messages
    std::deque<std::shared_ptr<ControlMessage<M>>> normal_message_queue;
    std::shared_ptr<ControlMessage<M>> in_flight;
    std::recursive_mutex message_mutex;
    std::condition_variable_any cv;
    std::function<bool(json message)> send_callback;
    std::vector<M> external_notify;
    bool paused;
    // Transiently true while the queue is paused, but is waiting to unpause
    bool resuming;
    bool running;
    bool new_message;
    bool is_registration_status_accepted;
    boost::uuids::random_generator uuid_generator;
    std::recursive_mutex next_message_mutex;
    std::optional<MessageId> next_message_to_send;

    Everest::SteadyTimer in_flight_timeout_timer;
    Everest::SteadyTimer notify_queue_timer;

    // This timer schedules the resumption of the message queue
    Everest::SteadyTimer resume_timer;
    // Counts the number of pause()/resume() calls.
    // Used by the resume timer callback to abort itself in case the timer triggered before it could be cancelled.
    u_int64_t pause_resume_ctr = 0;

    // key is the message id of the stop transaction and the value is the transaction id
    // this map is used for StopTransaction.req that have been put on the message queue without having received a
    // transactionId from the backend (e.g. when offline) it is used to replace the transactionId in the
    // StopTransaction.req
    std::map<std::string, int32_t> message_id_transaction_id_map;

    // key is the message id of a StartTransaction.req and value is a list of MeterValue.req message ids. It is used to
    // replace the transactionId within the MeterValue.req in case the transactionId was unknown at the time the message
    // was queued. This can happen when the CP has not received a StartTransaction.conf from the CSMS.
    std::map<std::string, std::vector<std::string>> start_transaction_mid_meter_values_mid_map;

    // This callback is called when a StartTransaction.req message could not be delivered due to a timeout or CALL_ERROR
    std::function<void(const std::string& new_message_id, const std::string& old_message_id)>
        start_transaction_message_retry_callback;

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

    void add_to_normal_message_queue(std::shared_ptr<ControlMessage<M>> message) {
        EVLOG_debug << "Adding message to normal message queue";
        {
            std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
            // A BootNotification message should always jump the queue
            if (message->messageType == M::BootNotification) {
                this->normal_message_queue.push_front(message);
            } else {
                this->normal_message_queue.push_back(message);
            }
            if (this->config.queue_all_messages) {
                ocpp::common::DBTransactionMessage db_message{
                    message->message, messagetype_to_string(message->messageType), message->message_attempts,
                    message->timestamp, message->uniqueId()};
                try {
                    this->database_handler->insert_message_queue_message(db_message, QueueType::Normal);
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not insert message into transaction queue: " << e.what();
                }
            }
            this->new_message = true;
            this->check_queue_sizes();
        }
        this->cv.notify_all();
        EVLOG_debug << "Notified message queue worker";
    }
    void add_to_transaction_message_queue(std::shared_ptr<ControlMessage<M>> message) {
        EVLOG_debug << "Adding message to transaction message queue";
        {
            std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
            this->transaction_message_queue.push_back(message);
            ocpp::common::DBTransactionMessage db_message{message->message, messagetype_to_string(message->messageType),
                                                          message->message_attempts, message->timestamp,
                                                          message->uniqueId()};
            try {
                this->database_handler->insert_message_queue_message(db_message);
            } catch (const QueryExecutionException& e) {
                EVLOG_warning << "Could not insert message into transaction queue: " << e.what();
            }
            this->new_message = true;
            this->check_queue_sizes();
        }
        this->cv.notify_all();
        EVLOG_debug << "Notified message queue worker";
    }

    void check_queue_sizes() {
        if (this->transaction_message_queue.size() + this->normal_message_queue.size() <=
            this->config.queues_total_size_threshold) {
            return;
        }
        EVLOG_warning << "Queue sizes exceed threshold (" << this->config.queues_total_size_threshold << ") with "
                      << this->transaction_message_queue.size() << " transaction and "
                      << this->normal_message_queue.size() << " normal messages in queue";

        while (this->transaction_message_queue.size() + this->normal_message_queue.size() >
                   this->config.queues_total_size_threshold &&
               !this->normal_message_queue.empty()) {
            this->drop_messages_from_normal_message_queue();
        }

        while (this->transaction_message_queue.size() + this->normal_message_queue.size() >
                   this->config.queues_total_size_threshold &&
               this->drop_update_messages_from_transactional_message_queue()) {
        }
    }

    void drop_messages_from_normal_message_queue() {
        // try to drop approx 10% of the allowed size (at least 1)
        int number_of_dropped_messages = std::min((int)this->normal_message_queue.size(),
                                                  std::max(this->config.queues_total_size_threshold / 10, 1));

        EVLOG_warning << "Dropping " << number_of_dropped_messages << " messages from normal message queue.";

        for (int i = 0; i < number_of_dropped_messages; i++) {
            if (this->config.queue_all_messages) {
                try {
                    database_handler->remove_message_queue_message(
                        this->normal_message_queue.front()->initial_unique_id, QueueType::Normal);
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not delete message from transaction queue: " << e.what();
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not delete message from transaction queue: " << e.what();
                }
            }
            this->normal_message_queue.pop_front();
        }
    }

    /**
     *  Heuristically drops every second update messag.
     *  Drops every first, third, ... update message in between two non-update message; disregards transaction
     * ids etc!
     * Cf. OCPP 2.0.1. specification 2.1.9 "QueueAllMessages"
     */
    bool drop_update_messages_from_transactional_message_queue() {
        int drop_count = 0;
        std::deque<std::shared_ptr<ControlMessage<M>>> temporary_swap_queue;
        bool remove_next_update_message = true;
        while (!transaction_message_queue.empty()) {
            auto element = transaction_message_queue.front();
            transaction_message_queue.pop_front();
            // drop every second update message (except last one)
            if (remove_next_update_message && element->is_transaction_update_message() &&
                transaction_message_queue.size() > 1) {
                EVLOG_debug << "Drop transactional message " << element->initial_unique_id;
                try {
                    database_handler->remove_message_queue_message(element->initial_unique_id);
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not delete message from transaction queue: " << e.what();
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not delete message from transaction queue: " << e.what();
                }
                drop_count++;
                remove_next_update_message = false;
            } else {
                remove_next_update_message = true;
                temporary_swap_queue.push_back(element);
            }
        }

        std::swap(transaction_message_queue, temporary_swap_queue);

        if (drop_count > 0) {
            EVLOG_warning << "Dropped " << drop_count << " transactional update messages to reduce queue size.";
            return true;
        } else {
            EVLOG_warning << "There are no further transaction update messages to drop!";
            return false;
        }
    }

    // The public resume() delegates the actual resumption to this method
    void resume_now(u_int64_t expected_pause_resume_ctr) {
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        if (this->pause_resume_ctr == expected_pause_resume_ctr) {
            this->paused = false;
            this->resuming = false;
            this->cv.notify_one();
            EVLOG_debug << "resume() notified message queue";
        }
    }

    // Computes the current message timeout = interval * attempt + message timeout
    std::chrono::seconds current_message_timeout(unsigned int attempt) {
        return std::chrono::seconds(this->config.message_timeout_seconds +
                                    (this->config.transaction_message_retry_interval * attempt));
    }

public:
    /// \brief Creates a new MessageQueue object with the provided \p configuration and \p send_callback
    MessageQueue(
        const std::function<bool(json message)>& send_callback, const MessageQueueConfig& config,
        const std::vector<M>& external_notify, std::shared_ptr<common::DatabaseHandlerCommon> database_handler,
        const std::function<void(const std::string& new_message_id, const std::string& old_message_id)>
            start_transaction_message_retry_callback =
                [](const std::string& new_message_id, const std::string& old_message_id) {}) :
        database_handler(std::move(database_handler)),
        config(config),
        external_notify(external_notify),
        paused(true),
        resuming(false),
        running(true),
        new_message(false),
        is_registration_status_accepted(false),
        uuid_generator(boost::uuids::random_generator()),
        start_transaction_message_retry_callback(start_transaction_message_retry_callback) {

        this->send_callback = send_callback;
        this->in_flight = nullptr;
    }

    MessageQueue(const std::function<bool(json message)>& send_callback, const MessageQueueConfig& config,
                 std::shared_ptr<common::DatabaseHandlerCommon> databaseHandler) :
        MessageQueue(send_callback, config, {}, databaseHandler) {
    }

    void start() {
        this->worker_thread = std::thread([this]() {
            // TODO(kai): implement message timeout
            while (this->running) {
                EVLOG_debug << "Waiting for a message from the message queue";

                std::unique_lock<std::recursive_mutex> lk(this->message_mutex);
                using namespace std::chrono_literals;
                // It's safe to wait on the cv here because we're guaranteed to only lock this->message_mutex once
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
                std::shared_ptr<ControlMessage<M>> message = nullptr;
                QueueType queue_type = QueueType::None;
                const auto now = DateTime();

                // Find the first allowed normal message
                auto selected_normal_message_it =
                    std::find_if(normal_message_queue.begin(), normal_message_queue.end(),
                                 [&](const std::shared_ptr<ControlMessage<M>>& msg) {
                                     return allowed_to_send_message(*msg, now, this->is_registration_status_accepted);
                                 });

                if (selected_normal_message_it != normal_message_queue.end()) {
                    message = *selected_normal_message_it;
                    queue_type = QueueType::Normal;
                }

                auto is_transaction_message_available = [&](const std::shared_ptr<ControlMessage<M>>& msg) {
                    if (!allowed_to_send_message(*msg, now, this->is_registration_status_accepted)) {
                        return false;
                    }
                    // no message selected from normal message queue, so select transaction message
                    if (message == nullptr) {
                        return true;
                    }
                    // message from normal message queue is BootNotification, this is prioritized
                    if (message->messageType == M::BootNotification) {
                        return false;
                    }
                    // transaction messages is older than normal message, so select transaction message
                    if (msg->timestamp <= message->timestamp) {
                        return true;
                    }
                    return false;
                };

                // Transaction messages must persist the order, so only check the first in the queue
                auto selected_transaction_message_it =
                    (!transaction_message_queue.empty() and
                     is_transaction_message_available(transaction_message_queue.front()))
                        ? transaction_message_queue.begin()
                        : transaction_message_queue.end();

                if (selected_transaction_message_it != transaction_message_queue.end()) {
                    message = *selected_transaction_message_it;
                    queue_type = QueueType::Transaction;
                }

                if (message == nullptr) {
                    EVLOG_debug << "No message in queue ready to be sent yet";
                    this->new_message = false;
                    continue;
                }

                {
                    std::lock_guard<std::recursive_mutex> lk(this->next_message_mutex);
                    if (next_message_to_send.has_value()) {
                        if (next_message_to_send.value() != message->uniqueId()) {
                            EVLOG_debug << "Message with id " << message->uniqueId()
                                        << " held back because message with id " << next_message_to_send.value()
                                        << " should be sent first";
                            continue;
                        }
                    }
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
                    if (this->in_flight && is_transaction_message(*this->in_flight)) {
                        EVLOG_info << "The message in flight is transaction related and will be sent again once the "
                                      "connection can be established again.";
                        if (this->in_flight->message.at(CALL_ACTION) == "TransactionEvent") {
                            this->in_flight->message.at(CALL_PAYLOAD)["offline"] = true;
                        }
                    } else if (this->config.queue_all_messages) {
                        EVLOG_info << "The message in flight  will be sent again once the connection can be "
                                      "established again since QueueAllMessages is set to 'true'.";
                    } else {
                        EVLOG_info << "The message in flight is not transaction related and will be dropped";
                        if (queue_type == QueueType::Normal) {
                            EnhancedMessage<M> enhanced_message;
                            enhanced_message.offline = true;
                            this->in_flight->promise.set_value(enhanced_message);
                            this->normal_message_queue.pop_front();
                        }
                    }
                    this->reset_in_flight();
                } else {
                    EVLOG_debug << "Successfully sent message. UID: " << this->in_flight->uniqueId();
                    this->in_flight_timeout_timer.timeout([this]() { this->handle_timeout_or_callerror(std::nullopt); },
                                                          this->current_message_timeout(message->message_attempts));
                    switch (queue_type) {
                    case QueueType::Normal:
                        this->normal_message_queue.erase(selected_normal_message_it);
                        break;
                    case QueueType::Transaction:
                        this->transaction_message_queue.erase(selected_transaction_message_it);
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

    /// \brief Resets next message to send. Can be used in situation when we dont want to reply to a CALL message
    void reset_next_message_to_send() {
        std::lock_guard<std::recursive_mutex> lk(this->next_message_mutex);
        this->next_message_to_send.reset();
    }

    /// \brief Gets all persisted messages of normal message queue and persisted message queue from the database
    void get_persisted_messages_from_db(bool ignore_security_event_notifications = false) {
        std::vector<QueueType> queue_types = {QueueType::Normal, QueueType::Transaction};
        // do for Normal and Transaction queue
        for (const auto queue_type : queue_types) {
            const auto persisted_messages = database_handler->get_message_queue_messages(queue_type);
            if (!persisted_messages.empty()) {
                for (auto& persisted_message : persisted_messages) {

                    if (ignore_security_event_notifications &&
                        persisted_message.message_type == "SecurityEventNotification") {
                        try {
                            // remove from database in case SecurityEventNotification.req should not be sent
                            this->database_handler->remove_message_queue_message(persisted_message.unique_id,
                                                                                 queue_type);
                        } catch (const QueryExecutionException& e) {
                            EVLOG_warning << "Could not delete message from message queue: " << e.what();
                        } catch (const std::exception& e) {
                            EVLOG_warning << "Could not delete message from message queue: " << e.what();
                        }
                    } else {
                        std::shared_ptr<ControlMessage<M>> message =
                            std::make_shared<ControlMessage<M>>(persisted_message.json_message);
                        message->messageType = string_to_messagetype(persisted_message.message_type);
                        message->timestamp = persisted_message.timestamp;
                        message->message_attempts = persisted_message.message_attempts;

                        if (queue_type == QueueType::Normal) {
                            normal_message_queue.push_back(message);
                        } else if (queue_type == QueueType::Transaction) {
                            transaction_message_queue.push_back(message);
                        }
                    }
                }
                this->new_message = true;
            }
        }

        if (!this->config.queue_all_messages) {
            // make sure to clear normal message queue table in case queue_all_messages is false, since without clearing
            // it here messages would not be removed in handle_call_result or handle_call_timeout_or_error
            this->database_handler->clear_message_queue(QueueType::Normal);
        }
    }

    /// \brief pushes a new \p call message onto the message queue
    template <class T> void push(Call<T> call, const bool stall_until_accepted = false) {
        if (!running) {
            return;
        }
        json call_json = call;
        push(call_json, stall_until_accepted);
    }

    void push(const json& message, const bool stall_until_accepted = false) {
        if (!running) {
            return;
        }

        auto control_message = std::make_shared<ControlMessage<M>>(message, stall_until_accepted);
        if (is_transaction_message(*control_message)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order

            // intentionally break this message for testing...
            // message->message[CALL_PAYLOAD]["broken"] = this->createMessageId();
            this->add_to_transaction_message_queue(control_message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            if (!this->paused || this->resuming || this->config.queue_all_messages ||
                control_message->messageType == M::BootNotification) {
                this->add_to_normal_message_queue(control_message);
            }
        }
        this->cv.notify_all();
    }

    /// \brief Sends a new \p call_result message over the websocket
    template <class T> void push(CallResult<T> call_result) {
        if (!running) {
            return;
        }

        this->send_callback(call_result);
        {
            std::lock_guard<std::recursive_mutex> lk(this->next_message_mutex);
            if (next_message_to_send.has_value()) {
                if (next_message_to_send.value() == call_result.uniqueId) {
                    next_message_to_send.reset();
                }
            }
        }

        this->cv.notify_all();
    }

    /// \brief Sends a new \p call_error message over the websocket
    void push(CallError call_error) {
        if (!running) {
            return;
        }

        this->send_callback(call_error);
        {
            std::lock_guard<std::recursive_mutex> lk(this->next_message_mutex);
            if (next_message_to_send.has_value()) {
                if (next_message_to_send.value() == call_error.uniqueId) {
                    next_message_to_send.reset();
                }
            }
        }

        this->cv.notify_all();
    }

    /// \brief pushes a new \p call message onto the message queue
    /// \returns a future from which the CallResult can be extracted
    template <class T> std::future<EnhancedMessage<M>> push_async(Call<T> call) {
        auto message = std::make_shared<ControlMessage<M>>(call);

        if (!running) {
            auto enhanced_message = EnhancedMessage<M>();
            enhanced_message.offline = true;
            message->promise.set_value(enhanced_message);
        } else if (is_transaction_message(message->messageType)) {
            // according to the spec the "transaction related messages" StartTransaction, StopTransaction and
            // MeterValues have to be delivered in chronological order
            this->add_to_transaction_message_queue(message);
        } else {
            // all other messages are allowed to "jump the queue" to improve user experience
            // TODO: decide if we only want to allow this for a subset of messages
            if (this->paused && !this->config.queue_all_messages && !this->resuming &&
                message->messageType != M::BootNotification) {
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
    EnhancedMessage<M> receive(std::string_view message) {
        EnhancedMessage<M> enhanced_message;

        enhanced_message.message = json::parse(message);
        enhanced_message.uniqueId = this->getMessageId(enhanced_message.message);
        enhanced_message.messageTypeId = this->getMessageTypeId(enhanced_message.message);

        if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
            enhanced_message.messageType = this->string_to_messagetype(enhanced_message.message.at(CALL_ACTION));
            enhanced_message.call_message = enhanced_message.message;

            {
                std::lock_guard<std::recursive_mutex> lk(this->next_message_mutex);
                // save the uid of the message we just received to ensure the next message we send is a response to
                // this message
                next_message_to_send.emplace(enhanced_message.uniqueId);
            }
        }

        // TODO(kai): what happens if we receive a CallResult or CallError out of order?
        if (enhanced_message.messageTypeId == MessageTypeId::CALLRESULT ||
            enhanced_message.messageTypeId == MessageTypeId::CALLERROR) {
            {
                std::lock_guard<std::recursive_mutex> lk(this->next_message_mutex);
                next_message_to_send.reset();
            }
            // we need to remove Call messages from in_flight if we receive a CallResult OR a CallError

            // TODO(kai): we need to do some error handling in the CallError case
            std::unique_lock<std::recursive_mutex> lk(this->message_mutex);
            if (this->in_flight == nullptr) {
                EVLOG_error << "Received a CALLRESULT OR CALLERROR without a message in flight, this should not happen";
                return enhanced_message;
            }
            if (this->in_flight->uniqueId() != enhanced_message.uniqueId) {
                EVLOG_error << "Received a CALLRESULT OR CALLERROR with mismatching uid: "
                            << this->in_flight->uniqueId() << " != " << enhanced_message.uniqueId;
                return enhanced_message;
            }
            if (enhanced_message.messageTypeId == MessageTypeId::CALLERROR) {
                EVLOG_error << "Received a CALLERROR for message with UID: " << enhanced_message.uniqueId;
                // make sure the original call message is attached to the callerror
                enhanced_message.call_message = this->in_flight->message;
                lk.unlock();
                this->handle_timeout_or_callerror(enhanced_message);
            } else {
                this->handle_call_result(enhanced_message);
            }
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

            const auto queue_type =
                is_transaction_message(*this->in_flight) ? QueueType::Transaction : QueueType::Normal;
            if (is_transaction_message(*this->in_flight) or this->config.queue_all_messages) {
                try {
                    // We only remove the message as soon as a response is received. Otherwise we might miss a message
                    // if the charging station just boots after sending, but before receiving the result.
                    this->database_handler->remove_message_queue_message(this->in_flight->initial_unique_id,
                                                                         queue_type);
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not delete message from message queue: " << e.what();
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not delete message from message queue: " << e.what();
                }
            }
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
    void handle_timeout_or_callerror(const std::optional<EnhancedMessage<M>>& enhanced_message_opt) {
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        // We got a timeout iff enhanced_message_opt is empty. Otherwise, enhanced_message_opt contains the CallError.
        bool timeout = !enhanced_message_opt.has_value();
        if (timeout) {
            EVLOG_warning << "Message timeout for: " << this->in_flight->messageType << " ("
                          << this->in_flight->uniqueId() << ")";
        } else {
            EVLOG_warning << "CALLERROR for: " << this->in_flight->messageType << " (" << this->in_flight->uniqueId()
                          << ")";
        }

        const auto queue_type = is_transaction_message(*this->in_flight) ? QueueType::Transaction : QueueType::Normal;
        if (is_transaction_message(*this->in_flight) or this->config.queue_all_messages) {
            if (this->in_flight->message_attempts < this->config.transaction_message_attempts) {
                EVLOG_warning << "Message shall be persisted and will therefore be sent again";
                // Generate a new message ID for the retry
                const auto old_message_id = this->in_flight->message[MESSAGE_ID];
                this->in_flight->message[MESSAGE_ID] = this->createMessageId();
                if (this->config.transaction_message_retry_interval > 0) {
                    // exponential backoff
                    this->in_flight->timestamp =
                        DateTime(this->in_flight->timestamp.to_time_point() +
                                 std::chrono::seconds(this->config.transaction_message_retry_interval) *
                                     this->in_flight->message_attempts);
                    EVLOG_debug << "Retry interval > 0: " << this->config.transaction_message_retry_interval
                                << " attempting to retry message at: " << this->in_flight->timestamp;
                } else {
                    // immediate retry
                    this->in_flight->timestamp = DateTime();
                    EVLOG_debug << "Retry interval of 0 means immediate retry";
                }

                EVLOG_warning << "Attempt: " << this->in_flight->message_attempts + 1 << "/"
                              << this->config.transaction_message_attempts << " will be sent at "
                              << this->in_flight->timestamp;

                if (queue_type == QueueType::Transaction) {
                    this->transaction_message_queue.push_front(this->in_flight);
                } else if (queue_type == QueueType::Normal) {
                    this->normal_message_queue.push_front(this->in_flight);
                }
                if (is_start_transaction_message(*this->in_flight)) {
                    this->start_transaction_message_retry_callback(this->in_flight->message[MESSAGE_ID],
                                                                   old_message_id);
                }
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
                try {
                    // also drop the message from the database
                    this->database_handler->remove_message_queue_message(this->in_flight->initial_unique_id,
                                                                         queue_type);
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Could not delete message from transaction queue: " << e.what();
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not delete message from transaction queue: " << e.what();
                }
            }
        } else if (is_boot_notification_message(this->in_flight->messageType)) {
            EVLOG_warning << "Message is BootNotification.req and will therefore be sent again";
            // Generate a new message ID for the retry
            this->in_flight->message[MESSAGE_ID] = this->createMessageId();
            // Spec does not define how to handle retries for BootNotification.req: We use the
            // the boot_notification_retry_interval_seconds
            this->in_flight->timestamp =
                DateTime(this->in_flight->timestamp.to_time_point() +
                         std::chrono::seconds(this->config.boot_notification_retry_interval_seconds));
            this->normal_message_queue.push_front(this->in_flight);
            this->notify_queue_timer.at(
                [this]() {
                    this->new_message = true;
                    this->cv.notify_all();
                },
                this->in_flight->timestamp.to_time_point());
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
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        this->pause_resume_ctr++;
        this->resume_timer.stop();
        this->paused = true;
        this->resuming = false;
        this->cv.notify_one();
        EVLOG_debug << "pause() notified message queue";
    }

    /// \brief Resumes the message queue
    void resume(std::chrono::seconds delay_on_reconnect) {
        EVLOG_debug << "resume() called";
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        if (!this->paused) {
            return;
        }
        this->pause_resume_ctr++;
        // Do not delay if this is the first call to resume(), i.e. this is the initial connection
        if (this->pause_resume_ctr > 1 && delay_on_reconnect > std::chrono::seconds(0)) {
            this->resuming = true;
            EVLOG_debug << "Delaying message queue resume by " << delay_on_reconnect.count() << " seconds";
            u_int64_t expected_pause_resume_ctr = this->pause_resume_ctr;
            this->resume_timer.timeout(
                [this, expected_pause_resume_ctr] { this->resume_now(expected_pause_resume_ctr); }, delay_on_reconnect);
        } else {
            this->resume_now(this->pause_resume_ctr);
        }
    }

    void set_registration_status_accepted() {
        {
            std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
            this->is_registration_status_accepted = true;
        }
        this->cv.notify_all();
    }

    bool is_transaction_message_queue_empty() {
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        return this->transaction_message_queue.empty();
    }

    bool contains_transaction_messages(const CiString<36> transaction_id) {
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        for (const auto control_message : this->transaction_message_queue) {
            if (control_message->messageType == v201::MessageType::TransactionEvent) {
                v201::TransactionEventRequest req = control_message->message.at(CALL_PAYLOAD);
                if (req.transactionInfo.transactionId == transaction_id) {
                    return true;
                }
            }
        }
        return false;
    }

    bool contains_stop_transaction_message(const int32_t transaction_id) {
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        for (const auto control_message : this->transaction_message_queue) {
            if (control_message->messageType == v16::MessageType::StopTransaction) {
                v16::StopTransactionRequest req = control_message->message.at(CALL_PAYLOAD);
                if (req.transactionId == transaction_id) {
                    return true;
                }
            }
        }
        return false;
    }

    /// \brief Set transaction_message_attempts to given \p transaction_message_attempts
    void update_transaction_message_attempts(const int transaction_message_attempts) {
        this->config.transaction_message_attempts = transaction_message_attempts;
    }

    /// \brief Set transaction_message_retry_interval to given \p transaction_message_retry_interval in seconds
    void update_transaction_message_retry_interval(const int transaction_message_retry_interval) {
        this->config.transaction_message_retry_interval = transaction_message_retry_interval;
    }

    /// \brief Set message_timeout to given \p timeout (in seconds)
    void update_message_timeout(const int timeout) {
        this->config.message_timeout_seconds = timeout;
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

    void add_meter_value_message_id(const std::string& start_transaction_message_id,
                                    const std::string& meter_value_message_id) {
        if (this->start_transaction_mid_meter_values_mid_map.count(start_transaction_message_id)) {
            this->start_transaction_mid_meter_values_mid_map.at(start_transaction_message_id)
                .push_back(meter_value_message_id);
        } else {
            std::vector<std::string> meter_value_message_ids;
            meter_value_message_ids.push_back(meter_value_message_id);
            this->start_transaction_mid_meter_values_mid_map[start_transaction_message_id] = meter_value_message_ids;
        }
    }

    void notify_start_transaction_handled(const std::string& start_transaction_message_id,
                                          const int32_t transaction_id) {
        this->cv.notify_one();

        // replace transaction id in meter values if start_transaction_message_id is present in map
        // this is necessary when the chargepoint queued MeterValue.req for a transaction with unknown transaction_id
        std::lock_guard<std::recursive_mutex> lk(this->message_mutex);
        if (this->start_transaction_mid_meter_values_mid_map.count(start_transaction_message_id)) {
            for (auto it = this->transaction_message_queue.begin(); it != transaction_message_queue.end(); ++it) {
                for (const auto& meter_value_message_id :
                     this->start_transaction_mid_meter_values_mid_map.at(start_transaction_message_id)) {

                    if (meter_value_message_id == (*it)->message.at(1)) {
                        EVLOG_debug << "Adding transactionId " << transaction_id << " to MeterValue.req";
                        (*it)->message.at(3)["transactionId"] = transaction_id;
                    }
                }
            }
        }
        this->start_transaction_mid_meter_values_mid_map.erase(start_transaction_message_id);
    }

    M string_to_messagetype(const std::string& s);
    std::string messagetype_to_string(M m);
};

} // namespace ocpp
#endif // OCPP_COMMON_MESSAGE_QUEUE_HPP
