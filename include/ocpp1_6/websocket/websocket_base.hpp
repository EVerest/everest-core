// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_BASE_HPP
#define OCPP_WEBSOCKET_BASE_HPP

#include <functional>
#include <memory>

namespace ocpp1_6 {

class ChargePointConfiguration;

///
/// \brief contains a websocket abstraction
///
class WebsocketBase {
protected:
    bool shutting_down;
    std::shared_ptr<ChargePointConfiguration> configuration;
    std::function<void()> connected_callback;
    std::function<void()> disconnected_callback;
    std::function<void(const std::string& message)> message_callback;

    /// \brief Indicates if the required callbacks are registered and the websocket is not shutting down
    /// \returns true if the websocket is properly initialized
    bool initialized();

public:
    /// \brief Creates a new WebsocketBase object with the providede \p configuration
    explicit WebsocketBase(std::shared_ptr<ChargePointConfiguration> configuration);

    /// \brief connect to a websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    virtual bool connect() = 0;

    /// \brief disconnect the websocket
    virtual void disconnect() = 0;

    /// \brief register a \p callback that is called when the websocket is connected successfully
    void register_connected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket is disconnected
    void register_disconnected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket receives a message
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    virtual bool send(const std::string& message) = 0;
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_BASE_HPP
