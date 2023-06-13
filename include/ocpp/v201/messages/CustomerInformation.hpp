// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_CUSTOMERINFORMATION_HPP
#define OCPP_V201_CUSTOMERINFORMATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP CustomerInformation message
struct CustomerInformationRequest : public ocpp::Message {
    int32_t requestId;
    bool report;
    bool clear;
    std::optional<CustomData> customData;
    std::optional<CertificateHashDataType> customerCertificate;
    std::optional<IdToken> idToken;
    std::optional<CiString<64>> customerIdentifier;

    /// \brief Provides the type of this CustomerInformation message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CustomerInformationRequest \p k to a given json object \p j
void to_json(json& j, const CustomerInformationRequest& k);

/// \brief Conversion from a given json object \p j to a given CustomerInformationRequest \p k
void from_json(const json& j, CustomerInformationRequest& k);

/// \brief Writes the string representation of the given CustomerInformationRequest \p k to the given output stream \p
/// os \returns an output stream with the CustomerInformationRequest written to
std::ostream& operator<<(std::ostream& os, const CustomerInformationRequest& k);

/// \brief Contains a OCPP CustomerInformationResponse message
struct CustomerInformationResponse : public ocpp::Message {
    CustomerInformationStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this CustomerInformationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CustomerInformationResponse \p k to a given json object \p j
void to_json(json& j, const CustomerInformationResponse& k);

/// \brief Conversion from a given json object \p j to a given CustomerInformationResponse \p k
void from_json(const json& j, CustomerInformationResponse& k);

/// \brief Writes the string representation of the given CustomerInformationResponse \p k to the given output stream \p
/// os \returns an output stream with the CustomerInformationResponse written to
std::ostream& operator<<(std::ostream& os, const CustomerInformationResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_CUSTOMERINFORMATION_HPP
