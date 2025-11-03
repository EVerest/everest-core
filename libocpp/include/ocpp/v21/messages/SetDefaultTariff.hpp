// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_SETDEFAULTTARIFF_HPP
#define OCPP_V21_SETDEFAULTTARIFF_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP SetDefaultTariff message
struct SetDefaultTariffRequest : public ocpp::Message {
    int32_t evseId;
    Tariff tariff;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetDefaultTariff message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetDefaultTariffRequest \p k to a given json object \p j
void to_json(json& j, const SetDefaultTariffRequest& k);

/// \brief Conversion from a given json object \p j to a given SetDefaultTariffRequest \p k
void from_json(const json& j, SetDefaultTariffRequest& k);

/// \brief Writes the string representation of the given SetDefaultTariffRequest \p k to the given output stream \p os
/// \returns an output stream with the SetDefaultTariffRequest written to
std::ostream& operator<<(std::ostream& os, const SetDefaultTariffRequest& k);

/// \brief Contains a OCPP SetDefaultTariffResponse message
struct SetDefaultTariffResponse : public ocpp::Message {
    TariffSetStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetDefaultTariffResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetDefaultTariffResponse \p k to a given json object \p j
void to_json(json& j, const SetDefaultTariffResponse& k);

/// \brief Conversion from a given json object \p j to a given SetDefaultTariffResponse \p k
void from_json(const json& j, SetDefaultTariffResponse& k);

/// \brief Writes the string representation of the given SetDefaultTariffResponse \p k to the given output stream \p os
/// \returns an output stream with the SetDefaultTariffResponse written to
std::ostream& operator<<(std::ostream& os, const SetDefaultTariffResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_SETDEFAULTTARIFF_HPP
