// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyAllowedEnergyTransfer.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyAllowedEnergyTransferRequest::get_type() const {
    return "NotifyAllowedEnergyTransfer";
}

void to_json(json& j, const NotifyAllowedEnergyTransferRequest& k) {
    // the required parts of the message
    j = json{
        {"transactionId", k.transactionId},
        {"allowedEnergyTransfer", json::array()},
    };
    for (auto val : k.allowedEnergyTransfer) {
        j["allowedEnergyTransfer"].push_back(ocpp::v2::conversions::energy_transfer_mode_enum_to_string(val));
    }
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyAllowedEnergyTransferRequest& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");
    for (auto val : j.at("allowedEnergyTransfer")) {
        k.allowedEnergyTransfer.push_back(ocpp::v2::conversions::string_to_energy_transfer_mode_enum(val));
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyAllowedEnergyTransferRequest \p k to the given output
/// stream \p os \returns an output stream with the NotifyAllowedEnergyTransferRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyAllowedEnergyTransferRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyAllowedEnergyTransferResponse::get_type() const {
    return "NotifyAllowedEnergyTransferResponse";
}

void to_json(json& j, const NotifyAllowedEnergyTransferResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::notify_allowed_energy_transfer_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyAllowedEnergyTransferResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_notify_allowed_energy_transfer_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyAllowedEnergyTransferResponse \p k to the given output
/// stream \p os \returns an output stream with the NotifyAllowedEnergyTransferResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyAllowedEnergyTransferResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
