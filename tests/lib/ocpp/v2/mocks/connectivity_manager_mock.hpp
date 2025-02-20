// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include "gmock/gmock.h"

#include <ocpp/v201/connectivity_manager.hpp>

namespace ocpp::v201 {
class ConnectivityManagerMock : public ConnectivityManagerInterface {
public:
    MOCK_METHOD(void, set_websocket_authorization_key, (const std::string& authorization_key));
    MOCK_METHOD(void, set_websocket_connection_options, (const WebsocketConnectionOptions& connection_options));
    MOCK_METHOD(void, set_websocket_connection_options_without_reconnect, ());
    MOCK_METHOD(void, set_websocket_connected_callback, (WebsocketConnectionCallback callback));
    MOCK_METHOD(void, set_websocket_disconnected_callback, (WebsocketConnectionCallback callback));
    MOCK_METHOD(void, set_websocket_connection_failed_callback, (WebsocketConnectionFailedCallback callback));
    MOCK_METHOD(void, set_configure_network_connection_profile_callback,
                (ConfigureNetworkConnectionProfileCallback callback));
    MOCK_METHOD(std::optional<NetworkConnectionProfile>, get_network_connection_profile,
                (const int32_t configuration_slot), (const));
    MOCK_METHOD(std::optional<int32_t>, get_priority_from_configuration_slot, (const int configuration_slot), (const));
    MOCK_METHOD(const std::vector<int>&, get_network_connection_slots, (), (const));
    MOCK_METHOD(bool, is_websocket_connected, ());
    MOCK_METHOD(void, connect, (std::optional<int32_t> network_profile_slot));
    MOCK_METHOD(void, disconnect, ());
    MOCK_METHOD(bool, send_to_websocket, (const std::string& message));
    MOCK_METHOD(void, on_network_disconnected, (OCPPInterfaceEnum ocpp_interface));
    MOCK_METHOD(void, on_charging_station_certificate_changed, ());
    MOCK_METHOD(void, confirm_successful_connection, ());
};
} // namespace ocpp::v201
