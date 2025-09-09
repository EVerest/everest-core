// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>

#include <iso15118/message/common_types.hpp>

namespace iso15118::d20 {

namespace dt = message_20::datatypes;

template <typename T> struct Limit {
    T max;
    T min;
};

struct Limits {
    Limit<message_20::datatypes::RationalNumber> power;
    Limit<message_20::datatypes::RationalNumber> current;
};

struct DcTransferLimits {
    Limits charge_limits;
    std::optional<Limits> discharge_limits;
    Limit<message_20::datatypes::RationalNumber> voltage;
    std::optional<message_20::datatypes::RationalNumber> power_ramp_limit;
};

struct AcTransferLimits {
    Limit<dt::RationalNumber> charge_power;
    std::optional<Limit<dt::RationalNumber>> charge_power_L2;
    std::optional<Limit<dt::RationalNumber>> charge_power_L3;

    dt::RationalNumber nominal_frequency;
    std::optional<dt::RationalNumber> max_power_asymmetry;
    std::optional<dt::RationalNumber> power_ramp_limitation;

    std::optional<Limit<dt::RationalNumber>> discharge_power;
    std::optional<Limit<dt::RationalNumber>> discharge_power_L2;
    std::optional<Limit<dt::RationalNumber>> discharge_power_L3;
};

} // namespace iso15118::d20
