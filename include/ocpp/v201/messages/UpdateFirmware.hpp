// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_UPDATEFIRMWARE_HPP
#define OCPP_V201_UPDATEFIRMWARE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP UpdateFirmware message
struct UpdateFirmwareRequest : public ocpp::Message {
    int32_t requestId;
    Firmware firmware;
    std::optional<CustomData> customData;
    std::optional<int32_t> retries;
    std::optional<int32_t> retryInterval;

    /// \brief Provides the type of this UpdateFirmware message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given UpdateFirmwareRequest \p k to a given json object \p j
void to_json(json& j, const UpdateFirmwareRequest& k);

/// \brief Conversion from a given json object \p j to a given UpdateFirmwareRequest \p k
void from_json(const json& j, UpdateFirmwareRequest& k);

/// \brief Writes the string representation of the given UpdateFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the UpdateFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareRequest& k);

/// \brief Contains a OCPP UpdateFirmwareResponse message
struct UpdateFirmwareResponse : public ocpp::Message {
    UpdateFirmwareStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this UpdateFirmwareResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given UpdateFirmwareResponse \p k to a given json object \p j
void to_json(json& j, const UpdateFirmwareResponse& k);

/// \brief Conversion from a given json object \p j to a given UpdateFirmwareResponse \p k
void from_json(const json& j, UpdateFirmwareResponse& k);

/// \brief Writes the string representation of the given UpdateFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the UpdateFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_UPDATEFIRMWARE_HPP
