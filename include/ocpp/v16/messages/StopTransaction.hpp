// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_STOPTRANSACTION_HPP
#define OCPP_V16_STOPTRANSACTION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP StopTransaction message
struct StopTransactionRequest : public ocpp::Message {
    int32_t meterStop;
    ocpp::DateTime timestamp;
    int32_t transactionId;
    std::optional<CiString<20>> idTag;
    std::optional<Reason> reason;
    std::optional<std::vector<TransactionData>> transactionData;

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

/// \brief Contains a OCPP StopTransactionResponse message
struct StopTransactionResponse : public ocpp::Message {
    std::optional<IdTagInfo> idTagInfo;

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

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_STOPTRANSACTION_HPP
