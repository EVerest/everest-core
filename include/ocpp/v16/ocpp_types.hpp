// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_OCPP_TYPES_HPP
#define OCPP_V16_OCPP_TYPES_HPP

#include <string>

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

struct IdTagInfo {
    AuthorizationStatus status;
    std::optional<ocpp::DateTime> expiryDate;
    std::optional<CiString<20>> parentIdTag;
};
/// \brief Conversion from a given IdTagInfo \p k to a given json object \p j
void to_json(json& j, const IdTagInfo& k);

/// \brief Conversion from a given json object \p j to a given IdTagInfo \p k
void from_json(const json& j, IdTagInfo& k);

// \brief Writes the string representation of the given IdTagInfo \p k to the given output stream \p os
/// \returns an output stream with the IdTagInfo written to
std::ostream& operator<<(std::ostream& os, const IdTagInfo& k);

struct CertificateHashDataType {
    HashAlgorithmEnumType hashAlgorithm;
    CiString<128> issuerNameHash;
    CiString<128> issuerKeyHash;
    CiString<40> serialNumber;
};
/// \brief Conversion from a given CertificateHashDataType \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataType& k);

/// \brief Conversion from a given json object \p j to a given CertificateHashDataType \p k
void from_json(const json& j, CertificateHashDataType& k);

// \brief Writes the string representation of the given CertificateHashDataType \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataType written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataType& k);

struct ChargingSchedulePeriod {
    int32_t startPeriod;
    float limit;
    std::optional<int32_t> numberPhases;
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
    std::optional<int32_t> duration;
    std::optional<ocpp::DateTime> startSchedule;
    std::optional<float> minChargingRate;
};
/// \brief Conversion from a given ChargingSchedule \p k to a given json object \p j
void to_json(json& j, const ChargingSchedule& k);

/// \brief Conversion from a given json object \p j to a given ChargingSchedule \p k
void from_json(const json& j, ChargingSchedule& k);

// \brief Writes the string representation of the given ChargingSchedule \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedule written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedule& k);

struct KeyValue {
    CiString<50> key;
    bool readonly;
    std::optional<CiString<500>> value;
};
/// \brief Conversion from a given KeyValue \p k to a given json object \p j
void to_json(json& j, const KeyValue& k);

/// \brief Conversion from a given json object \p j to a given KeyValue \p k
void from_json(const json& j, KeyValue& k);

// \brief Writes the string representation of the given KeyValue \p k to the given output stream \p os
/// \returns an output stream with the KeyValue written to
std::ostream& operator<<(std::ostream& os, const KeyValue& k);

struct LogParametersType {
    CiString<512> remoteLocation;
    std::optional<ocpp::DateTime> oldestTimestamp;
    std::optional<ocpp::DateTime> latestTimestamp;
};
/// \brief Conversion from a given LogParametersType \p k to a given json object \p j
void to_json(json& j, const LogParametersType& k);

/// \brief Conversion from a given json object \p j to a given LogParametersType \p k
void from_json(const json& j, LogParametersType& k);

// \brief Writes the string representation of the given LogParametersType \p k to the given output stream \p os
/// \returns an output stream with the LogParametersType written to
std::ostream& operator<<(std::ostream& os, const LogParametersType& k);

struct SampledValue {
    std::string value;
    std::optional<ReadingContext> context;
    std::optional<ValueFormat> format;
    std::optional<Measurand> measurand;
    std::optional<Phase> phase;
    std::optional<Location> location;
    std::optional<UnitOfMeasure> unit;
};
/// \brief Conversion from a given SampledValue \p k to a given json object \p j
void to_json(json& j, const SampledValue& k);

/// \brief Conversion from a given json object \p j to a given SampledValue \p k
void from_json(const json& j, SampledValue& k);

// \brief Writes the string representation of the given SampledValue \p k to the given output stream \p os
/// \returns an output stream with the SampledValue written to
std::ostream& operator<<(std::ostream& os, const SampledValue& k);

struct MeterValue {
    ocpp::DateTime timestamp;
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
    std::optional<int32_t> transactionId;
    std::optional<RecurrencyKindType> recurrencyKind;
    std::optional<ocpp::DateTime> validFrom;
    std::optional<ocpp::DateTime> validTo;
};
/// \brief Conversion from a given ChargingProfile \p k to a given json object \p j
void to_json(json& j, const ChargingProfile& k);

/// \brief Conversion from a given json object \p j to a given ChargingProfile \p k
void from_json(const json& j, ChargingProfile& k);

// \brief Writes the string representation of the given ChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ChargingProfile& k);

struct LocalAuthorizationList {
    CiString<20> idTag;
    std::optional<IdTagInfo> idTagInfo;
};
/// \brief Conversion from a given LocalAuthorizationList \p k to a given json object \p j
void to_json(json& j, const LocalAuthorizationList& k);

/// \brief Conversion from a given json object \p j to a given LocalAuthorizationList \p k
void from_json(const json& j, LocalAuthorizationList& k);

// \brief Writes the string representation of the given LocalAuthorizationList \p k to the given output stream \p os
/// \returns an output stream with the LocalAuthorizationList written to
std::ostream& operator<<(std::ostream& os, const LocalAuthorizationList& k);

struct FirmwareType {
    CiString<512> location;
    CiString<5500> signingCertificate;
    CiString<800> signature;
    std::optional<ocpp::DateTime> retrieveDateTime;
    std::optional<ocpp::DateTime> installDateTime;
};
/// \brief Conversion from a given FirmwareType \p k to a given json object \p j
void to_json(json& j, const FirmwareType& k);

/// \brief Conversion from a given json object \p j to a given FirmwareType \p k
void from_json(const json& j, FirmwareType& k);

// \brief Writes the string representation of the given FirmwareType \p k to the given output stream \p os
/// \returns an output stream with the FirmwareType written to
std::ostream& operator<<(std::ostream& os, const FirmwareType& k);

struct TransactionData {
    ocpp::DateTime timestamp;
    std::vector<SampledValue> sampledValue;
};
/// \brief Conversion from a given TransactionData \p k to a given json object \p j
void to_json(json& j, const TransactionData& k);

/// \brief Conversion from a given json object \p j to a given TransactionData \p k
void from_json(const json& j, TransactionData& k);

// \brief Writes the string representation of the given TransactionData \p k to the given output stream \p os
/// \returns an output stream with the TransactionData written to
std::ostream& operator<<(std::ostream& os, const TransactionData& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_OCPP_TYPES_HPP
