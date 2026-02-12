// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/d20/config.hpp>
#include <iso15118/d20/session.hpp>
#include <iso15118/message/authorization_setup.hpp>

namespace iso15118::d20::state {

message_20::AuthorizationSetupResponse
handle_request(const message_20::AuthorizationSetupRequest& req, d20::Session& session, bool cert_install_service,
               const everest::lib::util::fixed_vector<dt::Authorization, 2>& authorization_services);

} // namespace iso15118::d20::state
