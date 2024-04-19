// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "common.hpp"

namespace iso15118::message_20 {

struct DC_PreChargeRequest {
    Header header;

    Processing processing;
    RationalNumber present_voltage;
    RationalNumber target_voltage;
};

struct DC_PreChargeResponse {
    Header header;
    ResponseCode response_code;

    RationalNumber present_voltage;
};

} // namespace iso15118::message_20
