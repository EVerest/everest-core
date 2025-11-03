// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "profile_tests_common.hpp"
#include "ocpp/common/types.hpp"
#include "ocpp/v16/ocpp_enums.hpp"

// ----------------------------------------------------------------------------
// helper functions

namespace ocpp::v16 {
using json = nlohmann::json;

std::ostream& operator<<(std::ostream& os, const std::vector<ChargingProfile>& profiles) {
    if (profiles.size() > 0) {
        std::uint32_t count = 0;
        for (const auto& i : profiles) {
            os << "[" << count++ << "] " << i;
        }
    } else {
        os << "<no profiles>";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<ChargingSchedulePeriod>& profiles) {
    if (profiles.size() > 0) {
        std::uint32_t count = 0;
        for (const auto& i : profiles) {
            json j;
            to_json(j, i);
            os << "[" << count++ << "] " << j << std::endl;
        }
    } else {
        os << "<no profiles>";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<EnhancedChargingSchedulePeriod>& profiles) {
    if (profiles.size() > 0) {
        std::uint32_t count = 0;
        for (const auto& i : profiles) {
            json j;
            to_json(j, i);
            os << "[" << count++ << "] " << j << std::endl;
        }
    } else {
        os << "<no profiles>";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const EnhancedChargingSchedule& schedule) {
    json j;
    to_json(j, schedule);
    os << j;
    return os;
}

bool operator==(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b) {
    auto diff = std::abs(a.startPeriod - b.startPeriod);
    bool bRes = diff < 10; // allow for a small difference
    bRes = bRes && (a.limit == b.limit);
    bRes = bRes && optional_equal(a.numberPhases, b.numberPhases);
    return bRes;
}

bool operator==(const ChargingSchedule& a, const ChargingSchedule& b) {
    bool bRes = true;
    auto min = std::min(a.chargingSchedulePeriod.size(), b.chargingSchedulePeriod.size());
    EXPECT_GT(min, 0);
    for (std::uint32_t i = 0; bRes && i < min; i++) {
        SCOPED_TRACE(std::string("i=") + std::to_string(i));
        bRes = bRes && a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i];
        EXPECT_EQ(a.chargingSchedulePeriod[i], b.chargingSchedulePeriod[i]);
    }
    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit) && optional_equal(a.minChargingRate, b.minChargingRate);
    EXPECT_EQ(a.chargingRateUnit, b.chargingRateUnit);
    if (a.minChargingRate.has_value() && b.minChargingRate.has_value()) {
        EXPECT_EQ(a.minChargingRate.value(), b.minChargingRate.value());
    }
    bRes = bRes && optional_equal(a.startSchedule, b.startSchedule) && optional_equal(a.duration, b.duration);
    if (a.startSchedule.has_value() && b.startSchedule.has_value()) {
        EXPECT_EQ(floor_seconds(a.startSchedule.value()), floor_seconds(b.startSchedule.value()));
    }
    if (a.duration.has_value() && b.duration.has_value()) {
        EXPECT_EQ(a.duration.value(), b.duration.value());
    }
    return bRes;
}

bool operator==(const ChargingSchedulePeriod& a, const EnhancedChargingSchedulePeriod& b) {
    auto diff = std::abs(a.startPeriod - b.startPeriod);
    bool bRes = diff < 10; // allow for a small difference
    bRes = bRes && (a.limit == b.limit);
    bRes = bRes && optional_equal(a.numberPhases, b.numberPhases);
    // b.stackLevel ignored
    return bRes;
}

bool operator==(const EnhancedChargingSchedulePeriod& a, const EnhancedChargingSchedulePeriod& b) {
    bool bRes = a.startPeriod == b.startPeriod;
    bRes = bRes && (a.limit == b.limit);
    bRes = bRes && (a.stackLevel == b.stackLevel);
    bRes = bRes && (a.numberPhases.value_or(-1) == b.numberPhases.value_or(-1));
    return bRes;
}

bool operator==(const ChargingSchedule& a, const EnhancedChargingSchedule& b) {
    bool bRes = true;
    auto min = std::min(a.chargingSchedulePeriod.size(), b.chargingSchedulePeriod.size());
    EXPECT_GT(min, 0);
    for (std::uint32_t i = 0; bRes && i < min; i++) {
        SCOPED_TRACE(std::string("i=") + std::to_string(i));
        bRes = bRes && a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i];
        EXPECT_EQ(a.chargingSchedulePeriod[i], b.chargingSchedulePeriod[i]);
    }
    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit) && optional_equal(a.minChargingRate, b.minChargingRate);
    EXPECT_EQ(a.chargingRateUnit, b.chargingRateUnit);
    if (a.minChargingRate.has_value() && b.minChargingRate.has_value()) {
        EXPECT_EQ(a.minChargingRate.value(), b.minChargingRate.value());
    }
    bRes = bRes && optional_equal(a.startSchedule, b.startSchedule) && optional_equal(a.duration, b.duration);
    if (a.startSchedule.has_value() && b.startSchedule.has_value()) {
        EXPECT_EQ(floor_seconds(a.startSchedule.value()), floor_seconds(b.startSchedule.value()));
    }
    if (a.duration.has_value() && b.duration.has_value()) {
        EXPECT_EQ(a.duration.value(), b.duration.value());
    }
    return bRes;
}

bool operator==(const EnhancedChargingSchedule& a, const EnhancedChargingSchedule& b) {
    const DateTime opt("1970-01-01T00:00:00Z");
    bool bRes = a.chargingSchedulePeriod.size() == b.chargingSchedulePeriod.size();
    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit);
    if (bRes) {
        for (std::uint8_t i = 0; i < a.chargingSchedulePeriod.size(); i++) {
            bRes = bRes && (a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i]);
        }
    }
    bRes = bRes && (a.duration.value_or(-1) == b.duration.value_or(-1));
    bRes = bRes && (floor_seconds(a.startSchedule.value_or(opt)) == floor_seconds(b.startSchedule.value_or(opt)));
    bRes = bRes && (a.minChargingRate.value_or(-1.0) == b.minChargingRate.value_or(-1.0));
    return bRes;
}

bool operator==(const ChargingProfile& a, const ChargingProfile& b) {
    bool bRes = (a.chargingProfileId == b.chargingProfileId) && (a.stackLevel == b.stackLevel) &&
                (a.chargingProfilePurpose == b.chargingProfilePurpose) &&
                (a.chargingProfileKind == b.chargingProfileKind) && (a.chargingSchedule == b.chargingSchedule);
    bRes = bRes && optional_equal(a.transactionId, b.transactionId) &&
           optional_equal(a.recurrencyKind, b.recurrencyKind) && optional_equal(a.validFrom, b.validFrom) &&
           optional_equal(a.validTo, b.validTo);
    return bRes;
}

bool nearly_equal(const ocpp::DateTime& a, const ocpp::DateTime& b) {
    const auto difference = std::chrono::duration_cast<std::chrono::seconds>(a.to_time_point() - b.to_time_point());
    // allow +- 1 second to be considered equal
    const bool result = std::abs(difference.count()) <= 1;
    if (!result) {
        std::cerr << "nearly_equal (ocpp::DateTime)\n\tA: " << a << "\n\tB: " << b << std::endl;
    }
    return result;
}

bool operator==(const period_entry_t& a, const period_entry_t& b) {
    bool bRes = (a.start == b.start) && (a.end == b.end) && (a.limit == b.limit) && (a.stack_level == b.stack_level) &&
                (a.charging_rate_unit == b.charging_rate_unit);
    if (a.number_phases && b.number_phases) {
        bRes = bRes && a.number_phases.value() == b.number_phases.value();
    }
    if (a.min_charging_rate && b.min_charging_rate) {
        bRes = bRes && a.min_charging_rate.value() == b.min_charging_rate.value();
    }
    return bRes;
}

bool operator==(const std::vector<period_entry_t>& a, const std::vector<period_entry_t>& b) {
    bool bRes = a.size() == b.size();
    if (bRes) {
        for (std::uint8_t i = 0; i < a.size(); i++) {
            bRes = a[i] == b[i];
            if (!bRes) {
                break;
            }
        }
    }
    return bRes;
}

bool validate_profile_result(const std::vector<period_entry_t>& result) {
    bool bRes{true};
    DateTime last{"1900-01-01T00:00:00Z"};
    for (const auto& i : result) {
        // ensure no overlaps
        bRes = i.start < i.end;
        bRes = bRes && i.start >= last;
        last = i.end;
        if (!bRes) {
            break;
        }
    }
    return bRes;
}

std::ostream& operator<<(std::ostream& os, const period_entry_t& entry) {
    os << entry.start << " " << entry.end << " S:" << entry.stack_level << " " << entry.limit
       << ((entry.charging_rate_unit == ChargingRateUnit::A) ? "A" : "W");
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<period_entry_t>& entries) {
    for (const auto& i : entries) {
        os << i << std::endl;
    }
    return os;
}

} // namespace ocpp::v16
