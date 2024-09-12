// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v201/messages/SetNetworkProfile.hpp>

#include <functional>
#include <future>
#include <optional>
namespace ocpp {
namespace v201 {

class DeviceModel;

using WebsocketConnectedCallback = std::function<void(const int security_profile)>;
using WebsocketDisconnectedCallback = std::function<void()>;
using WebsocketConnectionFailedCallback = std::function<void(ConnectionFailedReason reason)>;
using ConfigureNetworkConnectionProfileCallback =
    std::function<bool(const NetworkConnectionProfile& network_connection_profile)>;

class ConnectivityManager {
private:
    /// \brief Reference to the device model
    DeviceModel& device_model;
    /// \brief Pointer to the evse security class
    std::shared_ptr<EvseSecurity> evse_security;
    /// \brief Pointer to the logger
    std::shared_ptr<MessageLogging> logging;
    /// \brief Pointer to the websocket
    std::unique_ptr<Websocket> websocket;
    /// \brief The message callback
    std::function<void(const std::string& message)> message_callback;
    /// \brief Callback that is called when the websocket is connected successfully
    std::optional<WebsocketConnectedCallback> websocket_connected_callback;
    /// \brief Callback that is called when the websocket connection is disconnected
    std::optional<WebsocketDisconnectedCallback> websocket_disconnected_callback;
    /// \brief Callback that is called when the websocket could not connect with a specific reason
    std::optional<WebsocketConnectionFailedCallback> websocket_connection_failed_callback;
    /// \brief Callback that is called to configure a network connection profile when none is configured
    std::optional<ConfigureNetworkConnectionProfileCallback> configure_network_connection_profile_callback;

    Everest::SteadyTimer websocket_timer;
    bool disable_automatic_websocket_reconnects;
    int network_configuration_priority;
    /// @brief Local cached network connection profiles
    std::vector<SetNetworkProfileRequest> network_connection_profiles;
    /// @brief local cached network conenction priorities
    std::vector<std::string> network_connection_priorities;
    WebsocketConnectionOptions current_connection_options{};

public:
    ConnectivityManager(DeviceModel& device_model, std::shared_ptr<EvseSecurity> evse_security,
                        std::shared_ptr<MessageLogging> logging,
                        const std::function<void(const std::string& message)>& message_callback);

    /// \brief Set the websocket \p authorization_key
    ///
    void set_websocket_authorization_key(const std::string& authorization_key);

    /// \brief Set the websocket \p connection_options
    ///
    void set_websocket_connection_options(const WebsocketConnectionOptions& connection_options);

    /// \brief Set the websocket connection options without triggering a reconnect
    ///
    void set_websocket_connection_options_without_reconnect();

    /// \brief Set the \p callback that is called when the websocket is connected.
    ///
    void set_websocket_connected_callback(WebsocketConnectedCallback callback);

    /// \brief Set the \p callback that is called when the websocket is disconnected.
    ///
    void set_websocket_disconnected_callback(WebsocketDisconnectedCallback callback);

    /// \brief Set the \p callback that is called when the websocket could not connect with a specific reason
    ///
    void set_websocket_connection_failed_callback(WebsocketConnectionFailedCallback callback);

    /// \brief Set the \p callback that is called to configure a network connection profile when none is configured
    ///
    void set_configure_network_connection_profile_callback(ConfigureNetworkConnectionProfileCallback callback);

    /// \brief Gets the configured NetworkConnectionProfile based on the given \p configuration_slot . The
    /// central system uri ofthe connection options will not contain ws:// or wss:// because this method removes it if
    /// present. This returns the value from the cached network connection profiles. \param
    /// network_configuration_priority \return
    std::optional<NetworkConnectionProfile> get_network_connection_profile(const int32_t configuration_slot);

    /// \brief Check if the websocket is connected
    /// \return True is the websocket is connected, else false
    ///
    bool is_websocket_connected();

    /// \brief Start the connectivity manager
    ///
    void start();

    /// \brief Stop the connectivity manager
    ///
    void stop();

    /// \brief Connect to the websocket
    ///
    void connect();

    /// \brief Disconnect the websocket with a specific \p reason
    ///
    void disconnect_websocket(WebsocketCloseReason code = WebsocketCloseReason::Normal);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    ///
    bool send_to_websocket(const std::string& message);

private:
    /// \brief Init the websocket
    ///
    void init_websocket();

    /// \brief Get the current websocket connection options
    /// \returns the current websocket connection options
    ///
    WebsocketConnectionOptions get_ws_connection_options(const int32_t configuration_slot);

    /// \brief Moves websocket network_configuration_priority to next profile
    ///
    void next_network_configuration_priority();

    /// @brief Cache all the network connection profiles. Must be called once during initialization
    void cache_network_connection_profiles();
};

} // namespace v201
} // namespace ocpp
