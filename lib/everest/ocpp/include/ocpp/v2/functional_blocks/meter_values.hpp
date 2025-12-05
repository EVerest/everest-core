// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v2/message_handler.hpp>

#include <ocpp/common/aligned_timer.hpp>
#include <ocpp/v2/average_meter_values.hpp>

namespace ocpp::v2 {
struct FunctionalBlockContext;
struct RequiredComponentVariable;

class MeterValuesInterface : public MessageHandlerInterface {
public:
    virtual ~MeterValuesInterface() override = default;
    virtual void update_aligned_data_interval() = 0;
    /// \brief Event handler that should be called when a new meter value is present
    /// \param evse_id
    /// \param meter_value
    virtual void on_meter_value(const int32_t evse_id, const MeterValue& meter_value) = 0;
    virtual MeterValue get_latest_meter_value_filtered(const MeterValue& meter_value, ReadingContextEnum context,
                                                       const RequiredComponentVariable& component_variable) = 0;
    // Functional Block J: MeterValues
    virtual void meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values,
                                  const bool initiated_by_trigger_message = false) = 0;
};

class MeterValues : public MeterValuesInterface {
public:
    virtual ~MeterValues() override = default;
    explicit MeterValues(const FunctionalBlockContext& functional_block_context);
    void handle_message(const ocpp::EnhancedMessage<MessageType>& message) override;
    void update_aligned_data_interval() override;
    void on_meter_value(const int32_t evse_id, const MeterValue& meter_value) override;
    MeterValue get_latest_meter_value_filtered(const MeterValue& meter_value, ReadingContextEnum context,
                                               const RequiredComponentVariable& component_variable) override;

    void meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values,
                          const bool initiated_by_trigger_message = false) override;

private: // Members
    const FunctionalBlockContext& context;

    ClockAlignedTimer aligned_meter_values_timer;
    AverageMeterValues aligned_data_evse0; // represents evseId = 0 meter value
};
} // namespace ocpp::v2
