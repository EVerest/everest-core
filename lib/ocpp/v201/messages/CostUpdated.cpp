// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/CostUpdated.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string CostUpdatedRequest::get_type() const {
    return "CostUpdated";
}

void to_json(json& j, const CostUpdatedRequest& k) {
    // the required parts of the message
    j = json{
        {"totalCost", k.totalCost},
        {"transactionId", k.transactionId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, CostUpdatedRequest& k) {
    // the required parts of the message
    k.totalCost = j.at("totalCost");
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given CostUpdatedRequest \p k to the given output stream \p os
/// \returns an output stream with the CostUpdatedRequest written to
std::ostream& operator<<(std::ostream& os, const CostUpdatedRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string CostUpdatedResponse::get_type() const {
    return "CostUpdatedResponse";
}

void to_json(json& j, const CostUpdatedResponse& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, CostUpdatedResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given CostUpdatedResponse \p k to the given output stream \p os
/// \returns an output stream with the CostUpdatedResponse written to
std::ostream& operator<<(std::ostream& os, const CostUpdatedResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
