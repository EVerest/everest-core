// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_SETNETWORKPROFILE_HPP
#define OCPP_V201_SETNETWORKPROFILE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP SetNetworkProfile message
struct SetNetworkProfileRequest : public ocpp::Message {
    int32_t configurationSlot;
    NetworkConnectionProfile connectionData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetNetworkProfile message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetNetworkProfileRequest \p k to a given json object \p j
void to_json(json& j, const SetNetworkProfileRequest& k);

/// \brief Conversion from a given json object \p j to a given SetNetworkProfileRequest \p k
void from_json(const json& j, SetNetworkProfileRequest& k);

/// \brief Writes the string representation of the given SetNetworkProfileRequest \p k to the given output stream \p os
/// \returns an output stream with the SetNetworkProfileRequest written to
std::ostream& operator<<(std::ostream& os, const SetNetworkProfileRequest& k);

/// \brief Contains a OCPP SetNetworkProfileResponse message
struct SetNetworkProfileResponse : public ocpp::Message {
    SetNetworkProfileStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this SetNetworkProfileResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetNetworkProfileResponse \p k to a given json object \p j
void to_json(json& j, const SetNetworkProfileResponse& k);

/// \brief Conversion from a given json object \p j to a given SetNetworkProfileResponse \p k
void from_json(const json& j, SetNetworkProfileResponse& k);

/// \brief Writes the string representation of the given SetNetworkProfileResponse \p k to the given output stream \p os
/// \returns an output stream with the SetNetworkProfileResponse written to
std::ostream& operator<<(std::ostream& os, const SetNetworkProfileResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_SETNETWORKPROFILE_HPP
