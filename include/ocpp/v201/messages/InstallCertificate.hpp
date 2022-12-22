// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_INSTALLCERTIFICATE_HPP
#define OCPP_V201_INSTALLCERTIFICATE_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP InstallCertificate message
struct InstallCertificateRequest : public ocpp::Message {
    InstallCertificateUseEnum certificateType;
    CiString<5500> certificate;
    boost::optional<CustomData> customData;

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

/// \brief Contains a OCPP InstallCertificateResponse message
struct InstallCertificateResponse : public ocpp::Message {
    InstallCertificateStatusEnum status;
    boost::optional<CustomData> customData;
    boost::optional<StatusInfo> statusInfo;

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

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_INSTALLCERTIFICATE_HPP
