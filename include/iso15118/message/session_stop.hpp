// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <string>

#include "common_types.hpp"

namespace iso15118::message_20 {

struct SessionStopRequest {
    Header header;
    datatypes::ChargingSession charging_session;
    std::optional<datatypes::Name> ev_termination_code;
    std::optional<datatypes::Description> ev_termination_explanation;
};

struct SessionStopResponse {
    Header header;
    datatypes::ResponseCode response_code;
};

} // namespace iso15118::message_20