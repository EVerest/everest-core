// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_GETLOCALLISTVERSION_HPP
#define OCPP_V16_GETLOCALLISTVERSION_HPP

#include <nlohmann/json_fwd.hpp>

#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP GetLocalListVersion message
struct GetLocalListVersionRequest : public ocpp::Message {

    /// \brief Provides the type of this GetLocalListVersion message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetLocalListVersionRequest \p k to a given json object \p j
void to_json(json& j, const GetLocalListVersionRequest& k);

/// \brief Conversion from a given json object \p j to a given GetLocalListVersionRequest \p k
void from_json(const json& j, GetLocalListVersionRequest& k);

/// \brief Writes the string representation of the given GetLocalListVersionRequest \p k to the given output stream \p
/// os \returns an output stream with the GetLocalListVersionRequest written to
std::ostream& operator<<(std::ostream& os, const GetLocalListVersionRequest& k);

/// \brief Contains a OCPP GetLocalListVersionResponse message
struct GetLocalListVersionResponse : public ocpp::Message {
    int32_t listVersion;

    /// \brief Provides the type of this GetLocalListVersionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetLocalListVersionResponse \p k to a given json object \p j
void to_json(json& j, const GetLocalListVersionResponse& k);

/// \brief Conversion from a given json object \p j to a given GetLocalListVersionResponse \p k
void from_json(const json& j, GetLocalListVersionResponse& k);

/// \brief Writes the string representation of the given GetLocalListVersionResponse \p k to the given output stream \p
/// os \returns an output stream with the GetLocalListVersionResponse written to
std::ostream& operator<<(std::ostream& os, const GetLocalListVersionResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_GETLOCALLISTVERSION_HPP
