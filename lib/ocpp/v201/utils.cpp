// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <algorithm>
#include <openssl/evp.h>
#include <openssl/sha.h>

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
                                                   const std::vector<MeasurandEnum>& measurands, bool include_signed) {
    auto meter_value = _meter_value;
    for (auto it = meter_value.sampledValue.begin(); it != meter_value.sampledValue.end();) {
        auto measurand = it->measurand;
        if (measurand.has_value()) {
            if (std::find(measurands.begin(), measurands.end(), measurand.value()) == measurands.end()) {
                it = meter_value.sampledValue.erase(it);
            } else {
                if (not include_signed) {
                    it->signedMeterValue.reset();
                }
                ++it;
            }
        } else {
            it = meter_value.sampledValue.erase(it);
        }
    }

    return meter_value;
}

std::vector<MeterValue> get_meter_values_with_measurands_applied(
    const std::vector<MeterValue>& meter_values, const std::vector<MeasurandEnum>& sampled_tx_ended_measurands,
    const std::vector<MeasurandEnum>& aligned_tx_ended_measurands, ocpp::DateTime max_timestamp,
    bool include_sampled_signed, bool include_aligned_signed) {
    std::vector<MeterValue> meter_values_result;

    for (const auto& meter_value : meter_values) {
        if (meter_value.sampledValue.empty() or meter_value.timestamp > max_timestamp) {
            continue;
        }

        auto context = meter_value.sampledValue.at(0).context;
        if (!context.has_value()) {
            continue;
        }

        switch (context.value()) {
        case ReadingContextEnum::Transaction_Begin:
        case ReadingContextEnum::Interruption_Begin:
        case ReadingContextEnum::Transaction_End:
        case ReadingContextEnum::Interruption_End:
        case ReadingContextEnum::Sample_Periodic:
            if (meter_value_has_any_measurand(meter_value, sampled_tx_ended_measurands)) {
                meter_values_result.push_back(get_meter_value_with_measurands_applied(
                    meter_value, sampled_tx_ended_measurands, include_sampled_signed));
            }
            break;

        case ReadingContextEnum::Sample_Clock:
            if (meter_value_has_any_measurand(meter_value, aligned_tx_ended_measurands)) {
                meter_values_result.push_back(get_meter_value_with_measurands_applied(
                    meter_value, aligned_tx_ended_measurands, include_aligned_signed));
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

std::string sha256(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_Digest(str.c_str(), str.size(), hash, NULL, EVP_sha256(), NULL);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto& byte : hash) {
        ss << std::setw(2) << (int)byte;
    }
    return ss.str();
}

std::string generate_token_hash(const IdToken& token) {
    return sha256(conversions::id_token_enum_to_string(token.type) + token.idToken.get());
}

ocpp::DateTime align_timestamp(const DateTime timestamp, std::chrono::seconds align_interval) {
    if (align_interval.count() < 0) {
        EVLOG_warning << "Invalid align interval value";
        return timestamp;
    }

    auto timestamp_sys = date::utc_clock::to_sys(timestamp.to_time_point());
    // get the current midnight
    auto midnight = std::chrono::floor<date::days>(timestamp_sys);
    auto seconds_since_midnight = std::chrono::duration_cast<std::chrono::seconds>(timestamp_sys - midnight);
    auto rounded_seconds = ((seconds_since_midnight + align_interval / 2) / align_interval) * align_interval;
    auto rounded_time = ocpp::DateTime(date::utc_clock::from_sys(midnight + rounded_seconds));

    // Output the original and rounded timestamps
    EVLOG_debug << "Original Timestamp: " << timestamp.to_rfc3339() << std::endl;
    EVLOG_debug << "Interval: " << align_interval.count() << std::endl;
    EVLOG_debug << "Rounded Timestamp: " << rounded_time;

    return rounded_time;
}

std::optional<float> get_total_power_active_import(const MeterValue& meter_value) {
    for (const auto& sampled_value : meter_value.sampledValue) {
        if (sampled_value.measurand == MeasurandEnum::Power_Active_Import and !sampled_value.phase.has_value()) {
            return sampled_value.value;
        }
    }
    return std::nullopt;
}

} // namespace utils
} // namespace v201
} // namespace ocpp
