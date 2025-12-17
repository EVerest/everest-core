// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/ChangeTransactionTariff.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string ChangeTransactionTariffRequest::get_type() const {
    return "ChangeTransactionTariff";
}

void to_json(json& j, const ChangeTransactionTariffRequest& k) {
    // the required parts of the message
    j = json{
        {"tariff", k.tariff},
        {"transactionId", k.transactionId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ChangeTransactionTariffRequest& k) {
    // the required parts of the message
    k.tariff = j.at("tariff");
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ChangeTransactionTariffRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the ChangeTransactionTariffRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeTransactionTariffRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ChangeTransactionTariffResponse::get_type() const {
    return "ChangeTransactionTariffResponse";
}

void to_json(json& j, const ChangeTransactionTariffResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::tariff_change_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ChangeTransactionTariffResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_tariff_change_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ChangeTransactionTariffResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the ChangeTransactionTariffResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeTransactionTariffResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
