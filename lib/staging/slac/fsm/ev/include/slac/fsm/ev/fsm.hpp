// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EV_SLAC_FSM_HPP
#define EV_SLAC_FSM_HPP

#include <fsm/fsm.hpp>

#include "context.hpp"

namespace slac::fsm::ev {

enum class Event {
    RESET,
    TRIGGER_MATCHING,
    SLAC_MESSAGE,

    // internal events
    FAILED,
    SUCCESS,
};

using FSMReturnType = int;

using FSM = ::fsm::FSM<Event, FSMReturnType>;
using FSMSimpleState = ::fsm::states::StateWithContext<FSM::SimpleStateType, Context>;
using FSMCompoundState = ::fsm::states::StateWithContext<FSM::CompoundStateType, Context>;

} // namespace slac::fsm::ev

#endif // EV_SLAC_FSM_HPP
