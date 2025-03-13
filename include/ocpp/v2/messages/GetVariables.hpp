// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_GETVARIABLES_HPP
#define OCPP_V2_GETVARIABLES_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP GetVariables message
struct GetVariablesRequest : public ocpp::Message {
    std::vector<GetVariableData> getVariableData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetVariables message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetVariablesRequest \p k to a given json object \p j
void to_json(json& j, const GetVariablesRequest& k);

/// \brief Conversion from a given json object \p j to a given GetVariablesRequest \p k
void from_json(const json& j, GetVariablesRequest& k);

/// \brief Writes the string representation of the given GetVariablesRequest \p k to the given output stream \p os
/// \returns an output stream with the GetVariablesRequest written to
std::ostream& operator<<(std::ostream& os, const GetVariablesRequest& k);

/// \brief Contains a OCPP GetVariablesResponse message
struct GetVariablesResponse : public ocpp::Message {
    std::vector<GetVariableResult> getVariableResult;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetVariablesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetVariablesResponse \p k to a given json object \p j
void to_json(json& j, const GetVariablesResponse& k);

/// \brief Conversion from a given json object \p j to a given GetVariablesResponse \p k
void from_json(const json& j, GetVariablesResponse& k);

/// \brief Writes the string representation of the given GetVariablesResponse \p k to the given output stream \p os
/// \returns an output stream with the GetVariablesResponse written to
std::ostream& operator<<(std::ostream& os, const GetVariablesResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_GETVARIABLES_HPP
