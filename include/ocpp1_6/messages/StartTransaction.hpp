// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_STARTTRANSACTION_HPP
#define OCPP1_6_STARTTRANSACTION_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 StartTransaction message
struct StartTransactionRequest : public Message {
    int32_t connectorId;
    CiString20Type idTag;
    int32_t meterStart;
    DateTime timestamp;
    boost::optional<int32_t> reservationId;

    /// \brief Provides the type of this StartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given StartTransactionRequest \p k to a given json object \p j
void to_json(json& j, const StartTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given StartTransactionRequest \p k
void from_json(const json& j, StartTransactionRequest& k);

/// \brief Writes the string representation of the given StartTransactionRequest \p k to the given output stream \p os
/// \returns an output stream with the StartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const StartTransactionRequest& k);

/// \brief Contains a OCPP 1.6 StartTransactionResponse message
struct StartTransactionResponse : public Message {
    IdTagInfo idTagInfo;
    int32_t transactionId;

    /// \brief Provides the type of this StartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given StartTransactionResponse \p k to a given json object \p j
void to_json(json& j, const StartTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given StartTransactionResponse \p k
void from_json(const json& j, StartTransactionResponse& k);

/// \brief Writes the string representation of the given StartTransactionResponse \p k to the given output stream \p os
/// \returns an output stream with the StartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const StartTransactionResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_STARTTRANSACTION_HPP
