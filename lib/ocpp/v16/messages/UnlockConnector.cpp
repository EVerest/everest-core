// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/UnlockConnector.hpp>

#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string UnlockConnectorRequest::get_type() const {
    return "UnlockConnector";
}

void to_json(json& j, const UnlockConnectorRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
    };
    // the optional parts of the message
}

void from_json(const json& j, UnlockConnectorRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given UnlockConnectorRequest \p k to the given output stream \p os
/// \returns an output stream with the UnlockConnectorRequest written to
std::ostream& operator<<(std::ostream& os, const UnlockConnectorRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string UnlockConnectorResponse::get_type() const {
    return "UnlockConnectorResponse";
}

void to_json(json& j, const UnlockConnectorResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::unlock_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, UnlockConnectorResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_unlock_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given UnlockConnectorResponse \p k to the given output stream \p os
/// \returns an output stream with the UnlockConnectorResponse written to
std::ostream& operator<<(std::ostream& os, const UnlockConnectorResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
