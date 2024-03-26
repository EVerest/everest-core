// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_STATES_OTHERS_HPP
#define EVSE_SLAC_STATES_OTHERS_HPP

#include "../fsm.hpp"
#include <chrono>

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

    enum class SubState {
        DELAY,
        SEND_RESET,
        DONE,
    } sub_state{SubState::DELAY};
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

struct WaitForLinkState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // for now returns true if link up detected is received
    bool handle_slac_message(slac::messages::HomeplugMessage&);

    bool link_status_req_sent{false};
    std::chrono::steady_clock::time_point start_time;
};

} // namespace slac::fsm::evse

#endif // EVSE_SLAC_STATES_OTHERS_HPP
