// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHANGECONFIGURATION_HPP
#define OCPP1_6_CHANGECONFIGURATION_HPP

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 ChangeConfiguration message
struct ChangeConfigurationRequest : public Message {
    CiString50Type key;
    CiString500Type value;

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

/// \brief Contains a OCPP 1.6 ChangeConfigurationResponse message
struct ChangeConfigurationResponse : public Message {
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

} // namespace ocpp1_6

#endif // OCPP1_6_CHANGECONFIGURATION_HPP
