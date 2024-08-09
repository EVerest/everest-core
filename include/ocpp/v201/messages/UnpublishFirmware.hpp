// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_UNPUBLISHFIRMWARE_HPP
#define OCPP_V201_UNPUBLISHFIRMWARE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP UnpublishFirmware message
struct UnpublishFirmwareRequest : public ocpp::Message {
    CiString<32> checksum;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this UnpublishFirmware message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given UnpublishFirmwareRequest \p k to a given json object \p j
void to_json(json& j, const UnpublishFirmwareRequest& k);

/// \brief Conversion from a given json object \p j to a given UnpublishFirmwareRequest \p k
void from_json(const json& j, UnpublishFirmwareRequest& k);

/// \brief Writes the string representation of the given UnpublishFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the UnpublishFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const UnpublishFirmwareRequest& k);

/// \brief Contains a OCPP UnpublishFirmwareResponse message
struct UnpublishFirmwareResponse : public ocpp::Message {
    UnpublishFirmwareStatusEnum status;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this UnpublishFirmwareResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given UnpublishFirmwareResponse \p k to a given json object \p j
void to_json(json& j, const UnpublishFirmwareResponse& k);

/// \brief Conversion from a given json object \p j to a given UnpublishFirmwareResponse \p k
void from_json(const json& j, UnpublishFirmwareResponse& k);

/// \brief Writes the string representation of the given UnpublishFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the UnpublishFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const UnpublishFirmwareResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_UNPUBLISHFIRMWARE_HPP
