// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_SETMONITORINGBASE_HPP
#define OCPP_V2_SETMONITORINGBASE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP SetMonitoringBase message
struct SetMonitoringBaseRequest : public ocpp::Message {
    MonitoringBaseEnum monitoringBase;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetMonitoringBase message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetMonitoringBaseRequest \p k to a given json object \p j
void to_json(json& j, const SetMonitoringBaseRequest& k);

/// \brief Conversion from a given json object \p j to a given SetMonitoringBaseRequest \p k
void from_json(const json& j, SetMonitoringBaseRequest& k);

/// \brief Writes the string representation of the given SetMonitoringBaseRequest \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringBaseRequest written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringBaseRequest& k);

/// \brief Contains a OCPP SetMonitoringBaseResponse message
struct SetMonitoringBaseResponse : public ocpp::Message {
    GenericDeviceModelStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetMonitoringBaseResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetMonitoringBaseResponse \p k to a given json object \p j
void to_json(json& j, const SetMonitoringBaseResponse& k);

/// \brief Conversion from a given json object \p j to a given SetMonitoringBaseResponse \p k
void from_json(const json& j, SetMonitoringBaseResponse& k);

/// \brief Writes the string representation of the given SetMonitoringBaseResponse \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringBaseResponse written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringBaseResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_SETMONITORINGBASE_HPP
