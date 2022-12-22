// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_UNLOCKCONNECTOR_HPP
#define OCPP_V16_UNLOCKCONNECTOR_HPP

#include <ocpp/v16/enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP UnlockConnector message
struct UnlockConnectorRequest : public ocpp::Message {
    int32_t connectorId;

    /// \brief Provides the type of this UnlockConnector message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given UnlockConnectorRequest \p k to a given json object \p j
void to_json(json& j, const UnlockConnectorRequest& k);

/// \brief Conversion from a given json object \p j to a given UnlockConnectorRequest \p k
void from_json(const json& j, UnlockConnectorRequest& k);

/// \brief Writes the string representation of the given UnlockConnectorRequest \p k to the given output stream \p os
/// \returns an output stream with the UnlockConnectorRequest written to
std::ostream& operator<<(std::ostream& os, const UnlockConnectorRequest& k);

/// \brief Contains a OCPP UnlockConnectorResponse message
struct UnlockConnectorResponse : public ocpp::Message {
    UnlockStatus status;

    /// \brief Provides the type of this UnlockConnectorResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given UnlockConnectorResponse \p k to a given json object \p j
void to_json(json& j, const UnlockConnectorResponse& k);

/// \brief Conversion from a given json object \p j to a given UnlockConnectorResponse \p k
void from_json(const json& j, UnlockConnectorResponse& k);

/// \brief Writes the string representation of the given UnlockConnectorResponse \p k to the given output stream \p os
/// \returns an output stream with the UnlockConnectorResponse written to
std::ostream& operator<<(std::ostream& os, const UnlockConnectorResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_UNLOCKCONNECTOR_HPP
