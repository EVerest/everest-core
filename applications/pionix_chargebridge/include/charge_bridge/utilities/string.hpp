// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <set>
#include <string>

namespace charge_bridge::utilities {
bool string_starts_with(std::string_view const& str, std::string_view const& pattern);
bool string_ends_with(std::string const& str, std::string const& pattern);

std::string string_after_pattern(std::string_view const& str, std::string_view const& pattern);
std::string& replace_all_in_place(std::string& source, std::string const& placeholder, std::string const& substitute);
std::string replace_all(std::string const& source, std::string const& placeholder, std::string const& substitute);

std::set<std::string> csv_to_set(std::string const& str);

} // namespace charge_bridge::utilities
