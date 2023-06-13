// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_DELETECERTIFICATE_HPP
#define OCPP_V201_DELETECERTIFICATE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP DeleteCertificate message
struct DeleteCertificateRequest : public ocpp::Message {
    CertificateHashDataType certificateHashData;
    std::optional<CustomData> customData;

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

/// \brief Contains a OCPP DeleteCertificateResponse message
struct DeleteCertificateResponse : public ocpp::Message {
    DeleteCertificateStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

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

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_DELETECERTIFICATE_HPP
