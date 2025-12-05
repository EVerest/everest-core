// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/message_queue.hpp>

namespace ocpp {
namespace v2 {

/// \brief Interface for handling OCPP2.0.1 CALL messages from the CSMS. Classes implementing a functional block shall
/// extend this interface.
class MessageHandlerInterface {

public:
    virtual ~MessageHandlerInterface() {
    }
    /// \brief Handles the given \p message from the CSMS. This includes dispatching a CALLRESULT as a response to the
    /// incoming \p message .
    /// @param message
    virtual void handle_message(const EnhancedMessage<MessageType>& message) = 0;
};

class MessageTypeNotImplementedException : public std::exception {
private:
    std::string message;

public:
    MessageTypeNotImplementedException(MessageType message_type) :
        message("Message is not implemented: " + conversions::messagetype_to_string(message_type)) {
    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

} // namespace v2
} // namespace ocpp