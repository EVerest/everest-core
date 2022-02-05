// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef SYNC_FSM_HPP
#define SYNC_FSM_HPP

#include <fsm/sync.hpp>

#include "fsm_defs.hpp"

struct FSM {
    using EventInfoType = fsm::EventInfo<EventBaseType, EventBufferType>;
    using StateHandleType = fsm::sync::StateHandle<EventInfoType, StateIDType>;
    using FSMInitContextType = StateHandleType::FSMInitContextType;

    using FSMContextType = StateHandleType::FSMContextType;
    using TransitionType = StateHandleType::TransitionWrapperType;

    // define state descriptors
    StateHandleType sd_light_off{{State::LightOff, "LightOff"}};

    struct ManualModeStateType : public StateHandleType {
        using StateHandleType::StateHandleType;
        int cur_brightness{0};
    } sd_manual_mode{{State::ManualMode, "ManualMode"}};

    struct MotionModeHierarchy {
        StateHandleType sd_mm_init{{State::MotionModeInit, "MotionModeInit"}};
        StateHandleType sd_mm_detect{{State::MotionModeDetect, "MotionModeDetect"}};
        MotionModeHierarchy();
        StateHandleType::TransitionTableType super;
    } sdh_motion_mode;

    // define transisitions
    StateHandleType& t_manual_mode_init();
    StateHandleType& t_manual_mode_on_press();

    FSM();
};

#endif // SYNC_FSM_HPP
