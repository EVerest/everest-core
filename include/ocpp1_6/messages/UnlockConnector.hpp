// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_UNLOCKCONNECTOR_HPP
#define OCPP1_6_UNLOCKCONNECTOR_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 UnlockConnector message
struct UnlockConnectorRequest : public Message {
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

/// \brief Contains a OCPP 1.6 UnlockConnectorResponse message
struct UnlockConnectorResponse : public Message {
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

} // namespace ocpp1_6

#endif // OCPP1_6_UNLOCKCONNECTOR_HPP
