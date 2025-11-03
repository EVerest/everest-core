// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_GETCHARGINGPROFILES_HPP
#define OCPP_V2_GETCHARGINGPROFILES_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP GetChargingProfiles message
struct GetChargingProfilesRequest : public ocpp::Message {
    int32_t requestId;
    ChargingProfileCriterion chargingProfile;
    std::optional<int32_t> evseId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetChargingProfiles message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetChargingProfilesRequest \p k to a given json object \p j
void to_json(json& j, const GetChargingProfilesRequest& k);

/// \brief Conversion from a given json object \p j to a given GetChargingProfilesRequest \p k
void from_json(const json& j, GetChargingProfilesRequest& k);

/// \brief Writes the string representation of the given GetChargingProfilesRequest \p k to the given output stream \p
/// os \returns an output stream with the GetChargingProfilesRequest written to
std::ostream& operator<<(std::ostream& os, const GetChargingProfilesRequest& k);

/// \brief Contains a OCPP GetChargingProfilesResponse message
struct GetChargingProfilesResponse : public ocpp::Message {
    GetChargingProfileStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetChargingProfilesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetChargingProfilesResponse \p k to a given json object \p j
void to_json(json& j, const GetChargingProfilesResponse& k);

/// \brief Conversion from a given json object \p j to a given GetChargingProfilesResponse \p k
void from_json(const json& j, GetChargingProfilesResponse& k);

/// \brief Writes the string representation of the given GetChargingProfilesResponse \p k to the given output stream \p
/// os \returns an output stream with the GetChargingProfilesResponse written to
std::ostream& operator<<(std::ostream& os, const GetChargingProfilesResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_GETCHARGINGPROFILES_HPP
