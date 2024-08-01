// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EV_SLAC_STATES_OTHERS_HPP
#define EV_SLAC_STATES_OTHERS_HPP

#include <chrono>

#include <slac/fsm/ev/fsm.hpp>

namespace slac::fsm::ev {

struct ResetState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;
};

struct ResetChipState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // for now returns true if CM_RESET_CNF is received
    bool check_for_valid_reset_conf();

    bool reset_delay_done{false};
    bool chip_reset_has_been_sent{false};

    enum class SubState {
        DELAY,
        SEND_RESET,
        DONE,
    } sub_state{SubState::DELAY};
};

struct InitSlacState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // only returns true, if valid
    bool check_for_valid_parm_conf();

    // sends out CM_SLAC_PARM.REQ, increases num of tries and returns the timeout until a response is expected
    int send_parm_req();

    std::chrono::time_point<std::chrono::steady_clock> next_timeout;

    int num_of_tries{0};

    uint8_t run_id[8];
};

struct MatchRequestState : public FSMSimpleState {
    MatchRequestState(Context& ctx, SessionParamaters session_parameters);

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // return the pointer to the NMK, if valid
    const uint8_t* check_for_valid_match_req_conf();

    // sends out CM_SLAC_PARM.REQ, increases num of tries and returns the timeout until a response is expected
    int send_match_req();

    std::chrono::time_point<std::chrono::steady_clock> next_timeout;

    int num_of_tries{0};

    SessionParamaters session_parameters;
};

struct JoinNetworkState : public FSMSimpleState {
    static constexpr auto SET_KEY_TIMEOUT_MS = 500;
    JoinNetworkState(Context& ctx, const uint8_t* nmk);

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // only returns true, if valid
    bool check_for_valid_set_key_conf();

    std::chrono::time_point<std::chrono::steady_clock> timeout;

    uint8_t nmk[slac::defs::NMK_LEN];
};

struct MatchedState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    void enter() final {
        ctx.log_info("Entered matched state");
        ctx.signal_state("MATCHED");
    }

    HandleEventReturnType handle_event(AllocatorType&, Event) final;
};

struct FailedState : public FSMSimpleState {
    using FSMSimpleState::FSMSimpleState;

    void enter() final {
        ctx.log_info("Entered failed state");
    }

    HandleEventReturnType handle_event(AllocatorType&, Event) final;
};

} // namespace slac::fsm::ev

#endif // EV_SLAC_STATES_OTHERS_HPP
