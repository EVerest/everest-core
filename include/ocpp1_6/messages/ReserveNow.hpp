// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_RESERVENOW_HPP
#define OCPP1_6_RESERVENOW_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct ReserveNowRequest : public Message {
    int32_t connectorId;
    DateTime expiryDate;
    CiString20Type idTag;
    int32_t reservationId;
    boost::optional<CiString20Type> parentIdTag;

    std::string get_type() const {
        return "ReserveNow";
    }

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

    friend std::ostream& operator<<(std::ostream& os, const ReserveNowRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct ReserveNowResponse : public Message {
    ReservationStatus status;

    std::string get_type() const {
        return "ReserveNowResponse";
    }

    friend void to_json(json& j, const ReserveNowResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::reservation_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, ReserveNowResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_reservation_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const ReserveNowResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_RESERVENOW_HPP
