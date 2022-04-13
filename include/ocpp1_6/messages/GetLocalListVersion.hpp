// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETLOCALLISTVERSION_HPP
#define OCPP1_6_GETLOCALLISTVERSION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetLocalListVersion message
struct GetLocalListVersionRequest : public Message {

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

/// \brief Contains a OCPP 1.6 GetLocalListVersionResponse message
struct GetLocalListVersionResponse : public Message {
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

} // namespace ocpp1_6

#endif // OCPP1_6_GETLOCALLISTVERSION_HPP
