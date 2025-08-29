// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeter/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "powermeter/API.hpp"
#include "powermeter/codec.hpp"

namespace everest::lib::API::V1_0::types::powermeter {

using json = nlohmann::json;

void from_json(const json& j, OCMFUserIdentificationStatus& k) {
    std::string s = j;
    if (s == "ASSIGNED") {
        k = OCMFUserIdentificationStatus::ASSIGNED;
        return;
    }
    if (s == "NOT_ASSIGNED") {
        k = OCMFUserIdentificationStatus::NOT_ASSIGNED;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type OCMFUserIdentificationStatus _API_1_0");
}
void to_json(json& j, const OCMFUserIdentificationStatus& k) noexcept {
    switch (k) {
    case OCMFUserIdentificationStatus::ASSIGNED:
        j = "ASSIGNED";
        return;
    case OCMFUserIdentificationStatus::NOT_ASSIGNED:
        j = "NOT_ASSIGNED";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::powermeter::OCMFUserIdentificationStatus";
}

void from_json(const json& j, OCMFIdentificationFlags& k) {
    std::string s = j;
    if (s == "RFID_NONE") {
        k = OCMFIdentificationFlags::RFID_NONE;
        return;
    }
    if (s == "RFID_PLAIN") {
        k = OCMFIdentificationFlags::RFID_PLAIN;
        return;
    }
    if (s == "RFID_RELATED") {
        k = OCMFIdentificationFlags::RFID_RELATED;
        return;
    }
    if (s == "RFID_PSK") {
        k = OCMFIdentificationFlags::RFID_PSK;
        return;
    }
    if (s == "OCPP_NONE") {
        k = OCMFIdentificationFlags::OCPP_NONE;
        return;
    }
    if (s == "OCPP_RS") {
        k = OCMFIdentificationFlags::OCPP_RS;
        return;
    }
    if (s == "OCPP_AUTH") {
        k = OCMFIdentificationFlags::OCPP_AUTH;
        return;
    }
    if (s == "OCPP_RS_TLS") {
        k = OCMFIdentificationFlags::OCPP_RS_TLS;
        return;
    }
    if (s == "OCPP_AUTH_TLS") {
        k = OCMFIdentificationFlags::OCPP_AUTH_TLS;
        return;
    }
    if (s == "OCPP_CACHE") {
        k = OCMFIdentificationFlags::OCPP_CACHE;
        return;
    }
    if (s == "OCPP_WHITELIST") {
        k = OCMFIdentificationFlags::OCPP_WHITELIST;
        return;
    }
    if (s == "OCPP_CERTIFIED") {
        k = OCMFIdentificationFlags::OCPP_CERTIFIED;
        return;
    }
    if (s == "ISO15118_NONE") {
        k = OCMFIdentificationFlags::ISO15118_NONE;
        return;
    }
    if (s == "ISO15118_PNC") {
        k = OCMFIdentificationFlags::ISO15118_PNC;
        return;
    }
    if (s == "PLMN_NONE") {
        k = OCMFIdentificationFlags::PLMN_NONE;
        return;
    }
    if (s == "PLMN_RING") {
        k = OCMFIdentificationFlags::PLMN_RING;
        return;
    }
    if (s == "PLMN_SMS") {
        k = OCMFIdentificationFlags::PLMN_SMS;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type OCMFIdentificationFlags _API_1_0");
}
void to_json(json& j, const OCMFIdentificationFlags& k) noexcept {
    switch (k) {
    case OCMFIdentificationFlags::RFID_NONE:
        j = "RFID_NONE";
        return;
    case OCMFIdentificationFlags::RFID_PLAIN:
        j = "RFID_PLAIN";
        return;
    case OCMFIdentificationFlags::RFID_RELATED:
        j = "RFID_RELATED";
        return;
    case OCMFIdentificationFlags::RFID_PSK:
        j = "RFID_PSK";
        return;
    case OCMFIdentificationFlags::OCPP_NONE:
        j = "OCPP_NONE";
        return;
    case OCMFIdentificationFlags::OCPP_RS:
        j = "OCPP_RS";
        return;
    case OCMFIdentificationFlags::OCPP_AUTH:
        j = "OCPP_AUTH";
        return;
    case OCMFIdentificationFlags::OCPP_RS_TLS:
        j = "OCPP_RS_TLS";
        return;
    case OCMFIdentificationFlags::OCPP_AUTH_TLS:
        j = "OCPP_AUTH_TLS";
        return;
    case OCMFIdentificationFlags::OCPP_CACHE:
        j = "OCPP_CACHE";
        return;
    case OCMFIdentificationFlags::OCPP_WHITELIST:
        j = "OCPP_WHITELIST";
        return;
    case OCMFIdentificationFlags::OCPP_CERTIFIED:
        j = "OCPP_CERTIFIED";
        return;
    case OCMFIdentificationFlags::ISO15118_NONE:
        j = "ISO15118_NONE";
        return;
    case OCMFIdentificationFlags::ISO15118_PNC:
        j = "ISO15118_PNC";
        return;
    case OCMFIdentificationFlags::PLMN_NONE:
        j = "PLMN_NONE";
        return;
    case OCMFIdentificationFlags::PLMN_RING:
        j = "PLMN_RING";
        return;
    case OCMFIdentificationFlags::PLMN_SMS:
        j = "PLMN_SMS";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::powermeter::OCMFIdentificationFlags";
}

void from_json(const json& j, OCMFIdentificationType& k) {
    std::string s = j;
    if (s == "NONE") {
        k = OCMFIdentificationType::NONE;
        return;
    }
    if (s == "DENIED") {
        k = OCMFIdentificationType::DENIED;
        return;
    }
    if (s == "UNDEFINED") {
        k = OCMFIdentificationType::UNDEFINED;
        return;
    }
    if (s == "ISO14443") {
        k = OCMFIdentificationType::ISO14443;
        return;
    }
    if (s == "ISO15693") {
        k = OCMFIdentificationType::ISO15693;
        return;
    }
    if (s == "EMAID") {
        k = OCMFIdentificationType::EMAID;
        return;
    }
    if (s == "EVCCID") {
        k = OCMFIdentificationType::EVCCID;
        return;
    }
    if (s == "EVCOID") {
        k = OCMFIdentificationType::EVCOID;
        return;
    }
    if (s == "ISO7812") {
        k = OCMFIdentificationType::ISO7812;
        return;
    }
    if (s == "CARD_TXN_NR") {
        k = OCMFIdentificationType::CARD_TXN_NR;
        return;
    }
    if (s == "CENTRAL") {
        k = OCMFIdentificationType::CENTRAL;
        return;
    }
    if (s == "CENTRAL_1") {
        k = OCMFIdentificationType::CENTRAL_1;
        return;
    }
    if (s == "CENTRAL_2") {
        k = OCMFIdentificationType::CENTRAL_2;
        return;
    }
    if (s == "LOCAL") {
        k = OCMFIdentificationType::LOCAL;
        return;
    }
    if (s == "LOCAL_1") {
        k = OCMFIdentificationType::LOCAL_1;
        return;
    }
    if (s == "LOCAL_2") {
        k = OCMFIdentificationType::LOCAL_2;
        return;
    }
    if (s == "PHONE_NUMBER") {
        k = OCMFIdentificationType::PHONE_NUMBER;
        return;
    }
    if (s == "KEY_CODE") {
        k = OCMFIdentificationType::KEY_CODE;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type OCMIdentificationType _API_1_0");
}
void to_json(json& j, const OCMFIdentificationType& k) noexcept {
    switch (k) {
    case OCMFIdentificationType::NONE:
        j = "NONE";
        return;
    case OCMFIdentificationType::DENIED:
        j = "DENIED";
        return;
    case OCMFIdentificationType::UNDEFINED:
        j = "UNDEFINED";
        return;
    case OCMFIdentificationType::ISO14443:
        j = "ISO14443";
        return;
    case OCMFIdentificationType::ISO15693:
        j = "ISO15693";
        return;
    case OCMFIdentificationType::EMAID:
        j = "EMAID";
        return;
    case OCMFIdentificationType::EVCCID:
        j = "EVCCID";
        return;
    case OCMFIdentificationType::EVCOID:
        j = "EVCOID";
        return;
    case OCMFIdentificationType::ISO7812:
        j = "ISO7812";
        return;
    case OCMFIdentificationType::CARD_TXN_NR:
        j = "CARD_TXN_NR";
        return;
    case OCMFIdentificationType::CENTRAL:
        j = "CENTRAL";
        return;
    case OCMFIdentificationType::CENTRAL_1:
        j = "CENTRAL_1";
        return;
    case OCMFIdentificationType::CENTRAL_2:
        j = "CENTRAL_2";
        return;
    case OCMFIdentificationType::LOCAL:
        j = "LOCAL";
        return;
    case OCMFIdentificationType::LOCAL_1:
        j = "LOCAL_1";
        return;
    case OCMFIdentificationType::LOCAL_2:
        j = "LOCAL_2";
        return;
    case OCMFIdentificationType::PHONE_NUMBER:
        j = "PHONE_NUMBER";
        return;
    case OCMFIdentificationType::KEY_CODE:
        j = "KEY_CODE";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::powermeter::OCMFIdentificationType";
}

void from_json(const json& j, OCMFIdentificationLevel& k) {
    std::string s = j;
    if (s == "NONE") {
        k = OCMFIdentificationLevel::NONE;
        return;
    }
    if (s == "HEARSAY") {
        k = OCMFIdentificationLevel::HEARSAY;
        return;
    }
    if (s == "TRUSTED") {
        k = OCMFIdentificationLevel::TRUSTED;
        return;
    }
    if (s == "VERIFIED") {
        k = OCMFIdentificationLevel::VERIFIED;
        return;
    }
    if (s == "CERTIFIED") {
        k = OCMFIdentificationLevel::CERTIFIED;
        return;
    }
    if (s == "SECURE") {
        k = OCMFIdentificationLevel::SECURE;
        return;
    }
    if (s == "MISMATCH") {
        k = OCMFIdentificationLevel::MISMATCH;
        return;
    }
    if (s == "INVALID") {
        k = OCMFIdentificationLevel::INVALID;
        return;
    }
    if (s == "OUTDATED") {
        k = OCMFIdentificationLevel::OUTDATED;
        return;
    }
    if (s == "UNKNOWN") {
        k = OCMFIdentificationLevel::UNKNOWN;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type OCMIdentificationLevel _API_1_0");
}
void to_json(json& j, const OCMFIdentificationLevel& k) noexcept {
    switch (k) {
    case OCMFIdentificationLevel::NONE:
        j = "NONE";
        return;
    case OCMFIdentificationLevel::HEARSAY:
        j = "HEARSAY";
        return;
    case OCMFIdentificationLevel::TRUSTED:
        j = "TRUSTED";
        return;
    case OCMFIdentificationLevel::VERIFIED:
        j = "VERIFIED";
        return;
    case OCMFIdentificationLevel::CERTIFIED:
        j = "CERTIFIED";
        return;
    case OCMFIdentificationLevel::SECURE:
        j = "SECURE";
        return;
    case OCMFIdentificationLevel::MISMATCH:
        j = "MISMATCH";
        return;
    case OCMFIdentificationLevel::INVALID:
        j = "INVALID";
        return;
    case OCMFIdentificationLevel::OUTDATED:
        j = "OUTDATED";
        return;
    case OCMFIdentificationLevel::UNKNOWN:
        j = "UNKNOWN";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::powermeter::OCMFIdentificationLevel";
}

void from_json(const json& j, TransactionStatus& k) {
    std::string s = j;
    if (s == "OK") {
        k = TransactionStatus::OK;
        return;
    }
    if (s == "NOT_SUPPORTED") {
        k = TransactionStatus::NOT_SUPPORTED;
        return;
    }
    if (s == "UNEXPECTED_ERROR") {
        k = TransactionStatus::UNEXPECTED_ERROR;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type TransactionStatus_API_1_0");
}
void to_json(json& j, const TransactionStatus& k) noexcept {
    switch (k) {
    case TransactionStatus::OK:
        j = "OK";
        return;
    case TransactionStatus::NOT_SUPPORTED:
        j = "NOT_SUPPORTED";
        return;
    case TransactionStatus::UNEXPECTED_ERROR:
        j = "UNEXPECTED_ERROR";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::powermeter::TransactionStatus";
}

void from_json(const json& j, Current& k) {
    if (j.contains("DC")) {
        k.DC.emplace(j.at("DC"));
    }
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
    if (j.contains("N")) {
        k.N.emplace(j.at("N"));
    }
}
void to_json(json& j, const Current& k) noexcept {
    j = json({});
    if (k.DC) {
        j["DC"] = k.DC.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
    if (k.N) {
        j["N"] = k.N.value();
    }
}

void from_json(const json& j, Voltage& k) {
    if (j.contains("DC")) {
        k.DC.emplace(j.at("DC"));
    }
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const Voltage& k) noexcept {
    j = json({});
    // the optional parts of the type
    if (k.DC) {
        j["DC"] = k.DC.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Frequency& k) {
    k.L1 = j.at("L1");

    // the optional parts of the type
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const Frequency& k) noexcept {
    j = json{
        {"L1", k.L1},
    };

    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Power& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const Power& k) noexcept {
    j = json{
        {"total", k.total},
    };
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Energy& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const Energy& k) noexcept {
    j = json{
        {"total", k.total},
    };
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, ReactivePower& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const ReactivePower& k) noexcept {
    j = json{
        {"total", k.total},
    };

    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, SignedMeterValue& k) {
    k.signed_meter_data = j.at("signed_meter_data");
    k.signing_method = j.at("signing_method");
    k.encoding_method = j.at("encoding_method");

    if (j.contains("public_key")) {
        k.public_key.emplace(j.at("public_key"));
    }
    if (j.contains("timestamp")) {
        k.timestamp.emplace(j.at("timestamp"));
    }
}
void to_json(json& j, const SignedMeterValue& k) noexcept {
    j = json{
        {"signed_meter_data", k.signed_meter_data},
        {"signing_method", k.signing_method},
        {"encoding_method", k.encoding_method},
    };
    if (k.public_key) {
        j["public_key"] = k.public_key.value();
    }
    if (k.timestamp) {
        j["timestamp"] = k.timestamp.value();
    }
}

void from_json(const json& j, SignedCurrent& k) {
    if (j.contains("DC")) {
        k.DC.emplace(j.at("DC"));
    }
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
    if (j.contains("N")) {
        k.N.emplace(j.at("N"));
    }
}
void to_json(json& j, const SignedCurrent& k) noexcept {
    j = json({});
    if (k.DC) {
        j["DC"] = k.DC.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
    if (k.N) {
        j["N"] = k.N.value();
    }
}

void from_json(const json& j, SignedVoltage& k) {
    if (j.contains("DC")) {
        k.DC.emplace(j.at("DC"));
    }
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const SignedVoltage& k) noexcept {
    j = json({});
    if (k.DC) {
        j["DC"] = k.DC.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, SignedFrequency& k) {
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const SignedFrequency& k) noexcept {
    j = json({});
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, SignedPower& k) {
    if (j.contains("total")) {
        k.total.emplace(j.at("total"));
    }
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const SignedPower& k) noexcept {
    j = json({});
    if (k.total) {
        j["total"] = k.total.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, SignedEnergy& k) {
    // the optional parts of the type
    if (j.contains("total")) {
        k.total.emplace(j.at("total"));
    }
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const SignedEnergy& k) noexcept {
    j = json({});
    if (k.total) {
        j["total"] = k.total.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, SignedReactivePower& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}
void to_json(json& j, const SignedReactivePower& k) noexcept {
    j = json({});
    if (k.total) {
        j["total"] = k.total.value();
    }
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Temperature& k) {
    k.temperature = j.at("temperature");
    if (j.contains("identification")) {
        k.identification.emplace(j.at("identification"));
    }
}
void to_json(json& j, const Temperature& k) noexcept {
    j = json{
        {"temperature", k.temperature},
    };
    if (k.identification) {
        j["identification"] = k.identification.value();
    }
}

void from_json(const json& j, PowermeterValues& k) {
    k.timestamp = j.at("timestamp");
    k.energy_Wh_import = j.at("energy_Wh_import");
    if (j.contains("meter_id")) {
        k.meter_id.emplace(j.at("meter_id"));
    }
    if (j.contains("phase_seq_error")) {
        k.phase_seq_error.emplace(j.at("phase_seq_error"));
    }
    if (j.contains("energy_Wh_export")) {
        k.energy_Wh_export.emplace(j.at("energy_Wh_export"));
    }
    if (j.contains("power_W")) {
        k.power_W.emplace(j.at("power_W"));
    }
    if (j.contains("voltage_V")) {
        k.voltage_V.emplace(j.at("voltage_V"));
    }
    if (j.contains("VAR")) {
        k.VAR.emplace(j.at("VAR"));
    }
    if (j.contains("current_A")) {
        k.current_A.emplace(j.at("current_A"));
    }
    if (j.contains("frequency_Hz")) {
        k.frequency_Hz.emplace(j.at("frequency_Hz"));
    }
    if (j.contains("energy_Wh_import_signed")) {
        k.energy_Wh_import_signed.emplace(j.at("energy_Wh_import_signed"));
    }
    if (j.contains("energy_Wh_export_signed")) {
        k.energy_Wh_export_signed.emplace(j.at("energy_Wh_export_signed"));
    }
    if (j.contains("power_W_signed")) {
        k.power_W_signed.emplace(j.at("power_W_signed"));
    }
    if (j.contains("voltage_V_signed")) {
        k.voltage_V_signed.emplace(j.at("voltage_V_signed"));
    }
    if (j.contains("VAR_signed")) {
        k.VAR_signed.emplace(j.at("VAR_signed"));
    }
    if (j.contains("current_A_signed")) {
        k.current_A_signed.emplace(j.at("current_A_signed"));
    }
    if (j.contains("frequency_Hz_signed")) {
        k.frequency_Hz_signed.emplace(j.at("frequency_Hz_signed"));
    }
    if (j.contains("signed_meter_value")) {
        k.signed_meter_value.emplace(j.at("signed_meter_value"));
    }
    if (j.contains("temperatures")) {
        json arr = j.at("temperatures");
        std::vector<Temperature> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.temperatures.emplace(vec);
    }
}
void to_json(json& j, const PowermeterValues& k) noexcept {
    j = json{
        {"timestamp", k.timestamp},
        {"energy_Wh_import", k.energy_Wh_import},
    };
    if (k.meter_id) {
        j["meter_id"] = k.meter_id.value();
    }
    if (k.phase_seq_error) {
        j["phase_seq_error"] = k.phase_seq_error.value();
    }
    if (k.energy_Wh_export) {
        j["energy_Wh_export"] = k.energy_Wh_export.value();
    }
    if (k.power_W) {
        j["power_W"] = k.power_W.value();
    }
    if (k.voltage_V) {
        j["voltage_V"] = k.voltage_V.value();
    }
    if (k.VAR) {
        j["VAR"] = k.VAR.value();
    }
    if (k.current_A) {
        j["current_A"] = k.current_A.value();
    }
    if (k.frequency_Hz) {
        j["frequency_Hz"] = k.frequency_Hz.value();
    }
    if (k.energy_Wh_import_signed) {
        j["energy_Wh_import_signed"] = k.energy_Wh_import_signed.value();
    }
    if (k.energy_Wh_export_signed) {
        j["energy_Wh_export_signed"] = k.energy_Wh_export_signed.value();
    }
    if (k.power_W_signed) {
        j["power_W_signed"] = k.power_W_signed.value();
    }
    if (k.voltage_V_signed) {
        j["voltage_V_signed"] = k.voltage_V_signed.value();
    }
    if (k.VAR_signed) {
        j["VAR_signed"] = k.VAR_signed.value();
    }
    if (k.current_A_signed) {
        j["current_A_signed"] = k.current_A_signed.value();
    }
    if (k.frequency_Hz_signed) {
        j["frequency_Hz_signed"] = k.frequency_Hz_signed.value();
    }
    if (k.signed_meter_value) {
        j["signed_meter_value"] = k.signed_meter_value.value();
    }
    if (k.temperatures) {
        j["temperatures"] = json::array();
        for (auto val : k.temperatures.value()) {
            j["temperatures"].push_back(val);
        }
    }
}

void from_json(const json& j, ReplyStartTransaction& k) {
    k.status = j.at("status");

    if (j.contains("error")) {
        k.error.emplace(j.at("error"));
    }
    if (j.contains("transaction_min_stop_time")) {
        k.transaction_min_stop_time.emplace(j.at("transaction_min_stop_time"));
    }
    if (j.contains("transaction_max_stop_time")) {
        k.transaction_max_stop_time.emplace(j.at("transaction_max_stop_time"));
    }
}
void to_json(json& j, const ReplyStartTransaction& k) noexcept {
    j = json{
        {"status", k.status},
    };
    if (k.error) {
        j["error"] = k.error.value();
    }
    if (k.transaction_min_stop_time) {
        j["transaction_min_stop_time"] = k.transaction_min_stop_time.value();
    }
    if (k.transaction_max_stop_time) {
        j["transaction_max_stop_time"] = k.transaction_max_stop_time.value();
    }
}

void from_json(const json& j, ReplyStopTransaction& k) {
    k.status = j.at("status");

    if (j.contains("signed_meter_value")) {
        k.signed_meter_value.emplace(j.at("signed_meter_value"));
    }
    if (j.contains("error")) {
        k.error.emplace(j.at("error"));
    }
}
void to_json(json& j, const ReplyStopTransaction& k) noexcept {
    j = json{
        {"status", k.status},
    };
    if (k.signed_meter_value) {
        j["signed_meter_value"] = k.signed_meter_value.value();
    }
    if (k.error) {
        j["error"] = k.error.value();
    }
}

void from_json(const json& j, RequestStartTransaction& k) {
    k.evse_id = j.at("evse_id");
    k.transaction_id = j.at("transaction_id");
    k.identification_status = j.at("identification_status");

    k.identification_flags.clear();
    if (j.contains("identification_flags")) {
        for (auto const& elem : j.at("identification_flags")) {
            k.identification_flags.push_back(elem);
        }
    }

    k.identification_type = j.at("identification_type");
    if (j.contains("identification_level")) {
        k.identification_level.emplace(j.at("identification_level"));
    }
    if (j.contains("identification_data")) {
        k.identification_data = j.at("identification_data");
    }
    if (j.contains("tariff_text")) {
        k.tariff_text = j.at("tariff_text");
    }
}
void to_json(json& j, const RequestStartTransaction& k) noexcept {
    j = json{
        {"evse_id", k.evse_id},
        {"transaction_id", k.transaction_id},
        {"identification_status", k.identification_status},
        {"identification_flags", k.identification_flags},
        {"identification_type", k.identification_type},
    };
    if (k.identification_level) {
        j["identification_level"] = k.identification_level.value();
    }
    if (k.identification_data) {
        j["identification_data"] = k.identification_data.value();
    }
    if (k.tariff_text) {
        j["tariff_text"] = k.tariff_text.value();
    }
}

} // namespace everest::lib::API::V1_0::types::powermeter
