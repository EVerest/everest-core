// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

namespace iso15118::d2::msg {

namespace data_types {} // namespace data_types

struct CableCheckRequest {
    Header header;
    data_types::DC_EVStatus ev_status;
};

struct CableCheckResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::DC_EVSEStatus evse_status;
    data_types::EVSEProcessing evse_processing;
};

} // namespace iso15118::d2::msg
