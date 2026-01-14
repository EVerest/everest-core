// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>
#include <optional>

namespace iso15118::d2::msg {

struct DC_CurrentDemandRequest {
    Header header;
    data_types::DcEvStatus ev_status;
    data_types::PhysicalValue ev_target_current;
    data_types::PhysicalValue ev_target_voltage;
    std::optional<data_types::PhysicalValue> ev_maximum_voltage_limit{std::nullopt};
    std::optional<data_types::PhysicalValue> ev_maximum_current_limit{std::nullopt};
    std::optional<data_types::PhysicalValue> ev_maximum_power_limit{std::nullopt};
    std::optional<bool> bulk_charging_complete{std::nullopt};
    std::optional<bool> charging_complete{std::nullopt};
    std::optional<data_types::PhysicalValue> remaining_time_to_full_soc{std::nullopt};
    std::optional<data_types::PhysicalValue> remaining_time_to_bulk_soc{std::nullopt};
};

struct DC_CurrentDemandResponse {
    Header header;
    data_types::ResponseCode response_code;
    data_types::DcEvseStatus evse_status;
    data_types::PhysicalValue evse_present_voltage;
    data_types::PhysicalValue evse_present_current;
    bool evse_current_limit_achieved;
    bool evse_voltage_limit_achieved;
    bool evse_power_limit_achieved;
    data_types::EVSEID evse_id;
    data_types::SAScheduleTupleID sa_schedule_tuple_id;
    std::optional<data_types::PhysicalValue> evse_maximum_voltage_limit{std::nullopt};
    std::optional<data_types::PhysicalValue> evse_maximum_current_limit{std::nullopt};
    std::optional<data_types::PhysicalValue> evse_maximum_power_limit{std::nullopt};
    std::optional<data_types::MeterInfo> meter_info{std::nullopt};
    std::optional<bool> receipt_required{std::nullopt};
};

} // namespace iso15118::d2::msg
