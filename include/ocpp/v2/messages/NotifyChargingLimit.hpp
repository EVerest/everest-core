// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_NOTIFYCHARGINGLIMIT_HPP
#define OCPP_V2_NOTIFYCHARGINGLIMIT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP NotifyChargingLimit message
struct NotifyChargingLimitRequest : public ocpp::Message {
    ChargingLimit chargingLimit;
    std::optional<CustomData> customData;
    std::optional<std::vector<ChargingSchedule>> chargingSchedule;
    std::optional<int32_t> evseId;

    /// \brief Provides the type of this NotifyChargingLimit message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyChargingLimitRequest \p k to a given json object \p j
void to_json(json& j, const NotifyChargingLimitRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyChargingLimitRequest \p k
void from_json(const json& j, NotifyChargingLimitRequest& k);

/// \brief Writes the string representation of the given NotifyChargingLimitRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyChargingLimitRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyChargingLimitRequest& k);

/// \brief Contains a OCPP NotifyChargingLimitResponse message
struct NotifyChargingLimitResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyChargingLimitResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyChargingLimitResponse \p k to a given json object \p j
void to_json(json& j, const NotifyChargingLimitResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyChargingLimitResponse \p k
void from_json(const json& j, NotifyChargingLimitResponse& k);

/// \brief Writes the string representation of the given NotifyChargingLimitResponse \p k to the given output stream \p
/// os \returns an output stream with the NotifyChargingLimitResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyChargingLimitResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_NOTIFYCHARGINGLIMIT_HPP
