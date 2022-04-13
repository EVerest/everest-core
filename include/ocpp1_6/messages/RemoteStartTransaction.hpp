// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_REMOTESTARTTRANSACTION_HPP
#define OCPP1_6_REMOTESTARTTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 RemoteStartTransaction message
struct RemoteStartTransactionRequest : public Message {
    CiString20Type idTag;
    boost::optional<int32_t> connectorId;
    boost::optional<ChargingProfile> chargingProfile;

    /// \brief Provides the type of this RemoteStartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RemoteStartTransactionRequest \p k to a given json object \p j
void to_json(json& j, const RemoteStartTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given RemoteStartTransactionRequest \p k
void from_json(const json& j, RemoteStartTransactionRequest& k);

/// \brief Writes the string representation of the given RemoteStartTransactionRequest \p k to the given output stream
/// \p os \returns an output stream with the RemoteStartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionRequest& k);

/// \brief Contains a OCPP 1.6 RemoteStartTransactionResponse message
struct RemoteStartTransactionResponse : public Message {
    RemoteStartStopStatus status;

    /// \brief Provides the type of this RemoteStartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RemoteStartTransactionResponse \p k to a given json object \p j
void to_json(json& j, const RemoteStartTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given RemoteStartTransactionResponse \p k
void from_json(const json& j, RemoteStartTransactionResponse& k);

/// \brief Writes the string representation of the given RemoteStartTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RemoteStartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_REMOTESTARTTRANSACTION_HPP
