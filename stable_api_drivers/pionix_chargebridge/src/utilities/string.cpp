// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <charge_bridge/utilities/string.hpp>

namespace charge_bridge::utilities {
bool string_starts_with(std::string const& str, std::string const pattern) {
    return str.rfind(pattern, 0) == 0;
}

std::string string_after_pattern(const std::string& str, const std::string& pattern) {
    if (string_starts_with(str, pattern)) {
        return str.substr(pattern.length());
    }
    return "";
}

std::string& replace_all_in_place(
    std::string& source,
    std::string const& placeholder,
    std::string const& substitute) {

    if (placeholder.empty()) {
        return source;
    }

    size_t start_pos = 0;

    while ((start_pos = source.find(placeholder, start_pos)) != std::string::npos) {
        source.replace(start_pos, placeholder.length(), substitute);
        start_pos += substitute.length();
    }

    return source;
}


std::string replace_all(std::string const& source, std::string const& placeholder, std::string const& substitute) {
    std::string result = source;
    return replace_all_in_place(result, placeholder, substitute);
}

} // namespace charge_bridge::utilities
