// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once
#include <string>

#include "../fsm.hpp"

namespace iso15118::d20::state {

struct SessionSetup : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;

private:
    std::string evse_id;
};

} // namespace iso15118::d20::state
