// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/ChangeConfiguration.hpp>

#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string ChangeConfigurationRequest::get_type() const {
    return "ChangeConfiguration";
}

void to_json(json& j, const ChangeConfigurationRequest& k) {
    // the required parts of the message
    j = json{
        {"key", k.key},
        {"value", k.value},
    };
    // the optional parts of the message
}

void from_json(const json& j, ChangeConfigurationRequest& k) {
    // the required parts of the message
    k.key = j.at("key");
    k.value = j.at("value");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ChangeConfigurationRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the ChangeConfigurationRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeConfigurationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ChangeConfigurationResponse::get_type() const {
    return "ChangeConfigurationResponse";
}

void to_json(json& j, const ChangeConfigurationResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::configuration_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ChangeConfigurationResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_configuration_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ChangeConfigurationResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the ChangeConfigurationResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeConfigurationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
