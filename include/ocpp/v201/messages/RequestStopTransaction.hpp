// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_REQUESTSTOPTRANSACTION_HPP
#define OCPP_V201_REQUESTSTOPTRANSACTION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP StopTransaction message
struct RequestStopTransactionRequest : public ocpp::Message {
    CiString<36> transactionId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this StopTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RequestStopTransactionRequest \p k to a given json object \p j
void to_json(json& j, const RequestStopTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given RequestStopTransactionRequest \p k
void from_json(const json& j, RequestStopTransactionRequest& k);

/// \brief Writes the string representation of the given RequestStopTransactionRequest \p k to the given output stream
/// \p os \returns an output stream with the RequestStopTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RequestStopTransactionRequest& k);

/// \brief Contains a OCPP StopTransactionResponse message
struct RequestStopTransactionResponse : public ocpp::Message {
    RequestStartStopStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this StopTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RequestStopTransactionResponse \p k to a given json object \p j
void to_json(json& j, const RequestStopTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given RequestStopTransactionResponse \p k
void from_json(const json& j, RequestStopTransactionResponse& k);

/// \brief Writes the string representation of the given RequestStopTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RequestStopTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RequestStopTransactionResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_REQUESTSTOPTRANSACTION_HPP
