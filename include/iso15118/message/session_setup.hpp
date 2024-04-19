// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

#include "common.hpp"

namespace iso15118::message_20 {

struct SessionSetupRequest {
    Header header;
    std::string evccid;
};

struct SessionSetupResponse {
    Header header;
    ResponseCode response_code;
    std::string evseid;
};

} // namespace iso15118::message_20
