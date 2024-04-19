// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <string>

#include "common.hpp"

namespace iso15118::message_20 {

struct SessionStopRequest {
    Header header;
    ChargingSession charging_session;
    std::optional<std::string> ev_termination_code;
    std::optional<std::string> ev_termination_explanation;
};

struct SessionStopResponse {
    Header header;
    ResponseCode response_code;
};

} // namespace iso15118::message_20