// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef _CONNECTOR_HPP_
#define _CONNECTOR_HPP_

#include <optional>

#include <everest/timer.hpp>

#include <utils/types.hpp>

#include <ConnectorStateMachine.hpp>
#include <generated/types/authorization.hpp>

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
    explicit Connector(int id) :
        id(id),
        transaction_active(false),
        reserved(false),
        is_reservable(true),
        state_machine(ConnectorState::AVAILABLE){};

    int id;

    bool transaction_active;
    ConnectorStateMachine state_machine;

    // identifier is set when transaction is running and none if not
    std::optional<Identifier> identifier = std::nullopt;

    bool is_reservable;
    bool reserved;

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
    bool is_unavailable();

    ConnectorState get_state() const;
};

struct ConnectorContext {

    ConnectorContext(int connector_id, int evse_index) : evse_index(evse_index), connector(connector_id){};

    int evse_index;
    Connector connector;
    Everest::SteadyTimer timeout_timer;
    std::mutex plug_in_mutex;
    std::mutex event_mutex;
};

namespace conversions {
std::string connector_state_to_string(const ConnectorState& state);
} // namespace conversions

} // namespace module

#endif //_CONNECTOR_HPP_
