// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_REMOTESTARTTRANSACTION_HPP
#define OCPP_V16_REMOTESTARTTRANSACTION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP RemoteStartTransaction message
struct RemoteStartTransactionRequest : public ocpp::Message {
    CiString<20> idTag;
    std::optional<int32_t> connectorId;
    std::optional<ChargingProfile> chargingProfile;

    /// \brief Provides the type of this RemoteStartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given RemoteStartTransactionRequest \p k to a given json object \p j
void to_json(json& j, const RemoteStartTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given RemoteStartTransactionRequest \p k
void from_json(const json& j, RemoteStartTransactionRequest& k);

/// \brief Writes the string representation of the given RemoteStartTransactionRequest \p k to the given output stream
/// \p os \returns an output stream with the RemoteStartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionRequest& k);

/// \brief Contains a OCPP RemoteStartTransactionResponse message
struct RemoteStartTransactionResponse : public ocpp::Message {
    RemoteStartStopStatus status;

    /// \brief Provides the type of this RemoteStartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given RemoteStartTransactionResponse \p k to a given json object \p j
void to_json(json& j, const RemoteStartTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given RemoteStartTransactionResponse \p k
void from_json(const json& j, RemoteStartTransactionResponse& k);

/// \brief Writes the string representation of the given RemoteStartTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RemoteStartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_REMOTESTARTTRANSACTION_HPP
