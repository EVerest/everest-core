// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "util.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace module::util {
std::string get_current_iso_time_string() {
    using std::chrono::system_clock;
    const auto date = system_clock::to_time_t(system_clock::now());

    auto string_stream = std::stringstream{};
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    string_stream << std::put_time(gmtime(&date), "%FT%TZ");
    const auto iso_time_string = string_stream.str();
    return iso_time_string;
}

} // namespace module::util
