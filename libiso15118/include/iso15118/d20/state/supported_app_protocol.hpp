// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

namespace iso15118::d20::state {

struct SupportedAppProtocol : public StateBase {
public:
    SupportedAppProtocol(Context& ctx) : StateBase(ctx, StateID::SupportedAppProtocol) {
    }

    void enter() final;

    Result feed(Event) final;
};

} // namespace iso15118::d20::state
