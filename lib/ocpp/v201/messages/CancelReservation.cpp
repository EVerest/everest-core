// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/CancelReservation.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string CancelReservationRequest::get_type() const {
    return "CancelReservation";
}

void to_json(json& j, const CancelReservationRequest& k) {
    // the required parts of the message
    j = json{
        {"reservationId", k.reservationId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, CancelReservationRequest& k) {
    // the required parts of the message
    k.reservationId = j.at("reservationId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given CancelReservationRequest \p k to the given output stream \p os
/// \returns an output stream with the CancelReservationRequest written to
std::ostream& operator<<(std::ostream& os, const CancelReservationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string CancelReservationResponse::get_type() const {
    return "CancelReservationResponse";
}

void to_json(json& j, const CancelReservationResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::cancel_reservation_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, CancelReservationResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_cancel_reservation_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given CancelReservationResponse \p k to the given output stream \p os
/// \returns an output stream with the CancelReservationResponse written to
std::ostream& operator<<(std::ostream& os, const CancelReservationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
