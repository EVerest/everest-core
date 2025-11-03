// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EV_SLAC_STATES_SOUNDING_HPP
#define EV_SLAC_STATES_SOUNDING_HPP

#include <chrono>

#include <slac/slac.hpp>

#include <everest/slac/fsm/ev/fsm.hpp>

namespace slac::fsm::ev {

struct SoundingState : public FSMSimpleState {
    SoundingState(Context& ctx, SessionParamaters session_parameters);

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    void enter() final;
    CallbackReturnType callback() final;

    // returns true, if CM_ATTEN_CHAR.IND is expected and fulfills expectations and response has been send
    bool handle_valid_atten_char_ind();

    // returns true if further message will need to be send
    bool do_sounding();

    // timepoint until we need to receive the atten_char_ind message
    std::chrono::time_point<std::chrono::steady_clock> sounding_timeout;

    // timepoint until we internally want to send the next message
    std::chrono::time_point<std::chrono::steady_clock> next_timeout;

    int count_start_atten_char_sent{0};
    int count_mnbc_sound_sent{0};

    SessionParamaters session_parameters;
};

} // namespace slac::fsm::ev

#endif // EV_SLAC_STATES_SOUNDING_HPP
