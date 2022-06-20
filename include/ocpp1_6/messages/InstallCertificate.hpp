// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_INSTALLCERTIFICATE_HPP
#define OCPP1_6_INSTALLCERTIFICATE_HPP

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 InstallCertificate message
struct InstallCertificateRequest : public Message {
    CertificateUseEnumType certificateType;
    CiString5500Type certificate;

    /// \brief Provides the type of this InstallCertificate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given InstallCertificateRequest \p k to a given json object \p j
void to_json(json& j, const InstallCertificateRequest& k);

/// \brief Conversion from a given json object \p j to a given InstallCertificateRequest \p k
void from_json(const json& j, InstallCertificateRequest& k);

/// \brief Writes the string representation of the given InstallCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the InstallCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateRequest& k);

/// \brief Contains a OCPP 1.6 InstallCertificateResponse message
struct InstallCertificateResponse : public Message {
    InstallCertificateStatusEnumType status;

    /// \brief Provides the type of this InstallCertificateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given InstallCertificateResponse \p k to a given json object \p j
void to_json(json& j, const InstallCertificateResponse& k);

/// \brief Conversion from a given json object \p j to a given InstallCertificateResponse \p k
void from_json(const json& j, InstallCertificateResponse& k);

/// \brief Writes the string representation of the given InstallCertificateResponse \p k to the given output stream \p
/// os \returns an output stream with the InstallCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_INSTALLCERTIFICATE_HPP
