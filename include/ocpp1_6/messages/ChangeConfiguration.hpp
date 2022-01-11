// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHANGECONFIGURATION_HPP
#define OCPP1_6_CHANGECONFIGURATION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ChangeConfiguration message
struct ChangeConfigurationRequest : public Message {
    CiString50Type key;
    CiString500Type value;

    /// \brief Provides the type of this ChangeConfiguration message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ChangeConfiguration";
    }

    /// \brief Conversion from a given ChangeConfigurationRequest \p k to a given json object \p j
    friend void to_json(json& j, const ChangeConfigurationRequest& k) {
        // the required parts of the message
        j = json{
            {"key", k.key},
            {"value", k.value},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ChangeConfigurationRequest \p k
    friend void from_json(const json& j, ChangeConfigurationRequest& k) {
        // the required parts of the message
        k.key = j.at("key");
        k.value = j.at("value");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ChangeConfigurationRequest \p k to the given output stream
    /// \p os \returns an output stream with the ChangeConfigurationRequest written to
    friend std::ostream& operator<<(std::ostream& os, const ChangeConfigurationRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 ChangeConfigurationResponse message
struct ChangeConfigurationResponse : public Message {
    ConfigurationStatus status;

    /// \brief Provides the type of this ChangeConfigurationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "ChangeConfigurationResponse";
    }

    /// \brief Conversion from a given ChangeConfigurationResponse \p k to a given json object \p j
    friend void to_json(json& j, const ChangeConfigurationResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::configuration_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given ChangeConfigurationResponse \p k
    friend void from_json(const json& j, ChangeConfigurationResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_configuration_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given ChangeConfigurationResponse \p k to the given output stream
    /// \p os \returns an output stream with the ChangeConfigurationResponse written to
    friend std::ostream& operator<<(std::ostream& os, const ChangeConfigurationResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_CHANGECONFIGURATION_HPP
