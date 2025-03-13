// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_INSTALLCERTIFICATE_HPP
#define OCPP_V2_INSTALLCERTIFICATE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP InstallCertificate message
struct InstallCertificateRequest : public ocpp::Message {
    InstallCertificateUseEnum certificateType;
    CiString<10000> certificate;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this InstallCertificate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given InstallCertificateRequest \p k to a given json object \p j
void to_json(json& j, const InstallCertificateRequest& k);

/// \brief Conversion from a given json object \p j to a given InstallCertificateRequest \p k
void from_json(const json& j, InstallCertificateRequest& k);

/// \brief Writes the string representation of the given InstallCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the InstallCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateRequest& k);

/// \brief Contains a OCPP InstallCertificateResponse message
struct InstallCertificateResponse : public ocpp::Message {
    InstallCertificateStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this InstallCertificateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given InstallCertificateResponse \p k to a given json object \p j
void to_json(json& j, const InstallCertificateResponse& k);

/// \brief Conversion from a given json object \p j to a given InstallCertificateResponse \p k
void from_json(const json& j, InstallCertificateResponse& k);

/// \brief Writes the string representation of the given InstallCertificateResponse \p k to the given output stream \p
/// os \returns an output stream with the InstallCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_INSTALLCERTIFICATE_HPP
