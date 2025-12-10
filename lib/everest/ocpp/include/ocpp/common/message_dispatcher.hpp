// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/message_queue.hpp>

namespace ocpp {

/// \brief Interface for dispatching OCPP messages that shall be send over the websocket. This interface defines
/// dispatching of Call, CallResult and CallError messages.
/// \tparam T Type specifies the OCPP protocol version
template <typename T> class MessageDispatcherInterface {

public:
    virtual ~MessageDispatcherInterface() = default;

    /// \brief Dispatches a Call message.
    /// \param call the OCPP Call message.
    /// \param triggered indicates if the call was triggered by a TriggerMessage. Default is false.
    virtual void dispatch_call(const json& call, bool triggered = false) = 0;

    /// \brief Dispatches a Call message asynchronously.
    /// \param call the OCPP Call message.
    /// \param triggered indicates if the call was triggered by a TriggerMessage. Default is false.
    /// \return std::future<ocpp::EnhancedMessage<T>> Future object containing the enhanced message
    ///         result of type T.
    virtual std::future<ocpp::EnhancedMessage<T>> dispatch_call_async(const json& call, bool triggered = false) = 0;

    /// \brief Dispatches a CallResult message.
    /// \param call_result the OCPP CallResult message.
    virtual void dispatch_call_result(const json& call_result) = 0;

    /// \brief Dispatches a CallError message.
    /// \param call_result the OCPP CallError message.
    virtual void dispatch_call_error(const json& call_error) = 0;
};

} // namespace ocpp
