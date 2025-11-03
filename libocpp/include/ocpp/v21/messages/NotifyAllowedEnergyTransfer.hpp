// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYALLOWEDENERGYTRANSFER_HPP
#define OCPP_V21_NOTIFYALLOWEDENERGYTRANSFER_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyAllowedEnergyTransfer message
struct NotifyAllowedEnergyTransferRequest : public ocpp::Message {
    CiString<36> transactionId;
    std::vector<EnergyTransferModeEnum> allowedEnergyTransfer;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyAllowedEnergyTransfer message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyAllowedEnergyTransferRequest \p k to a given json object \p j
void to_json(json& j, const NotifyAllowedEnergyTransferRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyAllowedEnergyTransferRequest \p k
void from_json(const json& j, NotifyAllowedEnergyTransferRequest& k);

/// \brief Writes the string representation of the given NotifyAllowedEnergyTransferRequest \p k to the given output
/// stream \p os \returns an output stream with the NotifyAllowedEnergyTransferRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyAllowedEnergyTransferRequest& k);

/// \brief Contains a OCPP NotifyAllowedEnergyTransferResponse message
struct NotifyAllowedEnergyTransferResponse : public ocpp::Message {
    NotifyAllowedEnergyTransferStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyAllowedEnergyTransferResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyAllowedEnergyTransferResponse \p k to a given json object \p j
void to_json(json& j, const NotifyAllowedEnergyTransferResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyAllowedEnergyTransferResponse \p k
void from_json(const json& j, NotifyAllowedEnergyTransferResponse& k);

/// \brief Writes the string representation of the given NotifyAllowedEnergyTransferResponse \p k to the given output
/// stream \p os \returns an output stream with the NotifyAllowedEnergyTransferResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyAllowedEnergyTransferResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYALLOWEDENERGYTRANSFER_HPP
