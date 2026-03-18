// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once
#include <string>

#include "../states.hpp"

namespace iso15118::d2::state {

struct PaymentServiceSelection : public StateBase {
    PaymentServiceSelection(Context& ctx) : StateBase(ctx, StateID::PaymentServiceSelection) {
    }

    void enter() final{
        // TODO
    };

    Result feed(Event) final {
        // TODO
        return Result{};
    };

private:
    std::string evse_id;
};

} // namespace iso15118::d2::state
