// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_REMOTESTOPTRANSACTION_HPP
#define OCPP1_6_REMOTESTOPTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 RemoteStopTransaction message
struct RemoteStopTransactionRequest : public Message {
    int32_t transactionId;

    /// \brief Provides the type of this RemoteStopTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RemoteStopTransactionRequest \p k to a given json object \p j
void to_json(json& j, const RemoteStopTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given RemoteStopTransactionRequest \p k
void from_json(const json& j, RemoteStopTransactionRequest& k);

/// \brief Writes the string representation of the given RemoteStopTransactionRequest \p k to the given output stream \p
/// os \returns an output stream with the RemoteStopTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RemoteStopTransactionRequest& k);

/// \brief Contains a OCPP 1.6 RemoteStopTransactionResponse message
struct RemoteStopTransactionResponse : public Message {
    RemoteStartStopStatus status;

    /// \brief Provides the type of this RemoteStopTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RemoteStopTransactionResponse \p k to a given json object \p j
void to_json(json& j, const RemoteStopTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given RemoteStopTransactionResponse \p k
void from_json(const json& j, RemoteStopTransactionResponse& k);

/// \brief Writes the string representation of the given RemoteStopTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RemoteStopTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RemoteStopTransactionResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_REMOTESTOPTRANSACTION_HPP
