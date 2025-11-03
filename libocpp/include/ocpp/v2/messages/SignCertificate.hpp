// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_SIGNCERTIFICATE_HPP
#define OCPP_V2_SIGNCERTIFICATE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP SignCertificate message
struct SignCertificateRequest : public ocpp::Message {
    CiString<5500> csr;
    std::optional<CertificateSigningUseEnum> certificateType;
    std::optional<CertificateHashDataType> hashRootCertificate;
    std::optional<int32_t> requestId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SignCertificate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SignCertificateRequest \p k to a given json object \p j
void to_json(json& j, const SignCertificateRequest& k);

/// \brief Conversion from a given json object \p j to a given SignCertificateRequest \p k
void from_json(const json& j, SignCertificateRequest& k);

/// \brief Writes the string representation of the given SignCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const SignCertificateRequest& k);

/// \brief Contains a OCPP SignCertificateResponse message
struct SignCertificateResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SignCertificateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SignCertificateResponse \p k to a given json object \p j
void to_json(json& j, const SignCertificateResponse& k);

/// \brief Conversion from a given json object \p j to a given SignCertificateResponse \p k
void from_json(const json& j, SignCertificateResponse& k);

/// \brief Writes the string representation of the given SignCertificateResponse \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const SignCertificateResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_SIGNCERTIFICATE_HPP
