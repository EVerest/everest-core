// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_SENDLOCALLIST_HPP
#define OCPP_V16_SENDLOCALLIST_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP SendLocalList message
struct SendLocalListRequest : public ocpp::Message {
    int32_t listVersion;
    UpdateType updateType;
    std::optional<std::vector<LocalAuthorizationList>> localAuthorizationList;

    /// \brief Provides the type of this SendLocalList message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
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
    UpdateStatus status;

    /// \brief Provides the type of this SendLocalListResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SendLocalListResponse \p k to a given json object \p j
void to_json(json& j, const SendLocalListResponse& k);

/// \brief Conversion from a given json object \p j to a given SendLocalListResponse \p k
void from_json(const json& j, SendLocalListResponse& k);

/// \brief Writes the string representation of the given SendLocalListResponse \p k to the given output stream \p os
/// \returns an output stream with the SendLocalListResponse written to
std::ostream& operator<<(std::ostream& os, const SendLocalListResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_SENDLOCALLIST_HPP
