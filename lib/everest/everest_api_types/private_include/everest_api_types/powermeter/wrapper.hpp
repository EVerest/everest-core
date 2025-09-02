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

OCMFUserIdentificationStatus_Internal toInternalApi(OCMFUserIdentificationStatus_External const& val);
OCMFUserIdentificationStatus_External toExternalApi(OCMFUserIdentificationStatus_Internal const& val);

using OCMFIdentificationFlags_Internal = ::types::powermeter::OCMFIdentificationFlags;
using OCMFIdentificationFlags_External = OCMFIdentificationFlags;

OCMFIdentificationFlags_Internal toInternalApi(OCMFIdentificationFlags_External const& val);
OCMFIdentificationFlags_External toExternalApi(OCMFIdentificationFlags_Internal const& val);

using OCMFIdentificationType_Internal = ::types::powermeter::OCMFIdentificationType;
using OCMFIdentificationType_External = OCMFIdentificationType;

OCMFIdentificationType_Internal toInternalApi(OCMFIdentificationType_External const& val);
OCMFIdentificationType_External toExternalApi(OCMFIdentificationType_Internal const& val);

using OCMFIdentificationLevel_Internal = ::types::powermeter::OCMFIdentificationLevel;
using OCMFIdentificationLevel_External = OCMFIdentificationLevel;

OCMFIdentificationLevel_Internal toInternalApi(OCMFIdentificationLevel_External const& val);
OCMFIdentificationLevel_External toExternalApi(OCMFIdentificationLevel_Internal const& val);

using Current_Internal = ::types::units::Current;
using Current_External = Current;

Current_Internal toInternalApi(Current_External const& val);
Current_External toExternalApi(Current_Internal const& val);

using Voltage_Internal = ::types::units::Voltage;
using Voltage_External = Voltage;

Voltage_Internal toInternalApi(Voltage_External const& val);
Voltage_External toExternalApi(Voltage_Internal const& val);

using Frequency_Internal = ::types::units::Frequency;
using Frequency_External = Frequency;

Frequency_Internal toInternalApi(Frequency_External const& val);
Frequency_External toExternalApi(Frequency_Internal const& val);

using Power_Internal = ::types::units::Power;
using Power_External = Power;

Power_Internal toInternalApi(Power_External const& val);
Power_External toExternalApi(Power_Internal const& val);

using ReactivePower_Internal = ::types::units::ReactivePower;
using ReactivePower_External = ReactivePower;

ReactivePower_Internal toInternalApi(ReactivePower_External const& val);
ReactivePower_External toExternalApi(ReactivePower_Internal const& val);

using Energy_Internal = ::types::units::Energy;
using Energy_External = Energy;

Energy_Internal toInternalApi(Energy_External const& val);
Energy_External toExternalApi(Energy_Internal const& val);

using SignedMeterValue_Internal = ::types::units_signed::SignedMeterValue;
using SignedMeterValue_External = SignedMeterValue;

SignedMeterValue_Internal toInternalApi(SignedMeterValue_External const& val);
SignedMeterValue_External toExternalApi(SignedMeterValue_Internal const& val);

using SignedCurrent_Internal = ::types::units_signed::Current;
using SignedCurrent_External = SignedCurrent;

SignedCurrent_Internal toInternalApi(SignedCurrent_External const& val);
SignedCurrent_External toExternalApi(SignedCurrent_Internal const& val);

using SignedVoltage_Internal = ::types::units_signed::Voltage;
using SignedVoltage_External = SignedVoltage;

SignedVoltage_Internal toInternalApi(SignedVoltage_External const& val);
SignedVoltage_External toExternalApi(SignedVoltage_Internal const& val);

using SignedFrequency_Internal = ::types::units_signed::Frequency;
using SignedFrequency_External = SignedFrequency;

SignedFrequency_Internal toInternalApi(SignedFrequency_External const& val);
SignedFrequency_External toExternalApi(SignedFrequency_Internal const& val);

using SignedPower_Internal = ::types::units_signed::Power;
using SignedPower_External = SignedPower;

SignedPower_Internal toInternalApi(SignedPower_External const& val);
SignedPower_External toExternalApi(SignedPower_Internal const& val);

using SignedReactivePower_Internal = ::types::units_signed::ReactivePower;
using SignedReactivePower_External = SignedReactivePower;

SignedReactivePower_Internal toInternalApi(SignedReactivePower_External const& val);
SignedReactivePower_External toExternalApi(SignedReactivePower_Internal const& val);

using SignedEnergy_Internal = ::types::units_signed::Energy;
using SignedEnergy_External = SignedEnergy;

SignedEnergy_Internal toInternalApi(SignedEnergy_External const& val);
SignedEnergy_External toExternalApi(SignedEnergy_Internal const& val);

using Temperature_Internal = ::types::temperature::Temperature;
using Temperature_External = Temperature;

Temperature_Internal toInternalApi(Temperature_External const& val);
Temperature_External toExternalApi(Temperature_Internal const& val);

using PowermeterValues_Internal = ::types::powermeter::Powermeter;
using PowermeterValues_External = PowermeterValues;

PowermeterValues_Internal toInternalApi(PowermeterValues_External const& val);
PowermeterValues_External toExternalApi(PowermeterValues_Internal const& val);

using TransactionStatus_Internal = ::types::powermeter::TransactionRequestStatus;
using TransactionStatus_External = TransactionStatus;

TransactionStatus_Internal toInternalApi(TransactionStatus_External const& val);
TransactionStatus_External toExternalApi(TransactionStatus_Internal const& val);

using ReplyStartTransaction_Internal = ::types::powermeter::TransactionStartResponse;
using ReplyStartTransaction_External = ReplyStartTransaction;

ReplyStartTransaction_Internal toInternalApi(ReplyStartTransaction_External const& val);
ReplyStartTransaction_External toExternalApi(ReplyStartTransaction_Internal const& val);

using ReplyStopTransaction_Internal = ::types::powermeter::TransactionStopResponse;
using ReplyStopTransaction_External = ReplyStopTransaction;

ReplyStopTransaction_Internal toInternalApi(ReplyStopTransaction_External const& val);
ReplyStopTransaction_External toExternalApi(ReplyStopTransaction_Internal const& val);

using RequestStartTransaction_Internal = ::types::powermeter::TransactionReq;
using RequestStartTransaction_External = RequestStartTransaction;

RequestStartTransaction_External toExternalApi(RequestStartTransaction_Internal const& val);

} // namespace everest::lib::API::V1_0::types::powermeter
