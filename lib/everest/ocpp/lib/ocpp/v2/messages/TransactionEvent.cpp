// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/TransactionEvent.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string TransactionEventRequest::get_type() const {
    return "TransactionEvent";
}

void to_json(json& j, const TransactionEventRequest& k) {
    // the required parts of the message
    j = json{
        {"eventType", conversions::transaction_event_enum_to_string(k.eventType)},
        {"timestamp", k.timestamp.to_rfc3339()},
        {"triggerReason", conversions::trigger_reason_enum_to_string(k.triggerReason)},
        {"seqNo", k.seqNo},
        {"transactionInfo", k.transactionInfo},
    };
    // the optional parts of the message
    if (k.costDetails) {
        j["costDetails"] = k.costDetails.value();
    }
    if (k.meterValue) {
        j["meterValue"] = json::array();
        for (auto val : k.meterValue.value()) {
            j["meterValue"].push_back(val);
        }
    }
    if (k.offline) {
        j["offline"] = k.offline.value();
    }
    if (k.numberOfPhasesUsed) {
        j["numberOfPhasesUsed"] = k.numberOfPhasesUsed.value();
    }
    if (k.cableMaxCurrent) {
        j["cableMaxCurrent"] = k.cableMaxCurrent.value();
    }
    if (k.reservationId) {
        j["reservationId"] = k.reservationId.value();
    }
    if (k.preconditioningStatus) {
        j["preconditioningStatus"] =
            conversions::preconditioning_status_enum_to_string(k.preconditioningStatus.value());
    }
    if (k.evseSleep) {
        j["evseSleep"] = k.evseSleep.value();
    }
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
    if (k.idToken) {
        j["idToken"] = k.idToken.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, TransactionEventRequest& k) {
    // the required parts of the message
    k.eventType = conversions::string_to_transaction_event_enum(j.at("eventType"));
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
    k.triggerReason = conversions::string_to_trigger_reason_enum(j.at("triggerReason"));
    k.seqNo = j.at("seqNo");
    k.transactionInfo = j.at("transactionInfo");

    // the optional parts of the message
    if (j.contains("costDetails")) {
        k.costDetails.emplace(j.at("costDetails"));
    }
    if (j.contains("meterValue")) {
        json arr = j.at("meterValue");
        std::vector<MeterValue> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.meterValue.emplace(vec);
    }
    if (j.contains("offline")) {
        k.offline.emplace(j.at("offline"));
    }
    if (j.contains("numberOfPhasesUsed")) {
        k.numberOfPhasesUsed.emplace(j.at("numberOfPhasesUsed"));
    }
    if (j.contains("cableMaxCurrent")) {
        k.cableMaxCurrent.emplace(j.at("cableMaxCurrent"));
    }
    if (j.contains("reservationId")) {
        k.reservationId.emplace(j.at("reservationId"));
    }
    if (j.contains("preconditioningStatus")) {
        k.preconditioningStatus.emplace(
            conversions::string_to_preconditioning_status_enum(j.at("preconditioningStatus")));
    }
    if (j.contains("evseSleep")) {
        k.evseSleep.emplace(j.at("evseSleep"));
    }
    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
    if (j.contains("idToken")) {
        k.idToken.emplace(j.at("idToken"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given TransactionEventRequest \p k to the given output stream \p os
/// \returns an output stream with the TransactionEventRequest written to
std::ostream& operator<<(std::ostream& os, const TransactionEventRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string TransactionEventResponse::get_type() const {
    return "TransactionEventResponse";
}

void to_json(json& j, const TransactionEventResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.totalCost) {
        j["totalCost"] = k.totalCost.value();
    }
    if (k.chargingPriority) {
        j["chargingPriority"] = k.chargingPriority.value();
    }
    if (k.idTokenInfo) {
        j["idTokenInfo"] = k.idTokenInfo.value();
    }
    if (k.transactionLimit) {
        j["transactionLimit"] = k.transactionLimit.value();
    }
    if (k.updatedPersonalMessage) {
        j["updatedPersonalMessage"] = k.updatedPersonalMessage.value();
    }
    if (k.updatedPersonalMessageExtra) {
        if (j.size() == 0) {
            j = json{{"updatedPersonalMessageExtra", json::array()}};
        } else {
            j["updatedPersonalMessageExtra"] = json::array();
        }
        for (auto val : k.updatedPersonalMessageExtra.value()) {
            j["updatedPersonalMessageExtra"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, TransactionEventResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("totalCost")) {
        k.totalCost.emplace(j.at("totalCost"));
    }
    if (j.contains("chargingPriority")) {
        k.chargingPriority.emplace(j.at("chargingPriority"));
    }
    if (j.contains("idTokenInfo")) {
        k.idTokenInfo.emplace(j.at("idTokenInfo"));
    }
    if (j.contains("transactionLimit")) {
        k.transactionLimit.emplace(j.at("transactionLimit"));
    }
    if (j.contains("updatedPersonalMessage")) {
        k.updatedPersonalMessage.emplace(j.at("updatedPersonalMessage"));
    }
    if (j.contains("updatedPersonalMessageExtra")) {
        json arr = j.at("updatedPersonalMessageExtra");
        std::vector<MessageContent> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.updatedPersonalMessageExtra.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given TransactionEventResponse \p k to the given output stream \p os
/// \returns an output stream with the TransactionEventResponse written to
std::ostream& operator<<(std::ostream& os, const TransactionEventResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
