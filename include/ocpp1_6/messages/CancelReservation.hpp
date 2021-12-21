// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CANCELRESERVATION_HPP
#define OCPP1_6_CANCELRESERVATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct CancelReservationRequest : public Message {
    int32_t reservationId;

    std::string get_type() const {
        return "CancelReservation";
    }

    friend void to_json(json& j, const CancelReservationRequest& k) {
        // the required parts of the message
        j = json{
            {"reservationId", k.reservationId},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, CancelReservationRequest& k) {
        // the required parts of the message
        k.reservationId = j.at("reservationId");

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const CancelReservationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct CancelReservationResponse : public Message {
    CancelReservationStatus status;

    std::string get_type() const {
        return "CancelReservationResponse";
    }

    friend void to_json(json& j, const CancelReservationResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::cancel_reservation_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, CancelReservationResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_cancel_reservation_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const CancelReservationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CANCELRESERVATION_HPP
