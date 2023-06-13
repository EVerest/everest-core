// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_CANCELRESERVATION_HPP
#define OCPP_V201_CANCELRESERVATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP CancelReservation message
struct CancelReservationRequest : public ocpp::Message {
    int32_t reservationId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this CancelReservation message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CancelReservationRequest \p k to a given json object \p j
void to_json(json& j, const CancelReservationRequest& k);

/// \brief Conversion from a given json object \p j to a given CancelReservationRequest \p k
void from_json(const json& j, CancelReservationRequest& k);

/// \brief Writes the string representation of the given CancelReservationRequest \p k to the given output stream \p os
/// \returns an output stream with the CancelReservationRequest written to
std::ostream& operator<<(std::ostream& os, const CancelReservationRequest& k);

/// \brief Contains a OCPP CancelReservationResponse message
struct CancelReservationResponse : public ocpp::Message {
    CancelReservationStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this CancelReservationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CancelReservationResponse \p k to a given json object \p j
void to_json(json& j, const CancelReservationResponse& k);

/// \brief Conversion from a given json object \p j to a given CancelReservationResponse \p k
void from_json(const json& j, CancelReservationResponse& k);

/// \brief Writes the string representation of the given CancelReservationResponse \p k to the given output stream \p os
/// \returns an output stream with the CancelReservationResponse written to
std::ostream& operator<<(std::ostream& os, const CancelReservationResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_CANCELRESERVATION_HPP
