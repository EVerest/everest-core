// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CERTIFICATESIGNED_HPP
#define OCPP1_6_CERTIFICATESIGNED_HPP

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 CertificateSigned message
struct CertificateSignedRequest : public Message {
    CiString10000Type certificateChain;

    /// \brief Provides the type of this CertificateSigned message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CertificateSignedRequest \p k to a given json object \p j
void to_json(json& j, const CertificateSignedRequest& k);

/// \brief Conversion from a given json object \p j to a given CertificateSignedRequest \p k
void from_json(const json& j, CertificateSignedRequest& k);

/// \brief Writes the string representation of the given CertificateSignedRequest \p k to the given output stream \p os
/// \returns an output stream with the CertificateSignedRequest written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedRequest& k);

/// \brief Contains a OCPP 1.6 CertificateSignedResponse message
struct CertificateSignedResponse : public Message {
    CertificateSignedStatusEnumType status;

    /// \brief Provides the type of this CertificateSignedResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CertificateSignedResponse \p k to a given json object \p j
void to_json(json& j, const CertificateSignedResponse& k);

/// \brief Conversion from a given json object \p j to a given CertificateSignedResponse \p k
void from_json(const json& j, CertificateSignedResponse& k);

/// \brief Writes the string representation of the given CertificateSignedResponse \p k to the given output stream \p os
/// \returns an output stream with the CertificateSignedResponse written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_CERTIFICATESIGNED_HPP
