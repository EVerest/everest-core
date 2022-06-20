// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_DELETECERTIFICATE_HPP
#define OCPP1_6_DELETECERTIFICATE_HPP

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 DeleteCertificate message
struct DeleteCertificateRequest : public Message {
    CertificateHashDataType certificateHashData;

    /// \brief Provides the type of this DeleteCertificate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given DeleteCertificateRequest \p k to a given json object \p j
void to_json(json& j, const DeleteCertificateRequest& k);

/// \brief Conversion from a given json object \p j to a given DeleteCertificateRequest \p k
void from_json(const json& j, DeleteCertificateRequest& k);

/// \brief Writes the string representation of the given DeleteCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the DeleteCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateRequest& k);

/// \brief Contains a OCPP 1.6 DeleteCertificateResponse message
struct DeleteCertificateResponse : public Message {
    DeleteCertificateStatusEnumType status;

    /// \brief Provides the type of this DeleteCertificateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given DeleteCertificateResponse \p k to a given json object \p j
void to_json(json& j, const DeleteCertificateResponse& k);

/// \brief Conversion from a given json object \p j to a given DeleteCertificateResponse \p k
void from_json(const json& j, DeleteCertificateResponse& k);

/// \brief Writes the string representation of the given DeleteCertificateResponse \p k to the given output stream \p os
/// \returns an output stream with the DeleteCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_DELETECERTIFICATE_HPP
