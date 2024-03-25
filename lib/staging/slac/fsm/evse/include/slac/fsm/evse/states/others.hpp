// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_STATES_OTHERS_HPP
#define EVSE_SLAC_STATES_OTHERS_HPP

#include "../fsm.hpp"

namespace slac::fsm::evse {

struct ResetState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // for now returns true if CM_SET_KEY_CNF is received
    bool handle_slac_message(slac::messages::HomeplugMessage&);

    bool setup_has_been_send{false};
};

struct ResetChipState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // for now returns true if CM_RESET_CNF is received
    bool handle_slac_message(slac::messages::HomeplugMessage&);

    bool reset_delay_done{false};
    bool chip_reset_has_been_sent{false};
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

} // namespace slac::fsm::evse

#endif // EVSE_SLAC_STATES_OTHERS_HPP
