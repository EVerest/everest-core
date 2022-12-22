// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_AUTHORIZE_HPP
#define OCPP_V16_AUTHORIZE_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP Authorize message
struct AuthorizeRequest : public ocpp::Message {
    CiString<20> idTag;

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

/// \brief Contains a OCPP AuthorizeResponse message
struct AuthorizeResponse : public ocpp::Message {
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

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_AUTHORIZE_HPP
