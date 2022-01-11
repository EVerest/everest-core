// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CANCELRESERVATION_HPP
#define OCPP1_6_CANCELRESERVATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 CancelReservation message
struct CancelReservationRequest : public Message {
    int32_t reservationId;

    /// \brief Provides the type of this CancelReservation message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "CancelReservation";
    }

    /// \brief Conversion from a given CancelReservationRequest \p k to a given json object \p j
    friend void to_json(json& j, const CancelReservationRequest& k) {
        // the required parts of the message
        j = json{
            {"reservationId", k.reservationId},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given CancelReservationRequest \p k
    friend void from_json(const json& j, CancelReservationRequest& k) {
        // the required parts of the message
        k.reservationId = j.at("reservationId");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given CancelReservationRequest \p k to the given output stream \p
    /// os \returns an output stream with the CancelReservationRequest written to
    friend std::ostream& operator<<(std::ostream& os, const CancelReservationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 CancelReservationResponse message
struct CancelReservationResponse : public Message {
    CancelReservationStatus status;

    /// \brief Provides the type of this CancelReservationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "CancelReservationResponse";
    }

    /// \brief Conversion from a given CancelReservationResponse \p k to a given json object \p j
    friend void to_json(json& j, const CancelReservationResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::cancel_reservation_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given CancelReservationResponse \p k
    friend void from_json(const json& j, CancelReservationResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_cancel_reservation_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given CancelReservationResponse \p k to the given output stream
    /// \p os \returns an output stream with the CancelReservationResponse written to
    friend std::ostream& operator<<(std::ostream& os, const CancelReservationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CANCELRESERVATION_HPP
