// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_EXTENDEDTRIGGERMESSAGE_HPP
#define OCPP_V16_EXTENDEDTRIGGERMESSAGE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP ExtendedTriggerMessage message
struct ExtendedTriggerMessageRequest : public ocpp::Message {
    MessageTriggerEnumType requestedMessage;
    std::optional<int32_t> connectorId;

    /// \brief Provides the type of this ExtendedTriggerMessage message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ExtendedTriggerMessageRequest \p k to a given json object \p j
void to_json(json& j, const ExtendedTriggerMessageRequest& k);

/// \brief Conversion from a given json object \p j to a given ExtendedTriggerMessageRequest \p k
void from_json(const json& j, ExtendedTriggerMessageRequest& k);

/// \brief Writes the string representation of the given ExtendedTriggerMessageRequest \p k to the given output stream
/// \p os \returns an output stream with the ExtendedTriggerMessageRequest written to
std::ostream& operator<<(std::ostream& os, const ExtendedTriggerMessageRequest& k);

/// \brief Contains a OCPP ExtendedTriggerMessageResponse message
struct ExtendedTriggerMessageResponse : public ocpp::Message {
    TriggerMessageStatusEnumType status;

    /// \brief Provides the type of this ExtendedTriggerMessageResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ExtendedTriggerMessageResponse \p k to a given json object \p j
void to_json(json& j, const ExtendedTriggerMessageResponse& k);

/// \brief Conversion from a given json object \p j to a given ExtendedTriggerMessageResponse \p k
void from_json(const json& j, ExtendedTriggerMessageResponse& k);

/// \brief Writes the string representation of the given ExtendedTriggerMessageResponse \p k to the given output stream
/// \p os \returns an output stream with the ExtendedTriggerMessageResponse written to
std::ostream& operator<<(std::ostream& os, const ExtendedTriggerMessageResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_EXTENDEDTRIGGERMESSAGE_HPP
