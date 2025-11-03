// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/types/temperature.hpp"
#include <everest_api_types/powermeter/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/powermeter.hpp"
#include "generated/types/units_signed.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::powermeter {

using OCMFUserIdentificationStatus_Internal = ::types::powermeter::OCMFUserIdentificationStatus;
using OCMFUserIdentificationStatus_External = OCMFUserIdentificationStatus;

OCMFUserIdentificationStatus_Internal to_internal_api(OCMFUserIdentificationStatus_External const& val);
OCMFUserIdentificationStatus_External to_external_api(OCMFUserIdentificationStatus_Internal const& val);

using OCMFIdentificationFlags_Internal = ::types::powermeter::OCMFIdentificationFlags;
using OCMFIdentificationFlags_External = OCMFIdentificationFlags;

OCMFIdentificationFlags_Internal to_internal_api(OCMFIdentificationFlags_External const& val);
OCMFIdentificationFlags_External to_external_api(OCMFIdentificationFlags_Internal const& val);

using OCMFIdentificationType_Internal = ::types::powermeter::OCMFIdentificationType;
using OCMFIdentificationType_External = OCMFIdentificationType;

OCMFIdentificationType_Internal to_internal_api(OCMFIdentificationType_External const& val);
OCMFIdentificationType_External to_external_api(OCMFIdentificationType_Internal const& val);

using OCMFIdentificationLevel_Internal = ::types::powermeter::OCMFIdentificationLevel;
using OCMFIdentificationLevel_External = OCMFIdentificationLevel;

OCMFIdentificationLevel_Internal to_internal_api(OCMFIdentificationLevel_External const& val);
OCMFIdentificationLevel_External to_external_api(OCMFIdentificationLevel_Internal const& val);

using Current_Internal = ::types::units::Current;
using Current_External = Current;

Current_Internal to_internal_api(Current_External const& val);
Current_External to_external_api(Current_Internal const& val);

using Voltage_Internal = ::types::units::Voltage;
using Voltage_External = Voltage;

Voltage_Internal to_internal_api(Voltage_External const& val);
Voltage_External to_external_api(Voltage_Internal const& val);

using Frequency_Internal = ::types::units::Frequency;
using Frequency_External = Frequency;

Frequency_Internal to_internal_api(Frequency_External const& val);
Frequency_External to_external_api(Frequency_Internal const& val);

using Power_Internal = ::types::units::Power;
using Power_External = Power;

Power_Internal to_internal_api(Power_External const& val);
Power_External to_external_api(Power_Internal const& val);

using ReactivePower_Internal = ::types::units::ReactivePower;
using ReactivePower_External = ReactivePower;

ReactivePower_Internal to_internal_api(ReactivePower_External const& val);
ReactivePower_External to_external_api(ReactivePower_Internal const& val);

using Energy_Internal = ::types::units::Energy;
using Energy_External = Energy;

Energy_Internal to_internal_api(Energy_External const& val);
Energy_External to_external_api(Energy_Internal const& val);

using SignedMeterValue_Internal = ::types::units_signed::SignedMeterValue;
using SignedMeterValue_External = SignedMeterValue;

SignedMeterValue_Internal to_internal_api(SignedMeterValue_External const& val);
SignedMeterValue_External to_external_api(SignedMeterValue_Internal const& val);

using SignedCurrent_Internal = ::types::units_signed::Current;
using SignedCurrent_External = SignedCurrent;

SignedCurrent_Internal to_internal_api(SignedCurrent_External const& val);
SignedCurrent_External to_external_api(SignedCurrent_Internal const& val);

using SignedVoltage_Internal = ::types::units_signed::Voltage;
using SignedVoltage_External = SignedVoltage;

SignedVoltage_Internal to_internal_api(SignedVoltage_External const& val);
SignedVoltage_External to_external_api(SignedVoltage_Internal const& val);

using SignedFrequency_Internal = ::types::units_signed::Frequency;
using SignedFrequency_External = SignedFrequency;

SignedFrequency_Internal to_internal_api(SignedFrequency_External const& val);
SignedFrequency_External to_external_api(SignedFrequency_Internal const& val);

using SignedPower_Internal = ::types::units_signed::Power;
using SignedPower_External = SignedPower;

SignedPower_Internal to_internal_api(SignedPower_External const& val);
SignedPower_External to_external_api(SignedPower_Internal const& val);

using SignedReactivePower_Internal = ::types::units_signed::ReactivePower;
using SignedReactivePower_External = SignedReactivePower;

SignedReactivePower_Internal to_internal_api(SignedReactivePower_External const& val);
SignedReactivePower_External to_external_api(SignedReactivePower_Internal const& val);

using SignedEnergy_Internal = ::types::units_signed::Energy;
using SignedEnergy_External = SignedEnergy;

SignedEnergy_Internal to_internal_api(SignedEnergy_External const& val);
SignedEnergy_External to_external_api(SignedEnergy_Internal const& val);

using Temperature_Internal = ::types::temperature::Temperature;
using Temperature_External = Temperature;

Temperature_Internal to_internal_api(Temperature_External const& val);
Temperature_External to_external_api(Temperature_Internal const& val);

using PowermeterValues_Internal = ::types::powermeter::Powermeter;
using PowermeterValues_External = PowermeterValues;

PowermeterValues_Internal to_internal_api(PowermeterValues_External const& val);
PowermeterValues_External to_external_api(PowermeterValues_Internal const& val);

using TransactionStatus_Internal = ::types::powermeter::TransactionRequestStatus;
using TransactionStatus_External = TransactionStatus;

TransactionStatus_Internal to_internal_api(TransactionStatus_External const& val);
TransactionStatus_External to_external_api(TransactionStatus_Internal const& val);

using ReplyStartTransaction_Internal = ::types::powermeter::TransactionStartResponse;
using ReplyStartTransaction_External = ReplyStartTransaction;

ReplyStartTransaction_Internal to_internal_api(ReplyStartTransaction_External const& val);
ReplyStartTransaction_External to_external_api(ReplyStartTransaction_Internal const& val);

using ReplyStopTransaction_Internal = ::types::powermeter::TransactionStopResponse;
using ReplyStopTransaction_External = ReplyStopTransaction;

ReplyStopTransaction_Internal to_internal_api(ReplyStopTransaction_External const& val);
ReplyStopTransaction_External to_external_api(ReplyStopTransaction_Internal const& val);

using RequestStartTransaction_Internal = ::types::powermeter::TransactionReq;
using RequestStartTransaction_External = RequestStartTransaction;

RequestStartTransaction_External to_external_api(RequestStartTransaction_Internal const& val);

} // namespace everest::lib::API::V1_0::types::powermeter
