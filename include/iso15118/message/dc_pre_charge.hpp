// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "common_types.hpp"

namespace iso15118::message_20 {

struct DC_PreChargeRequest {
    Header header;

    datatypes::Processing processing;
    datatypes::RationalNumber present_voltage;
    datatypes::RationalNumber target_voltage;
};

struct DC_PreChargeResponse {
    Header header;
    datatypes::ResponseCode response_code;

    datatypes::RationalNumber present_voltage;
};

} // namespace iso15118::message_20
