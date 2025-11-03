// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_REPORTDERCONTROL_HPP
#define OCPP_V21_REPORTDERCONTROL_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP ReportDERControl message
struct ReportDERControlRequest : public ocpp::Message {
    int32_t requestId;
    std::optional<std::vector<DERCurveGet>> curve;
    std::optional<std::vector<EnterServiceGet>> enterService;
    std::optional<std::vector<FixedPFGet>> fixedPFAbsorb;
    std::optional<std::vector<FixedPFGet>> fixedPFInject;
    std::optional<std::vector<FixedVarGet>> fixedVar;
    std::optional<std::vector<FreqDroopGet>> freqDroop;
    std::optional<std::vector<GradientGet>> gradient;
    std::optional<std::vector<LimitMaxDischargeGet>> limitMaxDischarge;
    std::optional<bool> tbc;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ReportDERControl message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ReportDERControlRequest \p k to a given json object \p j
void to_json(json& j, const ReportDERControlRequest& k);

/// \brief Conversion from a given json object \p j to a given ReportDERControlRequest \p k
void from_json(const json& j, ReportDERControlRequest& k);

/// \brief Writes the string representation of the given ReportDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the ReportDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const ReportDERControlRequest& k);

/// \brief Contains a OCPP ReportDERControlResponse message
struct ReportDERControlResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ReportDERControlResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ReportDERControlResponse \p k to a given json object \p j
void to_json(json& j, const ReportDERControlResponse& k);

/// \brief Conversion from a given json object \p j to a given ReportDERControlResponse \p k
void from_json(const json& j, ReportDERControlResponse& k);

/// \brief Writes the string representation of the given ReportDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the ReportDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const ReportDERControlResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_REPORTDERCONTROL_HPP
