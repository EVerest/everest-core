// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_RESERVENOW_HPP
#define OCPP_V16_RESERVENOW_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP ReserveNow message
struct ReserveNowRequest : public ocpp::Message {
    int32_t connectorId;
    ocpp::DateTime expiryDate;
    CiString<20> idTag;
    int32_t reservationId;
    std::optional<CiString<20>> parentIdTag;

    /// \brief Provides the type of this ReserveNow message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ReserveNowRequest \p k to a given json object \p j
void to_json(json& j, const ReserveNowRequest& k);

/// \brief Conversion from a given json object \p j to a given ReserveNowRequest \p k
void from_json(const json& j, ReserveNowRequest& k);

/// \brief Writes the string representation of the given ReserveNowRequest \p k to the given output stream \p os
/// \returns an output stream with the ReserveNowRequest written to
std::ostream& operator<<(std::ostream& os, const ReserveNowRequest& k);

/// \brief Contains a OCPP ReserveNowResponse message
struct ReserveNowResponse : public ocpp::Message {
    ReservationStatus status;

    /// \brief Provides the type of this ReserveNowResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ReserveNowResponse \p k to a given json object \p j
void to_json(json& j, const ReserveNowResponse& k);

/// \brief Conversion from a given json object \p j to a given ReserveNowResponse \p k
void from_json(const json& j, ReserveNowResponse& k);

/// \brief Writes the string representation of the given ReserveNowResponse \p k to the given output stream \p os
/// \returns an output stream with the ReserveNowResponse written to
std::ostream& operator<<(std::ostream& os, const ReserveNowResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_RESERVENOW_HPP
