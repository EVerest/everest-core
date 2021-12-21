// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SETCHARGINGPROFILE_HPP
#define OCPP1_6_SETCHARGINGPROFILE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct SetChargingProfileRequest : public Message {
    int32_t connectorId;
    ChargingProfile csChargingProfiles;

    std::string get_type() const {
        return "SetChargingProfile";
    }

    friend void to_json(json& j, const SetChargingProfileRequest& k) {
        // the required parts of the message
        j = json{
            {"connectorId", k.connectorId},
            {"csChargingProfiles", k.csChargingProfiles},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, SetChargingProfileRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.csChargingProfiles = j.at("csChargingProfiles");

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const SetChargingProfileRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct SetChargingProfileResponse : public Message {
    ChargingProfileStatus status;

    std::string get_type() const {
        return "SetChargingProfileResponse";
    }

    friend void to_json(json& j, const SetChargingProfileResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::charging_profile_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, SetChargingProfileResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_charging_profile_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const SetChargingProfileResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_SETCHARGINGPROFILE_HPP
