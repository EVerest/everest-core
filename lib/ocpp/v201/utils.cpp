// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <ocpp/common/utils.hpp>
#include <ocpp/v201/utils.hpp>

namespace ocpp {
namespace v201 {
namespace utils {

std::vector<MeasurandEnum> get_measurands_vec(const std::string& measurands_csv) {
    std::vector<MeasurandEnum> measurands;
    std::vector<std::string> measurands_strings = ocpp::get_vector_from_csv(measurands_csv);

    for (const auto& measurand_string : measurands_strings) {
        try {
            measurands.push_back(conversions::string_to_measurand_enum(measurand_string));
        } catch (std::out_of_range& e) {
            EVLOG_warning << "Could not convert string: " << measurand_string << " to MeasurandEnum";
        }
    }
    return measurands;
}

MeterValue get_meter_value_with_measurands_applied(const MeterValue& _meter_value,
                                                   const std::vector<MeasurandEnum>& measurands) {
    auto meter_value = _meter_value;
    for (auto it = meter_value.sampledValue.begin(); it != meter_value.sampledValue.end();) {
        if (it->measurand.has_value()) {
            if (std::find(measurands.begin(), measurands.end(), it->measurand.value()) == measurands.end()) {
                it = meter_value.sampledValue.erase(it);
            } else {
                ++it;
            }
        } else {
            it = meter_value.sampledValue.erase(it);
        }
    }
    return meter_value;
}

std::vector<MeterValue>
get_meter_values_with_measurands_and_interval_applied(const std::vector<MeterValue>& _meter_values,
                                                      const std::vector<MeasurandEnum>& aligned_measurands,
                                                      const std::vector<MeasurandEnum>& sample_measurands,
                                                      const int32_t aligned_interval, const int32_t sampled_interval) {
    std::vector<MeterValue> meter_values;

    if (_meter_values.empty()) {
        return meter_values;
    }

    auto next_aligned_timepoint = _meter_values.at(0).timestamp.to_time_point();
    auto next_sampled_timepoint = _meter_values.at(0).timestamp.to_time_point();

    // this assumes that all sampledValues of a MeterValue have the same ReadingContext, which is a
    // valid assumption but the schema definitions allow different ReadingContext(s) for SampledValues inside a
    // MeterValue
    for (const auto& meter_value : _meter_values) {
        if (!meter_value.sampledValue.empty() and meter_value.sampledValue.at(0).context.has_value()) {
            if ((meter_value.sampledValue.at(0).context.value() == ReadingContextEnum::Transaction_Begin or
                 meter_value.sampledValue.at(0).context.value() == ReadingContextEnum::Transaction_End) or
                meter_value.sampledValue.at(0).context.value() == ReadingContextEnum::Interruption_Begin or
                meter_value.sampledValue.at(0).context.value() == ReadingContextEnum::Interruption_End) {
                meter_values.push_back(get_meter_value_with_measurands_applied(meter_value, sample_measurands));
            }
            // ReadingContext is Sample_Clock so aligned_interval applies
            else if (aligned_interval > 0 and
                     meter_value.sampledValue.at(0).context.value() == ReadingContextEnum::Sample_Clock) {
                if (meter_value.timestamp.to_time_point() > next_aligned_timepoint) {
                    meter_values.push_back(get_meter_value_with_measurands_applied(meter_value, aligned_measurands));
                    next_aligned_timepoint =
                        meter_value.timestamp.to_time_point() + std::chrono::seconds(aligned_interval);
                }
            }
            // ReadingContext is Sample_Periodic so sampled_interval applies
            else if (sampled_interval > 0 and
                     meter_value.sampledValue.at(0).context.value() == ReadingContextEnum::Sample_Periodic) {
                if (meter_value.timestamp.to_time_point() > next_sampled_timepoint) {
                    meter_values.push_back(get_meter_value_with_measurands_applied(meter_value, sample_measurands));
                    next_sampled_timepoint =
                        meter_value.timestamp.to_time_point() + std::chrono::seconds(sampled_interval);
                }
            } else {
                meter_values.push_back(meter_value);
            }
        }
    }
    return meter_values;
}

TriggerReasonEnum stop_reason_to_trigger_reason_enum(const ReasonEnum& stop_reason) {
    switch (stop_reason) {
    case ReasonEnum::DeAuthorized:
        return TriggerReasonEnum::Deauthorized;
    case ReasonEnum::EmergencyStop:
        return TriggerReasonEnum::AbnormalCondition;
    case ReasonEnum::EnergyLimitReached:
        return TriggerReasonEnum::EnergyLimitReached;
    case ReasonEnum::EVDisconnected:
        return TriggerReasonEnum::EVCommunicationLost;
    case ReasonEnum::GroundFault:
        return TriggerReasonEnum::AbnormalCondition;
    case ReasonEnum::ImmediateReset:
        return TriggerReasonEnum::ResetCommand;
    case ReasonEnum::Local:
        return TriggerReasonEnum::StopAuthorized;
    case ReasonEnum::LocalOutOfCredit:
        return TriggerReasonEnum::ChargingStateChanged;
    case ReasonEnum::MasterPass:
        return TriggerReasonEnum::StopAuthorized;
    case ReasonEnum::Other:
        return TriggerReasonEnum::AbnormalCondition;
    case ReasonEnum::OvercurrentFault:
        return TriggerReasonEnum::AbnormalCondition;
    case ReasonEnum::PowerLoss:
        return TriggerReasonEnum::AbnormalCondition;
    case ReasonEnum::PowerQuality:
        return TriggerReasonEnum::AbnormalCondition;
    case ReasonEnum::Reboot:
        return TriggerReasonEnum::ResetCommand;
    case ReasonEnum::Remote:
        return TriggerReasonEnum::RemoteStop;
    case ReasonEnum::SOCLimitReached:
        return TriggerReasonEnum::EnergyLimitReached;
    case ReasonEnum::StoppedByEV:
        return TriggerReasonEnum::ChargingStateChanged;
    case ReasonEnum::TimeLimitReached:
        return TriggerReasonEnum::TimeLimitReached;
    case ReasonEnum::Timeout:
        return TriggerReasonEnum::EVConnectTimeout;
    default:
        return TriggerReasonEnum::AbnormalCondition;
    }
}

std::string sha256(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string generate_token_hash(const IdToken& token) {
    return sha256(conversions::id_token_enum_to_string(token.type) + token.idToken.get());
}

} // namespace utils
} // namespace v201
} // namespace ocpp
