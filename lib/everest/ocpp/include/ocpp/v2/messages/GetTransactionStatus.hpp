// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_GETTRANSACTIONSTATUS_HPP
#define OCPP_V2_GETTRANSACTIONSTATUS_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP GetTransactionStatus message
struct GetTransactionStatusRequest : public ocpp::Message {
    std::optional<CiString<36>> transactionId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetTransactionStatus message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetTransactionStatusRequest \p k to a given json object \p j
void to_json(json& j, const GetTransactionStatusRequest& k);

/// \brief Conversion from a given json object \p j to a given GetTransactionStatusRequest \p k
void from_json(const json& j, GetTransactionStatusRequest& k);

/// \brief Writes the string representation of the given GetTransactionStatusRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the GetTransactionStatusRequest written to
std::ostream& operator<<(std::ostream& os, const GetTransactionStatusRequest& k);

/// \brief Contains a OCPP GetTransactionStatusResponse message
struct GetTransactionStatusResponse : public ocpp::Message {
    bool messagesInQueue;
    std::optional<bool> ongoingIndicator;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetTransactionStatusResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetTransactionStatusResponse \p k to a given json object \p j
void to_json(json& j, const GetTransactionStatusResponse& k);

/// \brief Conversion from a given json object \p j to a given GetTransactionStatusResponse \p k
void from_json(const json& j, GetTransactionStatusResponse& k);

/// \brief Writes the string representation of the given GetTransactionStatusResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the GetTransactionStatusResponse written to
std::ostream& operator<<(std::ostream& os, const GetTransactionStatusResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_GETTRANSACTIONSTATUS_HPP
