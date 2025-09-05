// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeter/wrapper.hpp"
#include "generated/types/units_signed.hpp"
#include "powermeter/API.hpp"
#include <optional>

namespace everest::lib::API::V1_0::types::powermeter {

template <class T> T to_internal_api(T const& val) {
    return val;
}

template <class T> T to_external_api(T const& val) {
    return val;
}

template <class SrcT>
auto to_internal_api(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    if (src) {
        return std::make_optional(to_internal_api(src.value()));
    }
    return std::nullopt;
}

template <class SrcT>
auto to_external_api(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    if (src) {
        return std::make_optional(to_external_api(src.value()));
    }
    return std::nullopt;
}

OCMFUserIdentificationStatus_Internal to_internal_api(OCMFUserIdentificationStatus_External const& val) {
    switch (val) {
    case OCMFUserIdentificationStatus_External::ASSIGNED:
        return OCMFUserIdentificationStatus_Internal::ASSIGNED;
    case OCMFUserIdentificationStatus_External::NOT_ASSIGNED:
        return OCMFUserIdentificationStatus_Internal::NOT_ASSIGNED;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFUserIdentificationStatus API");
}

OCMFUserIdentificationStatus_External to_external_api(OCMFUserIdentificationStatus_Internal const& val) {
    switch (val) {
    case OCMFUserIdentificationStatus_Internal::ASSIGNED:
        return OCMFUserIdentificationStatus_External::ASSIGNED;
    case OCMFUserIdentificationStatus_Internal::NOT_ASSIGNED:
        return OCMFUserIdentificationStatus_External::NOT_ASSIGNED;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFUserIdentificationStatus API");
}

OCMFIdentificationFlags_Internal to_internal_api(OCMFIdentificationFlags_External const& val) {
    switch (val) {
    case OCMFIdentificationFlags_External::RFID_NONE:
        return OCMFIdentificationFlags_Internal::RFID_NONE;
    case OCMFIdentificationFlags_External::RFID_PLAIN:
        return OCMFIdentificationFlags_Internal::RFID_PLAIN;
    case OCMFIdentificationFlags_External::RFID_RELATED:
        return OCMFIdentificationFlags_Internal::RFID_RELATED;
    case OCMFIdentificationFlags_External::RFID_PSK:
        return OCMFIdentificationFlags_Internal::RFID_PSK;
    case OCMFIdentificationFlags_External::OCPP_NONE:
        return OCMFIdentificationFlags_Internal::OCPP_NONE;
    case OCMFIdentificationFlags_External::OCPP_RS:
        return OCMFIdentificationFlags_Internal::OCPP_RS;
    case OCMFIdentificationFlags_External::OCPP_AUTH:
        return OCMFIdentificationFlags_Internal::OCPP_AUTH;
    case OCMFIdentificationFlags_External::OCPP_RS_TLS:
        return OCMFIdentificationFlags_Internal::OCPP_RS_TLS;
    case OCMFIdentificationFlags_External::OCPP_AUTH_TLS:
        return OCMFIdentificationFlags_Internal::OCPP_AUTH_TLS;
    case OCMFIdentificationFlags_External::OCPP_CACHE:
        return OCMFIdentificationFlags_Internal::OCPP_CACHE;
    case OCMFIdentificationFlags_External::OCPP_WHITELIST:
        return OCMFIdentificationFlags_Internal::OCPP_WHITELIST;
    case OCMFIdentificationFlags_External::OCPP_CERTIFIED:
        return OCMFIdentificationFlags_Internal::OCPP_CERTIFIED;
    case OCMFIdentificationFlags_External::ISO15118_NONE:
        return OCMFIdentificationFlags_Internal::ISO15118_NONE;
    case OCMFIdentificationFlags_External::ISO15118_PNC:
        return OCMFIdentificationFlags_Internal::ISO15118_PNC;
    case OCMFIdentificationFlags_External::PLMN_NONE:
        return OCMFIdentificationFlags_Internal::PLMN_NONE;
    case OCMFIdentificationFlags_External::PLMN_RING:
        return OCMFIdentificationFlags_Internal::PLMN_RING;
    case OCMFIdentificationFlags_External::PLMN_SMS:
        return OCMFIdentificationFlags_Internal::PLMN_SMS;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFIdentificationFlags API");
}

OCMFIdentificationFlags_External to_external_api(OCMFIdentificationFlags_Internal const& val) {
    switch (val) {
    case OCMFIdentificationFlags_Internal::RFID_NONE:
        return OCMFIdentificationFlags_External::RFID_NONE;
    case OCMFIdentificationFlags_Internal::RFID_PLAIN:
        return OCMFIdentificationFlags_External::RFID_PLAIN;
    case OCMFIdentificationFlags_Internal::RFID_RELATED:
        return OCMFIdentificationFlags_External::RFID_RELATED;
    case OCMFIdentificationFlags_Internal::RFID_PSK:
        return OCMFIdentificationFlags_External::RFID_PSK;
    case OCMFIdentificationFlags_Internal::OCPP_NONE:
        return OCMFIdentificationFlags_External::OCPP_NONE;
    case OCMFIdentificationFlags_Internal::OCPP_RS:
        return OCMFIdentificationFlags_External::OCPP_RS;
    case OCMFIdentificationFlags_Internal::OCPP_AUTH:
        return OCMFIdentificationFlags_External::OCPP_AUTH;
    case OCMFIdentificationFlags_Internal::OCPP_RS_TLS:
        return OCMFIdentificationFlags_External::OCPP_RS_TLS;
    case OCMFIdentificationFlags_Internal::OCPP_AUTH_TLS:
        return OCMFIdentificationFlags_External::OCPP_AUTH_TLS;
    case OCMFIdentificationFlags_Internal::OCPP_CACHE:
        return OCMFIdentificationFlags_External::OCPP_CACHE;
    case OCMFIdentificationFlags_Internal::OCPP_WHITELIST:
        return OCMFIdentificationFlags_External::OCPP_WHITELIST;
    case OCMFIdentificationFlags_Internal::OCPP_CERTIFIED:
        return OCMFIdentificationFlags_External::OCPP_CERTIFIED;
    case OCMFIdentificationFlags_Internal::ISO15118_NONE:
        return OCMFIdentificationFlags_External::ISO15118_NONE;
    case OCMFIdentificationFlags_Internal::ISO15118_PNC:
        return OCMFIdentificationFlags_External::ISO15118_PNC;
    case OCMFIdentificationFlags_Internal::PLMN_NONE:
        return OCMFIdentificationFlags_External::PLMN_NONE;
    case OCMFIdentificationFlags_Internal::PLMN_RING:
        return OCMFIdentificationFlags_External::PLMN_RING;
    case OCMFIdentificationFlags_Internal::PLMN_SMS:
        return OCMFIdentificationFlags_External::PLMN_SMS;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFIdentificationFlags API");
}

OCMFIdentificationType_Internal to_internal_api(OCMFIdentificationType_External const& val) {
    switch (val) {
    case OCMFIdentificationType_External::NONE:
        return OCMFIdentificationType_Internal::NONE;
    case OCMFIdentificationType_External::DENIED:
        return OCMFIdentificationType_Internal::DENIED;
    case OCMFIdentificationType_External::UNDEFINED:
        return OCMFIdentificationType_Internal::UNDEFINED;
    case OCMFIdentificationType_External::ISO14443:
        return OCMFIdentificationType_Internal::ISO14443;
    case OCMFIdentificationType_External::ISO15693:
        return OCMFIdentificationType_Internal::ISO15693;
    case OCMFIdentificationType_External::EMAID:
        return OCMFIdentificationType_Internal::EMAID;
    case OCMFIdentificationType_External::EVCCID:
        return OCMFIdentificationType_Internal::EVCCID;
    case OCMFIdentificationType_External::EVCOID:
        return OCMFIdentificationType_Internal::EVCOID;
    case OCMFIdentificationType_External::ISO7812:
        return OCMFIdentificationType_Internal::ISO7812;
    case OCMFIdentificationType_External::CARD_TXN_NR:
        return OCMFIdentificationType_Internal::CARD_TXN_NR;
    case OCMFIdentificationType_External::CENTRAL:
        return OCMFIdentificationType_Internal::CENTRAL;
    case OCMFIdentificationType_External::CENTRAL_1:
        return OCMFIdentificationType_Internal::CENTRAL_1;
    case OCMFIdentificationType_External::CENTRAL_2:
        return OCMFIdentificationType_Internal::CENTRAL_2;
    case OCMFIdentificationType_External::LOCAL:
        return OCMFIdentificationType_Internal::LOCAL;
    case OCMFIdentificationType_External::LOCAL_1:
        return OCMFIdentificationType_Internal::LOCAL_1;
    case OCMFIdentificationType_External::LOCAL_2:
        return OCMFIdentificationType_Internal::LOCAL_2;
    case OCMFIdentificationType_External::PHONE_NUMBER:
        return OCMFIdentificationType_Internal::PHONE_NUMBER;
    case OCMFIdentificationType_External::KEY_CODE:
        return OCMFIdentificationType_Internal::KEY_CODE;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFIdentificationType API");
}

OCMFIdentificationType_External to_external_api(OCMFIdentificationType_Internal const& val) {
    switch (val) {
    case OCMFIdentificationType_Internal::NONE:
        return OCMFIdentificationType_External::NONE;
    case OCMFIdentificationType_Internal::DENIED:
        return OCMFIdentificationType_External::DENIED;
    case OCMFIdentificationType_Internal::UNDEFINED:
        return OCMFIdentificationType_External::UNDEFINED;
    case OCMFIdentificationType_Internal::ISO14443:
        return OCMFIdentificationType_External::ISO14443;
    case OCMFIdentificationType_Internal::ISO15693:
        return OCMFIdentificationType_External::ISO15693;
    case OCMFIdentificationType_Internal::EMAID:
        return OCMFIdentificationType_External::EMAID;
    case OCMFIdentificationType_Internal::EVCCID:
        return OCMFIdentificationType_External::EVCCID;
    case OCMFIdentificationType_Internal::EVCOID:
        return OCMFIdentificationType_External::EVCOID;
    case OCMFIdentificationType_Internal::ISO7812:
        return OCMFIdentificationType_External::ISO7812;
    case OCMFIdentificationType_Internal::CARD_TXN_NR:
        return OCMFIdentificationType_External::CARD_TXN_NR;
    case OCMFIdentificationType_Internal::CENTRAL:
        return OCMFIdentificationType_External::CENTRAL;
    case OCMFIdentificationType_Internal::CENTRAL_1:
        return OCMFIdentificationType_External::CENTRAL_1;
    case OCMFIdentificationType_Internal::CENTRAL_2:
        return OCMFIdentificationType_External::CENTRAL_2;
    case OCMFIdentificationType_Internal::LOCAL:
        return OCMFIdentificationType_External::LOCAL;
    case OCMFIdentificationType_Internal::LOCAL_1:
        return OCMFIdentificationType_External::LOCAL_1;
    case OCMFIdentificationType_Internal::LOCAL_2:
        return OCMFIdentificationType_External::LOCAL_2;
    case OCMFIdentificationType_Internal::PHONE_NUMBER:
        return OCMFIdentificationType_External::PHONE_NUMBER;
    case OCMFIdentificationType_Internal::KEY_CODE:
        return OCMFIdentificationType_External::KEY_CODE;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFIdentificationType API");
}

OCMFIdentificationLevel_Internal to_internal_api(OCMFIdentificationLevel_External const& val) {
    switch (val) {
    case OCMFIdentificationLevel_External::NONE:
        return OCMFIdentificationLevel_Internal::NONE;
    case OCMFIdentificationLevel_External::HEARSAY:
        return OCMFIdentificationLevel_Internal::HEARSAY;
    case OCMFIdentificationLevel_External::TRUSTED:
        return OCMFIdentificationLevel_Internal::TRUSTED;
    case OCMFIdentificationLevel_External::VERIFIED:
        return OCMFIdentificationLevel_Internal::VERIFIED;
    case OCMFIdentificationLevel_External::CERTIFIED:
        return OCMFIdentificationLevel_Internal::CERTIFIED;
    case OCMFIdentificationLevel_External::SECURE:
        return OCMFIdentificationLevel_Internal::SECURE;
    case OCMFIdentificationLevel_External::MISMATCH:
        return OCMFIdentificationLevel_Internal::MISMATCH;
    case OCMFIdentificationLevel_External::INVALID:
        return OCMFIdentificationLevel_Internal::INVALID;
    case OCMFIdentificationLevel_External::OUTDATED:
        return OCMFIdentificationLevel_Internal::OUTDATED;
    case OCMFIdentificationLevel_External::UNKNOWN:
        return OCMFIdentificationLevel_Internal::UNKNOWN;
    }

    throw std::out_of_range("No know conversion between internal and external OCMFIdentificationLevel API");
}

OCMFIdentificationLevel_External to_external_api(OCMFIdentificationLevel_Internal const& val) {
    switch (val) {
    case OCMFIdentificationLevel_Internal::NONE:
        return OCMFIdentificationLevel_External::NONE;
    case OCMFIdentificationLevel_Internal::HEARSAY:
        return OCMFIdentificationLevel_External::HEARSAY;
    case OCMFIdentificationLevel_Internal::TRUSTED:
        return OCMFIdentificationLevel_External::TRUSTED;
    case OCMFIdentificationLevel_Internal::VERIFIED:
        return OCMFIdentificationLevel_External::VERIFIED;
    case OCMFIdentificationLevel_Internal::CERTIFIED:
        return OCMFIdentificationLevel_External::CERTIFIED;
    case OCMFIdentificationLevel_Internal::SECURE:
        return OCMFIdentificationLevel_External::SECURE;
    case OCMFIdentificationLevel_Internal::MISMATCH:
        return OCMFIdentificationLevel_External::MISMATCH;
    case OCMFIdentificationLevel_Internal::INVALID:
        return OCMFIdentificationLevel_External::INVALID;
    case OCMFIdentificationLevel_Internal::OUTDATED:
        return OCMFIdentificationLevel_External::OUTDATED;
    case OCMFIdentificationLevel_Internal::UNKNOWN:
        return OCMFIdentificationLevel_External::UNKNOWN;
    }

    throw std::out_of_range("No know conversion between internal and external OCMFIdentificationLevel API");
}

Temperature_Internal to_internal_api(Temperature_External const& val) {
    auto result = Temperature_Internal();
    result.temperature = val.temperature;
    result.identification = to_internal_api(val.identification);
    return result;
}

Temperature_External to_external_api(Temperature_Internal const& val) {
    auto result = Temperature_External();
    result.temperature = val.temperature;
    result.identification = to_external_api(val.identification);
    return result;
}

Current_Internal to_internal_api(Current_External const& val) {
    auto result = Current_Internal();
    result.DC = to_internal_api(val.DC);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    result.N = to_internal_api(val.N);
    return result;
}

Current_External to_external_api(Current_Internal const& val) {
    auto result = Current_External();
    result.DC = to_external_api(val.DC);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    result.N = to_external_api(val.N);
    return result;
}

Voltage_Internal to_internal_api(Voltage_External const& val) {
    auto result = Voltage_Internal();
    result.DC = to_internal_api(val.DC);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

Voltage_External to_external_api(Voltage_Internal const& val) {
    auto result = Voltage_External();
    result.DC = to_external_api(val.DC);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

Frequency_Internal to_internal_api(Frequency_External const& val) {
    auto result = Frequency_Internal();
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

Frequency_External to_external_api(Frequency_Internal const& val) {
    auto result = Frequency_External();
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

Energy_Internal to_internal_api(Energy_External const& val) {
    auto result = Energy_Internal();
    result.total = to_internal_api(val.total);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

Energy_External to_external_api(Energy_Internal const& val) {
    auto result = Energy_External();
    result.total = to_external_api(val.total);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

Power_Internal to_internal_api(Power_External const& val) {
    auto result = Power_Internal();
    result.total = to_internal_api(val.total);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

Power_External to_external_api(Power_Internal const& val) {
    auto result = Power_External();
    result.total = to_external_api(val.total);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

ReactivePower_Internal to_internal_api(ReactivePower_External const& val) {
    auto result = ReactivePower_Internal();
    result.total = to_internal_api(val.total);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

ReactivePower_External to_external_api(ReactivePower_Internal const& val) {
    auto result = ReactivePower_External();
    result.total = to_external_api(val.total);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

SignedMeterValue_Internal to_internal_api(SignedMeterValue_External const& val) {
    SignedMeterValue_Internal result;
    result.signed_meter_data = val.signed_meter_data;
    result.signing_method = val.signing_method;
    result.encoding_method = val.encoding_method;
    result.public_key = val.public_key;
    result.timestamp = val.timestamp;
    return result;
}

SignedMeterValue_External to_external_api(SignedMeterValue_Internal const& val) {
    SignedMeterValue_External result;
    result.signed_meter_data = val.signed_meter_data;
    result.signing_method = val.signing_method;
    result.encoding_method = val.encoding_method;
    result.public_key = val.public_key;
    result.timestamp = val.timestamp;
    return result;
}

SignedCurrent_Internal to_internal_api(SignedCurrent_External const& val) {
    auto result = SignedCurrent_Internal();
    result.DC = to_internal_api(val.DC);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    result.N = to_internal_api(val.N);
    return result;
}

SignedCurrent_External to_external_api(SignedCurrent_Internal const& val) {
    auto result = SignedCurrent_External();
    result.DC = to_external_api(val.DC);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    result.N = to_external_api(val.N);
    return result;
}

SignedVoltage_Internal to_internal_api(SignedVoltage_External const& val) {
    auto result = SignedVoltage_Internal();
    result.DC = to_internal_api(val.DC);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

SignedVoltage_External to_external_api(SignedVoltage_Internal const& val) {
    auto result = SignedVoltage_External();
    result.DC = to_external_api(val.DC);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

SignedFrequency_Internal to_internal_api(SignedFrequency_External const& val) {
    auto result = SignedFrequency_Internal();
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

SignedFrequency_External to_external_api(SignedFrequency_Internal const& val) {
    auto result = SignedFrequency_External();
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

SignedPower_Internal to_internal_api(SignedPower_External const& val) {
    auto result = SignedPower_Internal();
    result.total = to_internal_api(val.total);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

SignedPower_External to_external_api(SignedPower_Internal const& val) {
    auto result = SignedPower_External();
    result.total = to_external_api(val.total);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

SignedReactivePower_Internal to_internal_api(SignedReactivePower_External const& val) {
    auto result = SignedReactivePower_Internal();
    result.total = to_internal_api(val.total);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

SignedReactivePower_External to_external_api(SignedReactivePower_Internal const& val) {
    auto result = SignedReactivePower_External();
    result.total = to_external_api(val.total);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

SignedEnergy_Internal to_internal_api(SignedEnergy_External const& val) {
    auto result = SignedEnergy_Internal();
    result.total = to_internal_api(val.total);
    result.L1 = to_internal_api(val.L1);
    result.L2 = to_internal_api(val.L2);
    result.L3 = to_internal_api(val.L3);
    return result;
}

SignedEnergy_External to_external_api(SignedEnergy_Internal const& val) {
    auto result = SignedEnergy_External();
    result.total = to_external_api(val.total);
    result.L1 = to_external_api(val.L1);
    result.L2 = to_external_api(val.L2);
    result.L3 = to_external_api(val.L3);
    return result;
}

PowermeterValues_Internal to_internal_api(PowermeterValues_External const& val) {
    auto result = PowermeterValues_Internal();
    result.timestamp = val.timestamp;
    result.energy_Wh_import = to_internal_api(val.energy_Wh_import);
    result.meter_id = to_internal_api(val.meter_id);
    result.phase_seq_error = to_internal_api(val.phase_seq_error);
    result.energy_Wh_export = to_internal_api(val.energy_Wh_export);
    result.power_W = to_internal_api(val.power_W);
    result.voltage_V = to_internal_api(val.voltage_V);
    result.VAR = to_internal_api(val.VAR);
    result.current_A = to_internal_api(val.current_A);
    result.frequency_Hz = to_internal_api(val.frequency_Hz);
    result.energy_Wh_import_signed = to_internal_api(val.energy_Wh_import_signed);
    result.energy_Wh_export_signed = to_internal_api(val.energy_Wh_export_signed);
    result.power_W_signed = to_internal_api(val.power_W_signed);
    result.voltage_V_signed = to_internal_api(val.voltage_V_signed);
    result.VAR_signed = to_internal_api(val.VAR_signed);
    result.current_A_signed = to_internal_api(val.current_A_signed);
    result.frequency_Hz_signed = to_internal_api(val.frequency_Hz_signed);
    result.signed_meter_value = to_internal_api(val.signed_meter_value);
    if (val.temperatures) {
        auto& tmp = result.temperatures.emplace();
        for (auto const& elem : val.temperatures.value()) {
            tmp.push_back(to_internal_api(elem));
        }
    }

    return result;
}

PowermeterValues_External to_external_api(PowermeterValues_Internal const& val) {
    auto result = PowermeterValues_External();
    result.timestamp = to_external_api(val.timestamp);
    result.energy_Wh_import = to_external_api(val.energy_Wh_import);
    result.meter_id = to_external_api(val.meter_id);
    result.phase_seq_error = val.phase_seq_error;
    result.energy_Wh_export = to_external_api(val.energy_Wh_export);
    result.power_W = to_external_api(val.power_W);
    result.voltage_V = to_external_api(val.voltage_V);
    result.VAR = to_external_api(val.VAR);
    result.current_A = to_external_api(val.current_A);
    result.frequency_Hz = to_external_api(val.frequency_Hz);
    result.energy_Wh_import_signed = to_external_api(val.energy_Wh_import_signed);
    result.energy_Wh_export_signed = to_external_api(val.energy_Wh_export_signed);
    result.power_W_signed = to_external_api(val.power_W_signed);
    result.voltage_V_signed = to_external_api(val.voltage_V_signed);
    result.VAR_signed = to_external_api(val.VAR_signed);
    result.current_A_signed = to_external_api(val.current_A_signed);
    result.frequency_Hz_signed = to_external_api(val.frequency_Hz_signed);
    result.signed_meter_value = to_external_api(val.signed_meter_value);
    if (val.temperatures) {
        auto& tmp = result.temperatures.emplace();
        for (auto const& elem : val.temperatures.value()) {
            tmp.push_back(to_external_api(elem));
        }
    }

    return result;
}

TransactionStatus_Internal to_internal_api(TransactionStatus_External const& val) {
    switch (val) {
    case TransactionStatus_External::OK:
        return TransactionStatus_Internal::OK;
    case TransactionStatus_External::NOT_SUPPORTED:
        return TransactionStatus_Internal::NOT_SUPPORTED;
    case TransactionStatus_External::UNEXPECTED_ERROR:
        return TransactionStatus_Internal::UNEXPECTED_ERROR;
    }

    throw std::out_of_range("No known conversion from external to internal TransactionStatus API");
}

TransactionStatus_External to_external_api(TransactionStatus_Internal const& val) {
    switch (val) {
    case TransactionStatus_Internal::OK:
        return TransactionStatus_External::OK;
    case TransactionStatus_Internal::NOT_SUPPORTED:
        return TransactionStatus_External::NOT_SUPPORTED;
    case TransactionStatus_Internal::UNEXPECTED_ERROR:
        return TransactionStatus_External::UNEXPECTED_ERROR;
    }

    throw std::out_of_range("No known conversion from internal to external TransactionStatus API");
}

ReplyStartTransaction_Internal to_internal_api(ReplyStartTransaction_External const& val) {
    auto internal = ReplyStartTransaction_Internal();
    internal.status = to_internal_api(val.status);
    internal.error = val.error;
    internal.transaction_min_stop_time = val.transaction_min_stop_time;
    internal.transaction_max_stop_time = val.transaction_max_stop_time;
    return internal;
}

ReplyStartTransaction_External to_external_api(ReplyStartTransaction_Internal const& val) {
    auto result = ReplyStartTransaction_External();
    result.status = to_external_api(val.status);
    result.error = val.error;
    result.transaction_min_stop_time = val.transaction_min_stop_time;
    result.transaction_max_stop_time = val.transaction_max_stop_time;
    return result;
}

ReplyStopTransaction_Internal to_internal_api(ReplyStopTransaction_External const& val) {
    auto internal = ReplyStopTransaction_Internal();
    internal.status = to_internal_api(val.status);
    if (val.signed_meter_value) {
        internal.signed_meter_value.emplace();
        internal.signed_meter_value->signed_meter_data = val.signed_meter_value->signed_meter_data;
        internal.signed_meter_value->signing_method = val.signed_meter_value->signing_method;
        internal.signed_meter_value->encoding_method = val.signed_meter_value->encoding_method;
        internal.signed_meter_value->public_key = val.signed_meter_value->public_key;
        internal.signed_meter_value->timestamp = val.signed_meter_value->timestamp;
    }
    internal.error = val.error;
    return internal;
}

ReplyStopTransaction_External to_external_api(ReplyStopTransaction_Internal const& val) {
    auto result = ReplyStopTransaction_External();
    result.status = to_external_api(val.status);
    if (val.signed_meter_value) {
        result.signed_meter_value.emplace();
        result.signed_meter_value->signed_meter_data = val.signed_meter_value->signed_meter_data;
        result.signed_meter_value->signing_method = val.signed_meter_value->signing_method;
        result.signed_meter_value->encoding_method = val.signed_meter_value->encoding_method;
        result.signed_meter_value->public_key = val.signed_meter_value->public_key;
        result.signed_meter_value->timestamp = val.signed_meter_value->timestamp;
    }
    result.error = val.error;
    return result;
}

RequestStartTransaction_External to_external_api(const RequestStartTransaction_Internal& val) {
    RequestStartTransaction result;
    result.evse_id = val.evse_id;
    result.transaction_id = val.transaction_id;
    result.identification_status = to_external_api(val.identification_status);
    for (auto elem : val.identification_flags) {
        result.identification_flags.push_back(to_external_api(elem));
    }
    result.identification_type = to_external_api(val.identification_type);
    if (val.identification_level) {
        result.identification_level = to_external_api(val.identification_level.value());
    }
    if (val.identification_data) {
        result.identification_data = val.identification_data;
    }
    result.tariff_text = val.tariff_text;
    return result;
}

} // namespace everest::lib::API::V1_0::types::powermeter
