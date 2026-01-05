// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>
#include <ctime>
#include <optional>

namespace iso15118::d20 {

struct UpdateDynamicModeParameters {
    std::optional<std::time_t> departure_time;
    std::optional<std::uint8_t> target_soc;
    std::optional<std::uint8_t> min_soc;
};

} // namespace iso15118::d20