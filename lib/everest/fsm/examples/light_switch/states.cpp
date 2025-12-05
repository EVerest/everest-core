// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2023 Pionix GmbH and Contributors to EVerest
#include "states.hpp"

SimpleState::HandleEventReturnType LightOn::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::PRESSED_ON) {
        current_brightness = (current_brightness % 3) + 1;
        ctx.set_brightness(current_brightness);
        return HANDLED_INTERNALLY;
    } else if (ev == Event::PRESSED_OFF) {
        return sa.create_simple<LightOff>(ctx);
    } else {
        return PASS_ON;
    }
}

SimpleState::HandleEventReturnType MotionMode::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::PRESSED_OFF) {
        return sa.create_simple<LightOff>(ctx);
    } else if (ev == Event::PRESSED_ON) {
        return sa.create_simple<LightOn>(ctx);
    } else {
        return PASS_ON;
    }
}

SimpleState::HandleEventReturnType MotionDetected::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::MOTION_TIMEOUT) {
        return sa.create_simple<MotionIdle>(ctx);
    } else {
        return PASS_ON;
    }
}

SimpleState::CallbackReturnType MotionDetected::callback() {
    if (timeout_started) {
        return Event::MOTION_TIMEOUT;
    }

    timeout_started = true;

    return TIMEOUT_MS;
}

SimpleState::HandleEventReturnType LightOff::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::PRESSED_ON) {
        return sa.create_simple<LightOn>(ctx);
    } else if (ev == Event::ENTER_MOTION_MODE) {
        sa.create_compound<MotionMode>(ctx);
        return sa.create_simple<MotionIdle>(ctx);
    } else {
        return PASS_ON;
    }
}

SimpleState::HandleEventReturnType MotionIdle::handle_event(AllocatorType& sa, Event ev) {
    if (ev == Event::MOTION_DETECT) {
        return sa.create_simple<MotionDetected>(ctx);
    } else {
        return PASS_ON;
    }
}
