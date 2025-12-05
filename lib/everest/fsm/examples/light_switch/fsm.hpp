// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef FSM_HPP
#define FSM_HPP

#include <fsm/buffer.hpp>
#include <fsm/fsm.hpp>

struct Context {
    void set_brightness(int value);
};

enum class Event {
    PRESSED_ON,
    PRESSED_OFF,
    ENTER_MOTION_MODE,
    MOTION_DETECT,
    MOTION_TIMEOUT,
};

#ifdef HEAP_FREE_MODE
using BufferType = fsm::buffer::SwapBuffer<24, 16, 2>;
using FSM = fsm::FSM<Event, int, BufferType>;
#else
using FSM = fsm::FSM<Event, int>;
#endif

using SimpleState = fsm::states::StateWithContext<FSM::SimpleStateType, Context>;
using CompoundState = fsm::states::StateWithContext<FSM::CompoundStateType, Context>;

static const auto PASS_ON = FSM::StateAllocatorType::PASS_ON;
static const auto HANDLED_INTERNALLY = FSM::StateAllocatorType::HANDLED_INTERNALLY;

#endif // FSM_HPP
