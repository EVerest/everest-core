// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>
#include <optional>

namespace iso15118::d2::msg {

struct AC_ChargingStatusRequest {
    Header header;
};

struct AC_ChargingStatusResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::EVSEID evse_id;
    data_types::SAScheduleTupleID sa_schedule_tuple_id;
    std::optional<data_types::PhysicalValue> evse_max_current{std::nullopt};
    std::optional<data_types::MeterInfo> meter_info{std::nullopt};
    std::optional<bool> receipt_required{std::nullopt};
    data_types::AcEvseStatus ac_evse_status;
};

} // namespace iso15118::d2::msg
