// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>

#include <iso15118/message/common_types.hpp>

namespace iso15118::d20 {

namespace dt = message_20::datatypes;

struct AcTargetPower {
    std::optional<dt::RationalNumber> target_active_power;
    std::optional<dt::RationalNumber> target_active_power_L2;
    std::optional<dt::RationalNumber> target_active_power_L3;
    std::optional<dt::RationalNumber> target_reactive_power;
    std::optional<dt::RationalNumber> target_reactive_power_L2;
    std::optional<dt::RationalNumber> target_reactive_power_L3;
    std::optional<dt::RationalNumber> target_frequency;
};

struct AcPresentPower {
    std::optional<dt::RationalNumber> present_active_power;
    std::optional<dt::RationalNumber> present_active_power_L2;
    std::optional<dt::RationalNumber> present_active_power_L3;
};

} // namespace iso15118::d20
