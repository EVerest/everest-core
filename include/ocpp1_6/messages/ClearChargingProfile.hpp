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
    std::string get_type() const;
};

/// \brief Conversion from a given ClearChargingProfileRequest \p k to a given json object \p j
void to_json(json& j, const ClearChargingProfileRequest& k);

/// \brief Conversion from a given json object \p j to a given ClearChargingProfileRequest \p k
void from_json(const json& j, ClearChargingProfileRequest& k);

/// \brief Writes the string representation of the given ClearChargingProfileRequest \p k to the given output stream \p
/// os \returns an output stream with the ClearChargingProfileRequest written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileRequest& k);

/// \brief Contains a OCPP 1.6 ClearChargingProfileResponse message
struct ClearChargingProfileResponse : public Message {
    ClearChargingProfileStatus status;

    /// \brief Provides the type of this ClearChargingProfileResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ClearChargingProfileResponse \p k to a given json object \p j
void to_json(json& j, const ClearChargingProfileResponse& k);

/// \brief Conversion from a given json object \p j to a given ClearChargingProfileResponse \p k
void from_json(const json& j, ClearChargingProfileResponse& k);

/// \brief Writes the string representation of the given ClearChargingProfileResponse \p k to the given output stream \p
/// os \returns an output stream with the ClearChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_CLEARCHARGINGPROFILE_HPP
