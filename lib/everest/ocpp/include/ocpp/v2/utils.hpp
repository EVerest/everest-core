// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef V2_UTILS_HPP
#define V2_UTILS_HPP

#include <ocpp/v2/ocpp_types.hpp>
#include <ocpp/v2/types.hpp>
namespace ocpp {
namespace v2 {
namespace utils {

/// \brief This function returns the configured Measurand as an std::vector
/// \brief std::vector<MeasurandEnum> of the configured AlignedDataMeasurands
std::vector<MeasurandEnum> get_measurands_vec(const std::string& measurands_csv);

/// \brief This function determines if any of the \p measurands is present in the \p _meter_value at all
/// \return True if any measurand is found, false otherwise
bool meter_value_has_any_measurand(const MeterValue& _meter_value, const std::vector<MeasurandEnum>& measurands);

/// \brief Applies the given \p measurands to the given \p _meter_value . The returned meter value will only contain
/// SampledValues which measurand is listed in the given \param measurands . If no measurand is set for the
/// SampledValue, the SampledValue will also be omitted.
/// \param _meter_value the meter value to be filtered
/// \param measurands applied measurands
/// \param include_signed if signed meter values should be included or not
/// \return filtered meter value
MeterValue get_meter_value_with_measurands_applied(const MeterValue& _meter_value,
                                                   const std::vector<MeasurandEnum>& measurands,
                                                   bool include_signed = true);

/// \brief Applies the given measurands to \p meter_values based on their ReadingContext.
/// Transaction_Begin, Interruption_Begin, Transaction_End, Interruption_End and Sample_Periodic will be filtered using
/// \p sampled_tx_ended_measurands.
/// Sample_Clock will be filtered using \p aligned_tx_ended_measurands
/// Any metervalue after \p max_timestamp will also be removed.
/// \p include_sampled_signed if a sampled signed meter values should be included or not
/// \p include_aligned_signed if a sampled aligned meter values should be included or not
/// \retval filtered meter values
std::vector<MeterValue> get_meter_values_with_measurands_applied(
    const std::vector<MeterValue>& meter_values, const std::vector<MeasurandEnum>& sampled_tx_ended_measurands,
    const std::vector<MeasurandEnum>& aligned_tx_ended_measurands, ocpp::DateTime max_timestamp,
    bool include_sampled_signed = true, bool include_aligned_signed = true);

///
/// \brief Set reading context of metervalue sampled values.
/// \param meter_value      The meter value to set context on
/// \param reading_context  Reading context to set.
/// \return The metervalue with the reading context
///
MeterValue set_meter_value_reading_context(const MeterValue& meter_value, const ReadingContextEnum reading_context);

/// \brief Returns the given \p str hashed using SHA256
/// \param str
/// \return
std::string sha256(const std::string& str);

/// \brief Return a SHA256 hash generated from a combination of the \p token type and id
/// \param token the token to generate the hash for
/// \return A SHA256 hash string
std::string generate_token_hash(const IdToken& token);

/// \brief Align the clock aligned timestamps to the interval values
/// \param timestamp the timestamp to align
/// \param align_interval the clock aligned interval to align to since midnight 00:00
/// \return DateTime type timestamp
ocpp::DateTime align_timestamp(const DateTime timestamp, std::chrono::seconds align_interval);

/// \brief Returns the total Power_Active_Import value from the \p meter_value or std::nullopt if it is not present
std::optional<float> get_total_power_active_import(const MeterValue& meter_value);

/// \brief Determines if a given \p security_event is critical as defined in the OCPP 2.0.1 appendix
bool is_critical(const std::string& security_event);

/// \brief Converts the given comma separated purposes into a vector of charging profile purposes.
std::vector<ChargingProfilePurposeEnum> get_charging_profile_purposes(const std::string& csl);

/// \brief Converts the given \p csl of ChargingProfilePurpose strings into a std::vector<ChargingProfilePurposeEnum>
std::vector<ChargingProfilePurposeEnum> get_purposes_to_ignore(const std::string& csl, const bool is_offline);

/// \brief Converts the given \p csl of OcppProtocolVersion strings into a std::vector<OcppProtocolVersion>
std::vector<OcppProtocolVersion> get_ocpp_protocol_versions(const std::string& csl);

} // namespace utils
} // namespace v2
} // namespace ocpp

#endif
