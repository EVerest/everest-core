// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/ReservationStatusUpdate.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string ReservationStatusUpdateRequest::get_type() const {
    return "ReservationStatusUpdate";
}

void to_json(json& j, const ReservationStatusUpdateRequest& k) {
    // the required parts of the message
    j = json{
        {"reservationId", k.reservationId},
        {"reservationUpdateStatus", conversions::reservation_update_status_enum_to_string(k.reservationUpdateStatus)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ReservationStatusUpdateRequest& k) {
    // the required parts of the message
    k.reservationId = j.at("reservationId");
    k.reservationUpdateStatus = conversions::string_to_reservation_update_status_enum(j.at("reservationUpdateStatus"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ReservationStatusUpdateRequest \p k to the given output stream
/// \p os \returns an output stream with the ReservationStatusUpdateRequest written to
std::ostream& operator<<(std::ostream& os, const ReservationStatusUpdateRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ReservationStatusUpdateResponse::get_type() const {
    return "ReservationStatusUpdateResponse";
}

void to_json(json& j, const ReservationStatusUpdateResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ReservationStatusUpdateResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ReservationStatusUpdateResponse \p k to the given output stream
/// \p os \returns an output stream with the ReservationStatusUpdateResponse written to
std::ostream& operator<<(std::ostream& os, const ReservationStatusUpdateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
