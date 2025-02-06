// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace iso15118::d20::state {
struct PowerDelivery : public StateBase {
    PowerDelivery(Context& ctx) : StateBase(ctx, StateID::PowerDelivery) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    float present_voltage{0};
};

} // namespace iso15118::d20::state
