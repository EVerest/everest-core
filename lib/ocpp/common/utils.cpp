// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/algorithm/string/predicate.hpp>
#include <mutex>
#include <regex>
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

bool is_rfc3339_datetime(const std::string& value) {
    static std::regex datetime_pattern{};
    static std::once_flag datetime_regex_once;
    std::call_once(datetime_regex_once, []() {
        datetime_pattern =
            std::regex{"\\d{4}-(?:0[1-9]|1[0-2])-(?:0[1-9]|[1-2]\\d|3[0-1])T(?:[0-1]\\d|2[0-3]):[0-5]\\d:["
                       "0-5]\\d(?:\\.\\d{0,3}|)(?:Z|(?:\\+|\\-)(?:\\d{2}):?(?:\\d{2}))"};
    });
    return std::regex_match(value, datetime_pattern);
}

std::vector<std::string> split_string(const std::string& string_to_split, const char c) {
    std::stringstream input(string_to_split);
    std::string temp;
    std::vector<std::string> result;

    while (std::getline(input, temp, c)) {
        result.push_back(temp);
    }

    return result;
}

std::string trim_string(const std::string& string_to_trim) {
    size_t first = string_to_trim.find_first_not_of(' ');
    if (std::string::npos == first) {
        return string_to_trim;
    }
    size_t last = string_to_trim.find_last_not_of(' ');
    return string_to_trim.substr(first, (last - first + 1));
}

bool is_boolean(const std::string& value) {
    return iequals(value, "true") || iequals(value, "false");
}

} // namespace ocpp
