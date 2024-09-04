// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>

#include <iso15118/message/common.hpp>

namespace iso15118::d20 {

template <typename T> struct Limit {
    T max;
    T min;
};

struct Limits {
    Limit<message_20::RationalNumber> power;
    Limit<message_20::RationalNumber> current;
};

struct DcTransferLimits {
    Limits charge_limits;
    std::optional<Limits> discharge_limits;
    Limit<message_20::RationalNumber> voltage;
    std::optional<message_20::RationalNumber> power_ramp_limit;
};

} // namespace iso15118::d20