// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/DataTransfer.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string DataTransferRequest::get_type() const {
    return "DataTransfer";
}

void to_json(json& j, const DataTransferRequest& k) {
    // the required parts of the message
    j = json{
        {"vendorId", k.vendorId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.messageId) {
        j["messageId"] = k.messageId.value();
    }
    if (k.data) {
        j["data"] = k.data.value();
    }
}

void from_json(const json& j, DataTransferRequest& k) {
    // the required parts of the message
    k.vendorId = j.at("vendorId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("messageId")) {
        k.messageId.emplace(j.at("messageId"));
    }
    if (j.contains("data")) {
        k.data.emplace(j.at("data"));
    }
}

/// \brief Writes the string representation of the given DataTransferRequest \p k to the given output stream \p os
/// \returns an output stream with the DataTransferRequest written to
std::ostream& operator<<(std::ostream& os, const DataTransferRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string DataTransferResponse::get_type() const {
    return "DataTransferResponse";
}

void to_json(json& j, const DataTransferResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::data_transfer_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.data) {
        j["data"] = k.data.value();
    }
}

void from_json(const json& j, DataTransferResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_data_transfer_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("data")) {
        k.data.emplace(j.at("data"));
    }
}

/// \brief Writes the string representation of the given DataTransferResponse \p k to the given output stream \p os
/// \returns an output stream with the DataTransferResponse written to
std::ostream& operator<<(std::ostream& os, const DataTransferResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
