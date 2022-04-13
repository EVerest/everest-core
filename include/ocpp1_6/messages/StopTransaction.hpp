// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_STOPTRANSACTION_HPP
#define OCPP1_6_STOPTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 StopTransaction message
struct StopTransactionRequest : public Message {
    int32_t meterStop;
    DateTime timestamp;
    int32_t transactionId;
    boost::optional<CiString20Type> idTag;
    boost::optional<Reason> reason;
    boost::optional<std::vector<TransactionData>> transactionData;

    /// \brief Provides the type of this StopTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given StopTransactionRequest \p k to a given json object \p j
void to_json(json& j, const StopTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given StopTransactionRequest \p k
void from_json(const json& j, StopTransactionRequest& k);

/// \brief Writes the string representation of the given StopTransactionRequest \p k to the given output stream \p os
/// \returns an output stream with the StopTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const StopTransactionRequest& k);

/// \brief Contains a OCPP 1.6 StopTransactionResponse message
struct StopTransactionResponse : public Message {
    boost::optional<IdTagInfo> idTagInfo;

    /// \brief Provides the type of this StopTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given StopTransactionResponse \p k to a given json object \p j
void to_json(json& j, const StopTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given StopTransactionResponse \p k
void from_json(const json& j, StopTransactionResponse& k);

/// \brief Writes the string representation of the given StopTransactionResponse \p k to the given output stream \p os
/// \returns an output stream with the StopTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const StopTransactionResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_STOPTRANSACTION_HPP
