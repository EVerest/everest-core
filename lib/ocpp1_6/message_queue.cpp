// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/message_queue.hpp>

#include <everest/logging.hpp>

namespace ocpp1_6 {

MessageQueue::MessageQueue(std::shared_ptr<ChargePointConfiguration> configuration,
                           const std::function<bool(json message)>& send_callback) :
    paused(true), running(true), new_message(false), uuid_generator(boost::uuids::random_generator()) {
    this->configuration = configuration;
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
            ControlMessage* message = nullptr;
            QueueType queue_type = QueueType::None;

            // send normal messages first
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

            // send transaction messages when normal message queue is empty
            if (!this->transaction_message_queue.empty() && message == nullptr) {
                auto& transaction_message = this->transaction_message_queue.front();
                EVLOG_debug << "transaction msg timestamp: " << transaction_message->timestamp;
                if (message == nullptr) {
                    if (transaction_message->timestamp <= now) {
                        EVLOG_debug << "transaction message timestamp <= now";
                        message = transaction_message;
                        queue_type = QueueType::Transaction;
                    } else {
                        // EVLOG_critical << "WAITING FOR REPEAT";
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
                    this->in_flight = nullptr;
                } else {
                    EVLOG_info << "The message in flight is not transaction related and will be dropped";
                    if (queue_type == QueueType::Normal) {
                        EnhancedMessage enhanced_message;
                        enhanced_message.offline = true;
                        this->in_flight->promise.set_value(enhanced_message);
                        this->normal_message_queue.pop();
                    }
                    this->in_flight = nullptr;
                }

            } else {
                EVLOG_debug << "Successfully sent message. UID: " << this->in_flight->uniqueId();
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

MessageId MessageQueue::getMessageId(const json::array_t& json_message) {
    return MessageId(json_message.at(MESSAGE_ID).get<std::string>());
}

MessageTypeId MessageQueue::getMessageTypeId(const json::array_t& json_message) {
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

bool MessageQueue::isValidMessageType(const json::array_t& json_message) {
    if (this->getMessageTypeId(json_message) != MessageTypeId::UNKNOWN) {
        return true;
    }
    return false;
}

bool MessageQueue::isTransactionMessage(ControlMessage* message) {
    if (message == nullptr) {
        return false;
    }
    if (message->messageType == MessageType::StartTransaction || message->messageType == MessageType::StopTransaction ||
        message->messageType == MessageType::MeterValues) {
        return true;
    }
    return false;
}

void MessageQueue::add_to_normal_message_queue(ControlMessage* message) {
    EVLOG_debug << "Adding message to normal message queue";
    {
        std::lock_guard<std::mutex> lk(this->message_mutex);
        this->normal_message_queue.push(message);
        this->new_message = true;
    }
    this->cv.notify_all();
    EVLOG_debug << "Notified message queue worker";
}

void MessageQueue::add_to_transaction_message_queue(ControlMessage* message) {
    EVLOG_debug << "Adding message to transaction message queue";
    {
        std::lock_guard<std::mutex> lk(this->message_mutex);
        this->transaction_message_queue.push_back(message);
        this->new_message = true;
    }
    this->cv.notify_all();
    EVLOG_debug << "Notified message queue worker";
}

EnhancedMessage MessageQueue::receive(const std::string& message) {
    EnhancedMessage enhanced_message;

    try {
        enhanced_message.message = json::parse(message);
        enhanced_message.uniqueId = this->getMessageId(enhanced_message.message);
        enhanced_message.messageTypeId = this->getMessageTypeId(enhanced_message.message);

        if (enhanced_message.messageTypeId == MessageTypeId::CALL) {
            enhanced_message.messageType = conversions::string_to_messagetype(enhanced_message.message.at(CALL_ACTION));
            enhanced_message.call_message = enhanced_message.message;
        }

        // TODO(kai): what happens if we receive a CallResult or CallError out of order?
        if (enhanced_message.messageTypeId == MessageTypeId::CALLRESULT ||
            enhanced_message.messageTypeId == MessageTypeId::CALLERROR) {
            // we need to remove Call messages from in_flight if we receive a CallResult OR a CallError

            // TODO(kai): we need to do some error handling in the CallError case
            std::unique_lock<std::mutex> lk(this->message_mutex);
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

                auto message_type =
                    conversions::string_to_messagetype(this->in_flight->message.at(CALL_ACTION).get<std::string>());
                if (this->isTransactionMessage(this->in_flight)) {
                    // repeatedly try to send a transaction related message based on the configured retry attempts and
                    // intervals

                    // compare message attempts to number of attempted transmissions
                    auto message_attempts = this->configuration->getTransactionMessageAttempts();
                    EVLOG_debug << "allowed message attempts: " << message_attempts;
                    if (this->in_flight->message_attempts >= message_attempts) {
                        EVLOG_error
                            << "Could not deliver message within the configured amount of attempts, dropping message";
                        this->in_flight->promise.set_value(enhanced_message);
                        this->transaction_message_queue.pop_front();
                    } else {
                        // retry is possible
                        // create a new unique message id
                        this->in_flight->message[MESSAGE_ID] = this->createMessageId();
                        this->in_flight->message_attempts += 1;

                        auto retry_interval = this->configuration->getTransactionMessageRetryInterval();
                        if (retry_interval > 0) {
                            // exponential backoff
                            // TODO(kai): make sure a timer notifies the message queue once this message can be sent
                            this->in_flight->timestamp =
                                DateTime(this->in_flight->timestamp.to_time_point() +
                                         std::chrono::seconds(retry_interval) * this->in_flight->message_attempts);
                            EVLOG_debug << "Retry interval > 0: " << retry_interval
                                        << " attempting to retry message at: " << this->in_flight->timestamp;
                        } else {
                            // immediate retry
                            this->in_flight->timestamp = DateTime();
                            EVLOG_debug << "Retry interval of 0 means immediate retry";
                        }

                        this->transaction_message_queue.pop_front();
                        this->transaction_message_queue.push_front(this->in_flight);
                    }
                    // TODO(kai): start retry timer, this timer can be influenced by the pause() and resume() functions
                    // TODO(kai): signal permanent failure to promise
                    // this could be done by setting the messageTypeId to CALLERROR
                    // notify promise of failure after the allowed amount of retransmissions has been reached:
                    // this->in_flight->promise.set_value(enhanced_message);
                } else {
                    // not a transaction related message, just dropping it
                    EVLOG_error << "Not a transaction related message, dropping it.";
                }
                this->in_flight = nullptr;
                this->cv.notify_one();

            } else {
                if (this->in_flight->uniqueId() == enhanced_message.uniqueId) {
                    if (this->isTransactionMessage(this->in_flight)) {
                        if (!this->transaction_message_queue.empty() &&
                            this->in_flight->uniqueId() == this->transaction_message_queue.front()->uniqueId()) {
                            this->transaction_message_queue.pop_front();
                        }
                    }
                    enhanced_message.call_message = this->in_flight->message;
                    enhanced_message.messageType = conversions::string_to_messagetype(
                        this->in_flight->message.at(CALL_ACTION).get<std::string>() + std::string("Response"));
                    this->in_flight->promise.set_value(enhanced_message);
                    this->in_flight = nullptr;
                    this->cv.notify_one();
                }
            }
        }

    } catch (const std::exception& e) {
        EVLOG_error << "json parse failed because: "
                    << "(" << e.what() << ")";
    }

    return enhanced_message;
}

void MessageQueue::stop() {
    EVLOG_debug << "stop()";
    // stop the running thread
    this->running = false;
    this->cv.notify_one();
    this->worker_thread.join();
    EVLOG_debug << "stop() notified message queue";
}

void MessageQueue::pause() {
    EVLOG_debug << "pause()";
    std::lock_guard<std::mutex> lk(this->message_mutex);
    this->paused = true;
    this->cv.notify_one();
    EVLOG_debug << "pause() notified message queue";
}

void MessageQueue::resume() {
    EVLOG_debug << "resume()";
    std::lock_guard<std::mutex> lk(this->message_mutex);
    this->paused = false;
    this->cv.notify_one();
    EVLOG_debug << "resume() notified message queue";
}

MessageId MessageQueue::createMessageId() {
    std::stringstream s;
    s << this->uuid_generator();
    return MessageId(s.str());
}

void MessageQueue::add_stopped_transaction_id(std::string stop_transaction_message_id, int32_t transaction_id) {

    EVLOG_critical << "adding " << stop_transaction_message_id << " for transaction " << transaction_id;
    this->message_id_transaction_id_map[stop_transaction_message_id] = transaction_id;
}

} // namespace ocpp1_6
