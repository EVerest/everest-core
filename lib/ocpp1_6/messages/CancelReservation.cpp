// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/messages/CancelReservation.hpp>
#include <ocpp1_6/ocpp_types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string CancelReservationRequest::get_type() const {
    return "CancelReservation";
}

void to_json(json& j, const CancelReservationRequest& k) {
    // the required parts of the message
    j = json{
        {"reservationId", k.reservationId},
    };
    // the optional parts of the message
}

void from_json(const json& j, CancelReservationRequest& k) {
    // the required parts of the message
    k.reservationId = j.at("reservationId");

    // the optional parts of the message
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
        {"status", conversions::cancel_reservation_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, CancelReservationResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_cancel_reservation_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given CancelReservationResponse \p k to the given output stream \p os
/// \returns an output stream with the CancelReservationResponse written to
std::ostream& operator<<(std::ostream& os, const CancelReservationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
