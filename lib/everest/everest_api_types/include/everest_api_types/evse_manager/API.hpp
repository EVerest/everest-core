// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/powermeter/API.hpp>
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::evse_manager {

enum class StopTransactionReason {
    EmergencyStop,
    EVDisconnected,
    HardReset,
    Local,
    Other,
    PowerLoss,
    Reboot,
    Remote,
    SoftReset,
    UnlockCommand,
    DeAuthorized,
    EnergyLimitReached,
    GroundFault,
    LocalOutOfCredit,
    MasterPass,
    OvercurrentFault,
    PowerQuality,
    SOCLimitReached,
    StoppedByEV,
    TimeLimitReached,
    Timeout,
    ReqEnergyTransferRejected
};

struct StopTransactionRequest {
    StopTransactionReason reason;
    std::optional<auth::ProvidedIdToken> id_tag;
};

enum class StartSessionReason {
    EVConnected,
    Authorized,
};

enum class SessionEventEnum {
    Authorized,
    Deauthorized,
    Enabled,
    Disabled,
    SessionStarted,
    AuthRequired,
    TransactionStarted,
    PrepareCharging,
    ChargingStarted,
    ChargingPausedEV,
    ChargingPausedEVSE,
    WaitingForEnergy,
    ChargingResumed,
    StoppingCharging,
    ChargingFinished,
    TransactionFinished,
    SessionFinished,
    ReservationStart,
    ReservationEnd,
    ReplugStarted,
    ReplugFinished,
    PluginTimeout,
    SwitchingPhases,
    SessionResumed,
};

struct SessionStarted {
    StartSessionReason reason;
    std::optional<auth::ProvidedIdToken> id_tag;
    powermeter::PowermeterValues meter_value;
    std::optional<powermeter::SignedMeterValue> signed_meter_value;
    std::optional<int32_t> reservation_id;
    std::optional<std::string> logging_path;
};

struct SessionFinished {
    powermeter::PowermeterValues meter_value;
};

struct TransactionStarted {
    auth::ProvidedIdToken id_tag;
    powermeter::PowermeterValues meter_value;
    std::optional<powermeter::SignedMeterValue> signed_meter_value;
    std::optional<int32_t> reservation_id;
};

struct TransactionFinished {
    types::powermeter::PowermeterValues meter_value;
    std::optional<powermeter::SignedMeterValue> start_signed_meter_value;
    std::optional<powermeter::SignedMeterValue> signed_meter_value;
    std::optional<StopTransactionReason> reason;
    std::optional<auth::ProvidedIdToken> id_tag;
};

struct ChargingStateChangedEvent {
    powermeter::PowermeterValues meter_value;
};

struct AuthorizationEvent {
    powermeter::PowermeterValues meter_value;
};

enum class ErrorSeverity {
    High,
    Medium,
    Low,
};

enum class ErrorState {
    Active,
    ClearedByModule,
    ClearedByReboot
};

struct ErrorOrigin {
    std::string module_id;
    std::string implementation_id;
};

struct Error {
    std::string type;
    std::string sub_type;

    std::string description;
    std::string message;
    ErrorSeverity severity;
    ErrorOrigin origin;
    std::string timestamp;
    std::string uuid;
    ErrorState state;
};

struct Limits {
    float max_current;
    int32_t nr_of_phases_available;
    std::optional<std::string> uuid;
};

struct EVInfo {
    std::optional<float> soc;
    std::optional<float> present_voltage;
    std::optional<float> present_current;
    std::optional<float> target_voltage;
    std::optional<float> target_current;
    std::optional<float> maximum_current_limit;
    std::optional<float> minimum_current_limit;
    std::optional<float> maximum_voltage_limit;
    std::optional<float> maximum_power_limit;
    std::optional<std::string> estimated_time_full;
    std::optional<std::string> departure_time;
    std::optional<std::string> estimated_time_bulk;
    std::optional<std::string> evcc_id;
    std::optional<float> remaining_energy_needed;
    std::optional<float> battery_capacity;
    std::optional<float> battery_full_soc;
    std::optional<float> battery_bulk_soc;
};

enum class CarManufacturer {
    VolkswagenGroup,
    Tesla,
    Unknown,
};

enum class ConnectorTypeEnum {
    cCCS1,
    cCCS2,
    cG105,
    cMCS,
    cTesla,
    cType1,
    cType2,
    s309_1P_16A,
    s309_1P_32A,
    s309_3P_16A,
    s309_3P_32A,
    sBS1361,
    sCEE_7_7,
    sType2,
    sType3,
    Other1PhMax16A,
    Other1PhOver16A,
    Other3Ph,
    Pan,
    wInductive,
    wResonant,
    Undetermined,
    Unknown,
};

struct Connector {
    int32_t id;
    std::optional<ConnectorTypeEnum> type;
};

struct Evse {
    int32_t id;
    std::vector<Connector> connectors;
};

enum class EnableSourceEnum {
    Unspecified,
    LocalAPI,
    LocalKeyLock,
    ServiceTechnician,
    RemoteKeyLock,
    MobileApp,
    FirmwareUpdate,
    CSMS,
};

enum class EnableStateEnum {
    Unassigned,
    Disable,
    Enable,
};

struct EnableDisableSource {
    EnableSourceEnum enable_source;
    EnableStateEnum enable_state;
    int32_t enable_priority;
};

struct SessionEvent {
    std::string uuid;
    std::string timestamp;
    types::evse_manager::SessionEventEnum event;
    std::optional<int32_t> connector_id;
    std::optional<SessionStarted> session_started;
    std::optional<SessionFinished> session_finished;
    std::optional<TransactionStarted> transaction_started;
    std::optional<TransactionFinished> transaction_finished;
    std::optional<ChargingStateChangedEvent> charging_state_changed_event;
    std::optional<AuthorizationEvent> authorization_event;
    std::optional<EnableDisableSource> source;
};

struct EnableDisableRequest {
    int32_t connector_id;
    EnableDisableSource source;
};

struct AuthorizeResponseArgs {
    auth::ProvidedIdToken token;
    auth::ValidationResult result;
};

struct PlugAndChargeConfiguration {
    std::optional<bool> pnc_enabled;
    std::optional<bool> central_contract_validation_allowed;
    std::optional<bool> contract_certificate_installation_enabled;
};

enum class EvseStateEnum {
    Unknown,
    Unplugged,
    Disabled,
    Preparing,
    AuthRequired,
    WaitingForEnergy,
    ChargingPausedEV,
    ChargingPausedEVSE,
    Charging,
    AuthTimeout,
    Finished,
    FinishedEVSE,
    FinishedEV
};

struct SessionInfo {
    EvseStateEnum state;
    int32_t charged_energy_wh;
    int32_t discharged_energy_wh;
    int32_t latest_total_w;
    int64_t session_duration_s;
    std::string timestamp;
    std::optional<std::string> selected_protocol;
    std::optional<int64_t> transaction_duration_s;
    std::optional<std::string> session_start_time;
    std::optional<std::string> transaction_start_time;
    std::optional<std::string> session_end_time;
    std::optional<std::string> transaction_end_time;
};

} // namespace everest::lib::API::V1_0::types::evse_manager
