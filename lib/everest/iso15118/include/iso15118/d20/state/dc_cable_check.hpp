// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace iso15118::d20::state {

struct DC_CableCheck : public StateBase {
    DC_CableCheck(Context& ctx) : StateBase(ctx, StateID::DC_CableCheck) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    bool cable_check_initiated{false};
    bool cable_check_done{false};
};

} // namespace iso15118::d20::state
