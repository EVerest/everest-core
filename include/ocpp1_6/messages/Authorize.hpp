// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_AUTHORIZE_HPP
#define OCPP1_6_AUTHORIZE_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 Authorize message
struct AuthorizeRequest : public Message {
    CiString20Type idTag;

    /// \brief Provides the type of this Authorize message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given AuthorizeRequest \p k to a given json object \p j
void to_json(json& j, const AuthorizeRequest& k);

/// \brief Conversion from a given json object \p j to a given AuthorizeRequest \p k
void from_json(const json& j, AuthorizeRequest& k);

/// \brief Writes the string representation of the given AuthorizeRequest \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeRequest written to
std::ostream& operator<<(std::ostream& os, const AuthorizeRequest& k);

/// \brief Contains a OCPP 1.6 AuthorizeResponse message
struct AuthorizeResponse : public Message {
    IdTagInfo idTagInfo;

    /// \brief Provides the type of this AuthorizeResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given AuthorizeResponse \p k to a given json object \p j
void to_json(json& j, const AuthorizeResponse& k);

/// \brief Conversion from a given json object \p j to a given AuthorizeResponse \p k
void from_json(const json& j, AuthorizeResponse& k);

/// \brief Writes the string representation of the given AuthorizeResponse \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeResponse written to
std::ostream& operator<<(std::ostream& os, const AuthorizeResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_AUTHORIZE_HPP
