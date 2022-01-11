// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_RESERVENOW_HPP
#define OCPP1_6_RESERVENOW_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ReserveNow message
struct ReserveNowRequest : public Message {
    int32_t connectorId;
    DateTime expiryDate;
    CiString20Type idTag;
    int32_t reservationId;
    boost::optional<CiString20Type> parentIdTag;

    /// \brief Provides the type of this ReserveNow message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ReserveNow";
    }

    /// \brief Conversion from a given ReserveNowRequest \p k to a given json object \p j
    friend void to_json(json& j, const ReserveNowRequest& k) {
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

    /// \brief Conversion from a given json object \p j to a given ReserveNowRequest \p k
    friend void from_json(const json& j, ReserveNowRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.expiryDate = DateTime(std::string(j.at("expiryDate")));
        ;
        k.idTag = j.at("idTag");
        k.reservationId = j.at("reservationId");

        // the optional parts of the message
        if (j.contains("parentIdTag")) {
            k.parentIdTag.emplace(j.at("parentIdTag"));
        }
    }

    /// \brief Writes the string representation of the given ReserveNowRequest \p k to the given output stream \p os
    /// \returns an output stream with the ReserveNowRequest written to
    friend std::ostream& operator<<(std::ostream& os, const ReserveNowRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 ReserveNowResponse message
struct ReserveNowResponse : public Message {
    ReservationStatus status;

    /// \brief Provides the type of this ReserveNowResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ReserveNowResponse";
    }

    /// \brief Conversion from a given ReserveNowResponse \p k to a given json object \p j
    friend void to_json(json& j, const ReserveNowResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::reservation_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ReserveNowResponse \p k
    friend void from_json(const json& j, ReserveNowResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_reservation_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ReserveNowResponse \p k to the given output stream \p os
    /// \returns an output stream with the ReserveNowResponse written to
    friend std::ostream& operator<<(std::ostream& os, const ReserveNowResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_RESERVENOW_HPP
