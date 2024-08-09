// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_CHANGECONFIGURATION_HPP
#define OCPP_V16_CHANGECONFIGURATION_HPP

#include <nlohmann/json_fwd.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP ChangeConfiguration message
struct ChangeConfigurationRequest : public ocpp::Message {
    CiString<50> key;
    CiString<500> value;

    /// \brief Provides the type of this ChangeConfiguration message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ChangeConfigurationRequest \p k to a given json object \p j
void to_json(json& j, const ChangeConfigurationRequest& k);

/// \brief Conversion from a given json object \p j to a given ChangeConfigurationRequest \p k
void from_json(const json& j, ChangeConfigurationRequest& k);

/// \brief Writes the string representation of the given ChangeConfigurationRequest \p k to the given output stream \p
/// os \returns an output stream with the ChangeConfigurationRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeConfigurationRequest& k);

/// \brief Contains a OCPP ChangeConfigurationResponse message
struct ChangeConfigurationResponse : public ocpp::Message {
    ConfigurationStatus status;

    /// \brief Provides the type of this ChangeConfigurationResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ChangeConfigurationResponse \p k to a given json object \p j
void to_json(json& j, const ChangeConfigurationResponse& k);

/// \brief Conversion from a given json object \p j to a given ChangeConfigurationResponse \p k
void from_json(const json& j, ChangeConfigurationResponse& k);

/// \brief Writes the string representation of the given ChangeConfigurationResponse \p k to the given output stream \p
/// os \returns an output stream with the ChangeConfigurationResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeConfigurationResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CHANGECONFIGURATION_HPP
