// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/ReserveNow.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string ReserveNowRequest::get_type() const {
    return "ReserveNow";
}

void to_json(json& j, const ReserveNowRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
        {"expiryDate", k.expiryDate.to_rfc3339()},
        {"idTag", k.idTag},
        {"reservationId", k.reservationId},
    };
    // the optional parts of the message
    if (k.parentIdTag) {
        j["parentIdTag"] = k.parentIdTag.value();
    }
}

void from_json(const json& j, ReserveNowRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    k.expiryDate = ocpp::DateTime(std::string(j.at("expiryDate")));
    k.idTag = j.at("idTag");
    k.reservationId = j.at("reservationId");

    // the optional parts of the message
    if (j.contains("parentIdTag")) {
        k.parentIdTag.emplace(j.at("parentIdTag"));
    }
}

/// \brief Writes the string representation of the given ReserveNowRequest \p k to the given output stream \p os
/// \returns an output stream with the ReserveNowRequest written to
std::ostream& operator<<(std::ostream& os, const ReserveNowRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ReserveNowResponse::get_type() const {
    return "ReserveNowResponse";
}

void to_json(json& j, const ReserveNowResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::reservation_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ReserveNowResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_reservation_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ReserveNowResponse \p k to the given output stream \p os
/// \returns an output stream with the ReserveNowResponse written to
std::ostream& operator<<(std::ostream& os, const ReserveNowResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
