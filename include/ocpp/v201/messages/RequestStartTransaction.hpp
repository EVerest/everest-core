// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_REQUESTSTARTTRANSACTION_HPP
#define OCPP_V201_REQUESTSTARTTRANSACTION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP StartTransaction message
struct RequestStartTransactionRequest : public ocpp::Message {
    IdToken idToken;
    int32_t remoteStartId;
    std::optional<CustomData> customData;
    std::optional<int32_t> evseId;
    std::optional<IdToken> groupIdToken;
    std::optional<ChargingProfile> chargingProfile;

    /// \brief Provides the type of this StartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RequestStartTransactionRequest \p k to a given json object \p j
void to_json(json& j, const RequestStartTransactionRequest& k);

/// \brief Conversion from a given json object \p j to a given RequestStartTransactionRequest \p k
void from_json(const json& j, RequestStartTransactionRequest& k);

/// \brief Writes the string representation of the given RequestStartTransactionRequest \p k to the given output stream
/// \p os \returns an output stream with the RequestStartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RequestStartTransactionRequest& k);

/// \brief Contains a OCPP StartTransactionResponse message
struct RequestStartTransactionResponse : public ocpp::Message {
    RequestStartStopStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;
    std::optional<CiString<36>> transactionId;

    /// \brief Provides the type of this StartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given RequestStartTransactionResponse \p k to a given json object \p j
void to_json(json& j, const RequestStartTransactionResponse& k);

/// \brief Conversion from a given json object \p j to a given RequestStartTransactionResponse \p k
void from_json(const json& j, RequestStartTransactionResponse& k);

/// \brief Writes the string representation of the given RequestStartTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RequestStartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RequestStartTransactionResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_REQUESTSTARTTRANSACTION_HPP
