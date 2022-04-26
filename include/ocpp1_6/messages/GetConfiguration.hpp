// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETCONFIGURATION_HPP
#define OCPP1_6_GETCONFIGURATION_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetConfiguration message
struct GetConfigurationRequest : public Message {
    boost::optional<std::vector<CiString50Type>> key;

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

/// \brief Contains a OCPP 1.6 GetConfigurationResponse message
struct GetConfigurationResponse : public Message {
    boost::optional<std::vector<KeyValue>> configurationKey;
    boost::optional<std::vector<CiString50Type>> unknownKey;

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

} // namespace ocpp1_6

#endif // OCPP1_6_GETCONFIGURATION_HPP
