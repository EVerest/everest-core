// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_SETDERCONTROL_HPP
#define OCPP_V21_SETDERCONTROL_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP SetDERControl message
struct SetDERControlRequest : public ocpp::Message {
    bool isDefault;
    CiString<36> controlId;
    DERControlEnum controlType;
    std::optional<DERCurve> curve;
    std::optional<EnterService> enterService;
    std::optional<FixedPF> fixedPFAbsorb;
    std::optional<FixedPF> fixedPFInject;
    std::optional<FixedVar> fixedVar;
    std::optional<FreqDroop> freqDroop;
    std::optional<Gradient> gradient;
    std::optional<LimitMaxDischarge> limitMaxDischarge;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetDERControl message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetDERControlRequest \p k to a given json object \p j
void to_json(json& j, const SetDERControlRequest& k);

/// \brief Conversion from a given json object \p j to a given SetDERControlRequest \p k
void from_json(const json& j, SetDERControlRequest& k);

/// \brief Writes the string representation of the given SetDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the SetDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const SetDERControlRequest& k);

/// \brief Contains a OCPP SetDERControlResponse message
struct SetDERControlResponse : public ocpp::Message {
    DERControlStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<std::vector<CiString<36>>> supersededIds;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetDERControlResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetDERControlResponse \p k to a given json object \p j
void to_json(json& j, const SetDERControlResponse& k);

/// \brief Conversion from a given json object \p j to a given SetDERControlResponse \p k
void from_json(const json& j, SetDERControlResponse& k);

/// \brief Writes the string representation of the given SetDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the SetDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const SetDERControlResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_SETDERCONTROL_HPP
