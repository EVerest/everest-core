// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_GETCERTIFICATESTATUS_HPP
#define OCPP_V201_GETCERTIFICATESTATUS_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP GetCertificateStatus message
struct GetCertificateStatusRequest : public ocpp::Message {
    OCSPRequestData ocspRequestData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetCertificateStatus message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetCertificateStatusRequest \p k to a given json object \p j
void to_json(json& j, const GetCertificateStatusRequest& k);

/// \brief Conversion from a given json object \p j to a given GetCertificateStatusRequest \p k
void from_json(const json& j, GetCertificateStatusRequest& k);

/// \brief Writes the string representation of the given GetCertificateStatusRequest \p k to the given output stream \p
/// os \returns an output stream with the GetCertificateStatusRequest written to
std::ostream& operator<<(std::ostream& os, const GetCertificateStatusRequest& k);

/// \brief Contains a OCPP GetCertificateStatusResponse message
struct GetCertificateStatusResponse : public ocpp::Message {
    GetCertificateStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;
    std::optional<CiString<5500>> ocspResult;

    /// \brief Provides the type of this GetCertificateStatusResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetCertificateStatusResponse \p k to a given json object \p j
void to_json(json& j, const GetCertificateStatusResponse& k);

/// \brief Conversion from a given json object \p j to a given GetCertificateStatusResponse \p k
void from_json(const json& j, GetCertificateStatusResponse& k);

/// \brief Writes the string representation of the given GetCertificateStatusResponse \p k to the given output stream \p
/// os \returns an output stream with the GetCertificateStatusResponse written to
std::ostream& operator<<(std::ostream& os, const GetCertificateStatusResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_GETCERTIFICATESTATUS_HPP
