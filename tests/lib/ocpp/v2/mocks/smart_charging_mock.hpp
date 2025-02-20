// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "gmock/gmock.h"

#include <ocpp/v2/functional_blocks/smart_charging.hpp>
#include <ocpp/v2/messages/SetChargingProfile.hpp>

namespace ocpp::v2 {
class SmartChargingMock : public SmartChargingInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>& message));
    MOCK_METHOD(std::vector<CompositeSchedule>, get_all_composite_schedules,
                (const int32_t duration, const ChargingRateUnitEnum& unit));
    MOCK_METHOD(void, delete_transaction_tx_profiles, (const std::string& transaction_id));
    MOCK_METHOD(SetChargingProfileResponse, conform_validate_and_add_profile,
                (ChargingProfile & profile, int32_t evse_id, ChargingLimitSourceEnum charging_limit_source,
                 AddChargingProfileSource source_of_request));
    MOCK_METHOD(ProfileValidationResultEnum, conform_and_validate_profile,
                (ChargingProfile & profile, int32_t evse_id, AddChargingProfileSource source_of_request));
};
} // namespace ocpp::v2
