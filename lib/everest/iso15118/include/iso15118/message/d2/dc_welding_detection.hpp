// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

namespace iso15118::d2::msg {

struct DC_WeldingDetectionRequest {
    Header header;
    data_types::DC_EVStatus ev_status;
};

struct DC_WeldingDetectionResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::DC_EVSEStatus evse_status;
    data_types::PhysicalValue evse_present_voltage;
};

} // namespace iso15118::d2::msg
