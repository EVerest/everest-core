// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_CLEARTARIFFS_HPP
#define OCPP_V21_CLEARTARIFFS_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP ClearTariffs message
struct ClearTariffsRequest : public ocpp::Message {
    std::optional<std::vector<CiString<60>>> tariffIds;
    std::optional<int32_t> evseId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClearTariffs message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearTariffsRequest \p k to a given json object \p j
void to_json(json& j, const ClearTariffsRequest& k);

/// \brief Conversion from a given json object \p j to a given ClearTariffsRequest \p k
void from_json(const json& j, ClearTariffsRequest& k);

/// \brief Writes the string representation of the given ClearTariffsRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearTariffsRequest written to
std::ostream& operator<<(std::ostream& os, const ClearTariffsRequest& k);

/// \brief Contains a OCPP ClearTariffsResponse message
struct ClearTariffsResponse : public ocpp::Message {
    std::vector<ClearTariffsResult> clearTariffsResult;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClearTariffsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearTariffsResponse \p k to a given json object \p j
void to_json(json& j, const ClearTariffsResponse& k);

/// \brief Conversion from a given json object \p j to a given ClearTariffsResponse \p k
void from_json(const json& j, ClearTariffsResponse& k);

/// \brief Writes the string representation of the given ClearTariffsResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearTariffsResponse written to
std::ostream& operator<<(std::ostream& os, const ClearTariffsResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_CLEARTARIFFS_HPP
