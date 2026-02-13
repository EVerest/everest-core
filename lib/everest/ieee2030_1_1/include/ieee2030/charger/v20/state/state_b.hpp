// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace ieee2030::charger::v20::state {

struct StateB : public StateBase {
public:
    StateB(Context& ctx) : StateBase(ctx, StateID::StateB) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    bool stop{false};
};

} // namespace ieee2030::charger::v20::state
