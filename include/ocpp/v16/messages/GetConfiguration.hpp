// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_GETCONFIGURATION_HPP
#define OCPP_V16_GETCONFIGURATION_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP GetConfiguration message
struct GetConfigurationRequest : public ocpp::Message {
    std::optional<std::vector<CiString<50>>> key;

    /// \brief Provides the type of this GetConfiguration message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetConfigurationRequest \p k to a given json object \p j
void to_json(json& j, const GetConfigurationRequest& k);

/// \brief Conversion from a given json object \p j to a given GetConfigurationRequest \p k
void from_json(const json& j, GetConfigurationRequest& k);

/// \brief Writes the string representation of the given GetConfigurationRequest \p k to the given output stream \p os
/// \returns an output stream with the GetConfigurationRequest written to
std::ostream& operator<<(std::ostream& os, const GetConfigurationRequest& k);

/// \brief Contains a OCPP GetConfigurationResponse message
struct GetConfigurationResponse : public ocpp::Message {
    std::optional<std::vector<KeyValue>> configurationKey;
    std::optional<std::vector<CiString<50>>> unknownKey;

    /// \brief Provides the type of this GetConfigurationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetConfigurationResponse \p k to a given json object \p j
void to_json(json& j, const GetConfigurationResponse& k);

/// \brief Conversion from a given json object \p j to a given GetConfigurationResponse \p k
void from_json(const json& j, GetConfigurationResponse& k);

/// \brief Writes the string representation of the given GetConfigurationResponse \p k to the given output stream \p os
/// \returns an output stream with the GetConfigurationResponse written to
std::ostream& operator<<(std::ostream& os, const GetConfigurationResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_GETCONFIGURATION_HPP
