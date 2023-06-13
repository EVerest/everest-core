// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_UTILS_HPP
#define OCPP_COMMON_UTILS_HPP

#include <string>

namespace ocpp {

/// \brief Case insensitive compare for a case insensitive (Ci)String
bool iequals(const std::string& lhs, const std::string rhs);

} // namespace ocpp

#endif
