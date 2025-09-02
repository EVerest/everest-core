// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/ocpp/API.hpp>

namespace everest::lib::API::V1_0::types::ocpp {

using json = nlohmann::json;

void to_json(json& j, AttributeEnum const& k) noexcept;
void from_json(const json& j, AttributeEnum& k);

void to_json(json& j, GetVariableStatusEnumType const& k) noexcept;
void from_json(const json& j, GetVariableStatusEnumType& k);

void to_json(json& j, SetVariableStatusEnumType const& k) noexcept;
void from_json(const json& j, SetVariableStatusEnumType& k);

void to_json(json& j, DataTransferStatus const& k) noexcept;
void from_json(const json& j, DataTransferStatus& k);

void to_json(json& j, RegistrationStatus const& k) noexcept;
void from_json(const json& j, RegistrationStatus& k);

void to_json(json& j, TransactionEvent const& k) noexcept;
void from_json(const json& j, TransactionEvent& k);

void to_json(json& j, CustomData const& k) noexcept;
void from_json(const json& j, CustomData& k);

void to_json(json& j, DataTransferRequest const& k) noexcept;
void from_json(const json& j, DataTransferRequest& k);

void to_json(json& j, DataTransferResponse const& k) noexcept;
void from_json(const json& j, DataTransferResponse& k);

void to_json(json& j, EVSE const& k) noexcept;
void from_json(const json& j, EVSE& k);

void to_json(json& j, Component const& k) noexcept;
void from_json(const json& j, Component& k);

void to_json(json& j, Variable const& k) noexcept;
void from_json(const json& j, Variable& k);

void to_json(json& j, ComponentVariable const& k) noexcept;
void from_json(const json& j, ComponentVariable& k);

void to_json(json& j, GetVariableRequest const& k) noexcept;
void from_json(const json& j, GetVariableRequest& k);

void to_json(json& j, GetVariableResult const& k) noexcept;
void from_json(const json& j, GetVariableResult& k);

void to_json(json& j, SetVariableRequest const& k) noexcept;
void from_json(const json& j, SetVariableRequest& k);

void to_json(json& j, SetVariableResult const& k) noexcept;
void from_json(const json& j, SetVariableResult& k);

void to_json(json& j, GetVariableRequestList const& k) noexcept;
void from_json(const json& j, GetVariableRequestList& k);

void to_json(json& j, GetVariableResultList const& k) noexcept;
void from_json(const json& j, GetVariableResultList& k);

void to_json(json& j, SetVariableRequestList const& k) noexcept;
void from_json(const json& j, SetVariableRequestList& k);

void to_json(json& j, SetVariableResultList const& k) noexcept;
void from_json(const json& j, SetVariableResultList& k);

void to_json(json& j, SetVariablesArgs const& k) noexcept;
void from_json(const json& j, SetVariablesArgs& k);

void to_json(json& j, SecurityEvent const& k) noexcept;
void from_json(const json& j, SecurityEvent& k);

void to_json(json& j, StatusInfoType const& k) noexcept;
void from_json(const json& j, StatusInfoType& k);

void to_json(json& j, BootNotificationResponse const& k) noexcept;
void from_json(const json& j, BootNotificationResponse& k);

void to_json(json& j, OcppTransactionEvent const& k) noexcept;
void from_json(const json& j, OcppTransactionEvent& k);

} // namespace everest::lib::API::V1_0::types::ocpp
