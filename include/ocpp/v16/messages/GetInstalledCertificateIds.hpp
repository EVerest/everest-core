// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_GETINSTALLEDCERTIFICATEIDS_HPP
#define OCPP_V16_GETINSTALLEDCERTIFICATEIDS_HPP

#include <boost/optional.hpp>

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP GetInstalledCertificateIds message
struct GetInstalledCertificateIdsRequest : public ocpp::Message {
    CertificateUseEnumType certificateType;

    /// \brief Provides the type of this GetInstalledCertificateIds message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetInstalledCertificateIdsRequest \p k to a given json object \p j
void to_json(json& j, const GetInstalledCertificateIdsRequest& k);

/// \brief Conversion from a given json object \p j to a given GetInstalledCertificateIdsRequest \p k
void from_json(const json& j, GetInstalledCertificateIdsRequest& k);

/// \brief Writes the string representation of the given GetInstalledCertificateIdsRequest \p k to the given output
/// stream \p os \returns an output stream with the GetInstalledCertificateIdsRequest written to
std::ostream& operator<<(std::ostream& os, const GetInstalledCertificateIdsRequest& k);

/// \brief Contains a OCPP GetInstalledCertificateIdsResponse message
struct GetInstalledCertificateIdsResponse : public ocpp::Message {
    GetInstalledCertificateStatusEnumType status;
    boost::optional<std::vector<CertificateHashDataType>> certificateHashData;

    /// \brief Provides the type of this GetInstalledCertificateIdsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetInstalledCertificateIdsResponse \p k to a given json object \p j
void to_json(json& j, const GetInstalledCertificateIdsResponse& k);

/// \brief Conversion from a given json object \p j to a given GetInstalledCertificateIdsResponse \p k
void from_json(const json& j, GetInstalledCertificateIdsResponse& k);

/// \brief Writes the string representation of the given GetInstalledCertificateIdsResponse \p k to the given output
/// stream \p os \returns an output stream with the GetInstalledCertificateIdsResponse written to
std::ostream& operator<<(std::ostream& os, const GetInstalledCertificateIdsResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_GETINSTALLEDCERTIFICATEIDS_HPP
