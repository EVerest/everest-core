// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <functional>
#include <map>
#include <memory>

#include <ocpp/v201/connector.hpp>

namespace ocpp {
namespace v201 {

/// \brief Represents an EVSE. An EVSE can contain multiple Connector objects, but can only supply energy to one of
/// them.
class Evse {

private:
    int32_t evse_id;
    std::map<int32_t, std::unique_ptr<Connector>> id_connector_map;
    std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)> status_notification_callback;

public:
    /// \brief Construct a new Evse object
    /// \param evse_id id of the evse
    /// \param number_of_connectors of the evse
    /// \param status_notification_callback that is called when the status of a connector changes
    Evse(const int32_t evse_id, const int32_t number_of_connectors,
         const std::function<void(const int32_t connector_id, const ConnectorStatusEnum& status)>&
             status_notification_callback);

    /// \brief Get the state of the connector with the given \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \return ConnectorStatusEnum
    ConnectorStatusEnum get_state(const int32_t connector_id);

    /// \brief Submits the given \p event to the state machine controller of the connector with the given
    /// \p connector_id
    /// \param connector_id id of the connector of the evse
    /// \param event
    void submit_event(const int32_t connector_id, ConnectorEvent event);

    /// \brief Triggers status notification callback for all connectors of the evse
    void trigger_status_notification_callbacks();
};

} // namespace v201
} // namespace ocpp
