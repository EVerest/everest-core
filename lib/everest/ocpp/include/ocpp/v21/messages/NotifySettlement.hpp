// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYSETTLEMENT_HPP
#define OCPP_V21_NOTIFYSETTLEMENT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifySettlement message
struct NotifySettlementRequest : public ocpp::Message {
    CiString<255> pspRef;
    PaymentStatusEnum status;
    float settlementAmount;
    ocpp::DateTime settlementTime;
    std::optional<CiString<36>> transactionId;
    std::optional<CiString<500>> statusInfo;
    std::optional<CiString<50>> receiptId;
    std::optional<CiString<2000>> receiptUrl;
    std::optional<Address> vatCompany;
    std::optional<CiString<20>> vatNumber;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifySettlement message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifySettlementRequest \p k to a given json object \p j
void to_json(json& j, const NotifySettlementRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifySettlementRequest \p k
void from_json(const json& j, NotifySettlementRequest& k);

/// \brief Writes the string representation of the given NotifySettlementRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifySettlementRequest written to
std::ostream& operator<<(std::ostream& os, const NotifySettlementRequest& k);

/// \brief Contains a OCPP NotifySettlementResponse message
struct NotifySettlementResponse : public ocpp::Message {
    std::optional<CiString<2000>> receiptUrl;
    std::optional<CiString<50>> receiptId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifySettlementResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifySettlementResponse \p k to a given json object \p j
void to_json(json& j, const NotifySettlementResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifySettlementResponse \p k
void from_json(const json& j, NotifySettlementResponse& k);

/// \brief Writes the string representation of the given NotifySettlementResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifySettlementResponse written to
std::ostream& operator<<(std::ostream& os, const NotifySettlementResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYSETTLEMENT_HPP
