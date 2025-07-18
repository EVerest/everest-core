// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_UTILS_HPP
#define OCPP_COMMON_UTILS_HPP

#include <limits>
#include <string>
#include <tuple>
#include <vector>

namespace ocpp {

/// \brief Case insensitive compare for a case insensitive (Ci)String
bool iequals(const std::string& lhs, const std::string rhs);

bool is_integer(const std::string& value);
std::tuple<bool, int> is_positive_integer(const std::string& value);
bool is_decimal_number(const std::string& value);

bool is_rfc3339_datetime(const std::string& value);
bool is_boolean(const std::string& value);

bool is_equal(const float& value1, const float& value2, const double& epsilon = std::numeric_limits<double>::epsilon());
bool is_equal(const double& value1, const double& value2,
              const double& epsilon = std::numeric_limits<double>::epsilon());

///
/// \brief Split string on a given character.
/// \param string_to_split  The string to split.
/// \param c                The character to split the string on.
/// \param trim             True if all strings must be trimmed as well. Defaults to 'false'.
/// \return A vector with the string 'segments'.
///
std::vector<std::string> split_string(const std::string& string_to_split, const char c, const bool trim = false);

///
/// \brief Trim string, removing leading and trailing white spaces.
/// \param string_to_trim   The string to trim.
/// \return The trimmed string.
///
std::string trim_string(const std::string& string_to_trim);

} // namespace ocpp

#endif
