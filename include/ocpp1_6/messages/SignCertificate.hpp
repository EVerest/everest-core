// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SIGNCERTIFICATE_HPP
#define OCPP1_6_SIGNCERTIFICATE_HPP

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 SignCertificate message
struct SignCertificateRequest : public Message {
    CiString5500Type csr;

    /// \brief Provides the type of this SignCertificate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SignCertificateRequest \p k to a given json object \p j
void to_json(json& j, const SignCertificateRequest& k);

/// \brief Conversion from a given json object \p j to a given SignCertificateRequest \p k
void from_json(const json& j, SignCertificateRequest& k);

/// \brief Writes the string representation of the given SignCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const SignCertificateRequest& k);

/// \brief Contains a OCPP 1.6 SignCertificateResponse message
struct SignCertificateResponse : public Message {
    GenericStatusEnumType status;

    /// \brief Provides the type of this SignCertificateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SignCertificateResponse \p k to a given json object \p j
void to_json(json& j, const SignCertificateResponse& k);

/// \brief Conversion from a given json object \p j to a given SignCertificateResponse \p k
void from_json(const json& j, SignCertificateResponse& k);

/// \brief Writes the string representation of the given SignCertificateResponse \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const SignCertificateResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_SIGNCERTIFICATE_HPP
