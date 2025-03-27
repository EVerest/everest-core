// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef _CONNECTOR_HPP_
#define _CONNECTOR_HPP_

#include <optional>

#include <everest/timer.hpp>

#include <utils/types.hpp>

#include <ConnectorStateMachine.hpp>
#include <generated/types/authorization.hpp>
#include <generated/types/evse_manager.hpp>

namespace module {

/// \brief Validated Identifier struct. Used to keep track of active Identifiers
struct Identifier {
    types::authorization::IdToken id_token;       ///< IdToken of the identifier
    types::authorization::AuthorizationType type; ///< Type of the provider of the identifier
    std::optional<types::authorization::AuthorizationStatus> authorization_status;
    std::optional<std::string> expiry_time; ///< Absolute UTC time point when reservation expires in RFC3339 format
    std::optional<types::authorization::IdToken> parent_id_token; ///< Parent id token of the identifier
};

struct Connector {
    explicit Connector(
        int id, const types::evse_manager::ConnectorTypeEnum type = types::evse_manager::ConnectorTypeEnum::Unknown) :
        id(id), transaction_active(false), state_machine(ConnectorState::AVAILABLE), type(type) {
    }

    int id;

    bool transaction_active;
    ConnectorStateMachine state_machine;
    types::evse_manager::ConnectorTypeEnum type;

    /**
     * @brief Submits the given \p event to the state machine
     *
     * @param event
     */
    void submit_event(ConnectorEvent event);

    /**
     * @brief Returns true if connector is in state UNAVAILABLE or UNAVAILABLE_FAULTED
     *
     * @return true
     * @return false
     */
    bool is_unavailable() const;

    ConnectorState get_state() const;
};

struct EVSEContext {

    EVSEContext(
        int evse_id, int evse_index, int connector_id,
        const types::evse_manager::ConnectorTypeEnum connector_type = types::evse_manager::ConnectorTypeEnum::Unknown) :
        evse_id(evse_id), evse_index(evse_index), transaction_active(false), plugged_in(false) {
        Connector c(connector_id, connector_type);
        connectors.push_back(c);
    }

    EVSEContext(int evse_id, int evse_index, const std::vector<Connector>& connectors) :
        evse_id(evse_id),
        evse_index(evse_index),
        transaction_active(false),
        connectors(connectors),
        plugged_in(false),
        plug_in_timeout(false) {
    }

    int32_t evse_id;
    int32_t evse_index;
    bool transaction_active;

    // identifier is set when transaction is running and none if not
    std::optional<Identifier> identifier = std::nullopt;
    std::vector<Connector> connectors;
    Everest::SteadyTimer timeout_timer;
    std::atomic<bool> timeout_in_progress{false};
    bool plugged_in;
    bool plug_in_timeout; // indicates no authorization received within connection_timeout. Replug is required for this
                          // EVSE to get authorization and start a transaction

    bool is_available();
    bool is_unavailable();
};

namespace conversions {
std::string connector_state_to_string(const ConnectorState& state);
} // namespace conversions

} // namespace module

#endif //_CONNECTOR_HPP_
