// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_REPORTCHARGINGPROFILES_HPP
#define OCPP_V2_REPORTCHARGINGPROFILES_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP ReportChargingProfiles message
struct ReportChargingProfilesRequest : public ocpp::Message {
    int32_t requestId;
    CiString<20> chargingLimitSource;
    std::vector<ChargingProfile> chargingProfile;
    int32_t evseId;
    std::optional<bool> tbc;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ReportChargingProfiles message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ReportChargingProfilesRequest \p k to a given json object \p j
void to_json(json& j, const ReportChargingProfilesRequest& k);

/// \brief Conversion from a given json object \p j to a given ReportChargingProfilesRequest \p k
void from_json(const json& j, ReportChargingProfilesRequest& k);

/// \brief Writes the string representation of the given ReportChargingProfilesRequest \p k to the given output stream
/// \p os \returns an output stream with the ReportChargingProfilesRequest written to
std::ostream& operator<<(std::ostream& os, const ReportChargingProfilesRequest& k);

/// \brief Contains a OCPP ReportChargingProfilesResponse message
struct ReportChargingProfilesResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ReportChargingProfilesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ReportChargingProfilesResponse \p k to a given json object \p j
void to_json(json& j, const ReportChargingProfilesResponse& k);

/// \brief Conversion from a given json object \p j to a given ReportChargingProfilesResponse \p k
void from_json(const json& j, ReportChargingProfilesResponse& k);

/// \brief Writes the string representation of the given ReportChargingProfilesResponse \p k to the given output stream
/// \p os \returns an output stream with the ReportChargingProfilesResponse written to
std::ostream& operator<<(std::ostream& os, const ReportChargingProfilesResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_REPORTCHARGINGPROFILES_HPP
