// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace iso15118::d20::state {

struct DC_ChargeParameterDiscovery : public StateBase {
    DC_ChargeParameterDiscovery(Context& ctx) : StateBase(ctx, StateID::DC_ChargeParameterDiscovery) {
    }

    void enter() final;

    Result feed(Event) final;
};

} // namespace iso15118::d20::state
