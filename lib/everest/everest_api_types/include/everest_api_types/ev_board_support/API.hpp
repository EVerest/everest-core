// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/evse_board_support/API.hpp>
#include <optional>
#include <stdint.h>
#include <string>

namespace everest::lib::API::V1_0::types::ev_board_support {

enum class EvCpState {
    A,
    B,
    C,
    D,
    E,
};

struct BspMeasurement {
    evse_board_support::ProximityPilot proximity_pilot;
    float cp_pwm_duty_cycle;
    std::optional<float> rcd_current_mA;
};

} // namespace everest::lib::API::V1_0::types::ev_board_support
