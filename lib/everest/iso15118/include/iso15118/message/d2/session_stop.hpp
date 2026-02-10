// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

namespace iso15118::d2::msg {

namespace data_types {
enum ChargingSession {
    Terminate,
    Pause
};
}; // namespace data_types

struct SessionStopRequest {
    Header header;
    data_types::ChargingSession charging_session;
};

struct SessionStopResponse {
    Header header;
    data_types::ResponseCode response_code;
};

} // namespace iso15118::d2::msg
