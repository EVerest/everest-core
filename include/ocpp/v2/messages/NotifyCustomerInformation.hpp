// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_NOTIFYCUSTOMERINFORMATION_HPP
#define OCPP_V2_NOTIFYCUSTOMERINFORMATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP NotifyCustomerInformation message
struct NotifyCustomerInformationRequest : public ocpp::Message {
    CiString<512> data;
    int32_t seqNo;
    ocpp::DateTime generatedAt;
    int32_t requestId;
    std::optional<bool> tbc;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyCustomerInformation message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyCustomerInformationRequest \p k to a given json object \p j
void to_json(json& j, const NotifyCustomerInformationRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyCustomerInformationRequest \p k
void from_json(const json& j, NotifyCustomerInformationRequest& k);

/// \brief Writes the string representation of the given NotifyCustomerInformationRequest \p k to the given output
/// stream \p os \returns an output stream with the NotifyCustomerInformationRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyCustomerInformationRequest& k);

/// \brief Contains a OCPP NotifyCustomerInformationResponse message
struct NotifyCustomerInformationResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyCustomerInformationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyCustomerInformationResponse \p k to a given json object \p j
void to_json(json& j, const NotifyCustomerInformationResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyCustomerInformationResponse \p k
void from_json(const json& j, NotifyCustomerInformationResponse& k);

/// \brief Writes the string representation of the given NotifyCustomerInformationResponse \p k to the given output
/// stream \p os \returns an output stream with the NotifyCustomerInformationResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyCustomerInformationResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_NOTIFYCUSTOMERINFORMATION_HPP
