// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_BATTERYSWAP_HPP
#define OCPP_V21_BATTERYSWAP_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP BatterySwap message
struct BatterySwapRequest : public ocpp::Message {
    std::vector<BatteryData> batteryData;
    BatterySwapEventEnum eventType;
    IdToken idToken;
    int32_t requestId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this BatterySwap message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given BatterySwapRequest \p k to a given json object \p j
void to_json(json& j, const BatterySwapRequest& k);

/// \brief Conversion from a given json object \p j to a given BatterySwapRequest \p k
void from_json(const json& j, BatterySwapRequest& k);

/// \brief Writes the string representation of the given BatterySwapRequest \p k to the given output stream \p os
/// \returns an output stream with the BatterySwapRequest written to
std::ostream& operator<<(std::ostream& os, const BatterySwapRequest& k);

/// \brief Contains a OCPP BatterySwapResponse message
struct BatterySwapResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this BatterySwapResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given BatterySwapResponse \p k to a given json object \p j
void to_json(json& j, const BatterySwapResponse& k);

/// \brief Conversion from a given json object \p j to a given BatterySwapResponse \p k
void from_json(const json& j, BatterySwapResponse& k);

/// \brief Writes the string representation of the given BatterySwapResponse \p k to the given output stream \p os
/// \returns an output stream with the BatterySwapResponse written to
std::ostream& operator<<(std::ostream& os, const BatterySwapResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_BATTERYSWAP_HPP
