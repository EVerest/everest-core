// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SETCHARGINGPROFILE_HPP
#define OCPP1_6_SETCHARGINGPROFILE_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 SetChargingProfile message
struct SetChargingProfileRequest : public Message {
    int32_t connectorId;
    ChargingProfile csChargingProfiles;

    /// \brief Provides the type of this SetChargingProfile message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetChargingProfileRequest \p k to a given json object \p j
void to_json(json& j, const SetChargingProfileRequest& k);

/// \brief Conversion from a given json object \p j to a given SetChargingProfileRequest \p k
void from_json(const json& j, SetChargingProfileRequest& k);

/// \brief Writes the string representation of the given SetChargingProfileRequest \p k to the given output stream \p os
/// \returns an output stream with the SetChargingProfileRequest written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileRequest& k);

/// \brief Contains a OCPP 1.6 SetChargingProfileResponse message
struct SetChargingProfileResponse : public Message {
    ChargingProfileStatus status;

    /// \brief Provides the type of this SetChargingProfileResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetChargingProfileResponse \p k to a given json object \p j
void to_json(json& j, const SetChargingProfileResponse& k);

/// \brief Conversion from a given json object \p j to a given SetChargingProfileResponse \p k
void from_json(const json& j, SetChargingProfileResponse& k);

/// \brief Writes the string representation of the given SetChargingProfileResponse \p k to the given output stream \p
/// os \returns an output stream with the SetChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_SETCHARGINGPROFILE_HPP
