// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::uk_random_delay {

struct CountDown {
    int32_t countdown_s;
    float current_limit_after_delay_A;
    float current_limit_during_delay_A;
    std::optional<std::string> start_time;
};

} // namespace everest::lib::API::V1_0::types::uk_random_delay
