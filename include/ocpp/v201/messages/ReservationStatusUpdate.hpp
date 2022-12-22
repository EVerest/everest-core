// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_RESERVATIONSTATUSUPDATE_HPP
#define OCPP_V201_RESERVATIONSTATUSUPDATE_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP ReservationStatusUpdate message
struct ReservationStatusUpdateRequest : public ocpp::Message {
    int32_t reservationId;
    ReservationUpdateStatusEnum reservationUpdateStatus;
    boost::optional<CustomData> customData;

    /// \brief Provides the type of this ReservationStatusUpdate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ReservationStatusUpdateRequest \p k to a given json object \p j
void to_json(json& j, const ReservationStatusUpdateRequest& k);

/// \brief Conversion from a given json object \p j to a given ReservationStatusUpdateRequest \p k
void from_json(const json& j, ReservationStatusUpdateRequest& k);

/// \brief Writes the string representation of the given ReservationStatusUpdateRequest \p k to the given output stream
/// \p os \returns an output stream with the ReservationStatusUpdateRequest written to
std::ostream& operator<<(std::ostream& os, const ReservationStatusUpdateRequest& k);

/// \brief Contains a OCPP ReservationStatusUpdateResponse message
struct ReservationStatusUpdateResponse : public ocpp::Message {
    boost::optional<CustomData> customData;

    /// \brief Provides the type of this ReservationStatusUpdateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ReservationStatusUpdateResponse \p k to a given json object \p j
void to_json(json& j, const ReservationStatusUpdateResponse& k);

/// \brief Conversion from a given json object \p j to a given ReservationStatusUpdateResponse \p k
void from_json(const json& j, ReservationStatusUpdateResponse& k);

/// \brief Writes the string representation of the given ReservationStatusUpdateResponse \p k to the given output stream
/// \p os \returns an output stream with the ReservationStatusUpdateResponse written to
std::ostream& operator<<(std::ostream& os, const ReservationStatusUpdateResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_RESERVATIONSTATUSUPDATE_HPP
