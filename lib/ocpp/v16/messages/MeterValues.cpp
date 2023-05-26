// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v16/messages/MeterValues.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string MeterValuesRequest::get_type() const {
    return "MeterValues";
}

void to_json(json& j, const MeterValuesRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
        {"meterValue", k.meterValue},
    };
    // the optional parts of the message
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
}

void from_json(const json& j, MeterValuesRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    for (auto val : j.at("meterValue")) {
        k.meterValue.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
}

/// \brief Writes the string representation of the given MeterValuesRequest \p k to the given output stream \p os
/// \returns an output stream with the MeterValuesRequest written to
std::ostream& operator<<(std::ostream& os, const MeterValuesRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string MeterValuesResponse::get_type() const {
    return "MeterValuesResponse";
}

void to_json(json& j, const MeterValuesResponse& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
}

void from_json(const json& j, MeterValuesResponse& k) {
    // the required parts of the message

    // the optional parts of the message
}

/// \brief Writes the string representation of the given MeterValuesResponse \p k to the given output stream \p os
/// \returns an output stream with the MeterValuesResponse written to
std::ostream& operator<<(std::ostream& os, const MeterValuesResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
