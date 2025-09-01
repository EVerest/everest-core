// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef LIMIT_DECIMAL_PLACES_HPP
#define LIMIT_DECIMAL_PLACES_HPP

#include <nlohmann/json.hpp>

namespace helpers {
double formatAndRoundDouble(double value, int precision = 3);
void roundFloatsInJson(nlohmann::json& j, int precision = 3);
} // namespace helpers

#endif // LIMIT_DECIMAL_PLACES_HPP
