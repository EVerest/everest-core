// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_SETCHARGINGPROFILE_HPP
#define OCPP_V2_SETCHARGINGPROFILE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP SetChargingProfile message
struct SetChargingProfileRequest : public ocpp::Message {
    int32_t evseId;
    ChargingProfile chargingProfile;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetChargingProfile message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetChargingProfileRequest \p k to a given json object \p j
void to_json(json& j, const SetChargingProfileRequest& k);

/// \brief Conversion from a given json object \p j to a given SetChargingProfileRequest \p k
void from_json(const json& j, SetChargingProfileRequest& k);

/// \brief Writes the string representation of the given SetChargingProfileRequest \p k to the given output stream \p os
/// \returns an output stream with the SetChargingProfileRequest written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileRequest& k);

/// \brief Contains a OCPP SetChargingProfileResponse message
struct SetChargingProfileResponse : public ocpp::Message {
    ChargingProfileStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this SetChargingProfileResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetChargingProfileResponse \p k to a given json object \p j
void to_json(json& j, const SetChargingProfileResponse& k);

/// \brief Conversion from a given json object \p j to a given SetChargingProfileResponse \p k
void from_json(const json& j, SetChargingProfileResponse& k);

/// \brief Writes the string representation of the given SetChargingProfileResponse \p k to the given output stream \p
/// os \returns an output stream with the SetChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_SETCHARGINGPROFILE_HPP
