// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <iso15118/message/d2/msg_data_types.hpp>

#include <optional>
#include <string>
#include <vector>

namespace iso15118::d2::msg {

namespace data_types {
enum ChargeProgress {
    Start,
    Stop,
    Renegotiate
};

struct ProfileEntry {
    unsigned int start; // Seconds from NOW
    PhysicalValue max_power;
    std::optional<uint8_t> max_number_of_phases_in_use{std::nullopt};
};
using ChargingProfile = std::vector<ProfileEntry>; // [1 - 24]

struct DcEvPowerDeliveryParameter {
    DcEvStatus dc_ev_status;
    std::optional<bool> bulk_charging_complete{std::nullopt};
    bool charging_complete;
};
}; // namespace data_types

struct PowerDeliveryRequest {
    Header header;
    data_types::ChargeProgress charge_progress;
    data_types::SAScheduleTupleID sa_schedule_tuple_id;
    std::optional<data_types::ChargingProfile> charging_profile;
    std::optional<data_types::DcEvPowerDeliveryParameter> dc_ev_power_delivery_parameter;
};

struct PowerDeliveryResponse {
    Header header;
    data_types::ResponseCode response_code;
    std::optional<data_types::AcEvseStatus> ac_evse_status;
    std::optional<data_types::DcEvseStatus> dc_evse_status;
};

} // namespace iso15118::d2::msg
