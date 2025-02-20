// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_AUTHORIZE_HPP
#define OCPP_V2_AUTHORIZE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP Authorize message
struct AuthorizeRequest : public ocpp::Message {
    IdToken idToken;
    std::optional<CustomData> customData;
    std::optional<CiString<5500>> certificate;
    std::optional<std::vector<OCSPRequestData>> iso15118CertificateHashData;

    /// \brief Provides the type of this Authorize message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
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
    IdTokenInfo idTokenInfo;
    std::optional<CustomData> customData;
    std::optional<AuthorizeCertificateStatusEnum> certificateStatus;

    /// \brief Provides the type of this AuthorizeResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given AuthorizeResponse \p k to a given json object \p j
void to_json(json& j, const AuthorizeResponse& k);

/// \brief Conversion from a given json object \p j to a given AuthorizeResponse \p k
void from_json(const json& j, AuthorizeResponse& k);

/// \brief Writes the string representation of the given AuthorizeResponse \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeResponse written to
std::ostream& operator<<(std::ostream& os, const AuthorizeResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_AUTHORIZE_HPP
