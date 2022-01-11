// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CLEARCHARGINGPROFILE_HPP
#define OCPP1_6_CLEARCHARGINGPROFILE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ClearChargingProfile message
struct ClearChargingProfileRequest : public Message {
    boost::optional<int32_t> id;
    boost::optional<int32_t> connectorId;
    boost::optional<ChargingProfilePurposeType> chargingProfilePurpose;
    boost::optional<int32_t> stackLevel;

    /// \brief Provides the type of this ClearChargingProfile message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ClearChargingProfile";
    }

    /// \brief Conversion from a given ClearChargingProfileRequest \p k to a given json object \p j
    friend void to_json(json& j, const ClearChargingProfileRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
        if (k.id) {
            j["id"] = k.id.value();
        }
        if (k.connectorId) {
            j["connectorId"] = k.connectorId.value();
        }
        if (k.chargingProfilePurpose) {
            j["chargingProfilePurpose"] =
                conversions::charging_profile_purpose_type_to_string(k.chargingProfilePurpose.value());
        }
        if (k.stackLevel) {
            j["stackLevel"] = k.stackLevel.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given ClearChargingProfileRequest \p k
    friend void from_json(const json& j, ClearChargingProfileRequest& k) {
        // the required parts of the message

        // the optional parts of the message
        if (j.contains("id")) {
            k.id.emplace(j.at("id"));
        }
        if (j.contains("connectorId")) {
            k.connectorId.emplace(j.at("connectorId"));
        }
        if (j.contains("chargingProfilePurpose")) {
            k.chargingProfilePurpose.emplace(
                conversions::string_to_charging_profile_purpose_type(j.at("chargingProfilePurpose")));
        }
        if (j.contains("stackLevel")) {
            k.stackLevel.emplace(j.at("stackLevel"));
        }
    }

    /// \brief Writes the string representation of the given ClearChargingProfileRequest \p k to the given output stream
    /// \p os \returns an output stream with the ClearChargingProfileRequest written to
    friend std::ostream& operator<<(std::ostream& os, const ClearChargingProfileRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 ClearChargingProfileResponse message
struct ClearChargingProfileResponse : public Message {
    ClearChargingProfileStatus status;

    /// \brief Provides the type of this ClearChargingProfileResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ClearChargingProfileResponse";
    }

    /// \brief Conversion from a given ClearChargingProfileResponse \p k to a given json object \p j
    friend void to_json(json& j, const ClearChargingProfileResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::clear_charging_profile_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ClearChargingProfileResponse \p k
    friend void from_json(const json& j, ClearChargingProfileResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_clear_charging_profile_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ClearChargingProfileResponse \p k to the given output
    /// stream \p os \returns an output stream with the ClearChargingProfileResponse written to
    friend std::ostream& operator<<(std::ostream& os, const ClearChargingProfileResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CLEARCHARGINGPROFILE_HPP
