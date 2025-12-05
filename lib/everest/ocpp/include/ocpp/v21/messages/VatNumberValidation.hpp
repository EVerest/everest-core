// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_VATNUMBERVALIDATION_HPP
#define OCPP_V21_VATNUMBERVALIDATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP VatNumberValidation message
struct VatNumberValidationRequest : public ocpp::Message {
    CiString<20> vatNumber;
    std::optional<int32_t> evseId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this VatNumberValidation message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given VatNumberValidationRequest \p k to a given json object \p j
void to_json(json& j, const VatNumberValidationRequest& k);

/// \brief Conversion from a given json object \p j to a given VatNumberValidationRequest \p k
void from_json(const json& j, VatNumberValidationRequest& k);

/// \brief Writes the string representation of the given VatNumberValidationRequest \p k to the given output stream \p
/// os \returns an output stream with the VatNumberValidationRequest written to
std::ostream& operator<<(std::ostream& os, const VatNumberValidationRequest& k);

/// \brief Contains a OCPP VatNumberValidationResponse message
struct VatNumberValidationResponse : public ocpp::Message {
    CiString<20> vatNumber;
    GenericStatusEnum status;
    std::optional<Address> company;
    std::optional<StatusInfo> statusInfo;
    std::optional<int32_t> evseId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this VatNumberValidationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given VatNumberValidationResponse \p k to a given json object \p j
void to_json(json& j, const VatNumberValidationResponse& k);

/// \brief Conversion from a given json object \p j to a given VatNumberValidationResponse \p k
void from_json(const json& j, VatNumberValidationResponse& k);

/// \brief Writes the string representation of the given VatNumberValidationResponse \p k to the given output stream \p
/// os \returns an output stream with the VatNumberValidationResponse written to
std::ostream& operator<<(std::ostream& os, const VatNumberValidationResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_VATNUMBERVALIDATION_HPP
