// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_UTILS_HPP
#define OCPP_COMMON_UTILS_HPP

#include <string>
#include <tuple>
#include <vector>

namespace ocpp {

/// \brief Case insensitive compare for a case insensitive (Ci)String
bool iequals(const std::string& lhs, const std::string rhs);

std::vector<std::string> get_vector_from_csv(const std::string& csv_str);

bool is_integer(const std::string& value);
std::tuple<bool, int> is_positive_integer(const std::string& value);
bool is_decimal_number(const std::string& value);

} // namespace ocpp

#endif
