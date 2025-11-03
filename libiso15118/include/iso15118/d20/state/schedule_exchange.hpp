// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

#include <cstdint>
#include <optional>

#include <iso15118/d20/dynamic_mode_parameters.hpp>

namespace iso15118::d20::state {
struct ScheduleExchange : public StateBase {
    ScheduleExchange(Context& ctx) : StateBase(ctx, StateID::ScheduleExchange) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    UpdateDynamicModeParameters dynamic_parameters;
    bool first_req_msg{true};
    bool timeout_ongoing_reached{false};
};

} // namespace iso15118::d20::state
