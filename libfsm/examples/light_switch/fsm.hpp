#ifndef FSM_HPP
#define FSM_HPP

#include <fsm/fsm.hpp>

#include "fsm_defs.hpp"

struct FSM {
    using EventInfoType = fsm::EventInfo<EventBaseType, EventBufferType>;
    using StateHandleType = fsm::StateHandle<EventInfoType, StateIDType>;
    using TransitionType = StateHandleType::TransitionWrapperType;
    using FSMContextType = StateHandleType::FSMContextType;

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

    // define transitions
    StateHandleType& t_manual_mode_init();
    StateHandleType& t_manual_mode_on_press();

    FSM();
};

#endif // FSM_HPP
