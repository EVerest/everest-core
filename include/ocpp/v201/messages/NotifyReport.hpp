// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_NOTIFYREPORT_HPP
#define OCPP_V201_NOTIFYREPORT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP NotifyReport message
struct NotifyReportRequest : public ocpp::Message {
    int32_t requestId;
    ocpp::DateTime generatedAt;
    int32_t seqNo;
    std::optional<CustomData> customData;
    std::optional<std::vector<ReportData>> reportData;
    std::optional<bool> tbc;

    /// \brief Provides the type of this NotifyReport message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given NotifyReportRequest \p k to a given json object \p j
void to_json(json& j, const NotifyReportRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyReportRequest \p k
void from_json(const json& j, NotifyReportRequest& k);

/// \brief Writes the string representation of the given NotifyReportRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyReportRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyReportRequest& k);

/// \brief Contains a OCPP NotifyReportResponse message
struct NotifyReportResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyReportResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given NotifyReportResponse \p k to a given json object \p j
void to_json(json& j, const NotifyReportResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyReportResponse \p k
void from_json(const json& j, NotifyReportResponse& k);

/// \brief Writes the string representation of the given NotifyReportResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyReportResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyReportResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_NOTIFYREPORT_HPP
