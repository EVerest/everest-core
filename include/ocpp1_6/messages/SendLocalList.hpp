// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SENDLOCALLIST_HPP
#define OCPP1_6_SENDLOCALLIST_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 SendLocalList message
struct SendLocalListRequest : public Message {
    int32_t listVersion;
    UpdateType updateType;
    boost::optional<std::vector<LocalAuthorizationList>> localAuthorizationList;

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

/// \brief Contains a OCPP 1.6 SendLocalListResponse message
struct SendLocalListResponse : public Message {
    UpdateStatus status;

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

} // namespace ocpp1_6

#endif // OCPP1_6_SENDLOCALLIST_HPP
