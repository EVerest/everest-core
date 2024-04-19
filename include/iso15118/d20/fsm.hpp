// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <fsm/fsm.hpp>

#include "context.hpp"

namespace iso15118::d20 {

enum class FsmEvent {
    RESET,
    V2GTP_MESSAGE,
    CONTROL_MESSAGE,

    // internal events
    FAILED,
};

using FsmReturnType = int;

using Fsm = fsm::FSM<FsmEvent, FsmReturnType>;
using FsmSimpleState = fsm::states::StateWithContext<Fsm::SimpleStateType, Context>;
using FsmCompoundState = fsm::states::StateWithContext<Fsm::CompoundStateType, Context>;

class Controller {
public:
    explicit Controller(Context&);
};

} // namespace iso15118::d20
