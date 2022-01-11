// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SETCHARGINGPROFILE_HPP
#define OCPP1_6_SETCHARGINGPROFILE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 SetChargingProfile message
struct SetChargingProfileRequest : public Message {
    int32_t connectorId;
    ChargingProfile csChargingProfiles;

    /// \brief Provides the type of this SetChargingProfile message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "SetChargingProfile";
    }

    /// \brief Conversion from a given SetChargingProfileRequest \p k to a given json object \p j
    friend void to_json(json& j, const SetChargingProfileRequest& k) {
        // the required parts of the message
        j = json{
            {"connectorId", k.connectorId},
            {"csChargingProfiles", k.csChargingProfiles},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given SetChargingProfileRequest \p k
    friend void from_json(const json& j, SetChargingProfileRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.csChargingProfiles = j.at("csChargingProfiles");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given SetChargingProfileRequest \p k to the given output stream
    /// \p os \returns an output stream with the SetChargingProfileRequest written to
    friend std::ostream& operator<<(std::ostream& os, const SetChargingProfileRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 SetChargingProfileResponse message
struct SetChargingProfileResponse : public Message {
    ChargingProfileStatus status;

    /// \brief Provides the type of this SetChargingProfileResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "SetChargingProfileResponse";
    }

    /// \brief Conversion from a given SetChargingProfileResponse \p k to a given json object \p j
    friend void to_json(json& j, const SetChargingProfileResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::charging_profile_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given SetChargingProfileResponse \p k
    friend void from_json(const json& j, SetChargingProfileResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_charging_profile_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given SetChargingProfileResponse \p k to the given output stream
    /// \p os \returns an output stream with the SetChargingProfileResponse written to
    friend std::ostream& operator<<(std::ostream& os, const SetChargingProfileResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_SETCHARGINGPROFILE_HPP
