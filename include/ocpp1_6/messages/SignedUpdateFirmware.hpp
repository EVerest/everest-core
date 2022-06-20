// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SIGNEDUPDATEFIRMWARE_HPP
#define OCPP1_6_SIGNEDUPDATEFIRMWARE_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 SignedUpdateFirmware message
struct SignedUpdateFirmwareRequest : public Message {
    int32_t requestId;
    FirmwareType firmware;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;

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

/// \brief Contains a OCPP 1.6 SignedUpdateFirmwareResponse message
struct SignedUpdateFirmwareResponse : public Message {
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

} // namespace ocpp1_6

#endif // OCPP1_6_SIGNEDUPDATEFIRMWARE_HPP
