// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_SENDLOCALLIST_HPP
#define OCPP_V201_SENDLOCALLIST_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP SendLocalList message
struct SendLocalListRequest : public ocpp::Message {
    int32_t versionNumber;
    UpdateEnum updateType;
    std::optional<CustomData> customData;
    std::optional<std::vector<AuthorizationData>> localAuthorizationList;

    /// \brief Provides the type of this SendLocalList message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SendLocalListRequest \p k to a given json object \p j
void to_json(json& j, const SendLocalListRequest& k);

/// \brief Conversion from a given json object \p j to a given SendLocalListRequest \p k
void from_json(const json& j, SendLocalListRequest& k);

/// \brief Writes the string representation of the given SendLocalListRequest \p k to the given output stream \p os
/// \returns an output stream with the SendLocalListRequest written to
std::ostream& operator<<(std::ostream& os, const SendLocalListRequest& k);

/// \brief Contains a OCPP SendLocalListResponse message
struct SendLocalListResponse : public ocpp::Message {
    SendLocalListStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this SendLocalListResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SendLocalListResponse \p k to a given json object \p j
void to_json(json& j, const SendLocalListResponse& k);

/// \brief Conversion from a given json object \p j to a given SendLocalListResponse \p k
void from_json(const json& j, SendLocalListResponse& k);

/// \brief Writes the string representation of the given SendLocalListResponse \p k to the given output stream \p os
/// \returns an output stream with the SendLocalListResponse written to
std::ostream& operator<<(std::ostream& os, const SendLocalListResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_SENDLOCALLIST_HPP
