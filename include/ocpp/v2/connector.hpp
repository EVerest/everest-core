// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <functional>
#include <mutex>

#include "component_state_manager.hpp"
#include "database_handler.hpp"
#include <ocpp/v201/ocpp_enums.hpp>
#include <optional>

namespace ocpp {
namespace v201 {

/// \brief Enum for ConnectorEvents
enum class ConnectorEvent {
    PlugIn,
    PlugOut,
    Reserve,
    ReservationCleared,
    Error,
    ErrorCleared,
    Unavailable,
    UnavailableCleared
};

namespace conversions {
/// \brief Converts the given ConnectorEvent \p e to human readable string
/// \returns a string representation of the ConnectorEvent
std::string connector_event_to_string(ConnectorEvent e);
} // namespace conversions

/// \brief Represents a Connector, thus electrical outlet on a Charging Station. Single physical Connector.
class Connector {
private:
    /// \brief ID of the EVSE this connector belongs to (>0)
    // cppcheck-suppress unusedStructMember
    int32_t evse_id;
    /// \brief ID of the connector itself (>0)
    int32_t connector_id;

    /// \brief Component responsible for maintaining and monitoring the operational status of CS, EVSEs, and connectors.
    std::shared_ptr<ComponentStateManagerInterface> component_state_manager;

    /// \brief status mutex to protect the status of the connector against concurrent updates
    std::mutex status_mutex;

public:
    /// \brief Construct a new Connector object
    /// \param evse_id id of the EVSE the connector is ap art of
    /// \param connector_id id of the connector
    /// \param component_state_manager A shared reference to the component state manager
    Connector(const int32_t evse_id, const int32_t connector_id,
              std::shared_ptr<ComponentStateManagerInterface> component_state_manager);

    /// \brief Gets the effective Operative/Inoperative status of this connector
    OperationalStatusEnum get_effective_operational_status();
    /// \brief Gets the effective Available/Unavailable/Faulted/Reserved/Occupied status of this connector
    ConnectorStatusEnum get_effective_connector_status();

    /// \brief Adjust the state of the connector according to the \p event that was submitted.
    /// \param event
    void submit_event(ConnectorEvent event);

    /// \brief Switches the operative status of the connector and recomputes its effective status
    /// \param new_status: The operative status to switch to
    /// \param persist: True if the updated operative status setting should be persisted
    void set_connector_operative_status(OperationalStatusEnum new_status, bool persist);

    /// \brief Restores the operative status of the connector to the persisted status and recomputes its effective
    /// status
    void restore_connector_operative_status();
};

} // namespace v201
} // namespace ocpp
