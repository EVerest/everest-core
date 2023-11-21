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

bool is_integer(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    // Check for + or - in the beginning
    size_t start_pos = 0;
    if (value[0] == '+' or value[0] == '-') {
        start_pos = 1;
    }

    return std::all_of(value.begin() + start_pos, value.end(), ::isdigit);
}

std::tuple<bool, int> is_positive_integer(const std::string& value) {
    bool valid = is_integer(value);
    auto value_int = 0;
    if (valid) {
        value_int = std::stoi(value);
        if (value_int < 0) {
            valid = false;
        }
    }
    return {valid, value_int};
}

bool is_decimal_number(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    // Check for + or - in the beginning
    size_t start_pos = 0;
    if (value[0] == '+' || value[0] == '-') {
        start_pos = 1;
    }
    int decimal_point_count = 0;

    for (size_t i = start_pos; i < value.length(); ++i) {
        if (value[i] == '.') {
            // Allow at most one decimal point
            if (++decimal_point_count > 1) {
                return false;
            }
        } else if (!std::isdigit(value[i])) {
            return false;
        }
    }
    return true;
}

} // namespace ocpp
