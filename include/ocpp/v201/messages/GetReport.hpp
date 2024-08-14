// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_GETREPORT_HPP
#define OCPP_V201_GETREPORT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP GetReport message
struct GetReportRequest : public ocpp::Message {
    int32_t requestId;
    std::optional<CustomData> customData;
    std::optional<std::vector<ComponentVariable>> componentVariable;
    std::optional<std::vector<ComponentCriterionEnum>> componentCriteria;

    /// \brief Provides the type of this GetReport message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetReportRequest \p k to a given json object \p j
void to_json(json& j, const GetReportRequest& k);

/// \brief Conversion from a given json object \p j to a given GetReportRequest \p k
void from_json(const json& j, GetReportRequest& k);

/// \brief Writes the string representation of the given GetReportRequest \p k to the given output stream \p os
/// \returns an output stream with the GetReportRequest written to
std::ostream& operator<<(std::ostream& os, const GetReportRequest& k);

/// \brief Contains a OCPP GetReportResponse message
struct GetReportResponse : public ocpp::Message {
    GenericDeviceModelStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this GetReportResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetReportResponse \p k to a given json object \p j
void to_json(json& j, const GetReportResponse& k);

/// \brief Conversion from a given json object \p j to a given GetReportResponse \p k
void from_json(const json& j, GetReportResponse& k);

/// \brief Writes the string representation of the given GetReportResponse \p k to the given output stream \p os
/// \returns an output stream with the GetReportResponse written to
std::ostream& operator<<(std::ostream& os, const GetReportResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_GETREPORT_HPP
