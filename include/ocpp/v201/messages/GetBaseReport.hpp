// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_GETBASEREPORT_HPP
#define OCPP_V201_GETBASEREPORT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP GetBaseReport message
struct GetBaseReportRequest : public ocpp::Message {
    int32_t requestId;
    ReportBaseEnum reportBase;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetBaseReport message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetBaseReportRequest \p k to a given json object \p j
void to_json(json& j, const GetBaseReportRequest& k);

/// \brief Conversion from a given json object \p j to a given GetBaseReportRequest \p k
void from_json(const json& j, GetBaseReportRequest& k);

/// \brief Writes the string representation of the given GetBaseReportRequest \p k to the given output stream \p os
/// \returns an output stream with the GetBaseReportRequest written to
std::ostream& operator<<(std::ostream& os, const GetBaseReportRequest& k);

/// \brief Contains a OCPP GetBaseReportResponse message
struct GetBaseReportResponse : public ocpp::Message {
    GenericDeviceModelStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this GetBaseReportResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetBaseReportResponse \p k to a given json object \p j
void to_json(json& j, const GetBaseReportResponse& k);

/// \brief Conversion from a given json object \p j to a given GetBaseReportResponse \p k
void from_json(const json& j, GetBaseReportResponse& k);

/// \brief Writes the string representation of the given GetBaseReportResponse \p k to the given output stream \p os
/// \returns an output stream with the GetBaseReportResponse written to
std::ostream& operator<<(std::ostream& os, const GetBaseReportResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_GETBASEREPORT_HPP
