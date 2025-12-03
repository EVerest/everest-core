// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/functional_blocks/meter_values.hpp>

#include <ocpp/common/constants.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/message_dispatcher.hpp>
#include <ocpp/v2/messages/MeterValues.hpp>

ocpp::v2::MeterValues::MeterValues(const FunctionalBlockContext& functional_block_context) :
    context(functional_block_context) {
}

void ocpp::v2::MeterValues::handle_message(const ocpp::EnhancedMessage<MessageType>& message) {
    throw MessageTypeNotImplementedException(message.messageType);
}

void ocpp::v2::MeterValues::update_aligned_data_interval() {
    using namespace std::chrono_literals;

    auto interval = std::chrono::seconds(
        this->context.device_model.get_value<int>(ControllerComponentVariables::AlignedDataInterval));
    if (interval <= 0s) {
        this->aligned_meter_values_timer.stop();
        return;
    }

    this->aligned_meter_values_timer.interval_starting_from(
        [this, interval]() {
            // J01.FR.20 if AlignedDataSendDuringIdle is true and any transaction is active, don't send clock aligned
            // meter values
            if (this->context.device_model
                    .get_optional_value<bool>(ControllerComponentVariables::AlignedDataSendDuringIdle)
                    .value_or(false)) {
                for (auto const& evse : this->context.evse_manager) {
                    if (evse.has_active_transaction()) {
                        return;
                    }
                }
            }

            const bool align_timestamps =
                this->context.device_model
                    .get_optional_value<bool>(ControllerComponentVariables::RoundClockAlignedTimestamps)
                    .value_or(false);

            // send evseID = 0 values
            auto meter_value = get_latest_meter_value_filtered(this->aligned_data_evse0.retrieve_processed_values(),
                                                               ReadingContextEnum::Sample_Clock,
                                                               ControllerComponentVariables::AlignedDataMeasurands);

            if (!meter_value.sampledValue.empty()) {
                if (align_timestamps) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, interval);
                }
                this->meter_values_req(0, std::vector<ocpp::v2::MeterValue>(1, meter_value));
            }
            this->aligned_data_evse0.clear_values();

            for (auto& evse : this->context.evse_manager) {
                if (evse.has_active_transaction()) {
                    continue;
                }

                // this will apply configured measurands and possibly reduce the entries of sampledValue
                // according to the configuration
                auto meter_value =
                    get_latest_meter_value_filtered(evse.get_idle_meter_value(), ReadingContextEnum::Sample_Clock,
                                                    ControllerComponentVariables::AlignedDataMeasurands);

                if (align_timestamps) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, interval);
                }

                if (!meter_value.sampledValue.empty()) {
                    // J01.FR.14 this is the only case where we send a MeterValue.req
                    this->meter_values_req(evse.get_id(), std::vector<ocpp::v2::MeterValue>(1, meter_value));
                    // clear the values
                }
                evse.clear_idle_meter_values();
            }
        },
        interval, std::chrono::floor<date::days>(date::utc_clock::to_sys(date::utc_clock::now())));
}

void ocpp::v2::MeterValues::on_meter_value(const int32_t evse_id, const MeterValue& meter_value) {
    if (evse_id == 0) {
        // if evseId = 0 then store in the chargepoint metervalues
        this->aligned_data_evse0.set_values(meter_value);
    } else {
        this->context.evse_manager.get_evse(evse_id).on_meter_value(meter_value);
    }
}

ocpp::v2::MeterValue
ocpp::v2::MeterValues::get_latest_meter_value_filtered(const MeterValue& meter_value, ReadingContextEnum context,
                                                       const RequiredComponentVariable& component_variable) {
    auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(
        meter_value, utils::get_measurands_vec(this->context.device_model.get_value<std::string>(component_variable)));
    for (auto& sampled_value : filtered_meter_value.sampledValue) {
        sampled_value.context = context;
    }
    return filtered_meter_value;
}

void ocpp::v2::MeterValues::meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values,
                                             const bool initiated_by_trigger_message) {
    MeterValuesRequest req;
    req.evseId = evse_id;
    req.meterValue = meter_values;

    ocpp::Call<MeterValuesRequest> call(req);
    this->context.message_dispatcher.dispatch_call(call, initiated_by_trigger_message);
}
