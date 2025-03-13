// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_CLEARDERCONTROL_HPP
#define OCPP_V21_CLEARDERCONTROL_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP ClearDERControl message
struct ClearDERControlRequest : public ocpp::Message {
    bool isDefault;
    std::optional<DERControlEnum> controlType;
    std::optional<CiString<36>> controlId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClearDERControl message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearDERControlRequest \p k to a given json object \p j
void to_json(json& j, const ClearDERControlRequest& k);

/// \brief Conversion from a given json object \p j to a given ClearDERControlRequest \p k
void from_json(const json& j, ClearDERControlRequest& k);

/// \brief Writes the string representation of the given ClearDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const ClearDERControlRequest& k);

/// \brief Contains a OCPP ClearDERControlResponse message
struct ClearDERControlResponse : public ocpp::Message {
    DERControlStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClearDERControlResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearDERControlResponse \p k to a given json object \p j
void to_json(json& j, const ClearDERControlResponse& k);

/// \brief Conversion from a given json object \p j to a given ClearDERControlResponse \p k
void from_json(const json& j, ClearDERControlResponse& k);

/// \brief Writes the string representation of the given ClearDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const ClearDERControlResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_CLEARDERCONTROL_HPP
