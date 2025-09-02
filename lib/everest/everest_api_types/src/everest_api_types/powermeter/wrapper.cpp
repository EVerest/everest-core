// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeter/wrapper.hpp"
#include "generated/types/units_signed.hpp"
#include "powermeter/API.hpp"
#include <optional>

namespace everest::lib::API::V1_0::types::powermeter {

template <class T> T toInternalApi(T const& val) {
    return val;
}

template <class T> T toExternalApi(T const& val) {
    return val;
}

template <class SrcT>
auto toInternalApi(std::optional<SrcT> const& src) -> std::optional<decltype(toInternalApi(src.value()))> {
    if (src) {
        return std::make_optional(toInternalApi(src.value()));
    }
    return std::nullopt;
}

template <class SrcT>
auto toExternalApi(std::optional<SrcT> const& src) -> std::optional<decltype(toExternalApi(src.value()))> {
    if (src) {
        return std::make_optional(toExternalApi(src.value()));
    }
    return std::nullopt;
}

OCMFUserIdentificationStatus_Internal toInternalApi(OCMFUserIdentificationStatus_External const& val) {
    switch (val) {
    case OCMFUserIdentificationStatus_External::ASSIGNED:
        return OCMFUserIdentificationStatus_Internal::ASSIGNED;
    case OCMFUserIdentificationStatus_External::NOT_ASSIGNED:
        return OCMFUserIdentificationStatus_Internal::NOT_ASSIGNED;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFUserIdentificationStatus API");
}

OCMFUserIdentificationStatus_External toExternalApi(OCMFUserIdentificationStatus_Internal const& val) {
    switch (val) {
    case OCMFUserIdentificationStatus_Internal::ASSIGNED:
        return OCMFUserIdentificationStatus_External::ASSIGNED;
    case OCMFUserIdentificationStatus_Internal::NOT_ASSIGNED:
        return OCMFUserIdentificationStatus_External::NOT_ASSIGNED;
    }
    throw std::out_of_range("No know conversion between internal and external OCMFUserIdentificationStatus API");
}

OCMFIdentificationFlags_Internal toInternalApi(OCMFIdentificationFlags_External const& val) {
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

OCMFIdentificationFlags_External toExternalApi(OCMFIdentificationFlags_Internal const& val) {
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

OCMFIdentificationType_Internal toInternalApi(OCMFIdentificationType_External const& val) {
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

OCMFIdentificationType_External toExternalApi(OCMFIdentificationType_Internal const& val) {
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

OCMFIdentificationLevel_Internal toInternalApi(OCMFIdentificationLevel_External const& val) {
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

OCMFIdentificationLevel_External toExternalApi(OCMFIdentificationLevel_Internal const& val) {
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

Temperature_Internal toInternalApi(Temperature_External const& val) {
    auto result = Temperature_Internal();
    result.temperature = val.temperature;
    result.identification = toInternalApi(val.identification);
    return result;
}

Temperature_External toExternalApi(Temperature_Internal const& val) {
    auto result = Temperature_External();
    result.temperature = val.temperature;
    result.identification = toExternalApi(val.identification);
    return result;
}

Current_Internal toInternalApi(Current_External const& val) {
    auto result = Current_Internal();
    result.DC = toInternalApi(val.DC);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    result.N = toInternalApi(val.N);
    return result;
}

Current_External toExternalApi(Current_Internal const& val) {
    auto result = Current_External();
    result.DC = toExternalApi(val.DC);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    result.N = toExternalApi(val.N);
    return result;
}

Voltage_Internal toInternalApi(Voltage_External const& val) {
    auto result = Voltage_Internal();
    result.DC = toInternalApi(val.DC);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

Voltage_External toExternalApi(Voltage_Internal const& val) {
    auto result = Voltage_External();
    result.DC = toExternalApi(val.DC);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

Frequency_Internal toInternalApi(Frequency_External const& val) {
    auto result = Frequency_Internal();
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

Frequency_External toExternalApi(Frequency_Internal const& val) {
    auto result = Frequency_External();
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

Energy_Internal toInternalApi(Energy_External const& val) {
    auto result = Energy_Internal();
    result.total = toInternalApi(val.total);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

Energy_External toExternalApi(Energy_Internal const& val) {
    auto result = Energy_External();
    result.total = toExternalApi(val.total);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

Power_Internal toInternalApi(Power_External const& val) {
    auto result = Power_Internal();
    result.total = toInternalApi(val.total);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

Power_External toExternalApi(Power_Internal const& val) {
    auto result = Power_External();
    result.total = toExternalApi(val.total);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

ReactivePower_Internal toInternalApi(ReactivePower_External const& val) {
    auto result = ReactivePower_Internal();
    result.total = toInternalApi(val.total);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

ReactivePower_External toExternalApi(ReactivePower_Internal const& val) {
    auto result = ReactivePower_External();
    result.total = toExternalApi(val.total);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

SignedMeterValue_Internal toInternalApi(SignedMeterValue_External const& val) {
    SignedMeterValue_Internal result;
    result.signed_meter_data = val.signed_meter_data;
    result.signing_method = val.signing_method;
    result.encoding_method = val.encoding_method;
    result.public_key = val.public_key;
    result.timestamp = val.timestamp;
    return result;
}

SignedMeterValue_External toExternalApi(SignedMeterValue_Internal const& val) {
    SignedMeterValue_External result;
    result.signed_meter_data = val.signed_meter_data;
    result.signing_method = val.signing_method;
    result.encoding_method = val.encoding_method;
    result.public_key = val.public_key;
    result.timestamp = val.timestamp;
    return result;
}

SignedCurrent_Internal toInternalApi(SignedCurrent_External const& val) {
    auto result = SignedCurrent_Internal();
    result.DC = toInternalApi(val.DC);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    result.N = toInternalApi(val.N);
    return result;
}

SignedCurrent_External toExternalApi(SignedCurrent_Internal const& val) {
    auto result = SignedCurrent_External();
    result.DC = toExternalApi(val.DC);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    result.N = toExternalApi(val.N);
    return result;
}

SignedVoltage_Internal toInternalApi(SignedVoltage_External const& val) {
    auto result = SignedVoltage_Internal();
    result.DC = toInternalApi(val.DC);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

SignedVoltage_External toExternalApi(SignedVoltage_Internal const& val) {
    auto result = SignedVoltage_External();
    result.DC = toExternalApi(val.DC);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

SignedFrequency_Internal toInternalApi(SignedFrequency_External const& val) {
    auto result = SignedFrequency_Internal();
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

SignedFrequency_External toExternalApi(SignedFrequency_Internal const& val) {
    auto result = SignedFrequency_External();
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

SignedPower_Internal toInternalApi(SignedPower_External const& val) {
    auto result = SignedPower_Internal();
    result.total = toInternalApi(val.total);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

SignedPower_External toExternalApi(SignedPower_Internal const& val) {
    auto result = SignedPower_External();
    result.total = toExternalApi(val.total);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

SignedReactivePower_Internal toInternalApi(SignedReactivePower_External const& val) {
    auto result = SignedReactivePower_Internal();
    result.total = toInternalApi(val.total);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

SignedReactivePower_External toExternalApi(SignedReactivePower_Internal const& val) {
    auto result = SignedReactivePower_External();
    result.total = toExternalApi(val.total);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

SignedEnergy_Internal toInternalApi(SignedEnergy_External const& val) {
    auto result = SignedEnergy_Internal();
    result.total = toInternalApi(val.total);
    result.L1 = toInternalApi(val.L1);
    result.L2 = toInternalApi(val.L2);
    result.L3 = toInternalApi(val.L3);
    return result;
}

SignedEnergy_External toExternalApi(SignedEnergy_Internal const& val) {
    auto result = SignedEnergy_External();
    result.total = toExternalApi(val.total);
    result.L1 = toExternalApi(val.L1);
    result.L2 = toExternalApi(val.L2);
    result.L3 = toExternalApi(val.L3);
    return result;
}

PowermeterValues_Internal toInternalApi(PowermeterValues_External const& val) {
    auto result = PowermeterValues_Internal();
    result.timestamp = val.timestamp;
    result.energy_Wh_import = toInternalApi(val.energy_Wh_import);
    result.meter_id = toInternalApi(val.meter_id);
    result.phase_seq_error = toInternalApi(val.phase_seq_error);
    result.energy_Wh_export = toInternalApi(val.energy_Wh_export);
    result.power_W = toInternalApi(val.power_W);
    result.voltage_V = toInternalApi(val.voltage_V);
    result.VAR = toInternalApi(val.VAR);
    result.current_A = toInternalApi(val.current_A);
    result.frequency_Hz = toInternalApi(val.frequency_Hz);
    result.energy_Wh_import_signed = toInternalApi(val.energy_Wh_import_signed);
    result.energy_Wh_export_signed = toInternalApi(val.energy_Wh_export_signed);
    result.power_W_signed = toInternalApi(val.power_W_signed);
    result.voltage_V_signed = toInternalApi(val.voltage_V_signed);
    result.VAR_signed = toInternalApi(val.VAR_signed);
    result.current_A_signed = toInternalApi(val.current_A_signed);
    result.frequency_Hz_signed = toInternalApi(val.frequency_Hz_signed);
    result.signed_meter_value = toInternalApi(val.signed_meter_value);
    if (val.temperatures) {
        auto& tmp = result.temperatures.emplace();
        for (auto const& elem : val.temperatures.value()) {
            tmp.push_back(toInternalApi(elem));
        }
    }

    return result;
}

PowermeterValues_External toExternalApi(PowermeterValues_Internal const& val) {
    auto result = PowermeterValues_External();
    result.timestamp = toExternalApi(val.timestamp);
    result.energy_Wh_import = toExternalApi(val.energy_Wh_import);
    result.meter_id = toExternalApi(val.meter_id);
    result.phase_seq_error = val.phase_seq_error;
    result.energy_Wh_export = toExternalApi(val.energy_Wh_export);
    result.power_W = toExternalApi(val.power_W);
    result.voltage_V = toExternalApi(val.voltage_V);
    result.VAR = toExternalApi(val.VAR);
    result.current_A = toExternalApi(val.current_A);
    result.frequency_Hz = toExternalApi(val.frequency_Hz);
    result.energy_Wh_import_signed = toExternalApi(val.energy_Wh_import_signed);
    result.energy_Wh_export_signed = toExternalApi(val.energy_Wh_export_signed);
    result.power_W_signed = toExternalApi(val.power_W_signed);
    result.voltage_V_signed = toExternalApi(val.voltage_V_signed);
    result.VAR_signed = toExternalApi(val.VAR_signed);
    result.current_A_signed = toExternalApi(val.current_A_signed);
    result.frequency_Hz_signed = toExternalApi(val.frequency_Hz_signed);
    result.signed_meter_value = toExternalApi(val.signed_meter_value);
    if (val.temperatures) {
        auto& tmp = result.temperatures.emplace();
        for (auto const& elem : val.temperatures.value()) {
            tmp.push_back(toExternalApi(elem));
        }
    }

    return result;
}

TransactionStatus_Internal toInternalApi(TransactionStatus_External const& val) {
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

TransactionStatus_External toExternalApi(TransactionStatus_Internal const& val) {
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

ReplyStartTransaction_Internal toInternalApi(ReplyStartTransaction_External const& val) {
    auto internal = ReplyStartTransaction_Internal();
    internal.status = toInternalApi(val.status);
    internal.error = val.error;
    internal.transaction_min_stop_time = val.transaction_min_stop_time;
    internal.transaction_max_stop_time = val.transaction_max_stop_time;
    return internal;
}

ReplyStartTransaction_External toExternalApi(ReplyStartTransaction_Internal const& val) {
    auto result = ReplyStartTransaction_External();
    result.status = toExternalApi(val.status);
    result.error = val.error;
    result.transaction_min_stop_time = val.transaction_min_stop_time;
    result.transaction_max_stop_time = val.transaction_max_stop_time;
    return result;
}

ReplyStopTransaction_Internal toInternalApi(ReplyStopTransaction_External const& val) {
    auto internal = ReplyStopTransaction_Internal();
    internal.status = toInternalApi(val.status);
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

ReplyStopTransaction_External toExternalApi(ReplyStopTransaction_Internal const& val) {
    auto result = ReplyStopTransaction_External();
    result.status = toExternalApi(val.status);
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

RequestStartTransaction_External toExternalApi(const RequestStartTransaction_Internal& val) {
    RequestStartTransaction result;
    result.evse_id = val.evse_id;
    result.transaction_id = val.transaction_id;
    result.identification_status = toExternalApi(val.identification_status);
    for (auto elem : val.identification_flags) {
        result.identification_flags.push_back(toExternalApi(elem));
    }
    result.identification_type = toExternalApi(val.identification_type);
    if (val.identification_level) {
        result.identification_level = toExternalApi(val.identification_level.value());
    }
    if (val.identification_data) {
        result.identification_data = val.identification_data;
    }
    result.tariff_text = val.tariff_text;
    return result;
}

} // namespace everest::lib::API::V1_0::types::powermeter
