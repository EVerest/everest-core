// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_REPORTCHARGINGPROFILES_HPP
#define OCPP_V201_REPORTCHARGINGPROFILES_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP ReportChargingProfiles message
struct ReportChargingProfilesRequest : public ocpp::Message {
    int32_t requestId;
    ChargingLimitSourceEnum chargingLimitSource;
    std::vector<ChargingProfile> chargingProfile;
    int32_t evseId;
    std::optional<CustomData> customData;
    std::optional<bool> tbc;

    /// \brief Provides the type of this ReportChargingProfiles message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
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
    std::string get_type() const;
};

/// \brief Conversion from a given ReportChargingProfilesResponse \p k to a given json object \p j
void to_json(json& j, const ReportChargingProfilesResponse& k);

/// \brief Conversion from a given json object \p j to a given ReportChargingProfilesResponse \p k
void from_json(const json& j, ReportChargingProfilesResponse& k);

/// \brief Writes the string representation of the given ReportChargingProfilesResponse \p k to the given output stream
/// \p os \returns an output stream with the ReportChargingProfilesResponse written to
std::ostream& operator<<(std::ostream& os, const ReportChargingProfilesResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_REPORTCHARGINGPROFILES_HPP
