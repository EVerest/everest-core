// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <utility>

#include <ocpp/v201/evse.hpp>

namespace ocpp {
namespace v201 {

Evse::Evse(const int32_t evse_id, const int32_t number_of_connectors,
           const std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)>&
               status_notification_callback) :
    evse_id(evse_id), status_notification_callback(status_notification_callback) {
    for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
        this->id_connector_map.insert(std::make_pair(
            connector_id,
            std::make_unique<Connector>(connector_id, [this, connector_id](const ConnectorStatusEnum& status) {
                this->status_notification_callback(connector_id, status);
            })));
    }
}

ConnectorStatusEnum Evse::get_state(const int32_t connector_id) {
    return this->id_connector_map.at(connector_id)->get_state();
}

void Evse::submit_event(const int32_t connector_id, ConnectorEvent event) {
    return this->id_connector_map.at(connector_id)->submit_event(event);
}

void Evse::trigger_status_notification_callbacks() {
    for (auto const& [connector_id, connector] : this->id_connector_map) {
        this->status_notification_callback(connector_id, connector->get_state());
    }
}

} // namespace v201
} // namespace ocpp
