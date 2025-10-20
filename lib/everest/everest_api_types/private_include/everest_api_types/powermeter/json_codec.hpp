// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/powermeter/API.hpp>

namespace everest::lib::API::V1_0::types::powermeter {

using json = nlohmann::json;

void from_json(const json& j, OCMFUserIdentificationStatus& k);
void to_json(json& j, const OCMFUserIdentificationStatus& k) noexcept;

void from_json(const json& j, OCMFIdentificationFlags& k);
void to_json(json& j, const OCMFIdentificationFlags& k) noexcept;

void from_json(const json& j, OCMFIdentificationType& k);
void to_json(json& j, const OCMFIdentificationType& k) noexcept;

void from_json(const json& j, OCMFIdentificationLevel& k);
void to_json(json& j, const OCMFIdentificationLevel& k) noexcept;

void from_json(const json& j, TransactionStatus& k);
void to_json(json& j, const TransactionStatus& k) noexcept;

void from_json(const json& j, Current& k);
void to_json(json& j, const Current& k) noexcept;

void from_json(const json& j, Voltage& k);
void to_json(json& j, const Voltage& k) noexcept;

void from_json(const json& j, Frequency& k);
void to_json(json& j, const Frequency& k) noexcept;

void from_json(const json& j, Power& k);
void to_json(json& j, const Power& k) noexcept;

void from_json(const json& j, Energy& k);
void to_json(json& j, const Energy& k) noexcept;

void from_json(const json& j, ReactivePower& k);
void to_json(json& j, const ReactivePower& k) noexcept;

void from_json(const json& j, SignedMeterValue& k);
void to_json(json& j, const SignedMeterValue& k) noexcept;

void from_json(const json& j, SignedCurrent& k);
void to_json(json& j, const SignedCurrent& k) noexcept;

void from_json(const json& j, SignedVoltage& k);
void to_json(json& j, const SignedVoltage& k) noexcept;

void from_json(const json& j, SignedFrequency& k);
void to_json(json& j, const SignedFrequency& k) noexcept;

void from_json(const json& j, SignedEnergy& k);
void to_json(json& j, const SignedEnergy& k) noexcept;

void from_json(const json& j, SignedPower& k);
void to_json(json& j, const SignedPower& k) noexcept;

void from_json(const json& j, SignedReactivePower& k);
void to_json(json& j, const SignedReactivePower& k) noexcept;

void from_json(const json& j, Temperature& k);
void to_json(json& j, const Temperature& k) noexcept;

void from_json(const json& j, PowermeterValues& k);
void to_json(json& j, const PowermeterValues& k) noexcept;

void from_json(const json& j, ReplyStartTransaction& k);
void to_json(json& j, const ReplyStartTransaction& k) noexcept;

void from_json(const json& j, ReplyStopTransaction& k);
void to_json(json& j, const ReplyStopTransaction& k) noexcept;

void from_json(const json& j, RequestStartTransaction& k);
void to_json(json& j, const RequestStartTransaction& k) noexcept;

} // namespace everest::lib::API::V1_0::types::powermeter
