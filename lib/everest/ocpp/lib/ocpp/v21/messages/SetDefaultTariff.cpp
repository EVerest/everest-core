// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/SetDefaultTariff.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string SetDefaultTariffRequest::get_type() const {
    return "SetDefaultTariff";
}

void to_json(json& j, const SetDefaultTariffRequest& k) {
    // the required parts of the message
    j = json{
        {"evseId", k.evseId},
        {"tariff", k.tariff},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetDefaultTariffRequest& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");
    k.tariff = j.at("tariff");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetDefaultTariffRequest \p k to the given output stream \p os
/// \returns an output stream with the SetDefaultTariffRequest written to
std::ostream& operator<<(std::ostream& os, const SetDefaultTariffRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetDefaultTariffResponse::get_type() const {
    return "SetDefaultTariffResponse";
}

void to_json(json& j, const SetDefaultTariffResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::tariff_set_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetDefaultTariffResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_tariff_set_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetDefaultTariffResponse \p k to the given output stream \p os
/// \returns an output stream with the SetDefaultTariffResponse written to
std::ostream& operator<<(std::ostream& os, const SetDefaultTariffResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
