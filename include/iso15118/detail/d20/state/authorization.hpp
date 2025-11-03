// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/d20/session.hpp>
#include <iso15118/message/authorization.hpp>

namespace iso15118::d20::state {

message_20::AuthorizationResponse handle_request(const message_20::AuthorizationRequest& req,
                                                 const d20::Session& session,
                                                 const message_20::datatypes::AuthStatus& authorization_status,
                                                 bool timeout_reached);

} // namespace iso15118::d20::state
