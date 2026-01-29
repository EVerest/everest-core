// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_manager/wrapper.hpp"
#include "auth/wrapper.hpp"
#include "evse_manager/API.hpp"
#include "powermeter/wrapper.hpp"
#include <optional>
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types {

namespace {
using namespace auth;
using namespace powermeter;
using namespace evse_manager;
template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    if (src) {
        return std::make_optional(to_internal_api(src.value()));
    }
    return std::nullopt;
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    if (src) {
        return std::make_optional(to_external_api(src.value()));
    }
    return std::nullopt;
}

} // namespace

namespace evse_manager {

StopTransactionReason_Internal to_internal_api(StopTransactionReason_External const& val) {
    using SrcT = StopTransactionReason_External;
    using TarT = StopTransactionReason_Internal;

    switch (val) {
    case SrcT::EmergencyStop:
        return TarT::EmergencyStop;
    case SrcT::EVDisconnected:
        return TarT::EVDisconnected;
    case SrcT::HardReset:
        return TarT::HardReset;
    case SrcT::Local:
        return TarT::Local;
    case SrcT::Other:
        return TarT::Other;
    case SrcT::PowerLoss:
        return TarT::PowerLoss;
    case SrcT::Reboot:
        return TarT::Reboot;
    case SrcT::Remote:
        return TarT::Remote;
    case SrcT::SoftReset:
        return TarT::SoftReset;
    case SrcT::UnlockCommand:
        return TarT::UnlockCommand;
    case SrcT::DeAuthorized:
        return TarT::DeAuthorized;
    case SrcT::EnergyLimitReached:
        return TarT::EnergyLimitReached;
    case SrcT::GroundFault:
        return TarT::GroundFault;
    case SrcT::LocalOutOfCredit:
        return TarT::LocalOutOfCredit;
    case SrcT::MasterPass:
        return TarT::MasterPass;
    case SrcT::OvercurrentFault:
        return TarT::OvercurrentFault;
    case SrcT::PowerQuality:
        return TarT::PowerQuality;
    case SrcT::SOCLimitReached:
        return TarT::SOCLimitReached;
    case SrcT::StoppedByEV:
        return TarT::StoppedByEV;
    case SrcT::TimeLimitReached:
        return TarT::TimeLimitReached;
    case SrcT::Timeout:
        return TarT::Timeout;
    case SrcT::ReqEnergyTransferRejected:
        return TarT::ReqEnergyTransferRejected;
    }
    throw std::out_of_range("Unexpected value for StopTransactionReason_External");
}

StopTransactionReason_External to_external_api(StopTransactionReason_Internal const& val) {
    using SrcT = StopTransactionReason_Internal;
    using TarT = StopTransactionReason_External;

    switch (val) {
    case SrcT::EmergencyStop:
        return TarT::EmergencyStop;
    case SrcT::EVDisconnected:
        return TarT::EVDisconnected;
    case SrcT::HardReset:
        return TarT::HardReset;
    case SrcT::Local:
        return TarT::Local;
    case SrcT::Other:
        return TarT::Other;
    case SrcT::PowerLoss:
        return TarT::PowerLoss;
    case SrcT::Reboot:
        return TarT::Reboot;
    case SrcT::Remote:
        return TarT::Remote;
    case SrcT::SoftReset:
        return TarT::SoftReset;
    case SrcT::UnlockCommand:
        return TarT::UnlockCommand;
    case SrcT::DeAuthorized:
        return TarT::DeAuthorized;
    case SrcT::EnergyLimitReached:
        return TarT::EnergyLimitReached;
    case SrcT::GroundFault:
        return TarT::GroundFault;
    case SrcT::LocalOutOfCredit:
        return TarT::LocalOutOfCredit;
    case SrcT::MasterPass:
        return TarT::MasterPass;
    case SrcT::OvercurrentFault:
        return TarT::OvercurrentFault;
    case SrcT::PowerQuality:
        return TarT::PowerQuality;
    case SrcT::SOCLimitReached:
        return TarT::SOCLimitReached;
    case SrcT::StoppedByEV:
        return TarT::StoppedByEV;
    case SrcT::TimeLimitReached:
        return TarT::TimeLimitReached;
    case SrcT::Timeout:
        return TarT::Timeout;
    case SrcT::ReqEnergyTransferRejected:
        return TarT::ReqEnergyTransferRejected;
    }
    throw std::out_of_range("Unexpected value for StopTransactionReason_Internal");
}

StopTransactionRequest_Internal to_internal_api(StopTransactionRequest_External const& val) {
    StopTransactionRequest_Internal result;
    result.reason = to_internal_api(val.reason);
    result.id_tag = optToInternal(val.id_tag);
    return result;
}

StopTransactionRequest_External to_external_api(StopTransactionRequest_Internal const& val) {
    StopTransactionRequest_External result;
    result.reason = to_external_api(val.reason);
    result.id_tag = optToExternal(val.id_tag);
    return result;
}

StartSessionReason_Internal to_internal_api(StartSessionReason_External const& val) {
    using SrcT = StartSessionReason_External;
    using TarT = StartSessionReason_Internal;
    switch (val) {
    case SrcT::EVConnected:
        return TarT::EVConnected;
    case SrcT::Authorized:
        return TarT::Authorized;
    }

    throw std::out_of_range("Unexpected value for StartSessionReason_External");
}

StartSessionReason_External to_external_api(StartSessionReason_Internal const& val) {
    using SrcT = StartSessionReason_Internal;
    using TarT = StartSessionReason_External;

    switch (val) {
    case SrcT::EVConnected:
        return TarT::EVConnected;
    case SrcT::Authorized:
        return TarT::Authorized;
    }

    throw std::out_of_range("Unexpected value for StartSessionReason_Internal");
}

SessionEventEnum_Internal to_internal_api(SessionEventEnum_External const& val) {
    using SrcT = SessionEventEnum_External;
    using TarT = SessionEventEnum_Internal;

    switch (val) {
    case SrcT::Authorized:
        return TarT::Authorized;
    case SrcT::Deauthorized:
        return TarT::Deauthorized;
    case SrcT::Enabled:
        return TarT::Enabled;
    case SrcT::Disabled:
        return TarT::Disabled;
    case SrcT::SessionStarted:
        return TarT::SessionStarted;
    case SrcT::AuthRequired:
        return TarT::AuthRequired;
    case SrcT::TransactionStarted:
        return TarT::TransactionStarted;
    case SrcT::PrepareCharging:
        return TarT::PrepareCharging;
    case SrcT::ChargingStarted:
        return TarT::ChargingStarted;
    case SrcT::ChargingPausedEV:
        return TarT::ChargingPausedEV;
    case SrcT::ChargingPausedEVSE:
        return TarT::ChargingPausedEVSE;
    case SrcT::WaitingForEnergy:
        return TarT::WaitingForEnergy;
    case SrcT::ChargingResumed:
        return TarT::ChargingResumed;
    case SrcT::StoppingCharging:
        return TarT::StoppingCharging;
    case SrcT::ChargingFinished:
        return TarT::ChargingFinished;
    case SrcT::TransactionFinished:
        return TarT::TransactionFinished;
    case SrcT::SessionFinished:
        return TarT::SessionFinished;
    case SrcT::ReservationStart:
        return TarT::ReservationStart;
    case SrcT::ReservationEnd:
        return TarT::ReservationEnd;
    case SrcT::ReplugStarted:
        return TarT::ReplugStarted;
    case SrcT::ReplugFinished:
        return TarT::ReplugFinished;
    case SrcT::PluginTimeout:
        return TarT::PluginTimeout;
    case SrcT::SwitchingPhases:
        return TarT::SwitchingPhases;
    case SrcT::SessionResumed:
        return TarT::SessionResumed;
    }

    throw std::out_of_range("Unexpected value for SessionEventEnum_Internal");
}

SessionEventEnum_External to_external_api(SessionEventEnum_Internal const& val) {
    using SrcT = SessionEventEnum_Internal;
    using TarT = SessionEventEnum_External;

    switch (val) {
    case SrcT::Authorized:
        return TarT::Authorized;
    case SrcT::Deauthorized:
        return TarT::Deauthorized;
    case SrcT::Enabled:
        return TarT::Enabled;
    case SrcT::Disabled:
        return TarT::Disabled;
    case SrcT::SessionStarted:
        return TarT::SessionStarted;
    case SrcT::AuthRequired:
        return TarT::AuthRequired;
    case SrcT::TransactionStarted:
        return TarT::TransactionStarted;
    case SrcT::PrepareCharging:
        return TarT::PrepareCharging;
    case SrcT::ChargingStarted:
        return TarT::ChargingStarted;
    case SrcT::ChargingPausedEV:
        return TarT::ChargingPausedEV;
    case SrcT::ChargingPausedEVSE:
        return TarT::ChargingPausedEVSE;
    case SrcT::WaitingForEnergy:
        return TarT::WaitingForEnergy;
    case SrcT::ChargingResumed:
        return TarT::ChargingResumed;
    case SrcT::StoppingCharging:
        return TarT::StoppingCharging;
    case SrcT::ChargingFinished:
        return TarT::ChargingFinished;
    case SrcT::TransactionFinished:
        return TarT::TransactionFinished;
    case SrcT::SessionFinished:
        return TarT::SessionFinished;
    case SrcT::ReservationStart:
        return TarT::ReservationStart;
    case SrcT::ReservationEnd:
        return TarT::ReservationEnd;
    case SrcT::ReplugStarted:
        return TarT::ReplugStarted;
    case SrcT::ReplugFinished:
        return TarT::ReplugFinished;
    case SrcT::PluginTimeout:
        return TarT::PluginTimeout;
    case SrcT::SwitchingPhases:
        return TarT::SwitchingPhases;
    case SrcT::SessionResumed:
        return TarT::SessionResumed;
    }

    throw std::out_of_range("Unexpected value for SessionEventEnum_Internal");
}

SessionEvent_Internal to_internal_api(SessionEvent_External const& val) {
    SessionEvent_Internal result;
    result.uuid = val.uuid;
    result.timestamp = val.timestamp;
    result.event = to_internal_api(val.event);
    result.connector_id = val.connector_id;
    result.session_started = optToInternal(val.session_started);
    result.session_finished = optToInternal(val.session_finished);
    result.transaction_started = optToInternal(val.transaction_started);
    result.transaction_finished = optToInternal(val.transaction_finished);
    result.charging_state_changed_event = optToInternal(val.charging_state_changed_event);
    result.authorization_event = optToInternal(val.authorization_event);
    result.source = optToInternal(val.source);
    return result;
}

SessionEvent_External to_external_api(SessionEvent_Internal const& val) {
    SessionEvent_External result;
    result.uuid = val.uuid;
    result.timestamp = val.timestamp;
    result.event = to_external_api(val.event);
    result.connector_id = val.connector_id;
    result.session_started = optToExternal(val.session_started);
    result.session_finished = optToExternal(val.session_finished);
    result.transaction_started = optToExternal(val.transaction_started);
    result.transaction_finished = optToExternal(val.transaction_finished);
    result.charging_state_changed_event = optToExternal(val.charging_state_changed_event);
    result.source = optToExternal(val.source);
    return result;
}

Limits_Internal to_internal_api(Limits_External const& val) {
    Limits_Internal result;
    result.max_current = val.max_current;
    result.nr_of_phases_available = val.nr_of_phases_available;
    result.uuid = val.uuid;
    return result;
}

Limits_External to_external_api(Limits_Internal const& val) {
    Limits_External result;
    result.max_current = val.max_current;
    result.nr_of_phases_available = val.nr_of_phases_available;
    result.uuid = val.uuid;
    return result;
}

EVInfo_Internal to_internal_api(EVInfo_External const& val) {
    EVInfo_Internal result;
    result.soc = val.soc;
    result.present_voltage = val.present_voltage;
    result.present_current = val.present_current;
    result.target_voltage = val.target_voltage;
    result.target_current = val.target_current;
    result.maximum_current_limit = val.maximum_current_limit;
    result.minimum_current_limit = val.minimum_current_limit;
    result.maximum_voltage_limit = val.maximum_voltage_limit;
    result.maximum_power_limit = val.maximum_power_limit;
    result.estimated_time_full = val.estimated_time_full;
    result.departure_time = val.departure_time;
    result.estimated_time_bulk = val.estimated_time_bulk;
    result.evcc_id = val.evcc_id;
    result.remaining_energy_needed = val.remaining_energy_needed;
    result.battery_capacity = val.battery_capacity;
    result.battery_full_soc = val.battery_full_soc;
    result.battery_bulk_soc = val.battery_bulk_soc;
    return result;
}

EVInfo_External to_external_api(EVInfo_Internal const& val) {
    EVInfo_External result;
    result.soc = val.soc;
    result.present_voltage = val.present_voltage;
    result.present_current = val.present_current;
    result.target_voltage = val.target_voltage;
    result.target_current = val.target_current;
    result.maximum_current_limit = val.maximum_current_limit;
    result.minimum_current_limit = val.minimum_current_limit;
    result.maximum_voltage_limit = val.maximum_voltage_limit;
    result.maximum_power_limit = val.maximum_power_limit;
    result.estimated_time_full = val.estimated_time_full;
    result.departure_time = val.departure_time;
    result.estimated_time_bulk = val.estimated_time_bulk;
    result.evcc_id = val.evcc_id;
    result.remaining_energy_needed = val.remaining_energy_needed;
    result.battery_capacity = val.battery_capacity;
    result.battery_full_soc = val.battery_full_soc;
    result.battery_bulk_soc = val.battery_bulk_soc;
    return result;
}

CarManufacturer_External to_external_api(CarManufacturer_Internal const& val) {
    using SrcT = CarManufacturer_Internal;
    using TarT = CarManufacturer_External;

    switch (val) {
    case SrcT::VolkswagenGroup:
        return TarT::VolkswagenGroup;
    case SrcT::Tesla:
        return TarT::Tesla;
    case SrcT::Unknown:
        return TarT::Unknown;
    }

    throw std::out_of_range("Unexpected value for CarManufacturer_Internal");
}

CarManufacturer_Internal to_external_api(CarManufacturer_External const& val) {
    using SrcT = CarManufacturer_External;
    using TarT = CarManufacturer_Internal;

    switch (val) {
    case SrcT::VolkswagenGroup:
        return TarT::VolkswagenGroup;
    case SrcT::Tesla:
        return TarT::Tesla;
    case SrcT::Unknown:
        return TarT::Unknown;
    }

    throw std::out_of_range("Unexpected value for CarManufacturer_Internal");
}

SessionStarted_Internal to_internal_api(SessionStarted_External const& val) {
    SessionStarted_Internal result;
    result.reason = to_internal_api(val.reason);
    result.id_tag = optToInternal(val.id_tag);
    result.meter_value = powermeter::to_internal_api(val.meter_value);
    result.signed_meter_value = optToInternal(val.signed_meter_value);
    result.reservation_id = val.reservation_id;
    result.logging_path = val.logging_path;

    return result;
}

SessionStarted_External to_external_api(SessionStarted_Internal const& val) {
    SessionStarted_External result;
    result.reason = to_external_api(val.reason);
    result.id_tag = optToExternal(val.id_tag);
    result.meter_value = powermeter::to_external_api(val.meter_value);
    result.signed_meter_value = optToExternal(val.signed_meter_value);
    result.reservation_id = val.reservation_id;
    result.logging_path = val.logging_path;

    return result;
}

SessionFinished_Internal to_internal_api(SessionFinished_External const& val) {
    SessionFinished_Internal result;
    result.meter_value = powermeter::to_internal_api(val.meter_value);
    return result;
}

SessionFinished_External to_external_api(SessionFinished_Internal const& val) {
    SessionFinished_External result;
    result.meter_value = powermeter::to_external_api(val.meter_value);
    return result;
}

TransactionStarted_Internal to_internal_api(TransactionStarted_External const& val) {
    TransactionStarted_Internal result;
    result.id_tag = auth::to_internal_api(val.id_tag);
    result.meter_value = to_internal_api(val.meter_value);
    result.signed_meter_value = optToInternal(val.signed_meter_value);
    result.reservation_id = val.reservation_id;
    return result;
}

TransactionStarted_External to_external_api(TransactionStarted_Internal const& val) {
    TransactionStarted_External result;
    result.id_tag = auth::to_external_api(val.id_tag);
    result.meter_value = powermeter::to_external_api(val.meter_value);
    result.signed_meter_value = optToExternal(val.signed_meter_value);
    result.reservation_id = val.reservation_id;
    return result;
}

TransactionFinished_Internal to_internal_api(TransactionFinished_External const& val) {
    TransactionFinished_Internal result;
    result.meter_value = powermeter::to_internal_api(val.meter_value);
    result.start_signed_meter_value = optToInternal(val.start_signed_meter_value);
    result.signed_meter_value = optToInternal(val.signed_meter_value);
    result.reason = optToInternal(val.reason);
    result.id_tag = optToInternal(val.id_tag);
    return result;
}

TransactionFinished_External to_external_api(TransactionFinished_Internal const& val) {
    TransactionFinished_External result;
    result.meter_value = powermeter::to_external_api(val.meter_value);
    result.start_signed_meter_value = optToExternal(val.start_signed_meter_value);
    result.signed_meter_value = optToExternal(val.signed_meter_value);
    result.reason = optToExternal(val.reason);
    result.id_tag = optToExternal(val.id_tag);
    return result;
}

ChargingStateChangedEvent_Internal to_internal_api(ChargingStateChangedEvent_External const& val) {
    ChargingStateChangedEvent_Internal result;
    result.meter_value = powermeter::to_internal_api(val.meter_value);
    return result;
}

ChargingStateChangedEvent_External to_external_api(ChargingStateChangedEvent_Internal const& val) {
    ChargingStateChangedEvent_External result;
    result.meter_value = powermeter::to_external_api(val.meter_value);
    return result;
}

AuthorizationEvent_Internal to_internal_api(AuthorizationEvent_External const& val) {
    AuthorizationEvent_Internal result;
    result.meter_value = powermeter::to_internal_api(val.meter_value);
    return result;
}

AuthorizationEvent_External to_external_api(AuthorizationEvent_Internal const& val) {
    AuthorizationEvent_External result;
    result.meter_value = powermeter::to_external_api(val.meter_value);
    return result;
}

ConnectorTypeEnum_Internal to_internal_api(ConnectorTypeEnum_External const& val) {
    using SrcT = ConnectorTypeEnum_External;
    using TarT = ConnectorTypeEnum_Internal;

    switch (val) {
    case SrcT::cCCS1:
        return TarT::cCCS1;
    case SrcT::cCCS2:
        return TarT::cCCS2;
    case SrcT::cG105:
        return TarT::cG105;
    case SrcT::cMCS:
        return TarT::cMCS;
    case SrcT::cTesla:
        return TarT::cTesla;
    case SrcT::cType1:
        return TarT::cType1;
    case SrcT::cType2:
        return TarT::cType2;
    case SrcT::s309_1P_16A:
        return TarT::s309_1P_16A;
    case SrcT::s309_1P_32A:
        return TarT::s309_1P_32A;
    case SrcT::s309_3P_16A:
        return TarT::s309_3P_16A;
    case SrcT::s309_3P_32A:
        return TarT::s309_3P_32A;
    case SrcT::sBS1361:
        return TarT::sBS1361;
    case SrcT::sCEE_7_7:
        return TarT::sCEE_7_7;
    case SrcT::sType2:
        return TarT::sType2;
    case SrcT::sType3:
        return TarT::sType3;
    case SrcT::Other1PhMax16A:
        return TarT::Other1PhMax16A;
    case SrcT::Other1PhOver16A:
        return TarT::Other1PhOver16A;
    case SrcT::Other3Ph:
        return TarT::Other3Ph;
    case SrcT::Pan:
        return TarT::Pan;
    case SrcT::wInductive:
        return TarT::wInductive;
    case SrcT::wResonant:
        return TarT::wResonant;
    case SrcT::Undetermined:
        return TarT::Undetermined;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range("Unexpected value for ConnectorTypeEnum_External");
}

ConnectorTypeEnum_External to_external_api(ConnectorTypeEnum_Internal const& val) {
    using SrcT = ConnectorTypeEnum_Internal;
    using TarT = ConnectorTypeEnum_External;

    switch (val) {
    case SrcT::cCCS1:
        return TarT::cCCS1;
    case SrcT::cCCS2:
        return TarT::cCCS2;
    case SrcT::cG105:
        return TarT::cG105;
    case SrcT::cMCS:
        return TarT::cMCS;
    case SrcT::cTesla:
        return TarT::cTesla;
    case SrcT::cType1:
        return TarT::cType1;
    case SrcT::cType2:
        return TarT::cType2;
    case SrcT::s309_1P_16A:
        return TarT::s309_1P_16A;
    case SrcT::s309_1P_32A:
        return TarT::s309_1P_32A;
    case SrcT::s309_3P_16A:
        return TarT::s309_3P_16A;
    case SrcT::s309_3P_32A:
        return TarT::s309_3P_32A;
    case SrcT::sBS1361:
        return TarT::sBS1361;
    case SrcT::sCEE_7_7:
        return TarT::sCEE_7_7;
    case SrcT::sType2:
        return TarT::sType2;
    case SrcT::sType3:
        return TarT::sType3;
    case SrcT::Other1PhMax16A:
        return TarT::Other1PhMax16A;
    case SrcT::Other1PhOver16A:
        return TarT::Other1PhOver16A;
    case SrcT::Other3Ph:
        return TarT::Other3Ph;
    case SrcT::Pan:
        return TarT::Pan;
    case SrcT::wInductive:
        return TarT::wInductive;
    case SrcT::wResonant:
        return TarT::wResonant;
    case SrcT::Undetermined:
        return TarT::Undetermined;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range("Unexpected value for ConnectorTypeEnum_External");
}

Connector_Internal to_internal_api(Connector_External const& val) {
    Connector_Internal result;
    result.id = val.id;
    result.type = optToInternal(val.type);

    return result;
}
Connector_External to_external_api(Connector_Internal const& val) {
    Connector_External result;
    result.id = val.id;
    result.type = optToExternal(val.type);

    return result;
}

Evse_Internal to_internal_api(Evse_External const& val) {
    Evse_Internal result;
    result.id = val.id;
    for (auto const& elem : val.connectors) {
        result.connectors.push_back(to_internal_api(elem));
    }
    return result;
}

Evse_External to_external_api(Evse_Internal const& val) {
    Evse_External result;
    result.id = val.id;
    for (auto const& elem : val.connectors) {
        result.connectors.push_back(to_external_api(elem));
    }
    return result;
}

EnableSourceEnum_Internal to_internal_api(EnableSourceEnum_External const& val) {
    using SrcT = EnableSourceEnum_External;
    using TarT = EnableSourceEnum_Internal;

    switch (val) {
    case SrcT::Unspecified:
        return TarT::Unspecified;
    case SrcT::LocalAPI:
        return TarT::LocalAPI;
    case SrcT::LocalKeyLock:
        return TarT::LocalKeyLock;
    case SrcT::ServiceTechnician:
        return TarT::ServiceTechnician;
    case SrcT::RemoteKeyLock:
        return TarT::RemoteKeyLock;
    case SrcT::MobileApp:
        return TarT::MobileApp;
    case SrcT::FirmwareUpdate:
        return TarT::FirmwareUpdate;
    case SrcT::CSMS:
        return TarT::CSMS;
    }
    throw std::out_of_range("Unexpected value for EnableSourceEnum_External");
}

EnableSourceEnum_External to_external_api(EnableSourceEnum_Internal const& val) {
    using SrcT = EnableSourceEnum_Internal;
    using TarT = EnableSourceEnum_External;

    switch (val) {
    case SrcT::Unspecified:
        return TarT::Unspecified;
    case SrcT::LocalAPI:
        return TarT::LocalAPI;
    case SrcT::LocalKeyLock:
        return TarT::LocalKeyLock;
    case SrcT::ServiceTechnician:
        return TarT::ServiceTechnician;
    case SrcT::RemoteKeyLock:
        return TarT::RemoteKeyLock;
    case SrcT::MobileApp:
        return TarT::MobileApp;
    case SrcT::FirmwareUpdate:
        return TarT::FirmwareUpdate;
    case SrcT::CSMS:
        return TarT::CSMS;
    }
    throw std::out_of_range("Unexpected value for EnableSourceEnum_Internal");
}

EnableStateEnum_Internal to_internal_api(EnableStateEnum_External const& val) {
    using SrcT = EnableStateEnum_External;
    using TarT = EnableStateEnum_Internal;

    switch (val) {
    case SrcT::Unassigned:
        return TarT::Unassigned;
    case SrcT::Disable:
        return TarT::Disable;
    case SrcT::Enable:
        return TarT::Enable;
    }
    throw std::out_of_range("Unexpected value for EnableStateEnum_External");
}

EnableStateEnum_External to_external_api(EnableStateEnum_Internal const& val) {
    using SrcT = EnableStateEnum_Internal;
    using TarT = EnableStateEnum_External;

    switch (val) {
    case SrcT::Unassigned:
        return TarT::Unassigned;
    case SrcT::Disable:
        return TarT::Disable;
    case SrcT::Enable:
        return TarT::Enable;
    }
    throw std::out_of_range("Unexpected value for EnableStateEnum_Internal");
}

EnableDisableSource_Internal to_internal_api(EnableDisableSource_External const& val) {
    EnableDisableSource_Internal result;
    result.enable_source = to_internal_api(val.enable_source);
    result.enable_state = to_internal_api(val.enable_state);
    result.enable_priority = val.enable_priority;

    return result;
}

EnableDisableSource_External to_external_api(EnableDisableSource_Internal const& val) {
    EnableDisableSource_External result;
    result.enable_source = to_external_api(val.enable_source);
    result.enable_state = to_external_api(val.enable_state);
    result.enable_priority = val.enable_priority;

    return result;
}

PlugAndChargeConfiguration_Internal to_internal_api(PlugAndChargeConfiguration_External const& val) {
    PlugAndChargeConfiguration_Internal result;
    result.pnc_enabled = val.pnc_enabled;
    result.central_contract_validation_allowed = val.central_contract_validation_allowed;
    result.contract_certificate_installation_enabled = val.contract_certificate_installation_enabled;

    return result;
}

PlugAndChargeConfiguration_External to_external_api(PlugAndChargeConfiguration_Internal const& val) {
    PlugAndChargeConfiguration_External result;
    result.pnc_enabled = val.pnc_enabled;
    result.central_contract_validation_allowed = val.central_contract_validation_allowed;
    result.contract_certificate_installation_enabled = val.contract_certificate_installation_enabled;

    return result;
}

} // namespace evse_manager
} // namespace everest::lib::API::V1_0::types
