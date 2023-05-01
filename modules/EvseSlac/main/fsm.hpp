// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_FSM_HPP
#define EVSE_SLAC_FSM_HPP

#include <fsm/fsm.hpp>

#include "context.hpp"

enum class Event {
    RESET,
    ENTER_BCD,
    LEAVE_BCD,
    SLAC_MESSAGE,

    // internal events
    RETRY_MATCHING,
    MATCH_COMPLETE,
    FAILED,
};

using FSMReturnType = int;

using FSM = fsm::FSM<Event, FSMReturnType>;
using FSMSimpleState = fsm::states::StateWithContext<FSM::SimpleStateType, Context>;
using FSMCompoundState = fsm::states::StateWithContext<FSM::CompoundStateType, Context>;


#endif // EVSE_SLAC_FSM_HPP
