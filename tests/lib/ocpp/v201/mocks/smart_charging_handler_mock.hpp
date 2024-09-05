// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include "gmock/gmock.h"
#include <cstdint>
#include <vector>

#include "ocpp/v201/messages/SetChargingProfile.hpp"
#include "ocpp/v201/smart_charging.hpp"

namespace ocpp::v201 {
class SmartChargingHandlerMock : public SmartChargingHandlerInterface {
public:
    MOCK_METHOD(SetChargingProfileResponse, validate_and_add_profile, (ChargingProfile & profile, int32_t evse_id));
    MOCK_METHOD(ProfileValidationResultEnum, validate_profile, (ChargingProfile & profile, int32_t evse_id));
    MOCK_METHOD(SetChargingProfileResponse, add_profile, (ChargingProfile & profile, int32_t evse_id));
    MOCK_METHOD(ClearChargingProfileResponse, clear_profiles, (const ClearChargingProfileRequest& request), (override));
    MOCK_METHOD(std::vector<ReportedChargingProfile>, get_reported_profiles,
                (const GetChargingProfilesRequest& request), (const, override));
    MOCK_METHOD(std::vector<ChargingProfile>, get_valid_profiles, (int32_t evse_id));
    MOCK_METHOD(CompositeSchedule, calculate_composite_schedule,
                (std::vector<ChargingProfile> & valid_profiles, const ocpp::DateTime& start_time,
                 const ocpp::DateTime& end_time, const int32_t evse_id,
                 std::optional<ChargingRateUnitEnum> charging_rate_unit));
};
} // namespace ocpp::v201
