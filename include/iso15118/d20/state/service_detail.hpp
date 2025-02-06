// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace iso15118::d20::state {

struct ServiceDetail : public StateBase {
public:
    ServiceDetail(Context& ctx) : StateBase(ctx, StateID::ServiceDetail) {
    }

    void enter() final;

    Result feed(Event) final;
};

} // namespace iso15118::d20::state
