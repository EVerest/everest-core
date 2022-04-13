// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/messages/ChangeAvailability.hpp>
#include <ocpp1_6/ocpp_types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string ChangeAvailabilityRequest::get_type() const {
    return "ChangeAvailability";
}

void to_json(json& j, const ChangeAvailabilityRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
        {"type", conversions::availability_type_to_string(k.type)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ChangeAvailabilityRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    k.type = conversions::string_to_availability_type(j.at("type"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ChangeAvailabilityRequest \p k to the given output stream \p os
/// \returns an output stream with the ChangeAvailabilityRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ChangeAvailabilityResponse::get_type() const {
    return "ChangeAvailabilityResponse";
}

void to_json(json& j, const ChangeAvailabilityResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::availability_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ChangeAvailabilityResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_availability_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ChangeAvailabilityResponse \p k to the given output stream \p
/// os \returns an output stream with the ChangeAvailabilityResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
