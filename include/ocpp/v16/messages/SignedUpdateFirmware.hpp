// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_SIGNEDUPDATEFIRMWARE_HPP
#define OCPP_V16_SIGNEDUPDATEFIRMWARE_HPP

#include <optional>

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP SignedUpdateFirmware message
struct SignedUpdateFirmwareRequest : public ocpp::Message {
    int32_t requestId;
    FirmwareType firmware;
    std::optional<int32_t> retries;
    std::optional<int32_t> retryInterval;

    /// \brief Provides the type of this SignedUpdateFirmware message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SignedUpdateFirmwareRequest \p k to a given json object \p j
void to_json(json& j, const SignedUpdateFirmwareRequest& k);

/// \brief Conversion from a given json object \p j to a given SignedUpdateFirmwareRequest \p k
void from_json(const json& j, SignedUpdateFirmwareRequest& k);

/// \brief Writes the string representation of the given SignedUpdateFirmwareRequest \p k to the given output stream \p
/// os \returns an output stream with the SignedUpdateFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const SignedUpdateFirmwareRequest& k);

/// \brief Contains a OCPP SignedUpdateFirmwareResponse message
struct SignedUpdateFirmwareResponse : public ocpp::Message {
    UpdateFirmwareStatusEnumType status;

    /// \brief Provides the type of this SignedUpdateFirmwareResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SignedUpdateFirmwareResponse \p k to a given json object \p j
void to_json(json& j, const SignedUpdateFirmwareResponse& k);

/// \brief Conversion from a given json object \p j to a given SignedUpdateFirmwareResponse \p k
void from_json(const json& j, SignedUpdateFirmwareResponse& k);

/// \brief Writes the string representation of the given SignedUpdateFirmwareResponse \p k to the given output stream \p
/// os \returns an output stream with the SignedUpdateFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const SignedUpdateFirmwareResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_SIGNEDUPDATEFIRMWARE_HPP
