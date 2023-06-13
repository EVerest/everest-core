// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_REMOTESTOPTRANSACTION_HPP
#define OCPP_V16_REMOTESTOPTRANSACTION_HPP

#include <nlohmann/json_fwd.hpp>

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP RemoteStopTransaction message
struct RemoteStopTransactionRequest : public ocpp::Message {
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

/// \brief Contains a OCPP RemoteStopTransactionResponse message
struct RemoteStopTransactionResponse : public ocpp::Message {
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

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_REMOTESTOPTRANSACTION_HPP
