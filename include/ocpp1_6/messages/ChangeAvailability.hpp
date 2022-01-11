// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHANGEAVAILABILITY_HPP
#define OCPP1_6_CHANGEAVAILABILITY_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ChangeAvailability message
struct ChangeAvailabilityRequest : public Message {
    int32_t connectorId;
    AvailabilityType type;

    /// \brief Provides the type of this ChangeAvailability message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ChangeAvailability";
    }

    /// \brief Conversion from a given ChangeAvailabilityRequest \p k to a given json object \p j
    friend void to_json(json& j, const ChangeAvailabilityRequest& k) {
        // the required parts of the message
        j = json{
            {"connectorId", k.connectorId},
            {"type", conversions::availability_type_to_string(k.type)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ChangeAvailabilityRequest \p k
    friend void from_json(const json& j, ChangeAvailabilityRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.type = conversions::string_to_availability_type(j.at("type"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ChangeAvailabilityRequest \p k to the given output stream
    /// \p os \returns an output stream with the ChangeAvailabilityRequest written to
    friend std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 ChangeAvailabilityResponse message
struct ChangeAvailabilityResponse : public Message {
    AvailabilityStatus status;

    /// \brief Provides the type of this ChangeAvailabilityResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ChangeAvailabilityResponse";
    }

    /// \brief Conversion from a given ChangeAvailabilityResponse \p k to a given json object \p j
    friend void to_json(json& j, const ChangeAvailabilityResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::availability_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ChangeAvailabilityResponse \p k
    friend void from_json(const json& j, ChangeAvailabilityResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_availability_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ChangeAvailabilityResponse \p k to the given output stream
    /// \p os \returns an output stream with the ChangeAvailabilityResponse written to
    friend std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CHANGEAVAILABILITY_HPP
