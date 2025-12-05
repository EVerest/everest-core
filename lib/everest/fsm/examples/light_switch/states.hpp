// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef STATES_HPP
#define STATES_HPP

#include "fsm.hpp"

struct LightOff : public SimpleState {
    using SimpleState::SimpleState;

    void enter() final {
        ctx.set_brightness(0);
    }

    HandleEventReturnType handle_event(AllocatorType&, Event) final;
};

struct LightOn : public SimpleState {
    using SimpleState::SimpleState;

    void enter() final {
        ctx.set_brightness(current_brightness);
    }

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

private:
    int current_brightness{1};
};

struct MotionMode : public CompoundState {
    using CompoundState::CompoundState;

    HandleEventReturnType handle_event(AllocatorType&, Event) final;
};

struct MotionIdle : public SimpleState {
    using SimpleState::SimpleState;

    void enter() final {
        ctx.set_brightness(0);
    }

    HandleEventReturnType handle_event(AllocatorType&, Event) final;
};

struct MotionDetected : public SimpleState {
    using SimpleState::SimpleState;

    void enter() final {
        ctx.set_brightness(3);
    }

    HandleEventReturnType handle_event(AllocatorType&, Event) final;

    CallbackReturnType callback() final;

private:
    static const int TIMEOUT_MS = 3000;
    bool timeout_started{false};
};

#endif // STATES_HPP
