// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_DATE_HPP
#define UTILS_DATE_HPP

#include <date/date.h>
#include <date/tz.h>

namespace Everest {
namespace Date {

std::string to_rfc3339(const std::chrono::time_point<date::utc_clock>& t);

std::chrono::time_point<date::utc_clock> from_rfc3339(const std::string& t);

} // namespace Date
} // namespace Everest

#endif // UTILS_CONFIG_HPP
