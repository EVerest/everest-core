// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_CLEARCHARGINGPROFILE_HPP
#define OCPP_V16_CLEARCHARGINGPROFILE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP ClearChargingProfile message
struct ClearChargingProfileRequest : public ocpp::Message {
    std::optional<int32_t> id;
    std::optional<int32_t> connectorId;
    std::optional<ChargingProfilePurposeType> chargingProfilePurpose;
    std::optional<int32_t> stackLevel;

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

/// \brief Contains a OCPP ClearChargingProfileResponse message
struct ClearChargingProfileResponse : public ocpp::Message {
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

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CLEARCHARGINGPROFILE_HPP
