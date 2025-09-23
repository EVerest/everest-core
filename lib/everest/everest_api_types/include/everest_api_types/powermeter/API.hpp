// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::powermeter {

enum class OCMFUserIdentificationStatus {
    ASSIGNED,
    NOT_ASSIGNED,
};

enum class OCMFIdentificationFlags {
    RFID_NONE,
    RFID_PLAIN,
    RFID_RELATED,
    RFID_PSK,
    OCPP_NONE,
    OCPP_RS,
    OCPP_AUTH,
    OCPP_RS_TLS,
    OCPP_AUTH_TLS,
    OCPP_CACHE,
    OCPP_WHITELIST,
    OCPP_CERTIFIED,
    ISO15118_NONE,
    ISO15118_PNC,
    PLMN_NONE,
    PLMN_RING,
    PLMN_SMS,
};

enum class OCMFIdentificationType {
    NONE,
    DENIED,
    UNDEFINED,
    ISO14443,
    ISO15693,
    EMAID,
    EVCCID,
    EVCOID,
    ISO7812,
    CARD_TXN_NR,
    CENTRAL,
    CENTRAL_1,
    CENTRAL_2,
    LOCAL,
    LOCAL_1,
    LOCAL_2,
    PHONE_NUMBER,
    KEY_CODE,
};

enum class OCMFIdentificationLevel {
    NONE,
    HEARSAY,
    TRUSTED,
    VERIFIED,
    CERTIFIED,
    SECURE,
    MISMATCH,
    INVALID,
    OUTDATED,
    UNKNOWN,
};

enum class TransactionStatus {
    OK,
    NOT_SUPPORTED,
    UNEXPECTED_ERROR,
};

struct Current {
    std::optional<float> DC; ///< DC current
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
    std::optional<float> N;  ///< AC Neutral value only
};

struct Voltage {
    std::optional<float> DC; ///< DC voltage
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
};

struct Frequency {
    float L1;                ///< AC L1 value
    std::optional<float> L2; ///< AC L2 value
    std::optional<float> L3; ///< AC L3 value
};

struct Power {
    float total;             ///< DC / AC Sum value
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
};

struct Energy {
    float total;             ///< DC / AC Sum value (which is relevant for billing)
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
};

struct ReactivePower {
    float total;             ///< VAR total
    std::optional<float> L1; ///< VAR phase A
    std::optional<float> L2; ///< VAR phase B
    std::optional<float> L3; ///< VAR phase C
};

struct SignedMeterValue {
    std::string signed_meter_data;         ///< Signed meter data (encoded in a string representation with eg. Base64)
    std::string signing_method;            ///< Method used to create the signature
    std::string encoding_method;           ///< Method used to encode the meter values before signing them
    std::optional<std::string> public_key; ///< Public key (encoded in a string representation with eg. Base64)
    std::optional<std::string> timestamp;  ///< Timestamp of measurement
};

struct SignedCurrent {
    std::optional<SignedMeterValue> DC; ///< DC current
    std::optional<SignedMeterValue> L1; ///< AC L1 value only
    std::optional<SignedMeterValue> L2; ///< AC L2 value only
    std::optional<SignedMeterValue> L3; ///< AC L3 value only
    std::optional<SignedMeterValue> N;  ///< AC Neutral value only
};

struct SignedVoltage {
    std::optional<SignedMeterValue> DC; ///< DC voltage
    std::optional<SignedMeterValue> L1; ///< AC L1 value only
    std::optional<SignedMeterValue> L2; ///< AC L2 value only
    std::optional<SignedMeterValue> L3; ///< AC L3 value only
};

struct SignedFrequency {
    std::optional<SignedMeterValue> L1; ///< AC L1 value
    std::optional<SignedMeterValue> L2; ///< AC L2 value
    std::optional<SignedMeterValue> L3; ///< AC L3 value
};

struct SignedPower {
    std::optional<SignedMeterValue> total; ///< DC / AC Sum value
    std::optional<SignedMeterValue> L1;    ///< AC L1 value only
    std::optional<SignedMeterValue> L2;    ///< AC L2 value only
    std::optional<SignedMeterValue> L3;    ///< AC L3 value only
};

struct SignedEnergy {
    std::optional<SignedMeterValue> total; ///< DC / AC Sum value (which is relevant for billing)
    std::optional<SignedMeterValue> L1;    ///< AC L1 value only
    std::optional<SignedMeterValue> L2;    ///< AC L2 value only
    std::optional<SignedMeterValue> L3;    ///< AC L3 value only
};

struct SignedReactivePower {
    std::optional<SignedMeterValue> total; ///< VAR total
    std::optional<SignedMeterValue> L1;    ///< VAR phase A
    std::optional<SignedMeterValue> L2;    ///< VAR phase B
    std::optional<SignedMeterValue> L3;    ///< VAR phase C
};

struct Temperature {
    float temperature;
    std::optional<std::string> identification;
    std::optional<std::string> location;
};

struct PowermeterValues {
    std::string timestamp;
    Energy energy_Wh_import;
    std::optional<std::string> meter_id;
    std::optional<bool> phase_seq_error;
    std::optional<Energy> energy_Wh_export;
    std::optional<Power> power_W;
    std::optional<Voltage> voltage_V;
    std::optional<ReactivePower> VAR;
    std::optional<Current> current_A;
    std::optional<Frequency> frequency_Hz;
    std::optional<SignedEnergy> energy_Wh_import_signed;
    std::optional<SignedEnergy> energy_Wh_export_signed;
    std::optional<SignedPower> power_W_signed;
    std::optional<SignedVoltage> voltage_V_signed;
    std::optional<SignedReactivePower> VAR_signed;
    std::optional<SignedCurrent> current_A_signed;
    std::optional<SignedFrequency> frequency_Hz_signed;
    std::optional<SignedMeterValue> signed_meter_value;
    std::optional<std::vector<Temperature>> temperatures;
};

struct ReplyStartTransaction {
    TransactionStatus status;
    std::optional<std::string> error;
    std::optional<std::string> transaction_min_stop_time;
    std::optional<std::string> transaction_max_stop_time;
};

struct ReplyStopTransaction {
    TransactionStatus status;
    std::optional<SignedMeterValue> start_signed_meter_value;
    std::optional<SignedMeterValue> signed_meter_value;
    std::optional<std::string> error;
};

struct RequestStartTransaction {
    std::string evse_id;
    std::string transaction_id;
    OCMFUserIdentificationStatus identification_status;
    std::vector<OCMFIdentificationFlags> identification_flags;
    OCMFIdentificationType identification_type;
    std::optional<OCMFIdentificationLevel> identification_level;
    std::optional<std::string> identification_data;
    std::optional<std::string> tariff_text;
};

} // namespace everest::lib::API::V1_0::types::powermeter
