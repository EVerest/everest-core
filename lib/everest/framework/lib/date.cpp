// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <utils/date.hpp>

namespace Everest {
namespace Date {

std::string to_rfc3339(const std::chrono::time_point<date::utc_clock>& t) {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(t));
}

std::chrono::time_point<date::utc_clock> from_rfc3339(const std::string& t) {
    std::istringstream infile{t};
    std::chrono::time_point<date::utc_clock> tp;
    infile >> date::parse("%FT%T", tp);
    return tp;
}

} // namespace Date
} // namespace Everest
