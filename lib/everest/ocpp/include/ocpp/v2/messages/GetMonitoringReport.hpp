// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_GETMONITORINGREPORT_HPP
#define OCPP_V2_GETMONITORINGREPORT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP GetMonitoringReport message
struct GetMonitoringReportRequest : public ocpp::Message {
    int32_t requestId;
    std::optional<std::vector<ComponentVariable>> componentVariable;
    std::optional<std::vector<MonitoringCriterionEnum>> monitoringCriteria;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetMonitoringReport message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetMonitoringReportRequest \p k to a given json object \p j
void to_json(json& j, const GetMonitoringReportRequest& k);

/// \brief Conversion from a given json object \p j to a given GetMonitoringReportRequest \p k
void from_json(const json& j, GetMonitoringReportRequest& k);

/// \brief Writes the string representation of the given GetMonitoringReportRequest \p k to the given output stream \p
/// os \returns an output stream with the GetMonitoringReportRequest written to
std::ostream& operator<<(std::ostream& os, const GetMonitoringReportRequest& k);

/// \brief Contains a OCPP GetMonitoringReportResponse message
struct GetMonitoringReportResponse : public ocpp::Message {
    GenericDeviceModelStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetMonitoringReportResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetMonitoringReportResponse \p k to a given json object \p j
void to_json(json& j, const GetMonitoringReportResponse& k);

/// \brief Conversion from a given json object \p j to a given GetMonitoringReportResponse \p k
void from_json(const json& j, GetMonitoringReportResponse& k);

/// \brief Writes the string representation of the given GetMonitoringReportResponse \p k to the given output stream \p
/// os \returns an output stream with the GetMonitoringReportResponse written to
std::ostream& operator<<(std::ostream& os, const GetMonitoringReportResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_GETMONITORINGREPORT_HPP
