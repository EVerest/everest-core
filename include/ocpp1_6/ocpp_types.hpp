// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_OCPP_TYPES_HPP
#define OCPP1_6_OCPP_TYPES_HPP

#include <string>

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

struct IdTagInfo {
    AuthorizationStatus status;
    boost::optional<DateTime> expiryDate;
    boost::optional<CiString20Type> parentIdTag;
};
/// \brief Conversion from a given IdTagInfo \p k to a given json object \p j
void to_json(json& j, const IdTagInfo& k);

/// \brief Conversion from a given json object \p j to a given IdTagInfo \p k
void from_json(const json& j, IdTagInfo& k);

// \brief Writes the string representation of the given IdTagInfo \p k to the given output stream \p os
/// \returns an output stream with the IdTagInfo written to
std::ostream& operator<<(std::ostream& os, const IdTagInfo& k);

struct ChargingSchedulePeriod {
    int32_t startPeriod;
    float limit;
    boost::optional<int32_t> numberPhases;
};
/// \brief Conversion from a given ChargingSchedulePeriod \p k to a given json object \p j
void to_json(json& j, const ChargingSchedulePeriod& k);

/// \brief Conversion from a given json object \p j to a given ChargingSchedulePeriod \p k
void from_json(const json& j, ChargingSchedulePeriod& k);

// \brief Writes the string representation of the given ChargingSchedulePeriod \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedulePeriod written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedulePeriod& k);

struct ChargingSchedule {
    ChargingRateUnit chargingRateUnit;
    std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
    boost::optional<int32_t> duration;
    boost::optional<DateTime> startSchedule;
    boost::optional<float> minChargingRate;
};
/// \brief Conversion from a given ChargingSchedule \p k to a given json object \p j
void to_json(json& j, const ChargingSchedule& k);

/// \brief Conversion from a given json object \p j to a given ChargingSchedule \p k
void from_json(const json& j, ChargingSchedule& k);

// \brief Writes the string representation of the given ChargingSchedule \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedule written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedule& k);

struct KeyValue {
    CiString50Type key;
    bool readonly;
    boost::optional<CiString500Type> value;
};
/// \brief Conversion from a given KeyValue \p k to a given json object \p j
void to_json(json& j, const KeyValue& k);

/// \brief Conversion from a given json object \p j to a given KeyValue \p k
void from_json(const json& j, KeyValue& k);

// \brief Writes the string representation of the given KeyValue \p k to the given output stream \p os
/// \returns an output stream with the KeyValue written to
std::ostream& operator<<(std::ostream& os, const KeyValue& k);

struct SampledValue {
    std::string value;
    boost::optional<ReadingContext> context;
    boost::optional<ValueFormat> format;
    boost::optional<Measurand> measurand;
    boost::optional<Phase> phase;
    boost::optional<Location> location;
    boost::optional<UnitOfMeasure> unit;
};
/// \brief Conversion from a given SampledValue \p k to a given json object \p j
void to_json(json& j, const SampledValue& k);

/// \brief Conversion from a given json object \p j to a given SampledValue \p k
void from_json(const json& j, SampledValue& k);

// \brief Writes the string representation of the given SampledValue \p k to the given output stream \p os
/// \returns an output stream with the SampledValue written to
std::ostream& operator<<(std::ostream& os, const SampledValue& k);

struct MeterValue {
    DateTime timestamp;
    std::vector<SampledValue> sampledValue;
};
/// \brief Conversion from a given MeterValue \p k to a given json object \p j
void to_json(json& j, const MeterValue& k);

/// \brief Conversion from a given json object \p j to a given MeterValue \p k
void from_json(const json& j, MeterValue& k);

// \brief Writes the string representation of the given MeterValue \p k to the given output stream \p os
/// \returns an output stream with the MeterValue written to
std::ostream& operator<<(std::ostream& os, const MeterValue& k);

struct ChargingProfile {
    int32_t chargingProfileId;
    int32_t stackLevel;
    ChargingProfilePurposeType chargingProfilePurpose;
    ChargingProfileKindType chargingProfileKind;
    ChargingSchedule chargingSchedule;
    boost::optional<int32_t> transactionId;
    boost::optional<RecurrencyKindType> recurrencyKind;
    boost::optional<DateTime> validFrom;
    boost::optional<DateTime> validTo;
};
/// \brief Conversion from a given ChargingProfile \p k to a given json object \p j
void to_json(json& j, const ChargingProfile& k);

/// \brief Conversion from a given json object \p j to a given ChargingProfile \p k
void from_json(const json& j, ChargingProfile& k);

// \brief Writes the string representation of the given ChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ChargingProfile& k);

struct LocalAuthorizationList {
    CiString20Type idTag;
    boost::optional<IdTagInfo> idTagInfo;
};
/// \brief Conversion from a given LocalAuthorizationList \p k to a given json object \p j
void to_json(json& j, const LocalAuthorizationList& k);

/// \brief Conversion from a given json object \p j to a given LocalAuthorizationList \p k
void from_json(const json& j, LocalAuthorizationList& k);

// \brief Writes the string representation of the given LocalAuthorizationList \p k to the given output stream \p os
/// \returns an output stream with the LocalAuthorizationList written to
std::ostream& operator<<(std::ostream& os, const LocalAuthorizationList& k);

struct TransactionData {
    DateTime timestamp;
    std::vector<SampledValue> sampledValue;
};
/// \brief Conversion from a given TransactionData \p k to a given json object \p j
void to_json(json& j, const TransactionData& k);

/// \brief Conversion from a given json object \p j to a given TransactionData \p k
void from_json(const json& j, TransactionData& k);

// \brief Writes the string representation of the given TransactionData \p k to the given output stream \p os
/// \returns an output stream with the TransactionData written to
std::ostream& operator<<(std::ostream& os, const TransactionData& k);

} // namespace ocpp1_6

#endif // OCPP1_6_OCPP_TYPES_HPP
