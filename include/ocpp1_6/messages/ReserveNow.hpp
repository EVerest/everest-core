// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_RESERVENOW_HPP
#define OCPP1_6_RESERVENOW_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ReserveNow message
struct ReserveNowRequest : public Message {
    int32_t connectorId;
    DateTime expiryDate;
    CiString20Type idTag;
    int32_t reservationId;
    boost::optional<CiString20Type> parentIdTag;

    /// \brief Provides the type of this ReserveNow message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ReserveNowRequest \p k to a given json object \p j
void to_json(json& j, const ReserveNowRequest& k);

/// \brief Conversion from a given json object \p j to a given ReserveNowRequest \p k
void from_json(const json& j, ReserveNowRequest& k);

/// \brief Writes the string representation of the given ReserveNowRequest \p k to the given output stream \p os
/// \returns an output stream with the ReserveNowRequest written to
std::ostream& operator<<(std::ostream& os, const ReserveNowRequest& k);

/// \brief Contains a OCPP 1.6 ReserveNowResponse message
struct ReserveNowResponse : public Message {
    ReservationStatus status;

    /// \brief Provides the type of this ReserveNowResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ReserveNowResponse \p k to a given json object \p j
void to_json(json& j, const ReserveNowResponse& k);

/// \brief Conversion from a given json object \p j to a given ReserveNowResponse \p k
void from_json(const json& j, ReserveNowResponse& k);

/// \brief Writes the string representation of the given ReserveNowResponse \p k to the given output stream \p os
/// \returns an output stream with the ReserveNowResponse written to
std::ostream& operator<<(std::ostream& os, const ReserveNowResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_RESERVENOW_HPP
