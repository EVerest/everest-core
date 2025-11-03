// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_GETTARIFFS_HPP
#define OCPP_V21_GETTARIFFS_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP GetTariffs message
struct GetTariffsRequest : public ocpp::Message {
    int32_t evseId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetTariffs message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetTariffsRequest \p k to a given json object \p j
void to_json(json& j, const GetTariffsRequest& k);

/// \brief Conversion from a given json object \p j to a given GetTariffsRequest \p k
void from_json(const json& j, GetTariffsRequest& k);

/// \brief Writes the string representation of the given GetTariffsRequest \p k to the given output stream \p os
/// \returns an output stream with the GetTariffsRequest written to
std::ostream& operator<<(std::ostream& os, const GetTariffsRequest& k);

/// \brief Contains a OCPP GetTariffsResponse message
struct GetTariffsResponse : public ocpp::Message {
    TariffGetStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<std::vector<TariffAssignment>> tariffAssignments;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetTariffsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetTariffsResponse \p k to a given json object \p j
void to_json(json& j, const GetTariffsResponse& k);

/// \brief Conversion from a given json object \p j to a given GetTariffsResponse \p k
void from_json(const json& j, GetTariffsResponse& k);

/// \brief Writes the string representation of the given GetTariffsResponse \p k to the given output stream \p os
/// \returns an output stream with the GetTariffsResponse written to
std::ostream& operator<<(std::ostream& os, const GetTariffsResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_GETTARIFFS_HPP
