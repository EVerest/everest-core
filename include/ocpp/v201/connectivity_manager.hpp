// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v201/messages/SetNetworkProfile.hpp>
#include <ocpp/v201/ocpp_types.hpp>

#include <functional>
#include <future>
#include <optional>
namespace ocpp {
namespace v201 {

class DeviceModel;

using WebsocketConnectionCallback = std::function<void(
    int configuration_slot, const NetworkConnectionProfile& network_connection_profile, OcppProtocolVersion version)>;
using WebsocketConnectionFailedCallback = std::function<void(ConnectionFailedReason reason)>;
using ConfigureNetworkConnectionProfileCallback = std::function<std::future<ConfigNetworkResult>(
    const int32_t configuration_slot, const NetworkConnectionProfile& network_connection_profile)>;

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
    std::optional<WebsocketConnectionCallback> websocket_connected_callback;
    /// \brief Callback that is called when the websocket connection is disconnected
    std::optional<WebsocketConnectionCallback> websocket_disconnected_callback;
    /// \brief Callback that is called when the websocket could not connect with a specific reason
    std::optional<WebsocketConnectionFailedCallback> websocket_connection_failed_callback;
    /// \brief Callback that is called to configure a network connection profile when none is configured
    std::optional<ConfigureNetworkConnectionProfileCallback> configure_network_connection_profile_callback;

    Everest::SteadyTimer websocket_timer;
    std::optional<int32_t> pending_configuration_slot;
    bool wants_to_be_connected;
    int32_t active_network_configuration_priority;
    int last_known_security_level;
    /// @brief Local cached network connection profiles
    std::vector<SetNetworkProfileRequest> cached_network_connection_profiles;
    /// @brief local cached network connection priorities
    std::vector<int32_t> network_connection_slots;
    OcppProtocolVersion connected_ocpp_version;

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
    void set_websocket_connected_callback(WebsocketConnectionCallback callback);

    /// \brief Set the \p callback that is called when the websocket is disconnected.
    ///
    void set_websocket_disconnected_callback(WebsocketConnectionCallback callback);

    /// \brief Set the \p callback that is called when the websocket could not connect with a specific reason
    ///
    void set_websocket_connection_failed_callback(WebsocketConnectionFailedCallback callback);

    /// \brief Set the \p callback that is called to configure a network connection profile when none is configured
    ///
    void set_configure_network_connection_profile_callback(ConfigureNetworkConnectionProfileCallback callback);

    /// \brief Gets the cached NetworkConnectionProfile based on the given \p configuration_slot.
    /// This returns the value from the cached network connection profiles.
    /// \return Returns a profile if the slot is found
    std::optional<NetworkConnectionProfile> get_network_connection_profile(const int32_t configuration_slot) const;

    /// \brief Get the priority of the given configuration slot.
    /// \param configuration_slot   The configuration slot to get the priority from.
    /// \return The priority if the configuration slot exists.
    ///
    std::optional<int32_t> get_priority_from_configuration_slot(const int configuration_slot) const;

    /// @brief Get the network connection slots sorted by priority.
    /// Each item in the vector contains the configured configuration slots, where the slot with index 0 has the highest
    /// priority.
    /// @return The network connection slots
    ///
    const std::vector<int>& get_network_connection_slots() const;

    /// \brief Check if the websocket is connected
    /// \return True is the websocket is connected, else false
    ///
    bool is_websocket_connected();

    /// \brief Connect to the websocket
    /// \param configuration_slot Optional the network_profile_slot to connect to. std::nullopt will select the slot
    /// internally.
    ///
    void connect(std::optional<int32_t> network_profile_slot = std::nullopt);

    /// \brief Disconnect the websocket
    ///
    void disconnect();

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    ///
    bool send_to_websocket(const std::string& message);

    ///
    /// \brief Can be called when a network is disconnected, for example when an ethernet cable is removed.
    ///
    /// This is introduced because the websocket can take several minutes to timeout when a network interface becomes
    /// unavailable, whereas the system can detect this sooner.
    ///
    /// \param ocpp_interface The interface that is disconnected.
    ///
    void on_network_disconnected(OCPPInterfaceEnum ocpp_interface);

    /// \brief Called when the charging station certificate is changed
    ///
    void on_charging_station_certificate_changed();

    /// \brief Confirms the connection is successful so the security profile requirements can be handled
    void confirm_successful_connection();

private:
    /// \brief Initializes the websocket and tries to connect
    ///
    void try_connect_websocket();

    /// \brief Get the current websocket connection options
    /// \return the current websocket connection options
    ///
    std::optional<WebsocketConnectionOptions> get_ws_connection_options(const int32_t configuration_slot);

    /// \brief Calls the configuration callback to get the interface to use, if there is a callback
    /// \param slot The configuration slot to get the interface for
    /// \param profile The network connection profile to get the interface for
    ///
    /// \return The network configuration containing the network interface to use, nullptr if the request failed or the
    /// callback is not configured
    std::optional<ConfigNetworkResult>
    handle_configure_network_connection_profile_callback(int slot, const NetworkConnectionProfile& profile);

    /// \brief Function invoked when the web socket connected with the \p security_profile
    ///
    void on_websocket_connected(OcppProtocolVersion protocol);

    /// \brief Function invoked when the web socket disconnected
    ///
    void on_websocket_disconnected();

    /// \brief Function invoked when the web socket closes
    ///
    void on_websocket_closed(ocpp::WebsocketCloseReason reason);

    ///
    /// \brief Get the active network configuration slot in use.
    /// \return The active slot the network is connected to or the pending slot.
    ///
    int get_active_network_configuration_slot() const;

    ///
    /// \brief Get the network configuration slot of the given priority.
    /// \param priority The priority to get the configuration slot.
    /// \return The configuration slot.
    ///
    int get_configuration_slot_from_priority(const int priority);

    ///
    /// \brief Get the next prioritized network configuration slot of the given configuration slot.
    /// \param configuration_slot The current configuration slot.
    /// \return The next prioritized configuration slot.
    ///
    int get_next_configuration_slot(int32_t configuration_slot);

    /// \brief Cache all the network connection profiles
    void cache_network_connection_profiles();

    /// \brief Removes all connection profiles from the cache that have a security profile lower than the currently
    /// connected security profile
    void check_cache_for_invalid_security_profiles();

    /// \brief Removes all connection profiles from the database that have a security profile lower than the currently
    /// connected security profile
    void remove_network_connection_profiles_below_actual_security_profile();
};

} // namespace v201
} // namespace ocpp
