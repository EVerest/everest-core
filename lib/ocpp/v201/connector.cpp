// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <utility>

#include <everest/logging.hpp>
#include <ocpp/v201/connector.hpp>

using QueryExecutionException = ocpp::common::QueryExecutionException;
using RequiredEntryNotFoundException = ocpp::common::RequiredEntryNotFoundException;

namespace ocpp {
namespace v201 {

namespace conversions {

std::string connector_event_to_string(ConnectorEvent e) {
    switch (e) {
    case ConnectorEvent::PlugIn:
        return "PlugIn";
    case ConnectorEvent::PlugOut:
        return "PlugOut";
    case ConnectorEvent::Reserve:
        return "Reserve";
    case ConnectorEvent::ReservationCleared:
        return "ReservationCleared";
    case ConnectorEvent::Error:
        return "Error";
    case ConnectorEvent::ErrorCleared:
        return "ErrorCleared";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ConnectorEvent");
}

} // namespace conversions

Connector::Connector(const int32_t evse_id, const int32_t connector_id,
                     std::shared_ptr<ComponentStateManagerInterface> component_state_manager) :
    evse_id(evse_id), connector_id(connector_id), component_state_manager(component_state_manager) {
}

void Connector::submit_event(ConnectorEvent event) {
    std::lock_guard lk(this->status_mutex);
    switch (event) {
    case ConnectorEvent::PlugIn:
        this->component_state_manager->set_connector_occupied(this->evse_id, this->connector_id, true);
        break;
    case ConnectorEvent::PlugOut:
        this->component_state_manager->set_connector_occupied(this->evse_id, this->connector_id, false);
        break;
    case ConnectorEvent::Reserve:
        this->component_state_manager->set_connector_reserved(this->evse_id, this->connector_id, true);
        break;
    case ConnectorEvent::ReservationCleared:
        this->component_state_manager->set_connector_reserved(this->evse_id, this->connector_id, false);
        break;
    case ConnectorEvent::Error:
        this->component_state_manager->set_connector_faulted(this->evse_id, this->connector_id, true);
        break;
    case ConnectorEvent::ErrorCleared:
        this->component_state_manager->set_connector_faulted(this->evse_id, this->connector_id, false);
        break;
    case ConnectorEvent::Unavailable:
        this->component_state_manager->set_connector_unavailable(this->evse_id, this->connector_id, true);
        break;
    case ConnectorEvent::UnavailableCleared:
        this->component_state_manager->set_connector_unavailable(this->evse_id, this->connector_id, false);
        break;
    }
}

void Connector::set_connector_operative_status(OperationalStatusEnum new_status, bool persist) {
    this->component_state_manager->set_connector_individual_operational_status(this->evse_id, this->connector_id,
                                                                               new_status, persist);
}

void Connector::restore_connector_operative_status() {
    try {
        auto persisted_status = this->component_state_manager->get_connector_persisted_operational_status(
            this->evse_id, this->connector_id);
        this->component_state_manager->set_connector_individual_operational_status(this->evse_id, this->connector_id,
                                                                                   persisted_status, false);
    } catch (const QueryExecutionException& e) {
        EVLOG_error << "QueryExecutionException while restoring connector status of evse_id: " << this->evse_id
                    << "; connector_id: " << this->connector_id << ": " << e.what();
    } catch (const std::exception& e) {
        EVLOG_error << "Internal error while restoring connector status of evse_id: " << this->evse_id
                    << "; connector_id: " << this->connector_id << ": " << e.what();
    }
}

OperationalStatusEnum Connector::get_effective_operational_status() {
    return this->component_state_manager->get_connector_effective_operational_status(this->evse_id, this->connector_id);
}

ConnectorStatusEnum Connector::get_effective_connector_status() {
    return this->component_state_manager->get_connector_effective_status(this->evse_id, this->connector_id);
}

} // namespace v201
} // namespace ocpp
