// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_GETCERTIFICATECHAINSTATUS_HPP
#define OCPP_V21_GETCERTIFICATECHAINSTATUS_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP GetCertificateChainStatus message
struct GetCertificateChainStatusRequest : public ocpp::Message {
    std::vector<CertificateStatusRequestInfo> certificateStatusRequests;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetCertificateChainStatus message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetCertificateChainStatusRequest \p k to a given json object \p j
void to_json(json& j, const GetCertificateChainStatusRequest& k);

/// \brief Conversion from a given json object \p j to a given GetCertificateChainStatusRequest \p k
void from_json(const json& j, GetCertificateChainStatusRequest& k);

/// \brief Writes the string representation of the given GetCertificateChainStatusRequest \p k to the given output
/// stream \p os
/// \returns an output stream with the GetCertificateChainStatusRequest written to
std::ostream& operator<<(std::ostream& os, const GetCertificateChainStatusRequest& k);

/// \brief Contains a OCPP GetCertificateChainStatusResponse message
struct GetCertificateChainStatusResponse : public ocpp::Message {
    std::vector<CertificateStatus> certificateStatus;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetCertificateChainStatusResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetCertificateChainStatusResponse \p k to a given json object \p j
void to_json(json& j, const GetCertificateChainStatusResponse& k);

/// \brief Conversion from a given json object \p j to a given GetCertificateChainStatusResponse \p k
void from_json(const json& j, GetCertificateChainStatusResponse& k);

/// \brief Writes the string representation of the given GetCertificateChainStatusResponse \p k to the given output
/// stream \p os
/// \returns an output stream with the GetCertificateChainStatusResponse written to
std::ostream& operator<<(std::ostream& os, const GetCertificateChainStatusResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_GETCERTIFICATECHAINSTATUS_HPP
