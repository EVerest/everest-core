// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

#include "common_types.hpp"

namespace iso15118::message_20 {

struct SessionSetupRequest {
    Header header;
    datatypes::Identifier evccid;
};

struct SessionSetupResponse {
    Header header;
    datatypes::ResponseCode response_code;
    datatypes::Identifier evseid;
};

} // namespace iso15118::message_20
