#ifndef FSM_DEFS_HPP
#define FSM_DEFS_HPP

#include <string>

#include <fsm/utils/Identifiable.hpp>

enum class EventID
{
    PressedOn,
    PressedMot,
    PressedOff,
    MotionDetect,
    MotionDetectTimeout
};

using EventTypeFactory = fsm::utils::IdentifiableTypeFactory<EventID>;

using EventPressedOn = EventTypeFactory::Derived<EventID::PressedOn>;
using EventPressedMot = EventTypeFactory::Derived<EventID::PressedMot>;
using EventPressedOff = EventTypeFactory::Derived<EventID::PressedOff>;
using EventMotionDetect = EventTypeFactory::Derived<EventID::MotionDetect>;
using EventMotionDetectTimeout = EventTypeFactory::Derived<EventID::MotionDetectTimeout>;

enum class State
{
    LightOff,
    ManualMode,
    MotionModeInit,
    MotionModeDetect
};

struct StateIDType {
    const State id;
    const std::string name;
};

using EventBufferType = std::aligned_union_t<0, EventPressedOn>;
using EventBaseType = EventTypeFactory::Base;

#endif // FSM_DEFS_HPP
