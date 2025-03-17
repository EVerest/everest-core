// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace iso15118::d20::state {

struct DC_PreCharge : public StateBase {
    DC_PreCharge(Context& ctx) : StateBase(ctx, StateID::DC_PreCharge) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    bool pre_charge_initiated{false};
    float present_voltage{0};
};

} // namespace iso15118::d20::state
