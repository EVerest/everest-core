// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "common_types.hpp"

namespace iso15118::message_20 {

struct DC_CableCheckRequest {
    Header header;
};

struct DC_CableCheckResponse {

    DC_CableCheckResponse() : processing(datatypes::Processing::Ongoing){};

    Header header;
    datatypes::ResponseCode response_code;

    datatypes::Processing processing;
};

} // namespace iso15118::message_20
