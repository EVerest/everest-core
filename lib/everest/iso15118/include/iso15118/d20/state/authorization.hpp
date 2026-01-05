// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"
#include <iso15118/message/authorization.hpp>

namespace iso15118::d20::state {
struct Authorization : public StateBase {
public:
    Authorization(Context& ctx) : StateBase(ctx, StateID::Authorization){};

    void enter() final;

    Result feed(Event) final;

private:
    message_20::datatypes::AuthStatus authorization_status{message_20::datatypes::AuthStatus::Pending};
    bool first_req_msg{true};
    bool timeout_ongoing_reached{false};
};

} // namespace iso15118::d20::state
