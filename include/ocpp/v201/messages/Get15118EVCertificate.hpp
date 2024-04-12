// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_GET15118EVCERTIFICATE_HPP
#define OCPP_V201_GET15118EVCERTIFICATE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP Get15118EVCertificate message
struct Get15118EVCertificateRequest : public ocpp::Message {
    CiString<50> iso15118SchemaVersion;
    CertificateActionEnum action;
    CiString<7500> exiRequest;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this Get15118EVCertificate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given Get15118EVCertificateRequest \p k to a given json object \p j
void to_json(json& j, const Get15118EVCertificateRequest& k);

/// \brief Conversion from a given json object \p j to a given Get15118EVCertificateRequest \p k
void from_json(const json& j, Get15118EVCertificateRequest& k);

/// \brief Writes the string representation of the given Get15118EVCertificateRequest \p k to the given output stream \p
/// os \returns an output stream with the Get15118EVCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const Get15118EVCertificateRequest& k);

/// \brief Contains a OCPP Get15118EVCertificateResponse message
struct Get15118EVCertificateResponse : public ocpp::Message {
    Iso15118EVCertificateStatusEnum status;
    CiString<7500> exiResponse;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this Get15118EVCertificateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given Get15118EVCertificateResponse \p k to a given json object \p j
void to_json(json& j, const Get15118EVCertificateResponse& k);

/// \brief Conversion from a given json object \p j to a given Get15118EVCertificateResponse \p k
void from_json(const json& j, Get15118EVCertificateResponse& k);

/// \brief Writes the string representation of the given Get15118EVCertificateResponse \p k to the given output stream
/// \p os \returns an output stream with the Get15118EVCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const Get15118EVCertificateResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_GET15118EVCERTIFICATE_HPP
