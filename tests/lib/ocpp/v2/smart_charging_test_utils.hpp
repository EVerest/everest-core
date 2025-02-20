// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "everest/logging.hpp"
#include "ocpp/v2/ocpp_types.hpp"
#include "ocpp/v2/profile.hpp"
#include "ocpp/v2/utils.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>
#include <vector>

namespace ocpp::v2 {

static const std::string BASE_JSON_PATH = std::string(TEST_PROFILES_LOCATION_V2) + "/json";

inline bool operator==(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b) {
    auto diff = std::abs(a.startPeriod - b.startPeriod);
    bool bRes = diff < 10; // allow for a small difference
    bRes = bRes && (a.limit == b.limit);
    bRes = bRes && (a.numberPhases == b.numberPhases);
    bRes = bRes && (a.phaseToUse == b.phaseToUse);
    return bRes;
}

inline bool operator!=(const ChargingSchedulePeriod& a, const ChargingSchedulePeriod& b) {
    return (!(a == b));
}

inline bool operator==(const CompositeSchedule& a, const CompositeSchedule& b) {
    bool bRes = true;

    if (a.chargingSchedulePeriod.size() != b.chargingSchedulePeriod.size()) {
        return false;
    }

    for (std::uint32_t i = 0; bRes && i < a.chargingSchedulePeriod.size(); i++) {
        bRes = a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i];
    }

    bRes = bRes && (a.evseId == b.evseId);
    bRes = bRes && (a.duration == b.duration);
    bRes = bRes && (a.scheduleStart == b.scheduleStart);
    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit);

    return bRes;
}

inline bool operator!=(const CompositeSchedule& a, const CompositeSchedule& b) {
    return (!(a == b));
}

inline bool operator==(const ChargingSchedule& a, const ChargingSchedule& b) {
    bool bRes = true;

    if (a.chargingSchedulePeriod.size() != b.chargingSchedulePeriod.size()) {
        return false;
    }

    for (std::uint32_t i = 0; bRes && i < a.chargingSchedulePeriod.size(); i++) {
        bRes = a.chargingSchedulePeriod[i] == b.chargingSchedulePeriod[i];
    }

    bRes = bRes && (a.chargingRateUnit == b.chargingRateUnit);
    bRes = bRes && (a.startSchedule == b.startSchedule);
    bRes = bRes && (a.duration == b.duration);
    bRes = bRes && (a.minChargingRate == b.minChargingRate);

    return bRes;
}

inline bool operator!=(const ChargingSchedule& a, const ChargingSchedule& b) {
    return !(a == b);
}

inline bool operator==(const period_entry_t& a, const period_entry_t& b) {
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

inline bool operator!=(const period_entry_t& a, const period_entry_t& b) {
    return !(a == b);
}

inline bool operator==(const std::vector<period_entry_t>& a, const std::vector<period_entry_t>& b) {
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

inline std::string to_string(const period_entry_t& entry) {
    std::string result = "Period Entry: {";
    result += "Start: " + entry.start.to_rfc3339() + ", ";
    result += "End: " + entry.end.to_rfc3339() + ", ";
    result += "Limit: " + std::to_string(entry.limit) + ", ";
    if (entry.number_phases.has_value()) {
        result += "Number of Phases: " + std::to_string(entry.number_phases.value()) + ", ";
    }
    result += "Stack Level: " + std::to_string(entry.stack_level) + ", ";
    result += "ChargingRateUnit:" + conversions::charging_rate_unit_enum_to_string(entry.charging_rate_unit);

    if (entry.min_charging_rate.has_value()) {
        result += ", Min Charging Rate: " + std::to_string(entry.min_charging_rate.value());
    }

    result += "}";
    return result;
}

inline std::ostream& operator<<(std::ostream& os, const period_entry_t& entry) {
    os << to_string(entry);
    return os;
}

static ocpp::DateTime dt(const std::string& dt_string) {
    ocpp::DateTime dt;

    if (dt_string.length() == 4) {
        dt = ocpp::DateTime("2024-01-01T0" + dt_string + ":00Z");
    } else if (dt_string.length() == 5) {
        dt = ocpp::DateTime("2024-01-01T" + dt_string + ":00Z");
    } else if (dt_string.length() == 7) {
        dt = ocpp::DateTime("2024-01-0" + dt_string + ":00Z");
    } else if (dt_string.length() == 8) {
        dt = ocpp::DateTime("2024-01-" + dt_string + ":00Z");
    } else if (dt_string.length() == 11) {
        dt = ocpp::DateTime("2024-" + dt_string + ":00Z");
    } else if (dt_string.length() == 16) {
        dt = ocpp::DateTime(dt_string + ":00Z");
    }

    return dt;
}

class SmartChargingTestUtils {
public:
    static std::vector<ChargingProfile> get_charging_profiles_from_directory(const std::string& path) {
        EVLOG_debug << "get_charging_profiles_from_directory: " << path;
        std::vector<ChargingProfile> profiles;
        for (const auto& entry : fs::directory_iterator(path)) {
            if (!entry.is_directory()) {
                fs::path path = entry.path();
                if (path.extension() == ".json") {
                    ChargingProfile profile = get_charging_profile_from_path(path);
                    std::cout << path << std::endl;
                    profiles.push_back(profile);
                }
            }
        }

        // Sort profiles by id in ascending order
        std::sort(profiles.begin(), profiles.end(),
                  [](const ChargingProfile& a, const ChargingProfile& b) { return a.id < b.id; });

        EVLOG_debug << "get_charging_profiles_from_directory END";
        return profiles;
    }

    static ChargingProfile get_charging_profile_from_path(const std::string& path) {
        EVLOG_debug << "get_charging_profile_from_path: " << path;
        std::ifstream f(path.c_str());
        json data = json::parse(f);

        ChargingProfile cp;
        from_json(data, cp);
        return cp;
    }

    static ChargingProfile get_charging_profile_from_file(const std::string& filename) {
        const std::string full_path = BASE_JSON_PATH + "/" + filename;

        return get_charging_profile_from_path(full_path);
    }

    static std::vector<ChargingProfile> get_charging_profiles_from_file(const std::string& filename) {
        std::vector<ChargingProfile> profiles;
        profiles.push_back(get_charging_profile_from_file(filename));
        return profiles;
    }

    /// \brief Returns a vector of ChargingProfiles to be used as a baseline for testing core functionality
    /// of generating an EnhancedChargingSchedule.
    static std::vector<ChargingProfile> get_baseline_profile_vector() {
        return get_charging_profiles_from_directory(BASE_JSON_PATH + "/" + "baseline/");
    }

    static std::string to_string(std::vector<ChargingProfile>& profiles) {
        std::string s;
        json cp_json;
        for (auto& profile : profiles) {
            if (!s.empty())
                s += ", ";
            to_json(cp_json, profile);
            s += cp_json.dump(4);
        }

        return "[" + s + "]";
    }

    /// \brief Validates that there is no overlap in the submitted period_entry_t collection
    /// \param period_entry_t collection
    /// \note If there are any overlapping period_entry_t entries the function returns false
    static bool validate_profile_result(const std::vector<period_entry_t>& result) {
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
};

} // namespace ocpp::v2