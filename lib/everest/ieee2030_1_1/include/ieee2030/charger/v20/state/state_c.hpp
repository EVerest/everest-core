// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

#include <ieee2030/common/messages/messages.hpp>

namespace ieee2030::charger::v20::state {

namespace state_c {
enum class InternalStates {
    C_1,
    C_2,
};

} // namespace state_c
struct StateC : public StateBase {
public:
    StateC(Context& ctx) : StateBase(ctx, StateID::StateC) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    bool stop{false};

    state_c::InternalStates states{state_c::InternalStates::C_1};

    messages::EV100 ev_100;
    messages::EV101 ev_101;
    messages::EV102 ev_102;
};

} // namespace ieee2030::charger::v20::state
