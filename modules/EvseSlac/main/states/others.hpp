// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_STATES_OTHERS_HPP
#define EVSE_SLAC_STATES_OTHERS_HPP

#include "../fsm.hpp"

struct ResetState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // for now returns true if CM_SET_KEY_CNF is received
    bool handle_slac_message(slac::messages::HomeplugMessage&);

    bool setup_has_been_send{false};
};

struct IdleState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
};

struct MatchedState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    void leave() final;
};

struct FailedState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
};

#endif // EVSE_SLAC_STATES_OTHERS_HPP
