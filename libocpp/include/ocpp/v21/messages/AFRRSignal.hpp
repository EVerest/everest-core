// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_AFRRSIGNAL_HPP
#define OCPP_V21_AFRRSIGNAL_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP AFRRSignal message
struct AFRRSignalRequest : public ocpp::Message {
    ocpp::DateTime timestamp;
    int32_t signal;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this AFRRSignal message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given AFRRSignalRequest \p k to a given json object \p j
void to_json(json& j, const AFRRSignalRequest& k);

/// \brief Conversion from a given json object \p j to a given AFRRSignalRequest \p k
void from_json(const json& j, AFRRSignalRequest& k);

/// \brief Writes the string representation of the given AFRRSignalRequest \p k to the given output stream \p os
/// \returns an output stream with the AFRRSignalRequest written to
std::ostream& operator<<(std::ostream& os, const AFRRSignalRequest& k);

/// \brief Contains a OCPP AFRRSignalResponse message
struct AFRRSignalResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this AFRRSignalResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given AFRRSignalResponse \p k to a given json object \p j
void to_json(json& j, const AFRRSignalResponse& k);

/// \brief Conversion from a given json object \p j to a given AFRRSignalResponse \p k
void from_json(const json& j, AFRRSignalResponse& k);

/// \brief Writes the string representation of the given AFRRSignalResponse \p k to the given output stream \p os
/// \returns an output stream with the AFRRSignalResponse written to
std::ostream& operator<<(std::ostream& os, const AFRRSignalResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_AFRRSIGNAL_HPP
