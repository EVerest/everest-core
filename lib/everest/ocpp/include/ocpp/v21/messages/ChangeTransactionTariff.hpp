// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_CHANGETRANSACTIONTARIFF_HPP
#define OCPP_V21_CHANGETRANSACTIONTARIFF_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP ChangeTransactionTariff message
struct ChangeTransactionTariffRequest : public ocpp::Message {
    Tariff tariff;
    CiString<36> transactionId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ChangeTransactionTariff message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ChangeTransactionTariffRequest \p k to a given json object \p j
void to_json(json& j, const ChangeTransactionTariffRequest& k);

/// \brief Conversion from a given json object \p j to a given ChangeTransactionTariffRequest \p k
void from_json(const json& j, ChangeTransactionTariffRequest& k);

/// \brief Writes the string representation of the given ChangeTransactionTariffRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the ChangeTransactionTariffRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeTransactionTariffRequest& k);

/// \brief Contains a OCPP ChangeTransactionTariffResponse message
struct ChangeTransactionTariffResponse : public ocpp::Message {
    TariffChangeStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ChangeTransactionTariffResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ChangeTransactionTariffResponse \p k to a given json object \p j
void to_json(json& j, const ChangeTransactionTariffResponse& k);

/// \brief Conversion from a given json object \p j to a given ChangeTransactionTariffResponse \p k
void from_json(const json& j, ChangeTransactionTariffResponse& k);

/// \brief Writes the string representation of the given ChangeTransactionTariffResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the ChangeTransactionTariffResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeTransactionTariffResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_CHANGETRANSACTIONTARIFF_HPP
