// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_PUBLISHFIRMWARE_HPP
#define OCPP_V201_PUBLISHFIRMWARE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP PublishFirmware message
struct PublishFirmwareRequest : public ocpp::Message {
    CiString<512> location;
    CiString<32> checksum;
    int32_t requestId;
    std::optional<CustomData> customData;
    std::optional<int32_t> retries;
    std::optional<int32_t> retryInterval;

    /// \brief Provides the type of this PublishFirmware message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given PublishFirmwareRequest \p k to a given json object \p j
void to_json(json& j, const PublishFirmwareRequest& k);

/// \brief Conversion from a given json object \p j to a given PublishFirmwareRequest \p k
void from_json(const json& j, PublishFirmwareRequest& k);

/// \brief Writes the string representation of the given PublishFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the PublishFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareRequest& k);

/// \brief Contains a OCPP PublishFirmwareResponse message
struct PublishFirmwareResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this PublishFirmwareResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given PublishFirmwareResponse \p k to a given json object \p j
void to_json(json& j, const PublishFirmwareResponse& k);

/// \brief Conversion from a given json object \p j to a given PublishFirmwareResponse \p k
void from_json(const json& j, PublishFirmwareResponse& k);

/// \brief Writes the string representation of the given PublishFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the PublishFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_PUBLISHFIRMWARE_HPP
