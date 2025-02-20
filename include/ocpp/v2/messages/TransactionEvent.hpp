// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_TRANSACTIONEVENT_HPP
#define OCPP_V2_TRANSACTIONEVENT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP TransactionEvent message
struct TransactionEventRequest : public ocpp::Message {
    TransactionEventEnum eventType;
    ocpp::DateTime timestamp;
    TriggerReasonEnum triggerReason;
    int32_t seqNo;
    Transaction transactionInfo;
    std::optional<CustomData> customData;
    std::optional<std::vector<MeterValue>> meterValue;
    std::optional<bool> offline;
    std::optional<int32_t> numberOfPhasesUsed;
    std::optional<int32_t> cableMaxCurrent;
    std::optional<int32_t> reservationId;
    std::optional<EVSE> evse;
    std::optional<IdToken> idToken;

    /// \brief Provides the type of this TransactionEvent message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given TransactionEventRequest \p k to a given json object \p j
void to_json(json& j, const TransactionEventRequest& k);

/// \brief Conversion from a given json object \p j to a given TransactionEventRequest \p k
void from_json(const json& j, TransactionEventRequest& k);

/// \brief Writes the string representation of the given TransactionEventRequest \p k to the given output stream \p os
/// \returns an output stream with the TransactionEventRequest written to
std::ostream& operator<<(std::ostream& os, const TransactionEventRequest& k);

/// \brief Contains a OCPP TransactionEventResponse message
struct TransactionEventResponse : public ocpp::Message {
    std::optional<CustomData> customData;
    std::optional<float> totalCost;
    std::optional<int32_t> chargingPriority;
    std::optional<IdTokenInfo> idTokenInfo;
    std::optional<MessageContent> updatedPersonalMessage;

    /// \brief Provides the type of this TransactionEventResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given TransactionEventResponse \p k to a given json object \p j
void to_json(json& j, const TransactionEventResponse& k);

/// \brief Conversion from a given json object \p j to a given TransactionEventResponse \p k
void from_json(const json& j, TransactionEventResponse& k);

/// \brief Writes the string representation of the given TransactionEventResponse \p k to the given output stream \p os
/// \returns an output stream with the TransactionEventResponse written to
std::ostream& operator<<(std::ostream& os, const TransactionEventResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_TRANSACTIONEVENT_HPP
