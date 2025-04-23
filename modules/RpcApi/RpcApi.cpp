// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);

    for (const auto& evse_manager : r_evse_manager) {
        // create one DataStore object per EVSE
        this->data.evses.emplace_back().connectors.emplace_back();
        data::DataStoreEvse& evse_data = this->data.evses.back();

        // subscribe to interface variables
        this->subscribe_evse_manager(evse_manager, evse_data);
    }

    m_websocket_server = std::make_unique<server::WebSocketServer>(config.websocket_tls_enabled, config.websocket_port);
    // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
    std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
    transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
    m_rpc_server = std::make_unique<rpc::RpcHandler>(std::move(transport_interfaces), data);
}

void RpcApi::ready() {
    // Start the WebSocket server
    m_rpc_server->start_server();

    invoke_ready(*p_main);
}

void RpcApi::subscribe_evse_manager(const std::unique_ptr<evse_managerIntf>& evse_manager, data::DataStoreEvse& evse_data) {
    evse_manager->subscribe_powermeter([this, &evse_data](const types::powermeter::Powermeter& powermeter) {
        this->meter_interface_to_datastore(powermeter, evse_data.meterdata);
    });

}

void RpcApi::meter_interface_to_datastore(const types::powermeter::Powermeter& powermeter, data::MeterDataStore& meter_data) {
    types::json_rpc_api::MeterDataObj meter_data_new; // default initialized
    if (meter_data.get_data().has_value()) {
        meter_data_new == meter_data.get_data().value();
    }
    // FIXME: copy all interface meter values to our internal object
    if (powermeter.energy_Wh_import.L1.has_value()) {
        meter_data_new.energy_Wh_import.L1 = powermeter.energy_Wh_import.L1.value();
    }

    // submit changes
    meter_data.set_data(meter_data_new);
}

} // namespace module
