// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_TRANSACTIONEVENT_HPP
#define OCPP_V201_TRANSACTIONEVENT_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP TransactionEvent message
struct TransactionEventRequest : public ocpp::Message {
    TransactionEventEnum eventType;
    ocpp::DateTime timestamp;
    TriggerReasonEnum triggerReason;
    int32_t seqNo;
    Transaction transactionInfo;
    boost::optional<CustomData> customData;
    boost::optional<std::vector<MeterValue>> meterValue;
    boost::optional<bool> offline;
    boost::optional<int32_t> numberOfPhasesUsed;
    boost::optional<int32_t> cableMaxCurrent;
    boost::optional<int32_t> reservationId;
    boost::optional<EVSE> evse;
    boost::optional<IdToken> idToken;

    /// \brief Provides the type of this TransactionEvent message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
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
    boost::optional<CustomData> customData;
    boost::optional<float> totalCost;
    boost::optional<int32_t> chargingPriority;
    boost::optional<IdTokenInfo> idTokenInfo;
    boost::optional<MessageContent> updatedPersonalMessage;

    /// \brief Provides the type of this TransactionEventResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given TransactionEventResponse \p k to a given json object \p j
void to_json(json& j, const TransactionEventResponse& k);

/// \brief Conversion from a given json object \p j to a given TransactionEventResponse \p k
void from_json(const json& j, TransactionEventResponse& k);

/// \brief Writes the string representation of the given TransactionEventResponse \p k to the given output stream \p os
/// \returns an output stream with the TransactionEventResponse written to
std::ostream& operator<<(std::ostream& os, const TransactionEventResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_TRANSACTIONEVENT_HPP
