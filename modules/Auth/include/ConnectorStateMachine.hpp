// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef _CONNECTOR_STATE_MACHINE_HPP_
#define _CONNECTOR_STATE_MACHINE_HPP_

#include <string>
#include <memory>

#include <fsm/sync.hpp>
#include <fsm/fsm.hpp>
#include <fsm/specialization/sync/posix.hpp>
#include <fsm/utils/Identifiable.hpp>

namespace module {

enum class ConnectorStatusTransition {
    ENABLE,
    DISABLE,
    ERROR_CLEARED,
    FAULTED,
    TRANSACTION_STARTED,
    SESSION_FINISHED
};

enum class ConnectorState {
    AVAILABLE,
    UNAVAILABLE,
    FAULTED,
    OCCUPIED,
    UNAVAILABLE_FAULTED,
    FAULTED_OCCUPIED
};

using EventTypeFactory = fsm::utils::IdentifiableTypeFactory<ConnectorStatusTransition>;

using Event_Enable = EventTypeFactory::Derived<ConnectorStatusTransition::ENABLE>;
using Event_Disable = EventTypeFactory::Derived<ConnectorStatusTransition::DISABLE>;
using Event_Error_Cleared = EventTypeFactory::Derived<ConnectorStatusTransition::ERROR_CLEARED>;
using Event_Faulted = EventTypeFactory::Derived<ConnectorStatusTransition::FAULTED>;
using Event_Transaction_Started = EventTypeFactory::Derived<ConnectorStatusTransition::TRANSACTION_STARTED>;
using Event_Session_Finished = EventTypeFactory::Derived<ConnectorStatusTransition::SESSION_FINISHED>;

using EventBufferType = std::aligned_union_t<0, Event_Enable>;
using EventBaseType = EventTypeFactory::Base;

struct StateIdType {
    const ConnectorState id;
    const std::string name;
};

struct ConnectorStateMachine {

    using EventInfoType = fsm::EventInfo<EventBaseType, EventBufferType>;
    using StateHandleType = fsm::sync::StateHandle<EventInfoType, StateIdType>;
    using TransitionType = StateHandleType::TransitionWrapperType;
    using FSMContextType = StateHandleType::FSMContextType;
    using ConnectorStateMachineController = fsm::sync::PosixController<ConnectorStateMachine::StateHandleType>;

    ConnectorStateMachine();
    std::shared_ptr<ConnectorStateMachineController> controller;
    ConnectorState state;

    // define state descriptors
    StateHandleType sd_available{{ConnectorState::AVAILABLE, "Available"}};
    StateHandleType sd_unavailable{{ConnectorState::UNAVAILABLE, "Unavailable"}};
    StateHandleType sd_faulted{{ConnectorState::FAULTED, "Faulted"}};
    StateHandleType sd_occupied{{ConnectorState::OCCUPIED, "Occupied"}};
    StateHandleType sd_unavailable_faulted{{ConnectorState::UNAVAILABLE_FAULTED, "UnavailableFaulted"}};
    StateHandleType sd_faulted_occupied{{ConnectorState::FAULTED_OCCUPIED, "FaultedOccupied"}};

    ConnectorState get_state() const;
};

} // namespace module

#endif //_CONNECTOR_STATE_MACHINE_HPP_
