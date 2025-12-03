// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_OCPP_TYPES_HPP
#define OCPP_V2_OCPP_TYPES_HPP

#include <string>

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>

namespace ocpp {
namespace v2 {

using CustomData = nlohmann::json;

struct StatusInfo {
    CiString<20> reasonCode;
    std::optional<CiString<1024>> additionalInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given StatusInfo \p k to a given json object \p j
void to_json(json& j, const StatusInfo& k);

/// \brief Conversion from a given json object \p j to a given StatusInfo \p k
void from_json(const json& j, StatusInfo& k);

/// \brief Writes the string representation of the given StatusInfo \p k to the given output stream \p os
/// \returns an output stream with the StatusInfo written to
std::ostream& operator<<(std::ostream& os, const StatusInfo& k);

struct PeriodicEventStreamParams {
    std::optional<int32_t> interval;
    std::optional<int32_t> values;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given PeriodicEventStreamParams \p k to a given json object \p j
void to_json(json& j, const PeriodicEventStreamParams& k);

/// \brief Conversion from a given json object \p j to a given PeriodicEventStreamParams \p k
void from_json(const json& j, PeriodicEventStreamParams& k);

/// \brief Writes the string representation of the given PeriodicEventStreamParams \p k to the given output stream \p os
/// \returns an output stream with the PeriodicEventStreamParams written to
std::ostream& operator<<(std::ostream& os, const PeriodicEventStreamParams& k);

struct AdditionalInfo {
    CiString<255> additionalIdToken;
    CiString<50> type;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given AdditionalInfo \p k to a given json object \p j
void to_json(json& j, const AdditionalInfo& k);

/// \brief Conversion from a given json object \p j to a given AdditionalInfo \p k
void from_json(const json& j, AdditionalInfo& k);

/// \brief Writes the string representation of the given AdditionalInfo \p k to the given output stream \p os
/// \returns an output stream with the AdditionalInfo written to
std::ostream& operator<<(std::ostream& os, const AdditionalInfo& k);

struct IdToken {
    CiString<255> idToken;
    CiString<20> type;
    std::optional<std::vector<AdditionalInfo>> additionalInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given IdToken \p k to a given json object \p j
void to_json(json& j, const IdToken& k);

/// \brief Conversion from a given json object \p j to a given IdToken \p k
void from_json(const json& j, IdToken& k);

/// \brief Writes the string representation of the given IdToken \p k to the given output stream \p os
/// \returns an output stream with the IdToken written to
std::ostream& operator<<(std::ostream& os, const IdToken& k);

struct OCSPRequestData {
    HashAlgorithmEnum hashAlgorithm;
    CiString<128> issuerNameHash;
    CiString<128> issuerKeyHash;
    CiString<40> serialNumber;
    CiString<2000> responderURL;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given OCSPRequestData \p k to a given json object \p j
void to_json(json& j, const OCSPRequestData& k);

/// \brief Conversion from a given json object \p j to a given OCSPRequestData \p k
void from_json(const json& j, OCSPRequestData& k);

/// \brief Writes the string representation of the given OCSPRequestData \p k to the given output stream \p os
/// \returns an output stream with the OCSPRequestData written to
std::ostream& operator<<(std::ostream& os, const OCSPRequestData& k);

struct MessageContent {
    MessageFormatEnum format;
    CiString<1024> content;
    std::optional<CiString<8>> language;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given MessageContent \p k to a given json object \p j
void to_json(json& j, const MessageContent& k);

/// \brief Conversion from a given json object \p j to a given MessageContent \p k
void from_json(const json& j, MessageContent& k);

/// \brief Writes the string representation of the given MessageContent \p k to the given output stream \p os
/// \returns an output stream with the MessageContent written to
std::ostream& operator<<(std::ostream& os, const MessageContent& k);

struct IdTokenInfo {
    AuthorizationStatusEnum status;
    std::optional<ocpp::DateTime> cacheExpiryDateTime;
    std::optional<int32_t> chargingPriority;
    std::optional<IdToken> groupIdToken;
    std::optional<CiString<8>> language1;
    std::optional<CiString<8>> language2;
    std::optional<std::vector<int32_t>> evseId;
    std::optional<MessageContent> personalMessage;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given IdTokenInfo \p k to a given json object \p j
void to_json(json& j, const IdTokenInfo& k);

/// \brief Conversion from a given json object \p j to a given IdTokenInfo \p k
void from_json(const json& j, IdTokenInfo& k);

/// \brief Writes the string representation of the given IdTokenInfo \p k to the given output stream \p os
/// \returns an output stream with the IdTokenInfo written to
std::ostream& operator<<(std::ostream& os, const IdTokenInfo& k);

struct TariffConditions {
    std::optional<std::string> startTimeOfDay;
    std::optional<std::string> endTimeOfDay;
    std::optional<std::vector<DayOfWeekEnum>> dayOfWeek;
    std::optional<std::string> validFromDate;
    std::optional<std::string> validToDate;
    std::optional<EvseKindEnum> evseKind;
    std::optional<float> minEnergy;
    std::optional<float> maxEnergy;
    std::optional<float> minCurrent;
    std::optional<float> maxCurrent;
    std::optional<float> minPower;
    std::optional<float> maxPower;
    std::optional<int32_t> minTime;
    std::optional<int32_t> maxTime;
    std::optional<int32_t> minChargingTime;
    std::optional<int32_t> maxChargingTime;
    std::optional<int32_t> minIdleTime;
    std::optional<int32_t> maxIdleTime;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffConditions \p k to a given json object \p j
void to_json(json& j, const TariffConditions& k);

/// \brief Conversion from a given json object \p j to a given TariffConditions \p k
void from_json(const json& j, TariffConditions& k);

/// \brief Writes the string representation of the given TariffConditions \p k to the given output stream \p os
/// \returns an output stream with the TariffConditions written to
std::ostream& operator<<(std::ostream& os, const TariffConditions& k);

struct TariffEnergyPrice {
    float priceKwh;
    std::optional<TariffConditions> conditions;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffEnergyPrice \p k to a given json object \p j
void to_json(json& j, const TariffEnergyPrice& k);

/// \brief Conversion from a given json object \p j to a given TariffEnergyPrice \p k
void from_json(const json& j, TariffEnergyPrice& k);

/// \brief Writes the string representation of the given TariffEnergyPrice \p k to the given output stream \p os
/// \returns an output stream with the TariffEnergyPrice written to
std::ostream& operator<<(std::ostream& os, const TariffEnergyPrice& k);

struct TaxRate {
    CiString<20> type;
    float tax;
    std::optional<int32_t> stack;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TaxRate \p k to a given json object \p j
void to_json(json& j, const TaxRate& k);

/// \brief Conversion from a given json object \p j to a given TaxRate \p k
void from_json(const json& j, TaxRate& k);

/// \brief Writes the string representation of the given TaxRate \p k to the given output stream \p os
/// \returns an output stream with the TaxRate written to
std::ostream& operator<<(std::ostream& os, const TaxRate& k);

struct TariffEnergy {
    std::vector<TariffEnergyPrice> prices;
    std::optional<std::vector<TaxRate>> taxRates;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffEnergy \p k to a given json object \p j
void to_json(json& j, const TariffEnergy& k);

/// \brief Conversion from a given json object \p j to a given TariffEnergy \p k
void from_json(const json& j, TariffEnergy& k);

/// \brief Writes the string representation of the given TariffEnergy \p k to the given output stream \p os
/// \returns an output stream with the TariffEnergy written to
std::ostream& operator<<(std::ostream& os, const TariffEnergy& k);

struct TariffTimePrice {
    float priceMinute;
    std::optional<TariffConditions> conditions;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffTimePrice \p k to a given json object \p j
void to_json(json& j, const TariffTimePrice& k);

/// \brief Conversion from a given json object \p j to a given TariffTimePrice \p k
void from_json(const json& j, TariffTimePrice& k);

/// \brief Writes the string representation of the given TariffTimePrice \p k to the given output stream \p os
/// \returns an output stream with the TariffTimePrice written to
std::ostream& operator<<(std::ostream& os, const TariffTimePrice& k);

struct TariffTime {
    std::vector<TariffTimePrice> prices;
    std::optional<std::vector<TaxRate>> taxRates;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffTime \p k to a given json object \p j
void to_json(json& j, const TariffTime& k);

/// \brief Conversion from a given json object \p j to a given TariffTime \p k
void from_json(const json& j, TariffTime& k);

/// \brief Writes the string representation of the given TariffTime \p k to the given output stream \p os
/// \returns an output stream with the TariffTime written to
std::ostream& operator<<(std::ostream& os, const TariffTime& k);

struct TariffConditionsFixed {
    std::optional<std::string> startTimeOfDay;
    std::optional<std::string> endTimeOfDay;
    std::optional<std::vector<DayOfWeekEnum>> dayOfWeek;
    std::optional<std::string> validFromDate;
    std::optional<std::string> validToDate;
    std::optional<EvseKindEnum> evseKind;
    std::optional<CiString<20>> paymentBrand;
    std::optional<CiString<20>> paymentRecognition;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffConditionsFixed \p k to a given json object \p j
void to_json(json& j, const TariffConditionsFixed& k);

/// \brief Conversion from a given json object \p j to a given TariffConditionsFixed \p k
void from_json(const json& j, TariffConditionsFixed& k);

/// \brief Writes the string representation of the given TariffConditionsFixed \p k to the given output stream \p os
/// \returns an output stream with the TariffConditionsFixed written to
std::ostream& operator<<(std::ostream& os, const TariffConditionsFixed& k);

struct TariffFixedPrice {
    float priceFixed;
    std::optional<TariffConditionsFixed> conditions;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffFixedPrice \p k to a given json object \p j
void to_json(json& j, const TariffFixedPrice& k);

/// \brief Conversion from a given json object \p j to a given TariffFixedPrice \p k
void from_json(const json& j, TariffFixedPrice& k);

/// \brief Writes the string representation of the given TariffFixedPrice \p k to the given output stream \p os
/// \returns an output stream with the TariffFixedPrice written to
std::ostream& operator<<(std::ostream& os, const TariffFixedPrice& k);

struct TariffFixed {
    std::vector<TariffFixedPrice> prices;
    std::optional<std::vector<TaxRate>> taxRates;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffFixed \p k to a given json object \p j
void to_json(json& j, const TariffFixed& k);

/// \brief Conversion from a given json object \p j to a given TariffFixed \p k
void from_json(const json& j, TariffFixed& k);

/// \brief Writes the string representation of the given TariffFixed \p k to the given output stream \p os
/// \returns an output stream with the TariffFixed written to
std::ostream& operator<<(std::ostream& os, const TariffFixed& k);

struct Price {
    std::optional<float> exclTax;
    std::optional<float> inclTax;
    std::optional<std::vector<TaxRate>> taxRates;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Price \p k to a given json object \p j
void to_json(json& j, const Price& k);

/// \brief Conversion from a given json object \p j to a given Price \p k
void from_json(const json& j, Price& k);

/// \brief Writes the string representation of the given Price \p k to the given output stream \p os
/// \returns an output stream with the Price written to
std::ostream& operator<<(std::ostream& os, const Price& k);

struct Tariff {
    CiString<60> tariffId;
    CiString<3> currency;
    std::optional<std::vector<MessageContent>> description;
    std::optional<TariffEnergy> energy;
    std::optional<ocpp::DateTime> validFrom;
    std::optional<TariffTime> chargingTime;
    std::optional<TariffTime> idleTime;
    std::optional<TariffFixed> fixedFee;
    std::optional<TariffTime> reservationTime;
    std::optional<TariffFixed> reservationFixed;
    std::optional<Price> minCost;
    std::optional<Price> maxCost;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Tariff \p k to a given json object \p j
void to_json(json& j, const Tariff& k);

/// \brief Conversion from a given json object \p j to a given Tariff \p k
void from_json(const json& j, Tariff& k);

/// \brief Writes the string representation of the given Tariff \p k to the given output stream \p os
/// \returns an output stream with the Tariff written to
std::ostream& operator<<(std::ostream& os, const Tariff& k);

struct BatteryData {
    int32_t evseId;
    CiString<50> serialNumber;
    float soC;
    float soH;
    std::optional<ocpp::DateTime> productionDate;
    std::optional<CiString<500>> vendorInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given BatteryData \p k to a given json object \p j
void to_json(json& j, const BatteryData& k);

/// \brief Conversion from a given json object \p j to a given BatteryData \p k
void from_json(const json& j, BatteryData& k);

/// \brief Writes the string representation of the given BatteryData \p k to the given output stream \p os
/// \returns an output stream with the BatteryData written to
std::ostream& operator<<(std::ostream& os, const BatteryData& k);

struct Modem {
    std::optional<CiString<20>> iccid;
    std::optional<CiString<20>> imsi;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Modem \p k to a given json object \p j
void to_json(json& j, const Modem& k);

/// \brief Conversion from a given json object \p j to a given Modem \p k
void from_json(const json& j, Modem& k);

/// \brief Writes the string representation of the given Modem \p k to the given output stream \p os
/// \returns an output stream with the Modem written to
std::ostream& operator<<(std::ostream& os, const Modem& k);

struct ChargingStation {
    CiString<20> model;
    CiString<50> vendorName;
    std::optional<CiString<25>> serialNumber;
    std::optional<Modem> modem;
    std::optional<CiString<50>> firmwareVersion;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingStation \p k to a given json object \p j
void to_json(json& j, const ChargingStation& k);

/// \brief Conversion from a given json object \p j to a given ChargingStation \p k
void from_json(const json& j, ChargingStation& k);

/// \brief Writes the string representation of the given ChargingStation \p k to the given output stream \p os
/// \returns an output stream with the ChargingStation written to
std::ostream& operator<<(std::ostream& os, const ChargingStation& k);

struct EVSE {
    int32_t id;
    std::optional<int32_t> connectorId;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVSE \p k to a given json object \p j
void to_json(json& j, const EVSE& k);

/// \brief Conversion from a given json object \p j to a given EVSE \p k
void from_json(const json& j, EVSE& k);

/// \brief Writes the string representation of the given EVSE \p k to the given output stream \p os
/// \returns an output stream with the EVSE written to
std::ostream& operator<<(std::ostream& os, const EVSE& k);

struct ClearChargingProfile {
    std::optional<int32_t> evseId;
    std::optional<ChargingProfilePurposeEnum> chargingProfilePurpose;
    std::optional<int32_t> stackLevel;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ClearChargingProfile \p k to a given json object \p j
void to_json(json& j, const ClearChargingProfile& k);

/// \brief Conversion from a given json object \p j to a given ClearChargingProfile \p k
void from_json(const json& j, ClearChargingProfile& k);

/// \brief Writes the string representation of the given ClearChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ClearChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfile& k);

struct ClearTariffsResult {
    TariffClearStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CiString<60>> tariffId;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ClearTariffsResult \p k to a given json object \p j
void to_json(json& j, const ClearTariffsResult& k);

/// \brief Conversion from a given json object \p j to a given ClearTariffsResult \p k
void from_json(const json& j, ClearTariffsResult& k);

/// \brief Writes the string representation of the given ClearTariffsResult \p k to the given output stream \p os
/// \returns an output stream with the ClearTariffsResult written to
std::ostream& operator<<(std::ostream& os, const ClearTariffsResult& k);

struct ClearMonitoringResult {
    ClearMonitoringStatusEnum status;
    int32_t id;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ClearMonitoringResult \p k to a given json object \p j
void to_json(json& j, const ClearMonitoringResult& k);

/// \brief Conversion from a given json object \p j to a given ClearMonitoringResult \p k
void from_json(const json& j, ClearMonitoringResult& k);

/// \brief Writes the string representation of the given ClearMonitoringResult \p k to the given output stream \p os
/// \returns an output stream with the ClearMonitoringResult written to
std::ostream& operator<<(std::ostream& os, const ClearMonitoringResult& k);

struct CertificateHashDataType {
    HashAlgorithmEnum hashAlgorithm;
    CiString<128> issuerNameHash;
    CiString<128> issuerKeyHash;
    CiString<40> serialNumber;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CertificateHashDataType \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataType& k);

/// \brief Conversion from a given json object \p j to a given CertificateHashDataType \p k
void from_json(const json& j, CertificateHashDataType& k);

/// \brief Writes the string representation of the given CertificateHashDataType \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataType written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataType& k);

struct CertificateStatusRequestInfo {
    CertificateHashDataType certificateHashData;
    CertificateStatusSourceEnum source;
    std::vector<CiString<2000>> urls;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CertificateStatusRequestInfo \p k to a given json object \p j
void to_json(json& j, const CertificateStatusRequestInfo& k);

/// \brief Conversion from a given json object \p j to a given CertificateStatusRequestInfo \p k
void from_json(const json& j, CertificateStatusRequestInfo& k);

/// \brief Writes the string representation of the given CertificateStatusRequestInfo \p k to the given output stream \p
/// os \returns an output stream with the CertificateStatusRequestInfo written to
std::ostream& operator<<(std::ostream& os, const CertificateStatusRequestInfo& k);

struct CertificateStatus {
    CertificateHashDataType certificateHashData;
    CertificateStatusSourceEnum source;
    CertificateStatusEnum status;
    ocpp::DateTime nextUpdate;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CertificateStatus \p k to a given json object \p j
void to_json(json& j, const CertificateStatus& k);

/// \brief Conversion from a given json object \p j to a given CertificateStatus \p k
void from_json(const json& j, CertificateStatus& k);

/// \brief Writes the string representation of the given CertificateStatus \p k to the given output stream \p os
/// \returns an output stream with the CertificateStatus written to
std::ostream& operator<<(std::ostream& os, const CertificateStatus& k);

struct ChargingProfileCriterion {
    std::optional<ChargingProfilePurposeEnum> chargingProfilePurpose;
    std::optional<int32_t> stackLevel;
    std::optional<std::vector<int32_t>> chargingProfileId;
    std::optional<std::vector<CiString<20>>> chargingLimitSource;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingProfileCriterion \p k to a given json object \p j
void to_json(json& j, const ChargingProfileCriterion& k);

/// \brief Conversion from a given json object \p j to a given ChargingProfileCriterion \p k
void from_json(const json& j, ChargingProfileCriterion& k);

/// \brief Writes the string representation of the given ChargingProfileCriterion \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfileCriterion written to
std::ostream& operator<<(std::ostream& os, const ChargingProfileCriterion& k);

struct V2XFreqWattPoint {
    float frequency;
    float power;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given V2XFreqWattPoint \p k to a given json object \p j
void to_json(json& j, const V2XFreqWattPoint& k);

/// \brief Conversion from a given json object \p j to a given V2XFreqWattPoint \p k
void from_json(const json& j, V2XFreqWattPoint& k);

/// \brief Writes the string representation of the given V2XFreqWattPoint \p k to the given output stream \p os
/// \returns an output stream with the V2XFreqWattPoint written to
std::ostream& operator<<(std::ostream& os, const V2XFreqWattPoint& k);

struct V2XSignalWattPoint {
    int32_t signal;
    float power;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given V2XSignalWattPoint \p k to a given json object \p j
void to_json(json& j, const V2XSignalWattPoint& k);

/// \brief Conversion from a given json object \p j to a given V2XSignalWattPoint \p k
void from_json(const json& j, V2XSignalWattPoint& k);

/// \brief Writes the string representation of the given V2XSignalWattPoint \p k to the given output stream \p os
/// \returns an output stream with the V2XSignalWattPoint written to
std::ostream& operator<<(std::ostream& os, const V2XSignalWattPoint& k);

struct ChargingSchedulePeriod {
    int32_t startPeriod;
    std::optional<float> limit;
    std::optional<float> limit_L2;
    std::optional<float> limit_L3;
    std::optional<int32_t> numberPhases;
    std::optional<int32_t> phaseToUse;
    std::optional<float> dischargeLimit;
    std::optional<float> dischargeLimit_L2;
    std::optional<float> dischargeLimit_L3;
    std::optional<float> setpoint;
    std::optional<float> setpoint_L2;
    std::optional<float> setpoint_L3;
    std::optional<float> setpointReactive;
    std::optional<float> setpointReactive_L2;
    std::optional<float> setpointReactive_L3;
    std::optional<bool> preconditioningRequest;
    std::optional<bool> evseSleep;
    std::optional<float> v2xBaseline;
    std::optional<OperationModeEnum> operationMode;
    std::optional<std::vector<V2XFreqWattPoint>> v2xFreqWattCurve;
    std::optional<std::vector<V2XSignalWattPoint>> v2xSignalWattCurve;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingSchedulePeriod \p k to a given json object \p j
void to_json(json& j, const ChargingSchedulePeriod& k);

/// \brief Conversion from a given json object \p j to a given ChargingSchedulePeriod \p k
void from_json(const json& j, ChargingSchedulePeriod& k);

/// \brief Writes the string representation of the given ChargingSchedulePeriod \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedulePeriod written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedulePeriod& k);

struct CompositeSchedule {
    int32_t evseId;
    int32_t duration;
    ocpp::DateTime scheduleStart;
    ChargingRateUnitEnum chargingRateUnit;
    std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CompositeSchedule \p k to a given json object \p j
void to_json(json& j, const CompositeSchedule& k);

/// \brief Conversion from a given json object \p j to a given CompositeSchedule \p k
void from_json(const json& j, CompositeSchedule& k);

/// \brief Writes the string representation of the given CompositeSchedule \p k to the given output stream \p os
/// \returns an output stream with the CompositeSchedule written to
std::ostream& operator<<(std::ostream& os, const CompositeSchedule& k);

struct CertificateHashDataChain {
    CertificateHashDataType certificateHashData;
    GetCertificateIdUseEnum certificateType;
    std::optional<std::vector<CertificateHashDataType>> childCertificateHashData;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CertificateHashDataChain \p k to a given json object \p j
void to_json(json& j, const CertificateHashDataChain& k);

/// \brief Conversion from a given json object \p j to a given CertificateHashDataChain \p k
void from_json(const json& j, CertificateHashDataChain& k);

/// \brief Writes the string representation of the given CertificateHashDataChain \p k to the given output stream \p os
/// \returns an output stream with the CertificateHashDataChain written to
std::ostream& operator<<(std::ostream& os, const CertificateHashDataChain& k);

struct LogParameters {
    CiString<2000> remoteLocation;
    std::optional<ocpp::DateTime> oldestTimestamp;
    std::optional<ocpp::DateTime> latestTimestamp;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given LogParameters \p k to a given json object \p j
void to_json(json& j, const LogParameters& k);

/// \brief Conversion from a given json object \p j to a given LogParameters \p k
void from_json(const json& j, LogParameters& k);

/// \brief Writes the string representation of the given LogParameters \p k to the given output stream \p os
/// \returns an output stream with the LogParameters written to
std::ostream& operator<<(std::ostream& os, const LogParameters& k);

struct Component {
    CiString<50> name;
    std::optional<EVSE> evse;
    std::optional<CiString<50>> instance;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Component \p k to a given json object \p j
void to_json(json& j, const Component& k);

/// \brief Conversion from a given json object \p j to a given Component \p k
void from_json(const json& j, Component& k);

/// \brief Writes the string representation of the given Component \p k to the given output stream \p os
/// \returns an output stream with the Component written to
std::ostream& operator<<(std::ostream& os, const Component& k);

struct Variable {
    CiString<50> name;
    std::optional<CiString<50>> instance;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Variable \p k to a given json object \p j
void to_json(json& j, const Variable& k);

/// \brief Conversion from a given json object \p j to a given Variable \p k
void from_json(const json& j, Variable& k);

/// \brief Writes the string representation of the given Variable \p k to the given output stream \p os
/// \returns an output stream with the Variable written to
std::ostream& operator<<(std::ostream& os, const Variable& k);

struct ComponentVariable {
    Component component;
    std::optional<Variable> variable;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ComponentVariable \p k to a given json object \p j
void to_json(json& j, const ComponentVariable& k);

/// \brief Conversion from a given json object \p j to a given ComponentVariable \p k
void from_json(const json& j, ComponentVariable& k);

/// \brief Writes the string representation of the given ComponentVariable \p k to the given output stream \p os
/// \returns an output stream with the ComponentVariable written to
std::ostream& operator<<(std::ostream& os, const ComponentVariable& k);

struct ConstantStreamData {
    int32_t id;
    PeriodicEventStreamParams params;
    int32_t variableMonitoringId;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ConstantStreamData \p k to a given json object \p j
void to_json(json& j, const ConstantStreamData& k);

/// \brief Conversion from a given json object \p j to a given ConstantStreamData \p k
void from_json(const json& j, ConstantStreamData& k);

/// \brief Writes the string representation of the given ConstantStreamData \p k to the given output stream \p os
/// \returns an output stream with the ConstantStreamData written to
std::ostream& operator<<(std::ostream& os, const ConstantStreamData& k);

struct TariffAssignment {
    CiString<60> tariffId;
    TariffKindEnum tariffKind;
    std::optional<ocpp::DateTime> validFrom;
    std::optional<std::vector<int32_t>> evseIds;
    std::optional<std::vector<CiString<255>>> idTokens;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TariffAssignment \p k to a given json object \p j
void to_json(json& j, const TariffAssignment& k);

/// \brief Conversion from a given json object \p j to a given TariffAssignment \p k
void from_json(const json& j, TariffAssignment& k);

/// \brief Writes the string representation of the given TariffAssignment \p k to the given output stream \p os
/// \returns an output stream with the TariffAssignment written to
std::ostream& operator<<(std::ostream& os, const TariffAssignment& k);

struct GetVariableData {
    Component component;
    Variable variable;
    std::optional<AttributeEnum> attributeType;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given GetVariableData \p k to a given json object \p j
void to_json(json& j, const GetVariableData& k);

/// \brief Conversion from a given json object \p j to a given GetVariableData \p k
void from_json(const json& j, GetVariableData& k);

/// \brief Writes the string representation of the given GetVariableData \p k to the given output stream \p os
/// \returns an output stream with the GetVariableData written to
std::ostream& operator<<(std::ostream& os, const GetVariableData& k);

struct GetVariableResult {
    GetVariableStatusEnum attributeStatus;
    Component component;
    Variable variable;
    std::optional<StatusInfo> attributeStatusInfo;
    std::optional<AttributeEnum> attributeType;
    std::optional<CiString<2500>> attributeValue;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given GetVariableResult \p k to a given json object \p j
void to_json(json& j, const GetVariableResult& k);

/// \brief Conversion from a given json object \p j to a given GetVariableResult \p k
void from_json(const json& j, GetVariableResult& k);

/// \brief Writes the string representation of the given GetVariableResult \p k to the given output stream \p os
/// \returns an output stream with the GetVariableResult written to
std::ostream& operator<<(std::ostream& os, const GetVariableResult& k);

struct SignedMeterValue {
    CiString<32768> signedMeterData;
    CiString<50> encodingMethod;
    std::optional<CiString<50>> signingMethod;
    std::optional<CiString<2500>> publicKey;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SignedMeterValue \p k to a given json object \p j
void to_json(json& j, const SignedMeterValue& k);

/// \brief Conversion from a given json object \p j to a given SignedMeterValue \p k
void from_json(const json& j, SignedMeterValue& k);

/// \brief Writes the string representation of the given SignedMeterValue \p k to the given output stream \p os
/// \returns an output stream with the SignedMeterValue written to
std::ostream& operator<<(std::ostream& os, const SignedMeterValue& k);

struct UnitOfMeasure {
    std::optional<CiString<20>> unit;
    std::optional<int32_t> multiplier;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given UnitOfMeasure \p k to a given json object \p j
void to_json(json& j, const UnitOfMeasure& k);

/// \brief Conversion from a given json object \p j to a given UnitOfMeasure \p k
void from_json(const json& j, UnitOfMeasure& k);

/// \brief Writes the string representation of the given UnitOfMeasure \p k to the given output stream \p os
/// \returns an output stream with the UnitOfMeasure written to
std::ostream& operator<<(std::ostream& os, const UnitOfMeasure& k);

struct SampledValue {
    float value;
    std::optional<MeasurandEnum> measurand;
    std::optional<ReadingContextEnum> context;
    std::optional<PhaseEnum> phase;
    std::optional<LocationEnum> location;
    std::optional<SignedMeterValue> signedMeterValue;
    std::optional<UnitOfMeasure> unitOfMeasure;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SampledValue \p k to a given json object \p j
void to_json(json& j, const SampledValue& k);

/// \brief Conversion from a given json object \p j to a given SampledValue \p k
void from_json(const json& j, SampledValue& k);

/// \brief Writes the string representation of the given SampledValue \p k to the given output stream \p os
/// \returns an output stream with the SampledValue written to
std::ostream& operator<<(std::ostream& os, const SampledValue& k);

struct MeterValue {
    std::vector<SampledValue> sampledValue;
    ocpp::DateTime timestamp;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given MeterValue \p k to a given json object \p j
void to_json(json& j, const MeterValue& k);

/// \brief Conversion from a given json object \p j to a given MeterValue \p k
void from_json(const json& j, MeterValue& k);

/// \brief Writes the string representation of the given MeterValue \p k to the given output stream \p os
/// \returns an output stream with the MeterValue written to
std::ostream& operator<<(std::ostream& os, const MeterValue& k);

struct LimitAtSoC {
    int32_t soc;
    float limit;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given LimitAtSoC \p k to a given json object \p j
void to_json(json& j, const LimitAtSoC& k);

/// \brief Conversion from a given json object \p j to a given LimitAtSoC \p k
void from_json(const json& j, LimitAtSoC& k);

/// \brief Writes the string representation of the given LimitAtSoC \p k to the given output stream \p os
/// \returns an output stream with the LimitAtSoC written to
std::ostream& operator<<(std::ostream& os, const LimitAtSoC& k);

struct RelativeTimeInterval {
    int32_t start;
    std::optional<int32_t> duration;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given RelativeTimeInterval \p k to a given json object \p j
void to_json(json& j, const RelativeTimeInterval& k);

/// \brief Conversion from a given json object \p j to a given RelativeTimeInterval \p k
void from_json(const json& j, RelativeTimeInterval& k);

/// \brief Writes the string representation of the given RelativeTimeInterval \p k to the given output stream \p os
/// \returns an output stream with the RelativeTimeInterval written to
std::ostream& operator<<(std::ostream& os, const RelativeTimeInterval& k);

struct Cost {
    CostKindEnum costKind;
    int32_t amount;
    std::optional<int32_t> amountMultiplier;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Cost \p k to a given json object \p j
void to_json(json& j, const Cost& k);

/// \brief Conversion from a given json object \p j to a given Cost \p k
void from_json(const json& j, Cost& k);

/// \brief Writes the string representation of the given Cost \p k to the given output stream \p os
/// \returns an output stream with the Cost written to
std::ostream& operator<<(std::ostream& os, const Cost& k);

struct ConsumptionCost {
    float startValue;
    std::vector<Cost> cost;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ConsumptionCost \p k to a given json object \p j
void to_json(json& j, const ConsumptionCost& k);

/// \brief Conversion from a given json object \p j to a given ConsumptionCost \p k
void from_json(const json& j, ConsumptionCost& k);

/// \brief Writes the string representation of the given ConsumptionCost \p k to the given output stream \p os
/// \returns an output stream with the ConsumptionCost written to
std::ostream& operator<<(std::ostream& os, const ConsumptionCost& k);

struct SalesTariffEntry {
    RelativeTimeInterval relativeTimeInterval;
    std::optional<int32_t> ePriceLevel;
    std::optional<std::vector<ConsumptionCost>> consumptionCost;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SalesTariffEntry \p k to a given json object \p j
void to_json(json& j, const SalesTariffEntry& k);

/// \brief Conversion from a given json object \p j to a given SalesTariffEntry \p k
void from_json(const json& j, SalesTariffEntry& k);

/// \brief Writes the string representation of the given SalesTariffEntry \p k to the given output stream \p os
/// \returns an output stream with the SalesTariffEntry written to
std::ostream& operator<<(std::ostream& os, const SalesTariffEntry& k);

struct SalesTariff {
    int32_t id;
    std::vector<SalesTariffEntry> salesTariffEntry;
    std::optional<CiString<32>> salesTariffDescription;
    std::optional<int32_t> numEPriceLevels;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SalesTariff \p k to a given json object \p j
void to_json(json& j, const SalesTariff& k);

/// \brief Conversion from a given json object \p j to a given SalesTariff \p k
void from_json(const json& j, SalesTariff& k);

/// \brief Writes the string representation of the given SalesTariff \p k to the given output stream \p os
/// \returns an output stream with the SalesTariff written to
std::ostream& operator<<(std::ostream& os, const SalesTariff& k);

struct RationalNumber {
    int32_t exponent;
    int32_t value;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given RationalNumber \p k to a given json object \p j
void to_json(json& j, const RationalNumber& k);

/// \brief Conversion from a given json object \p j to a given RationalNumber \p k
void from_json(const json& j, RationalNumber& k);

/// \brief Writes the string representation of the given RationalNumber \p k to the given output stream \p os
/// \returns an output stream with the RationalNumber written to
std::ostream& operator<<(std::ostream& os, const RationalNumber& k);

struct PriceRule {
    RationalNumber energyFee;
    RationalNumber powerRangeStart;
    std::optional<int32_t> parkingFeePeriod;
    std::optional<int32_t> carbonDioxideEmission;
    std::optional<int32_t> renewableGenerationPercentage;
    std::optional<RationalNumber> parkingFee;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given PriceRule \p k to a given json object \p j
void to_json(json& j, const PriceRule& k);

/// \brief Conversion from a given json object \p j to a given PriceRule \p k
void from_json(const json& j, PriceRule& k);

/// \brief Writes the string representation of the given PriceRule \p k to the given output stream \p os
/// \returns an output stream with the PriceRule written to
std::ostream& operator<<(std::ostream& os, const PriceRule& k);

struct PriceRuleStack {
    int32_t duration;
    std::vector<PriceRule> priceRule;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given PriceRuleStack \p k to a given json object \p j
void to_json(json& j, const PriceRuleStack& k);

/// \brief Conversion from a given json object \p j to a given PriceRuleStack \p k
void from_json(const json& j, PriceRuleStack& k);

/// \brief Writes the string representation of the given PriceRuleStack \p k to the given output stream \p os
/// \returns an output stream with the PriceRuleStack written to
std::ostream& operator<<(std::ostream& os, const PriceRuleStack& k);

struct TaxRule {
    int32_t taxRuleID;
    bool appliesToEnergyFee;
    bool appliesToParkingFee;
    bool appliesToOverstayFee;
    bool appliesToMinimumMaximumCost;
    RationalNumber taxRate;
    std::optional<CiString<100>> taxRuleName;
    std::optional<bool> taxIncludedInPrice;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TaxRule \p k to a given json object \p j
void to_json(json& j, const TaxRule& k);

/// \brief Conversion from a given json object \p j to a given TaxRule \p k
void from_json(const json& j, TaxRule& k);

/// \brief Writes the string representation of the given TaxRule \p k to the given output stream \p os
/// \returns an output stream with the TaxRule written to
std::ostream& operator<<(std::ostream& os, const TaxRule& k);

struct OverstayRule {
    RationalNumber overstayFee;
    int32_t startTime;
    int32_t overstayFeePeriod;
    std::optional<CiString<32>> overstayRuleDescription;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given OverstayRule \p k to a given json object \p j
void to_json(json& j, const OverstayRule& k);

/// \brief Conversion from a given json object \p j to a given OverstayRule \p k
void from_json(const json& j, OverstayRule& k);

/// \brief Writes the string representation of the given OverstayRule \p k to the given output stream \p os
/// \returns an output stream with the OverstayRule written to
std::ostream& operator<<(std::ostream& os, const OverstayRule& k);

struct OverstayRuleList {
    std::vector<OverstayRule> overstayRule;
    std::optional<RationalNumber> overstayPowerThreshold;
    std::optional<int32_t> overstayTimeThreshold;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given OverstayRuleList \p k to a given json object \p j
void to_json(json& j, const OverstayRuleList& k);

/// \brief Conversion from a given json object \p j to a given OverstayRuleList \p k
void from_json(const json& j, OverstayRuleList& k);

/// \brief Writes the string representation of the given OverstayRuleList \p k to the given output stream \p os
/// \returns an output stream with the OverstayRuleList written to
std::ostream& operator<<(std::ostream& os, const OverstayRuleList& k);

struct AdditionalSelectedServices {
    RationalNumber serviceFee;
    CiString<80> serviceName;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given AdditionalSelectedServices \p k to a given json object \p j
void to_json(json& j, const AdditionalSelectedServices& k);

/// \brief Conversion from a given json object \p j to a given AdditionalSelectedServices \p k
void from_json(const json& j, AdditionalSelectedServices& k);

/// \brief Writes the string representation of the given AdditionalSelectedServices \p k to the given output stream \p
/// os \returns an output stream with the AdditionalSelectedServices written to
std::ostream& operator<<(std::ostream& os, const AdditionalSelectedServices& k);

struct AbsolutePriceSchedule {
    ocpp::DateTime timeAnchor;
    int32_t priceScheduleID;
    CiString<3> currency;
    CiString<8> language;
    CiString<2000> priceAlgorithm;
    std::vector<PriceRuleStack> priceRuleStacks;
    std::optional<CiString<160>> priceScheduleDescription;
    std::optional<RationalNumber> minimumCost;
    std::optional<RationalNumber> maximumCost;
    std::optional<std::vector<TaxRule>> taxRules;
    std::optional<OverstayRuleList> overstayRuleList;
    std::optional<std::vector<AdditionalSelectedServices>> additionalSelectedServices;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given AbsolutePriceSchedule \p k to a given json object \p j
void to_json(json& j, const AbsolutePriceSchedule& k);

/// \brief Conversion from a given json object \p j to a given AbsolutePriceSchedule \p k
void from_json(const json& j, AbsolutePriceSchedule& k);

/// \brief Writes the string representation of the given AbsolutePriceSchedule \p k to the given output stream \p os
/// \returns an output stream with the AbsolutePriceSchedule written to
std::ostream& operator<<(std::ostream& os, const AbsolutePriceSchedule& k);

struct PriceLevelScheduleEntry {
    int32_t duration;
    int32_t priceLevel;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given PriceLevelScheduleEntry \p k to a given json object \p j
void to_json(json& j, const PriceLevelScheduleEntry& k);

/// \brief Conversion from a given json object \p j to a given PriceLevelScheduleEntry \p k
void from_json(const json& j, PriceLevelScheduleEntry& k);

/// \brief Writes the string representation of the given PriceLevelScheduleEntry \p k to the given output stream \p os
/// \returns an output stream with the PriceLevelScheduleEntry written to
std::ostream& operator<<(std::ostream& os, const PriceLevelScheduleEntry& k);

struct PriceLevelSchedule {
    std::vector<PriceLevelScheduleEntry> priceLevelScheduleEntries;
    ocpp::DateTime timeAnchor;
    int32_t priceScheduleId;
    int32_t numberOfPriceLevels;
    std::optional<CiString<32>> priceScheduleDescription;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given PriceLevelSchedule \p k to a given json object \p j
void to_json(json& j, const PriceLevelSchedule& k);

/// \brief Conversion from a given json object \p j to a given PriceLevelSchedule \p k
void from_json(const json& j, PriceLevelSchedule& k);

/// \brief Writes the string representation of the given PriceLevelSchedule \p k to the given output stream \p os
/// \returns an output stream with the PriceLevelSchedule written to
std::ostream& operator<<(std::ostream& os, const PriceLevelSchedule& k);

struct ChargingSchedule {
    int32_t id;
    ChargingRateUnitEnum chargingRateUnit;
    std::vector<ChargingSchedulePeriod> chargingSchedulePeriod;
    std::optional<LimitAtSoC> limitAtSoC;
    std::optional<ocpp::DateTime> startSchedule;
    std::optional<int32_t> duration;
    std::optional<float> minChargingRate;
    std::optional<float> powerTolerance;
    std::optional<int32_t> signatureId;
    std::optional<CiString<88>> digestValue;
    std::optional<bool> useLocalTime;
    std::optional<int32_t> randomizedDelay;
    std::optional<SalesTariff> salesTariff;
    std::optional<AbsolutePriceSchedule> absolutePriceSchedule;
    std::optional<PriceLevelSchedule> priceLevelSchedule;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingSchedule \p k to a given json object \p j
void to_json(json& j, const ChargingSchedule& k);

/// \brief Conversion from a given json object \p j to a given ChargingSchedule \p k
void from_json(const json& j, ChargingSchedule& k);

/// \brief Writes the string representation of the given ChargingSchedule \p k to the given output stream \p os
/// \returns an output stream with the ChargingSchedule written to
std::ostream& operator<<(std::ostream& os, const ChargingSchedule& k);

struct ChargingLimit {
    CiString<20> chargingLimitSource;
    std::optional<bool> isLocalGeneration;
    std::optional<bool> isGridCritical;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingLimit \p k to a given json object \p j
void to_json(json& j, const ChargingLimit& k);

/// \brief Conversion from a given json object \p j to a given ChargingLimit \p k
void from_json(const json& j, ChargingLimit& k);

/// \brief Writes the string representation of the given ChargingLimit \p k to the given output stream \p os
/// \returns an output stream with the ChargingLimit written to
std::ostream& operator<<(std::ostream& os, const ChargingLimit& k);

struct MessageInfo {
    int32_t id;
    MessagePriorityEnum priority;
    MessageContent message;
    std::optional<Component> display;
    std::optional<MessageStateEnum> state;
    std::optional<ocpp::DateTime> startDateTime;
    std::optional<ocpp::DateTime> endDateTime;
    std::optional<CiString<36>> transactionId;
    std::optional<std::vector<MessageContent>> messageExtra;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given MessageInfo \p k to a given json object \p j
void to_json(json& j, const MessageInfo& k);

/// \brief Conversion from a given json object \p j to a given MessageInfo \p k
void from_json(const json& j, MessageInfo& k);

/// \brief Writes the string representation of the given MessageInfo \p k to the given output stream \p os
/// \returns an output stream with the MessageInfo written to
std::ostream& operator<<(std::ostream& os, const MessageInfo& k);

struct ACChargingParameters {
    float energyAmount;
    float evMinCurrent;
    float evMaxCurrent;
    float evMaxVoltage;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ACChargingParameters \p k to a given json object \p j
void to_json(json& j, const ACChargingParameters& k);

/// \brief Conversion from a given json object \p j to a given ACChargingParameters \p k
void from_json(const json& j, ACChargingParameters& k);

/// \brief Writes the string representation of the given ACChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the ACChargingParameters written to
std::ostream& operator<<(std::ostream& os, const ACChargingParameters& k);

struct DERChargingParameters {
    std::optional<std::vector<DERControlEnum>> evSupportedDERControl;
    std::optional<float> evOverExcitedMaxDischargePower;
    std::optional<float> evOverExcitedPowerFactor;
    std::optional<float> evUnderExcitedMaxDischargePower;
    std::optional<float> evUnderExcitedPowerFactor;
    std::optional<float> maxApparentPower;
    std::optional<float> maxChargeApparentPower;
    std::optional<float> maxChargeApparentPower_L2;
    std::optional<float> maxChargeApparentPower_L3;
    std::optional<float> maxDischargeApparentPower;
    std::optional<float> maxDischargeApparentPower_L2;
    std::optional<float> maxDischargeApparentPower_L3;
    std::optional<float> maxChargeReactivePower;
    std::optional<float> maxChargeReactivePower_L2;
    std::optional<float> maxChargeReactivePower_L3;
    std::optional<float> minChargeReactivePower;
    std::optional<float> minChargeReactivePower_L2;
    std::optional<float> minChargeReactivePower_L3;
    std::optional<float> maxDischargeReactivePower;
    std::optional<float> maxDischargeReactivePower_L2;
    std::optional<float> maxDischargeReactivePower_L3;
    std::optional<float> minDischargeReactivePower;
    std::optional<float> minDischargeReactivePower_L2;
    std::optional<float> minDischargeReactivePower_L3;
    std::optional<float> nominalVoltage;
    std::optional<float> nominalVoltageOffset;
    std::optional<float> maxNominalVoltage;
    std::optional<float> minNominalVoltage;
    std::optional<CiString<50>> evInverterManufacturer;
    std::optional<CiString<50>> evInverterModel;
    std::optional<CiString<50>> evInverterSerialNumber;
    std::optional<CiString<50>> evInverterSwVersion;
    std::optional<CiString<50>> evInverterHwVersion;
    std::optional<std::vector<IslandingDetectionEnum>> evIslandingDetectionMethod;
    std::optional<float> evIslandingTripTime;
    std::optional<float> evMaximumLevel1DCInjection;
    std::optional<float> evDurationLevel1DCInjection;
    std::optional<float> evMaximumLevel2DCInjection;
    std::optional<float> evDurationLevel2DCInjection;
    std::optional<float> evReactiveSusceptance;
    std::optional<float> evSessionTotalDischargeEnergyAvailable;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given DERChargingParameters \p k to a given json object \p j
void to_json(json& j, const DERChargingParameters& k);

/// \brief Conversion from a given json object \p j to a given DERChargingParameters \p k
void from_json(const json& j, DERChargingParameters& k);

/// \brief Writes the string representation of the given DERChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the DERChargingParameters written to
std::ostream& operator<<(std::ostream& os, const DERChargingParameters& k);

struct EVPriceRule {
    float energyFee;
    float powerRangeStart;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVPriceRule \p k to a given json object \p j
void to_json(json& j, const EVPriceRule& k);

/// \brief Conversion from a given json object \p j to a given EVPriceRule \p k
void from_json(const json& j, EVPriceRule& k);

/// \brief Writes the string representation of the given EVPriceRule \p k to the given output stream \p os
/// \returns an output stream with the EVPriceRule written to
std::ostream& operator<<(std::ostream& os, const EVPriceRule& k);

struct EVAbsolutePriceScheduleEntry {
    int32_t duration;
    std::vector<EVPriceRule> evPriceRule;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVAbsolutePriceScheduleEntry \p k to a given json object \p j
void to_json(json& j, const EVAbsolutePriceScheduleEntry& k);

/// \brief Conversion from a given json object \p j to a given EVAbsolutePriceScheduleEntry \p k
void from_json(const json& j, EVAbsolutePriceScheduleEntry& k);

/// \brief Writes the string representation of the given EVAbsolutePriceScheduleEntry \p k to the given output stream \p
/// os \returns an output stream with the EVAbsolutePriceScheduleEntry written to
std::ostream& operator<<(std::ostream& os, const EVAbsolutePriceScheduleEntry& k);

struct EVAbsolutePriceSchedule {
    ocpp::DateTime timeAnchor;
    CiString<3> currency;
    std::vector<EVAbsolutePriceScheduleEntry> evAbsolutePriceScheduleEntries;
    CiString<2000> priceAlgorithm;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVAbsolutePriceSchedule \p k to a given json object \p j
void to_json(json& j, const EVAbsolutePriceSchedule& k);

/// \brief Conversion from a given json object \p j to a given EVAbsolutePriceSchedule \p k
void from_json(const json& j, EVAbsolutePriceSchedule& k);

/// \brief Writes the string representation of the given EVAbsolutePriceSchedule \p k to the given output stream \p os
/// \returns an output stream with the EVAbsolutePriceSchedule written to
std::ostream& operator<<(std::ostream& os, const EVAbsolutePriceSchedule& k);

struct EVPowerScheduleEntry {
    int32_t duration;
    float power;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVPowerScheduleEntry \p k to a given json object \p j
void to_json(json& j, const EVPowerScheduleEntry& k);

/// \brief Conversion from a given json object \p j to a given EVPowerScheduleEntry \p k
void from_json(const json& j, EVPowerScheduleEntry& k);

/// \brief Writes the string representation of the given EVPowerScheduleEntry \p k to the given output stream \p os
/// \returns an output stream with the EVPowerScheduleEntry written to
std::ostream& operator<<(std::ostream& os, const EVPowerScheduleEntry& k);

struct EVPowerSchedule {
    std::vector<EVPowerScheduleEntry> evPowerScheduleEntries;
    ocpp::DateTime timeAnchor;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVPowerSchedule \p k to a given json object \p j
void to_json(json& j, const EVPowerSchedule& k);

/// \brief Conversion from a given json object \p j to a given EVPowerSchedule \p k
void from_json(const json& j, EVPowerSchedule& k);

/// \brief Writes the string representation of the given EVPowerSchedule \p k to the given output stream \p os
/// \returns an output stream with the EVPowerSchedule written to
std::ostream& operator<<(std::ostream& os, const EVPowerSchedule& k);

struct EVEnergyOffer {
    EVPowerSchedule evPowerSchedule;
    std::optional<EVAbsolutePriceSchedule> evAbsolutePriceSchedule;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EVEnergyOffer \p k to a given json object \p j
void to_json(json& j, const EVEnergyOffer& k);

/// \brief Conversion from a given json object \p j to a given EVEnergyOffer \p k
void from_json(const json& j, EVEnergyOffer& k);

/// \brief Writes the string representation of the given EVEnergyOffer \p k to the given output stream \p os
/// \returns an output stream with the EVEnergyOffer written to
std::ostream& operator<<(std::ostream& os, const EVEnergyOffer& k);

struct DCChargingParameters {
    float evMaxCurrent;
    float evMaxVoltage;
    std::optional<float> evMaxPower;
    std::optional<float> evEnergyCapacity;
    std::optional<float> energyAmount;
    std::optional<int32_t> stateOfCharge;
    std::optional<int32_t> fullSoC;
    std::optional<int32_t> bulkSoC;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given DCChargingParameters \p k to a given json object \p j
void to_json(json& j, const DCChargingParameters& k);

/// \brief Conversion from a given json object \p j to a given DCChargingParameters \p k
void from_json(const json& j, DCChargingParameters& k);

/// \brief Writes the string representation of the given DCChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the DCChargingParameters written to
std::ostream& operator<<(std::ostream& os, const DCChargingParameters& k);

struct V2XChargingParameters {
    std::optional<float> minChargePower;
    std::optional<float> minChargePower_L2;
    std::optional<float> minChargePower_L3;
    std::optional<float> maxChargePower;
    std::optional<float> maxChargePower_L2;
    std::optional<float> maxChargePower_L3;
    std::optional<float> minDischargePower;
    std::optional<float> minDischargePower_L2;
    std::optional<float> minDischargePower_L3;
    std::optional<float> maxDischargePower;
    std::optional<float> maxDischargePower_L2;
    std::optional<float> maxDischargePower_L3;
    std::optional<float> minChargeCurrent;
    std::optional<float> maxChargeCurrent;
    std::optional<float> minDischargeCurrent;
    std::optional<float> maxDischargeCurrent;
    std::optional<float> minVoltage;
    std::optional<float> maxVoltage;
    std::optional<float> evTargetEnergyRequest;
    std::optional<float> evMinEnergyRequest;
    std::optional<float> evMaxEnergyRequest;
    std::optional<float> evMinV2XEnergyRequest;
    std::optional<float> evMaxV2XEnergyRequest;
    std::optional<int32_t> targetSoC;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given V2XChargingParameters \p k to a given json object \p j
void to_json(json& j, const V2XChargingParameters& k);

/// \brief Conversion from a given json object \p j to a given V2XChargingParameters \p k
void from_json(const json& j, V2XChargingParameters& k);

/// \brief Writes the string representation of the given V2XChargingParameters \p k to the given output stream \p os
/// \returns an output stream with the V2XChargingParameters written to
std::ostream& operator<<(std::ostream& os, const V2XChargingParameters& k);

struct ChargingNeeds {
    EnergyTransferModeEnum requestedEnergyTransfer;
    std::optional<ACChargingParameters> acChargingParameters;
    std::optional<DERChargingParameters> derChargingParameters;
    std::optional<EVEnergyOffer> evEnergyOffer;
    std::optional<DCChargingParameters> dcChargingParameters;
    std::optional<V2XChargingParameters> v2xChargingParameters;
    std::optional<std::vector<EnergyTransferModeEnum>> availableEnergyTransfer;
    std::optional<ControlModeEnum> controlMode;
    std::optional<MobilityNeedsModeEnum> mobilityNeedsMode;
    std::optional<ocpp::DateTime> departureTime;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingNeeds \p k to a given json object \p j
void to_json(json& j, const ChargingNeeds& k);

/// \brief Conversion from a given json object \p j to a given ChargingNeeds \p k
void from_json(const json& j, ChargingNeeds& k);

/// \brief Writes the string representation of the given ChargingNeeds \p k to the given output stream \p os
/// \returns an output stream with the ChargingNeeds written to
std::ostream& operator<<(std::ostream& os, const ChargingNeeds& k);

struct EventData {
    int32_t eventId;
    ocpp::DateTime timestamp;
    EventTriggerEnum trigger;
    CiString<2500> actualValue;
    Component component;
    EventNotificationEnum eventNotificationType;
    Variable variable;
    std::optional<int32_t> cause;
    std::optional<CiString<50>> techCode;
    std::optional<CiString<500>> techInfo;
    std::optional<bool> cleared;
    std::optional<CiString<36>> transactionId;
    std::optional<int32_t> variableMonitoringId;
    std::optional<int32_t> severity;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EventData \p k to a given json object \p j
void to_json(json& j, const EventData& k);

/// \brief Conversion from a given json object \p j to a given EventData \p k
void from_json(const json& j, EventData& k);

/// \brief Writes the string representation of the given EventData \p k to the given output stream \p os
/// \returns an output stream with the EventData written to
std::ostream& operator<<(std::ostream& os, const EventData& k);

struct VariableMonitoring {
    int32_t id;
    bool transaction;
    float value;
    MonitorEnum type;
    int32_t severity;
    std::optional<EventNotificationEnum> eventNotificationType;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given VariableMonitoring \p k to a given json object \p j
void to_json(json& j, const VariableMonitoring& k);

/// \brief Conversion from a given json object \p j to a given VariableMonitoring \p k
void from_json(const json& j, VariableMonitoring& k);

/// \brief Writes the string representation of the given VariableMonitoring \p k to the given output stream \p os
/// \returns an output stream with the VariableMonitoring written to
std::ostream& operator<<(std::ostream& os, const VariableMonitoring& k);

struct MonitoringData {
    Component component;
    Variable variable;
    std::vector<VariableMonitoring> variableMonitoring;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given MonitoringData \p k to a given json object \p j
void to_json(json& j, const MonitoringData& k);

/// \brief Conversion from a given json object \p j to a given MonitoringData \p k
void from_json(const json& j, MonitoringData& k);

/// \brief Writes the string representation of the given MonitoringData \p k to the given output stream \p os
/// \returns an output stream with the MonitoringData written to
std::ostream& operator<<(std::ostream& os, const MonitoringData& k);

struct StreamDataElement {
    float t;
    CiString<2500> v;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given StreamDataElement \p k to a given json object \p j
void to_json(json& j, const StreamDataElement& k);

/// \brief Conversion from a given json object \p j to a given StreamDataElement \p k
void from_json(const json& j, StreamDataElement& k);

/// \brief Writes the string representation of the given StreamDataElement \p k to the given output stream \p os
/// \returns an output stream with the StreamDataElement written to
std::ostream& operator<<(std::ostream& os, const StreamDataElement& k);

struct NotifyPeriodicEventStream {
    std::vector<StreamDataElement> data;
    int32_t id;
    int32_t pending;
    ocpp::DateTime basetime;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given NotifyPeriodicEventStream \p k to a given json object \p j
void to_json(json& j, const NotifyPeriodicEventStream& k);

/// \brief Conversion from a given json object \p j to a given NotifyPeriodicEventStream \p k
void from_json(const json& j, NotifyPeriodicEventStream& k);

/// \brief Writes the string representation of the given NotifyPeriodicEventStream \p k to the given output stream \p os
/// \returns an output stream with the NotifyPeriodicEventStream written to
std::ostream& operator<<(std::ostream& os, const NotifyPeriodicEventStream& k);

struct VariableAttribute {
    std::optional<AttributeEnum> type;
    std::optional<CiString<2500>> value;
    std::optional<MutabilityEnum> mutability;
    std::optional<bool> persistent;
    std::optional<bool> constant;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given VariableAttribute \p k to a given json object \p j
void to_json(json& j, const VariableAttribute& k);

/// \brief Conversion from a given json object \p j to a given VariableAttribute \p k
void from_json(const json& j, VariableAttribute& k);

/// \brief Writes the string representation of the given VariableAttribute \p k to the given output stream \p os
/// \returns an output stream with the VariableAttribute written to
std::ostream& operator<<(std::ostream& os, const VariableAttribute& k);

struct VariableCharacteristics {
    DataEnum dataType;
    bool supportsMonitoring;
    std::optional<CiString<16>> unit;
    std::optional<float> minLimit;
    std::optional<float> maxLimit;
    std::optional<int32_t> maxElements;
    std::optional<CiString<1000>> valuesList;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given VariableCharacteristics \p k to a given json object \p j
void to_json(json& j, const VariableCharacteristics& k);

/// \brief Conversion from a given json object \p j to a given VariableCharacteristics \p k
void from_json(const json& j, VariableCharacteristics& k);

/// \brief Writes the string representation of the given VariableCharacteristics \p k to the given output stream \p os
/// \returns an output stream with the VariableCharacteristics written to
std::ostream& operator<<(std::ostream& os, const VariableCharacteristics& k);

struct ReportData {
    Component component;
    Variable variable;
    std::vector<VariableAttribute> variableAttribute;
    std::optional<VariableCharacteristics> variableCharacteristics;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ReportData \p k to a given json object \p j
void to_json(json& j, const ReportData& k);

/// \brief Conversion from a given json object \p j to a given ReportData \p k
void from_json(const json& j, ReportData& k);

/// \brief Writes the string representation of the given ReportData \p k to the given output stream \p os
/// \returns an output stream with the ReportData written to
std::ostream& operator<<(std::ostream& os, const ReportData& k);

struct Address {
    CiString<50> name;
    CiString<100> address1;
    CiString<100> city;
    CiString<50> country;
    std::optional<CiString<100>> address2;
    std::optional<CiString<20>> postalCode;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Address \p k to a given json object \p j
void to_json(json& j, const Address& k);

/// \brief Conversion from a given json object \p j to a given Address \p k
void from_json(const json& j, Address& k);

/// \brief Writes the string representation of the given Address \p k to the given output stream \p os
/// \returns an output stream with the Address written to
std::ostream& operator<<(std::ostream& os, const Address& k);

struct ChargingScheduleUpdate {
    std::optional<float> limit;
    std::optional<float> limit_L2;
    std::optional<float> limit_L3;
    std::optional<float> dischargeLimit;
    std::optional<float> dischargeLimit_L2;
    std::optional<float> dischargeLimit_L3;
    std::optional<float> setpoint;
    std::optional<float> setpoint_L2;
    std::optional<float> setpoint_L3;
    std::optional<float> setpointReactive;
    std::optional<float> setpointReactive_L2;
    std::optional<float> setpointReactive_L3;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingScheduleUpdate \p k to a given json object \p j
void to_json(json& j, const ChargingScheduleUpdate& k);

/// \brief Conversion from a given json object \p j to a given ChargingScheduleUpdate \p k
void from_json(const json& j, ChargingScheduleUpdate& k);

/// \brief Writes the string representation of the given ChargingScheduleUpdate \p k to the given output stream \p os
/// \returns an output stream with the ChargingScheduleUpdate written to
std::ostream& operator<<(std::ostream& os, const ChargingScheduleUpdate& k);

struct ChargingProfile {
    int32_t id;
    int32_t stackLevel;
    ChargingProfilePurposeEnum chargingProfilePurpose;
    ChargingProfileKindEnum chargingProfileKind;
    std::vector<ChargingSchedule> chargingSchedule;
    std::optional<RecurrencyKindEnum> recurrencyKind;
    std::optional<ocpp::DateTime> validFrom;
    std::optional<ocpp::DateTime> validTo;
    std::optional<CiString<36>> transactionId;
    std::optional<int32_t> maxOfflineDuration;
    std::optional<bool> invalidAfterOfflineDuration;
    std::optional<int32_t> dynUpdateInterval;
    std::optional<ocpp::DateTime> dynUpdateTime;
    std::optional<CiString<256>> priceScheduleSignature;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingProfile \p k to a given json object \p j
void to_json(json& j, const ChargingProfile& k);

/// \brief Conversion from a given json object \p j to a given ChargingProfile \p k
void from_json(const json& j, ChargingProfile& k);

/// \brief Writes the string representation of the given ChargingProfile \p k to the given output stream \p os
/// \returns an output stream with the ChargingProfile written to
std::ostream& operator<<(std::ostream& os, const ChargingProfile& k);

struct DERCurvePoints {
    float x;
    float y;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given DERCurvePoints \p k to a given json object \p j
void to_json(json& j, const DERCurvePoints& k);

/// \brief Conversion from a given json object \p j to a given DERCurvePoints \p k
void from_json(const json& j, DERCurvePoints& k);

/// \brief Writes the string representation of the given DERCurvePoints \p k to the given output stream \p os
/// \returns an output stream with the DERCurvePoints written to
std::ostream& operator<<(std::ostream& os, const DERCurvePoints& k);

struct Hysteresis {
    std::optional<float> hysteresisHigh;
    std::optional<float> hysteresisLow;
    std::optional<float> hysteresisDelay;
    std::optional<float> hysteresisGradient;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Hysteresis \p k to a given json object \p j
void to_json(json& j, const Hysteresis& k);

/// \brief Conversion from a given json object \p j to a given Hysteresis \p k
void from_json(const json& j, Hysteresis& k);

/// \brief Writes the string representation of the given Hysteresis \p k to the given output stream \p os
/// \returns an output stream with the Hysteresis written to
std::ostream& operator<<(std::ostream& os, const Hysteresis& k);

struct ReactivePowerParams {
    std::optional<float> vRef;
    std::optional<bool> autonomousVRefEnable;
    std::optional<float> autonomousVRefTimeConstant;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ReactivePowerParams \p k to a given json object \p j
void to_json(json& j, const ReactivePowerParams& k);

/// \brief Conversion from a given json object \p j to a given ReactivePowerParams \p k
void from_json(const json& j, ReactivePowerParams& k);

/// \brief Writes the string representation of the given ReactivePowerParams \p k to the given output stream \p os
/// \returns an output stream with the ReactivePowerParams written to
std::ostream& operator<<(std::ostream& os, const ReactivePowerParams& k);

struct VoltageParams {
    std::optional<float> hv10MinMeanValue;
    std::optional<float> hv10MinMeanTripDelay;
    std::optional<PowerDuringCessationEnum> powerDuringCessation;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given VoltageParams \p k to a given json object \p j
void to_json(json& j, const VoltageParams& k);

/// \brief Conversion from a given json object \p j to a given VoltageParams \p k
void from_json(const json& j, VoltageParams& k);

/// \brief Writes the string representation of the given VoltageParams \p k to the given output stream \p os
/// \returns an output stream with the VoltageParams written to
std::ostream& operator<<(std::ostream& os, const VoltageParams& k);

struct DERCurve {
    std::vector<DERCurvePoints> curveData;
    int32_t priority;
    DERUnitEnum yUnit;
    std::optional<Hysteresis> hysteresis;
    std::optional<ReactivePowerParams> reactivePowerParams;
    std::optional<VoltageParams> voltageParams;
    std::optional<float> responseTime;
    std::optional<ocpp::DateTime> startTime;
    std::optional<float> duration;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given DERCurve \p k to a given json object \p j
void to_json(json& j, const DERCurve& k);

/// \brief Conversion from a given json object \p j to a given DERCurve \p k
void from_json(const json& j, DERCurve& k);

/// \brief Writes the string representation of the given DERCurve \p k to the given output stream \p os
/// \returns an output stream with the DERCurve written to
std::ostream& operator<<(std::ostream& os, const DERCurve& k);

struct DERCurveGet {
    DERCurve curve;
    CiString<36> id;
    DERControlEnum curveType;
    bool isDefault;
    bool isSuperseded;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given DERCurveGet \p k to a given json object \p j
void to_json(json& j, const DERCurveGet& k);

/// \brief Conversion from a given json object \p j to a given DERCurveGet \p k
void from_json(const json& j, DERCurveGet& k);

/// \brief Writes the string representation of the given DERCurveGet \p k to the given output stream \p os
/// \returns an output stream with the DERCurveGet written to
std::ostream& operator<<(std::ostream& os, const DERCurveGet& k);

struct EnterService {
    int32_t priority;
    float highVoltage;
    float lowVoltage;
    float highFreq;
    float lowFreq;
    std::optional<float> delay;
    std::optional<float> randomDelay;
    std::optional<float> rampRate;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EnterService \p k to a given json object \p j
void to_json(json& j, const EnterService& k);

/// \brief Conversion from a given json object \p j to a given EnterService \p k
void from_json(const json& j, EnterService& k);

/// \brief Writes the string representation of the given EnterService \p k to the given output stream \p os
/// \returns an output stream with the EnterService written to
std::ostream& operator<<(std::ostream& os, const EnterService& k);

struct EnterServiceGet {
    EnterService enterService;
    CiString<36> id;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given EnterServiceGet \p k to a given json object \p j
void to_json(json& j, const EnterServiceGet& k);

/// \brief Conversion from a given json object \p j to a given EnterServiceGet \p k
void from_json(const json& j, EnterServiceGet& k);

/// \brief Writes the string representation of the given EnterServiceGet \p k to the given output stream \p os
/// \returns an output stream with the EnterServiceGet written to
std::ostream& operator<<(std::ostream& os, const EnterServiceGet& k);

struct FixedPF {
    int32_t priority;
    float displacement;
    bool excitation;
    std::optional<ocpp::DateTime> startTime;
    std::optional<float> duration;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given FixedPF \p k to a given json object \p j
void to_json(json& j, const FixedPF& k);

/// \brief Conversion from a given json object \p j to a given FixedPF \p k
void from_json(const json& j, FixedPF& k);

/// \brief Writes the string representation of the given FixedPF \p k to the given output stream \p os
/// \returns an output stream with the FixedPF written to
std::ostream& operator<<(std::ostream& os, const FixedPF& k);

struct FixedPFGet {
    FixedPF fixedPF;
    CiString<36> id;
    bool isDefault;
    bool isSuperseded;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given FixedPFGet \p k to a given json object \p j
void to_json(json& j, const FixedPFGet& k);

/// \brief Conversion from a given json object \p j to a given FixedPFGet \p k
void from_json(const json& j, FixedPFGet& k);

/// \brief Writes the string representation of the given FixedPFGet \p k to the given output stream \p os
/// \returns an output stream with the FixedPFGet written to
std::ostream& operator<<(std::ostream& os, const FixedPFGet& k);

struct FixedVar {
    int32_t priority;
    float setpoint;
    DERUnitEnum unit;
    std::optional<ocpp::DateTime> startTime;
    std::optional<float> duration;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given FixedVar \p k to a given json object \p j
void to_json(json& j, const FixedVar& k);

/// \brief Conversion from a given json object \p j to a given FixedVar \p k
void from_json(const json& j, FixedVar& k);

/// \brief Writes the string representation of the given FixedVar \p k to the given output stream \p os
/// \returns an output stream with the FixedVar written to
std::ostream& operator<<(std::ostream& os, const FixedVar& k);

struct FixedVarGet {
    FixedVar fixedVar;
    CiString<36> id;
    bool isDefault;
    bool isSuperseded;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given FixedVarGet \p k to a given json object \p j
void to_json(json& j, const FixedVarGet& k);

/// \brief Conversion from a given json object \p j to a given FixedVarGet \p k
void from_json(const json& j, FixedVarGet& k);

/// \brief Writes the string representation of the given FixedVarGet \p k to the given output stream \p os
/// \returns an output stream with the FixedVarGet written to
std::ostream& operator<<(std::ostream& os, const FixedVarGet& k);

struct FreqDroop {
    int32_t priority;
    float overFreq;
    float underFreq;
    float overDroop;
    float underDroop;
    float responseTime;
    std::optional<ocpp::DateTime> startTime;
    std::optional<float> duration;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given FreqDroop \p k to a given json object \p j
void to_json(json& j, const FreqDroop& k);

/// \brief Conversion from a given json object \p j to a given FreqDroop \p k
void from_json(const json& j, FreqDroop& k);

/// \brief Writes the string representation of the given FreqDroop \p k to the given output stream \p os
/// \returns an output stream with the FreqDroop written to
std::ostream& operator<<(std::ostream& os, const FreqDroop& k);

struct FreqDroopGet {
    FreqDroop freqDroop;
    CiString<36> id;
    bool isDefault;
    bool isSuperseded;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given FreqDroopGet \p k to a given json object \p j
void to_json(json& j, const FreqDroopGet& k);

/// \brief Conversion from a given json object \p j to a given FreqDroopGet \p k
void from_json(const json& j, FreqDroopGet& k);

/// \brief Writes the string representation of the given FreqDroopGet \p k to the given output stream \p os
/// \returns an output stream with the FreqDroopGet written to
std::ostream& operator<<(std::ostream& os, const FreqDroopGet& k);

struct Gradient {
    int32_t priority;
    float gradient;
    float softGradient;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Gradient \p k to a given json object \p j
void to_json(json& j, const Gradient& k);

/// \brief Conversion from a given json object \p j to a given Gradient \p k
void from_json(const json& j, Gradient& k);

/// \brief Writes the string representation of the given Gradient \p k to the given output stream \p os
/// \returns an output stream with the Gradient written to
std::ostream& operator<<(std::ostream& os, const Gradient& k);

struct GradientGet {
    Gradient gradient;
    CiString<36> id;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given GradientGet \p k to a given json object \p j
void to_json(json& j, const GradientGet& k);

/// \brief Conversion from a given json object \p j to a given GradientGet \p k
void from_json(const json& j, GradientGet& k);

/// \brief Writes the string representation of the given GradientGet \p k to the given output stream \p os
/// \returns an output stream with the GradientGet written to
std::ostream& operator<<(std::ostream& os, const GradientGet& k);

struct LimitMaxDischarge {
    int32_t priority;
    std::optional<float> pctMaxDischargePower;
    std::optional<DERCurve> powerMonitoringMustTrip;
    std::optional<ocpp::DateTime> startTime;
    std::optional<float> duration;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given LimitMaxDischarge \p k to a given json object \p j
void to_json(json& j, const LimitMaxDischarge& k);

/// \brief Conversion from a given json object \p j to a given LimitMaxDischarge \p k
void from_json(const json& j, LimitMaxDischarge& k);

/// \brief Writes the string representation of the given LimitMaxDischarge \p k to the given output stream \p os
/// \returns an output stream with the LimitMaxDischarge written to
std::ostream& operator<<(std::ostream& os, const LimitMaxDischarge& k);

struct LimitMaxDischargeGet {
    CiString<36> id;
    bool isDefault;
    bool isSuperseded;
    LimitMaxDischarge limitMaxDischarge;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given LimitMaxDischargeGet \p k to a given json object \p j
void to_json(json& j, const LimitMaxDischargeGet& k);

/// \brief Conversion from a given json object \p j to a given LimitMaxDischargeGet \p k
void from_json(const json& j, LimitMaxDischargeGet& k);

/// \brief Writes the string representation of the given LimitMaxDischargeGet \p k to the given output stream \p os
/// \returns an output stream with the LimitMaxDischargeGet written to
std::ostream& operator<<(std::ostream& os, const LimitMaxDischargeGet& k);

struct AuthorizationData {
    IdToken idToken;
    std::optional<IdTokenInfo> idTokenInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given AuthorizationData \p k to a given json object \p j
void to_json(json& j, const AuthorizationData& k);

/// \brief Conversion from a given json object \p j to a given AuthorizationData \p k
void from_json(const json& j, AuthorizationData& k);

/// \brief Writes the string representation of the given AuthorizationData \p k to the given output stream \p os
/// \returns an output stream with the AuthorizationData written to
std::ostream& operator<<(std::ostream& os, const AuthorizationData& k);

struct APN {
    CiString<2000> apn;
    APNAuthenticationEnum apnAuthentication;
    std::optional<CiString<50>> apnUserName;
    std::optional<CiString<64>> apnPassword;
    std::optional<int32_t> simPin;
    std::optional<CiString<6>> preferredNetwork;
    std::optional<bool> useOnlyPreferredNetwork;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given APN \p k to a given json object \p j
void to_json(json& j, const APN& k);

/// \brief Conversion from a given json object \p j to a given APN \p k
void from_json(const json& j, APN& k);

/// \brief Writes the string representation of the given APN \p k to the given output stream \p os
/// \returns an output stream with the APN written to
std::ostream& operator<<(std::ostream& os, const APN& k);

struct VPN {
    CiString<2000> server;
    CiString<50> user;
    CiString<64> password;
    CiString<255> key;
    VPNEnum type;
    std::optional<CiString<50>> group;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given VPN \p k to a given json object \p j
void to_json(json& j, const VPN& k);

/// \brief Conversion from a given json object \p j to a given VPN \p k
void from_json(const json& j, VPN& k);

/// \brief Writes the string representation of the given VPN \p k to the given output stream \p os
/// \returns an output stream with the VPN written to
std::ostream& operator<<(std::ostream& os, const VPN& k);

struct NetworkConnectionProfile {
    OCPPInterfaceEnum ocppInterface;
    OCPPTransportEnum ocppTransport;
    int32_t messageTimeout;
    CiString<2000> ocppCsmsUrl;
    int32_t securityProfile;
    std::optional<APN> apn;
    std::optional<OCPPVersionEnum> ocppVersion;
    std::optional<CiString<48>> identity;
    std::optional<CiString<64>> basicAuthPassword;
    std::optional<VPN> vpn;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given NetworkConnectionProfile \p k to a given json object \p j
void to_json(json& j, const NetworkConnectionProfile& k);

/// \brief Conversion from a given json object \p j to a given NetworkConnectionProfile \p k
void from_json(const json& j, NetworkConnectionProfile& k);

/// \brief Writes the string representation of the given NetworkConnectionProfile \p k to the given output stream \p os
/// \returns an output stream with the NetworkConnectionProfile written to
std::ostream& operator<<(std::ostream& os, const NetworkConnectionProfile& k);

struct SetMonitoringData {
    float value;
    MonitorEnum type;
    int32_t severity;
    Component component;
    Variable variable;
    std::optional<int32_t> id;
    std::optional<PeriodicEventStreamParams> periodicEventStream;
    std::optional<bool> transaction;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SetMonitoringData \p k to a given json object \p j
void to_json(json& j, const SetMonitoringData& k);

/// \brief Conversion from a given json object \p j to a given SetMonitoringData \p k
void from_json(const json& j, SetMonitoringData& k);

/// \brief Writes the string representation of the given SetMonitoringData \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringData written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringData& k);

struct SetMonitoringResult {
    SetMonitoringStatusEnum status;
    MonitorEnum type;
    Component component;
    Variable variable;
    int32_t severity;
    std::optional<int32_t> id;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SetMonitoringResult \p k to a given json object \p j
void to_json(json& j, const SetMonitoringResult& k);

/// \brief Conversion from a given json object \p j to a given SetMonitoringResult \p k
void from_json(const json& j, SetMonitoringResult& k);

/// \brief Writes the string representation of the given SetMonitoringResult \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringResult written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringResult& k);

struct SetVariableData {
    CiString<2500> attributeValue;
    Component component;
    Variable variable;
    std::optional<AttributeEnum> attributeType;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SetVariableData \p k to a given json object \p j
void to_json(json& j, const SetVariableData& k);

/// \brief Conversion from a given json object \p j to a given SetVariableData \p k
void from_json(const json& j, SetVariableData& k);

/// \brief Writes the string representation of the given SetVariableData \p k to the given output stream \p os
/// \returns an output stream with the SetVariableData written to
std::ostream& operator<<(std::ostream& os, const SetVariableData& k);

struct SetVariableResult {
    SetVariableStatusEnum attributeStatus;
    Component component;
    Variable variable;
    std::optional<AttributeEnum> attributeType;
    std::optional<StatusInfo> attributeStatusInfo;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given SetVariableResult \p k to a given json object \p j
void to_json(json& j, const SetVariableResult& k);

/// \brief Conversion from a given json object \p j to a given SetVariableResult \p k
void from_json(const json& j, SetVariableResult& k);

/// \brief Writes the string representation of the given SetVariableResult \p k to the given output stream \p os
/// \returns an output stream with the SetVariableResult written to
std::ostream& operator<<(std::ostream& os, const SetVariableResult& k);

struct CostDimension {
    CostDimensionEnum type;
    float volume;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CostDimension \p k to a given json object \p j
void to_json(json& j, const CostDimension& k);

/// \brief Conversion from a given json object \p j to a given CostDimension \p k
void from_json(const json& j, CostDimension& k);

/// \brief Writes the string representation of the given CostDimension \p k to the given output stream \p os
/// \returns an output stream with the CostDimension written to
std::ostream& operator<<(std::ostream& os, const CostDimension& k);

struct ChargingPeriod {
    ocpp::DateTime startPeriod;
    std::optional<std::vector<CostDimension>> dimensions;
    std::optional<CiString<60>> tariffId;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given ChargingPeriod \p k to a given json object \p j
void to_json(json& j, const ChargingPeriod& k);

/// \brief Conversion from a given json object \p j to a given ChargingPeriod \p k
void from_json(const json& j, ChargingPeriod& k);

/// \brief Writes the string representation of the given ChargingPeriod \p k to the given output stream \p os
/// \returns an output stream with the ChargingPeriod written to
std::ostream& operator<<(std::ostream& os, const ChargingPeriod& k);

struct TotalPrice {
    std::optional<float> exclTax;
    std::optional<float> inclTax;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TotalPrice \p k to a given json object \p j
void to_json(json& j, const TotalPrice& k);

/// \brief Conversion from a given json object \p j to a given TotalPrice \p k
void from_json(const json& j, TotalPrice& k);

/// \brief Writes the string representation of the given TotalPrice \p k to the given output stream \p os
/// \returns an output stream with the TotalPrice written to
std::ostream& operator<<(std::ostream& os, const TotalPrice& k);

struct TotalCost {
    CiString<3> currency;
    TariffCostEnum typeOfCost;
    TotalPrice total;
    std::optional<Price> fixed;
    std::optional<Price> energy;
    std::optional<Price> chargingTime;
    std::optional<Price> idleTime;
    std::optional<Price> reservationTime;
    std::optional<Price> reservationFixed;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TotalCost \p k to a given json object \p j
void to_json(json& j, const TotalCost& k);

/// \brief Conversion from a given json object \p j to a given TotalCost \p k
void from_json(const json& j, TotalCost& k);

/// \brief Writes the string representation of the given TotalCost \p k to the given output stream \p os
/// \returns an output stream with the TotalCost written to
std::ostream& operator<<(std::ostream& os, const TotalCost& k);

struct TotalUsage {
    float energy;
    int32_t chargingTime;
    int32_t idleTime;
    std::optional<int32_t> reservationTime;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TotalUsage \p k to a given json object \p j
void to_json(json& j, const TotalUsage& k);

/// \brief Conversion from a given json object \p j to a given TotalUsage \p k
void from_json(const json& j, TotalUsage& k);

/// \brief Writes the string representation of the given TotalUsage \p k to the given output stream \p os
/// \returns an output stream with the TotalUsage written to
std::ostream& operator<<(std::ostream& os, const TotalUsage& k);

struct CostDetails {
    TotalCost totalCost;
    TotalUsage totalUsage;
    std::optional<std::vector<ChargingPeriod>> chargingPeriods;
    std::optional<bool> failureToCalculate;
    std::optional<CiString<500>> failureReason;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given CostDetails \p k to a given json object \p j
void to_json(json& j, const CostDetails& k);

/// \brief Conversion from a given json object \p j to a given CostDetails \p k
void from_json(const json& j, CostDetails& k);

/// \brief Writes the string representation of the given CostDetails \p k to the given output stream \p os
/// \returns an output stream with the CostDetails written to
std::ostream& operator<<(std::ostream& os, const CostDetails& k);

struct TransactionLimit {
    std::optional<float> maxCost;
    std::optional<float> maxEnergy;
    std::optional<int32_t> maxTime;
    std::optional<int32_t> maxSoC;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given TransactionLimit \p k to a given json object \p j
void to_json(json& j, const TransactionLimit& k);

/// \brief Conversion from a given json object \p j to a given TransactionLimit \p k
void from_json(const json& j, TransactionLimit& k);

/// \brief Writes the string representation of the given TransactionLimit \p k to the given output stream \p os
/// \returns an output stream with the TransactionLimit written to
std::ostream& operator<<(std::ostream& os, const TransactionLimit& k);

struct Transaction {
    CiString<36> transactionId;
    std::optional<ChargingStateEnum> chargingState;
    std::optional<int32_t> timeSpentCharging;
    std::optional<ReasonEnum> stoppedReason;
    std::optional<int32_t> remoteStartId;
    std::optional<OperationModeEnum> operationMode;
    std::optional<CiString<60>> tariffId;
    std::optional<TransactionLimit> transactionLimit;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Transaction \p k to a given json object \p j
void to_json(json& j, const Transaction& k);

/// \brief Conversion from a given json object \p j to a given Transaction \p k
void from_json(const json& j, Transaction& k);

/// \brief Writes the string representation of the given Transaction \p k to the given output stream \p os
/// \returns an output stream with the Transaction written to
std::ostream& operator<<(std::ostream& os, const Transaction& k);

struct Firmware {
    CiString<2000> location;
    ocpp::DateTime retrieveDateTime;
    std::optional<ocpp::DateTime> installDateTime;
    std::optional<CiString<5500>> signingCertificate;
    std::optional<CiString<800>> signature;
    std::optional<CustomData> customData;
};
/// \brief Conversion from a given Firmware \p k to a given json object \p j
void to_json(json& j, const Firmware& k);

/// \brief Conversion from a given json object \p j to a given Firmware \p k
void from_json(const json& j, Firmware& k);

/// \brief Writes the string representation of the given Firmware \p k to the given output stream \p os
/// \returns an output stream with the Firmware written to
std::ostream& operator<<(std::ostream& os, const Firmware& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_OCPP_TYPES_HPP
