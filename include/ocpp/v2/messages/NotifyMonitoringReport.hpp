// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_NOTIFYMONITORINGREPORT_HPP
#define OCPP_V2_NOTIFYMONITORINGREPORT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP NotifyMonitoringReport message
struct NotifyMonitoringReportRequest : public ocpp::Message {
    int32_t requestId;
    int32_t seqNo;
    ocpp::DateTime generatedAt;
    std::optional<CustomData> customData;
    std::optional<std::vector<MonitoringData>> monitor;
    std::optional<bool> tbc;

    /// \brief Provides the type of this NotifyMonitoringReport message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyMonitoringReportRequest \p k to a given json object \p j
void to_json(json& j, const NotifyMonitoringReportRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyMonitoringReportRequest \p k
void from_json(const json& j, NotifyMonitoringReportRequest& k);

/// \brief Writes the string representation of the given NotifyMonitoringReportRequest \p k to the given output stream
/// \p os \returns an output stream with the NotifyMonitoringReportRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyMonitoringReportRequest& k);

/// \brief Contains a OCPP NotifyMonitoringReportResponse message
struct NotifyMonitoringReportResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyMonitoringReportResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyMonitoringReportResponse \p k to a given json object \p j
void to_json(json& j, const NotifyMonitoringReportResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyMonitoringReportResponse \p k
void from_json(const json& j, NotifyMonitoringReportResponse& k);

/// \brief Writes the string representation of the given NotifyMonitoringReportResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyMonitoringReportResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyMonitoringReportResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_NOTIFYMONITORINGREPORT_HPP
