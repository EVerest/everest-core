// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_STATES_OTHERS_HPP
#define EVSE_SLAC_STATES_OTHERS_HPP

#include "../fsm.hpp"

struct ResetState : public FSMBaseState {
    using FSMBaseState::FSMBaseState;

    int enter() final;
    FSM::CallbackResultType callback() final;
    FSM::NextStateType handle_event(Event ev) final;

    // for now returns true if CM_SET_KEY_CNF is received
    bool handle_slac_message(slac::messages::HomeplugMessage&);

    bool setup_has_been_send{false};
};

struct IdleState : public FSMBaseState {
    using FSMBaseState::FSMBaseState;

    int enter() final;
    FSM::NextStateType handle_event(Event ev) final;
};

struct MatchedState : public FSMBaseState {
    using FSMBaseState::FSMBaseState;

    int enter() final;
    void leave() final;
    FSM::NextStateType handle_event(Event ev) final;
};

struct FailedState : public FSMBaseState {
    using FSMBaseState::FSMBaseState;

    int enter() final;
    FSM::NextStateType handle_event(Event ev) final;
};

#endif // EVSE_SLAC_STATES_OTHERS_HPP
