// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_UPDATEFIRMWARE_HPP
#define OCPP1_6_UPDATEFIRMWARE_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 UpdateFirmware message
struct UpdateFirmwareRequest : public Message {
    std::string location;
    DateTime retrieveDate;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;

    /// \brief Provides the type of this UpdateFirmware message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given UpdateFirmwareRequest \p k to a given json object \p j
void to_json(json& j, const UpdateFirmwareRequest& k);

/// \brief Conversion from a given json object \p j to a given UpdateFirmwareRequest \p k
void from_json(const json& j, UpdateFirmwareRequest& k);

/// \brief Writes the string representation of the given UpdateFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the UpdateFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareRequest& k);

/// \brief Contains a OCPP 1.6 UpdateFirmwareResponse message
struct UpdateFirmwareResponse : public Message {

    /// \brief Provides the type of this UpdateFirmwareResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given UpdateFirmwareResponse \p k to a given json object \p j
void to_json(json& j, const UpdateFirmwareResponse& k);

/// \brief Conversion from a given json object \p j to a given UpdateFirmwareResponse \p k
void from_json(const json& j, UpdateFirmwareResponse& k);

/// \brief Writes the string representation of the given UpdateFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the UpdateFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const UpdateFirmwareResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_UPDATEFIRMWARE_HPP
