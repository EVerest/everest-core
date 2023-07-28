// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

#include <ocpp/common/utils.hpp>

namespace ocpp {

bool iequals(const std::string& lhs, const std::string rhs) {
    return boost::algorithm::iequals(lhs, rhs);
}

std::vector<std::string> get_vector_from_csv(const std::string& csv_str) {
    std::vector<std::string> csv;
    std::string str;
    std::stringstream ss(csv_str);
    while (std::getline(ss, str, ',')) {
        csv.push_back(str);
    }
    return csv;
}

} // namespace ocpp
