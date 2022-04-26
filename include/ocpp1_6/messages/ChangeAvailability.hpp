// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHANGEAVAILABILITY_HPP
#define OCPP1_6_CHANGEAVAILABILITY_HPP

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ChangeAvailability message
struct ChangeAvailabilityRequest : public Message {
    int32_t connectorId;
    AvailabilityType type;

    /// \brief Provides the type of this ChangeAvailability message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ChangeAvailabilityRequest \p k to a given json object \p j
void to_json(json& j, const ChangeAvailabilityRequest& k);

/// \brief Conversion from a given json object \p j to a given ChangeAvailabilityRequest \p k
void from_json(const json& j, ChangeAvailabilityRequest& k);

/// \brief Writes the string representation of the given ChangeAvailabilityRequest \p k to the given output stream \p os
/// \returns an output stream with the ChangeAvailabilityRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityRequest& k);

/// \brief Contains a OCPP 1.6 ChangeAvailabilityResponse message
struct ChangeAvailabilityResponse : public Message {
    AvailabilityStatus status;

    /// \brief Provides the type of this ChangeAvailabilityResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ChangeAvailabilityResponse \p k to a given json object \p j
void to_json(json& j, const ChangeAvailabilityResponse& k);

/// \brief Conversion from a given json object \p j to a given ChangeAvailabilityResponse \p k
void from_json(const json& j, ChangeAvailabilityResponse& k);

/// \brief Writes the string representation of the given ChangeAvailabilityResponse \p k to the given output stream \p
/// os \returns an output stream with the ChangeAvailabilityResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_CHANGEAVAILABILITY_HPP
