// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <algorithm>

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

bool meter_value_has_any_measurand(const MeterValue& _meter_value, const std::vector<MeasurandEnum>& measurands) {
    auto compare = [](const SampledValue& a, MeasurandEnum b) { return a.measurand == b; };

    return std::find_first_of(_meter_value.sampledValue.begin(), _meter_value.sampledValue.end(), measurands.begin(),
                              measurands.end(), compare) != _meter_value.sampledValue.end();
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
get_meter_values_with_measurands_applied(const std::vector<MeterValue>& meter_values,
                                         const std::vector<MeasurandEnum>& sampled_tx_ended_measurands,
                                         const std::vector<MeasurandEnum>& aligned_tx_ended_measurands) {
    std::vector<MeterValue> meter_values_result;

    for (const auto& meter_value : meter_values) {
        if (meter_value.sampledValue.empty() or !meter_value.sampledValue.at(0).context.has_value()) {
            continue;
        }

        switch (meter_value.sampledValue.at(0).context.value()) {
        case ReadingContextEnum::Transaction_Begin:
        case ReadingContextEnum::Interruption_Begin:
        case ReadingContextEnum::Transaction_End:
        case ReadingContextEnum::Interruption_End:
        case ReadingContextEnum::Sample_Periodic:
            if (meter_value_has_any_measurand(meter_value, sampled_tx_ended_measurands)) {
                meter_values_result.push_back(
                    get_meter_value_with_measurands_applied(meter_value, sampled_tx_ended_measurands));
            }
            break;

        case ReadingContextEnum::Sample_Clock:
            if (meter_value_has_any_measurand(meter_value, aligned_tx_ended_measurands)) {
                meter_values_result.push_back(
                    get_meter_value_with_measurands_applied(meter_value, aligned_tx_ended_measurands));
            }
            break;

        case ReadingContextEnum::Other:
        case ReadingContextEnum::Trigger:
            // Nothing to do for these
            break;
        }
    }

    return meter_values_result;
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
