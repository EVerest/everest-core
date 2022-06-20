// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_EXTENDEDTRIGGERMESSAGE_HPP
#define OCPP1_6_EXTENDEDTRIGGERMESSAGE_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ExtendedTriggerMessage message
struct ExtendedTriggerMessageRequest : public Message {
    MessageTriggerEnumType requestedMessage;
    boost::optional<int32_t> connectorId;

    /// \brief Provides the type of this ExtendedTriggerMessage message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ExtendedTriggerMessageRequest \p k to a given json object \p j
void to_json(json& j, const ExtendedTriggerMessageRequest& k);

/// \brief Conversion from a given json object \p j to a given ExtendedTriggerMessageRequest \p k
void from_json(const json& j, ExtendedTriggerMessageRequest& k);

/// \brief Writes the string representation of the given ExtendedTriggerMessageRequest \p k to the given output stream
/// \p os \returns an output stream with the ExtendedTriggerMessageRequest written to
std::ostream& operator<<(std::ostream& os, const ExtendedTriggerMessageRequest& k);

/// \brief Contains a OCPP 1.6 ExtendedTriggerMessageResponse message
struct ExtendedTriggerMessageResponse : public Message {
    TriggerMessageStatusEnumType status;

    /// \brief Provides the type of this ExtendedTriggerMessageResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ExtendedTriggerMessageResponse \p k to a given json object \p j
void to_json(json& j, const ExtendedTriggerMessageResponse& k);

/// \brief Conversion from a given json object \p j to a given ExtendedTriggerMessageResponse \p k
void from_json(const json& j, ExtendedTriggerMessageResponse& k);

/// \brief Writes the string representation of the given ExtendedTriggerMessageResponse \p k to the given output stream
/// \p os \returns an output stream with the ExtendedTriggerMessageResponse written to
std::ostream& operator<<(std::ostream& os, const ExtendedTriggerMessageResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_EXTENDEDTRIGGERMESSAGE_HPP
