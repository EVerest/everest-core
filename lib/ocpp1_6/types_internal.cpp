// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>
#include <iostream>
#include <ocpp1_6/types_internal.hpp>
#include <sstream>
#include <stdexcept>

namespace ocpp1_6 {
CiString::CiString(const std::string& data, size_t length) : length(length) {
    this->set(data);
}

CiString::CiString(size_t length) : length(length) {
}

std::string CiString::get() const {
    return this->data;
}

void CiString::set(const std::string& data) {
    if (data.length() <= this->length) {
        for (const char& character : data) {
            // printable ASCII starts at code 0x20 (space) and ends with code 0x7e (tilde)
            if (character < 0x20 || character > 0x7e) {
                throw std::runtime_error("CiString can only contain printable ASCII characters");
            }
        }
        this->data = data;
    } else {
        throw std::runtime_error("CiString length (" + std::to_string(data.length()) + ") exceeds permitted length (" +
                                 std::to_string(this->length) + ")");
    }
}

std::ostream& operator<<(std::ostream& os, const CiString& str) {
    os << str.get();
    return os;
}

DateTimeImpl::DateTimeImpl() {
    this->timepoint = date::utc_clock::now();
}

DateTimeImpl::DateTimeImpl(std::chrono::time_point<date::utc_clock> timepoint) : timepoint(timepoint) {
}

DateTimeImpl::DateTimeImpl(const std::string& timepoint_str) {
    this->from_rfc3339(timepoint_str);
}

std::string DateTimeImpl::to_rfc3339() const {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(this->timepoint));
}

void DateTimeImpl::from_rfc3339(const std::string& timepoint_str) {
    std::istringstream in{timepoint_str};
    in >> date::parse("%FT%T%Ez", this->timepoint);
    if (in.fail()) {
        in.clear();
        in.seekg(0);
        in >> date::parse("%FT%TZ", this->timepoint);
        if (in.fail()) {
            in.clear();
            in.seekg(0);
            in >> date::parse("%FT%T", this->timepoint);
            if (in.fail()) {
                EVLOG_error << "timepoint string parsing failed";
            }
        }
    }
}

std::chrono::time_point<date::utc_clock> DateTimeImpl::to_time_point() {
    return this->timepoint;
}

std::ostream& operator<<(std::ostream& os, const DateTimeImpl& dt) {
    os << dt.to_rfc3339();
    return os;
}

} // namespace ocpp1_6
