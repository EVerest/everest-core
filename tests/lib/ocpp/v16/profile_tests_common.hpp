// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef PROFILE_TESTS_COMMON_HPP
#define PROFILE_TESTS_COMMON_HPP

#include <chrono>
#include <iostream>
#include <optional>
#include <vector>

#include <gtest/gtest.h>

#include "ocpp/common/types.hpp"
#include "ocpp/v16/ocpp_types.hpp"
#include "ocpp/v16/profile.hpp"
#include "ocpp/v16/types.hpp"

// ----------------------------------------------------------------------------
// helper functions
namespace ocpp {
inline bool operator==(const DateTime& a, const DateTime& b) {
    return a.to_time_point() == b.to_time_point();
}

inline bool operator==(const DateTime& a, const std::string& b) {
    return a == DateTime(b);
}

inline bool operator==(const DateTime& a, const char* b) {
    return a == DateTime(b);
}

inline ocpp::DateTime floor_seconds(const ocpp::DateTime& dt) {
    return ocpp::DateTime(std::chrono::floor<std::chrono::seconds>(dt.to_time_point()));
}
} // namespace ocpp

namespace ocpp::v16 {
using json = nlohmann::json;

template <typename A> bool optional_equal(const std::optional<A>& a, const std::optional<A>& b) {
    bool bRes{true};
    if (a.has_value() && b.has_value()) {
        bRes = a.value() == b.value();
    }
    return bRes;
}

inline bool optional_equal(const std::optional<DateTime>& a, const std::optional<DateTime>& b) {
    bool bRes{true};
    if (a.has_value() && b.has_value()) {
        bRes = floor_seconds(a.value()) == floor_seconds(b.value());
    }
    return bRes;
}

std::ostream& operator<<(std::ostream& os, const std::vector<ChargingProfile>& profiles);
std::ostream& operator<<(std::ostream& os, const std::vector<ChargingSchedulePeriod>& profiles);
std::ostream& operator<<(std::ostream& os, const std::vector<EnhancedChargingSchedulePeriod>& profiles);
std::ostream& operator<<(std::ostream& os, const EnhancedChargingSchedule& schedule);
bool operator==(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b);
bool operator==(const ChargingSchedule& a, const ChargingSchedule& b);
bool operator==(const ChargingSchedulePeriod& a, const EnhancedChargingSchedulePeriod& b);
bool operator==(const EnhancedChargingSchedulePeriod& a, const EnhancedChargingSchedulePeriod& b);
bool operator==(const ChargingSchedule& a, const EnhancedChargingSchedule& b);
bool operator==(const EnhancedChargingSchedule& a, const EnhancedChargingSchedule& b);
bool operator==(const ChargingProfile& a, const ChargingProfile& b);
bool nearly_equal(const ocpp::DateTime& a, const ocpp::DateTime& b);

bool operator==(const period_entry_t& a, const period_entry_t& b);
bool operator==(const std::vector<period_entry_t>& a, const std::vector<period_entry_t>& b);

bool validate_profile_result(const std::vector<period_entry_t>& result);
std::ostream& operator<<(std::ostream& os, const period_entry_t& entry);
std::ostream& operator<<(std::ostream& os, const std::vector<period_entry_t>& entries);

} // namespace ocpp::v16

#endif // PROFILE_TESTS_COMMON_HPP
