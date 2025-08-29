// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <date/date.h>
#include <date/tz.h>

namespace everest::utils {

std::string to_rfc3339(const std::chrono::time_point<date::utc_clock>& t);

std::chrono::time_point<date::utc_clock> from_rfc3339(const std::string& t);

} // namespace everest::utils
