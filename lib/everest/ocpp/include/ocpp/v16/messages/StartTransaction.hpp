// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_STARTTRANSACTION_HPP
#define OCPP_V16_STARTTRANSACTION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP StartTransaction message
struct StartTransactionRequest : public ocpp::Message {
    int32_t connectorId;
    CiString<20> idTag;
    int32_t meterStart;
    ocpp::DateTime timestamp;
    std::optional<int32_t> reservationId;

    /// \brief Provides the type of this StartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given StartTransactionRequest \p k to a given json object \p j
void to_json(json& j, const StartTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given StartTransactionRequest \p k
void from_json(const json& j, StartTransactionRequest& k);

/// \brief Writes the string representation of the given StartTransactionRequest \p k to the given output stream \p os
/// \returns an output stream with the StartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const StartTransactionRequest& k);

/// \brief Contains a OCPP StartTransactionResponse message
struct StartTransactionResponse : public ocpp::Message {
    IdTagInfo idTagInfo;
    int32_t transactionId;

    /// \brief Provides the type of this StartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given StartTransactionResponse \p k to a given json object \p j
void to_json(json& j, const StartTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given StartTransactionResponse \p k
void from_json(const json& j, StartTransactionResponse& k);

/// \brief Writes the string representation of the given StartTransactionResponse \p k to the given output stream \p os
/// \returns an output stream with the StartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const StartTransactionResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_STARTTRANSACTION_HPP
