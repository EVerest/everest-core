// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <gmock/gmock.h>

#include <ocpp/v2/functional_blocks/meter_values.hpp>

namespace ocpp::v2 {

class MeterValuesMock : public MeterValuesInterface {
public:
    MOCK_METHOD(void, handle_message, (const ocpp::EnhancedMessage<MessageType>& message), (override));
    MOCK_METHOD(void, update_aligned_data_interval, (), (override));
    MOCK_METHOD(void, on_meter_value, (const std::int32_t evse_id, const MeterValue& meter_value), (override));
    MOCK_METHOD(MeterValue, get_latest_meter_value_filtered,
                (const MeterValue& meter_value, ReadingContextEnum context,
                 const RequiredComponentVariable& component_variable),
                (override));
    MOCK_METHOD(void, meter_values_req,
                (const std::int32_t evse_id, const std::vector<MeterValue>& meter_values,
                 const bool initiated_by_trigger_message),
                (override));
};

} // namespace ocpp::v2
