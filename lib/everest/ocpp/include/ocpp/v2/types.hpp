// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef V2_TYPES_HPP
#define V2_TYPES_HPP

#include <ocpp/v2/ocpp_types.hpp>

#include <ostream>
#include <string>

namespace ocpp {
namespace v2 {

/// \brief Contains all supported OCPP 2.0.1 message types
enum class MessageType {
    Authorize,
    AuthorizeResponse,
    BootNotification,
    BootNotificationResponse,
    CancelReservation,
    CancelReservationResponse,
    CertificateSigned,
    CertificateSignedResponse,
    ChangeAvailability,
    ChangeAvailabilityResponse,
    ClearCache,
    ClearCacheResponse,
    ClearChargingProfile,
    ClearChargingProfileResponse,
    ClearDisplayMessage,
    ClearDisplayMessageResponse,
    ClearedChargingLimit,
    ClearedChargingLimitResponse,
    ClearVariableMonitoring,
    ClearVariableMonitoringResponse,
    CostUpdated,
    CostUpdatedResponse,
    CustomerInformation,
    CustomerInformationResponse,
    DataTransfer,
    DataTransferResponse,
    DeleteCertificate,
    DeleteCertificateResponse,
    FirmwareStatusNotification,
    FirmwareStatusNotificationResponse,
    Get15118EVCertificate,
    Get15118EVCertificateResponse,
    GetBaseReport,
    GetBaseReportResponse,
    GetCertificateStatus,
    GetCertificateStatusResponse,
    GetChargingProfiles,
    GetChargingProfilesResponse,
    GetCompositeSchedule,
    GetCompositeScheduleResponse,
    GetDisplayMessages,
    GetDisplayMessagesResponse,
    GetInstalledCertificateIds,
    GetInstalledCertificateIdsResponse,
    GetLocalListVersion,
    GetLocalListVersionResponse,
    GetLog,
    GetLogResponse,
    GetMonitoringReport,
    GetMonitoringReportResponse,
    GetReport,
    GetReportResponse,
    GetTransactionStatus,
    GetTransactionStatusResponse,
    GetVariables,
    GetVariablesResponse,
    Heartbeat,
    HeartbeatResponse,
    InstallCertificate,
    InstallCertificateResponse,
    LogStatusNotification,
    LogStatusNotificationResponse,
    MeterValues,
    MeterValuesResponse,
    NotifyChargingLimit,
    NotifyChargingLimitResponse,
    NotifyCustomerInformation,
    NotifyCustomerInformationResponse,
    NotifyDisplayMessages,
    NotifyDisplayMessagesResponse,
    NotifyEVChargingNeeds,
    NotifyEVChargingNeedsResponse,
    NotifyEVChargingSchedule,
    NotifyEVChargingScheduleResponse,
    NotifyEvent,
    NotifyEventResponse,
    NotifyMonitoringReport,
    NotifyMonitoringReportResponse,
    NotifyReport,
    NotifyReportResponse,
    PublishFirmware,
    PublishFirmwareResponse,
    PublishFirmwareStatusNotification,
    PublishFirmwareStatusNotificationResponse,
    ReportChargingProfiles,
    ReportChargingProfilesResponse,
    RequestStartTransaction,
    RequestStartTransactionResponse,
    RequestStopTransaction,
    RequestStopTransactionResponse,
    ReservationStatusUpdate,
    ReservationStatusUpdateResponse,
    ReserveNow,
    ReserveNowResponse,
    Reset,
    ResetResponse,
    SecurityEventNotification,
    SecurityEventNotificationResponse,
    SendLocalList,
    SendLocalListResponse,
    SetChargingProfile,
    SetChargingProfileResponse,
    SetDisplayMessage,
    SetDisplayMessageResponse,
    SetMonitoringBase,
    SetMonitoringBaseResponse,
    SetMonitoringLevel,
    SetMonitoringLevelResponse,
    SetNetworkProfile,
    SetNetworkProfileResponse,
    SetVariableMonitoring,
    SetVariableMonitoringResponse,
    SetVariables,
    SetVariablesResponse,
    SignCertificate,
    SignCertificateResponse,
    StatusNotification,
    StatusNotificationResponse,
    TransactionEvent,
    TransactionEventResponse,
    TriggerMessage,
    TriggerMessageResponse,
    UnlockConnector,
    UnlockConnectorResponse,
    UnpublishFirmware,
    UnpublishFirmwareResponse,
    UpdateFirmware,
    UpdateFirmwareResponse,
    AdjustPeriodicEventStream,
    AdjustPeriodicEventStreamResponse,
    AFRRSignal,
    AFRRSignalResponse,
    BatterySwap,
    BatterySwapResponse,
    ChangeTransactionTariff,
    ChangeTransactionTariffResponse,
    ClearDERControl,
    ClearDERControlResponse,
    ClearTariffs,
    ClearTariffsResponse,
    ClosePeriodicEventStream,
    ClosePeriodicEventStreamResponse,
    GetCRL,
    GetCRLResponse,
    GetDERControl,
    GetDERControlResponse,
    GetPeriodicEventStream,
    GetPeriodicEventStreamResponse,
    GetTariffs,
    GetTariffsResponse,
    NotifyAllowedEnergyTransfer,
    NotifyAllowedEnergyTransferResponse,
    NotifyDERAlarm,
    NotifyDERAlarmResponse,
    NotifyDERStartStop,
    NotifyDERStartStopResponse,
    NotifyPeriodicEventStream,
    NotifyPeriodicEventStreamResponse,
    NotifyPriorityCharging,
    NotifyPriorityChargingResponse,
    NotifySettlement,
    NotifySettlementResponse,
    OpenPeriodicEventStream,
    OpenPeriodicEventStreamResponse,
    PullDynamicScheduleUpdate,
    PullDynamicScheduleUpdateResponse,
    RequestBatterySwap,
    RequestBatterySwapResponse,
    SetDefaultTariff,
    SetDefaultTariffResponse,
    SetDERControl,
    SetDERControlResponse,
    UpdateDynamicSchedule,
    UpdateDynamicScheduleResponse,
    UsePriorityCharging,
    UsePriorityChargingResponse,
    VatNumberValidation,
    VatNumberValidationResponse,
    InternalError, // not in spec, for internal use
};

/// \brief This enhances the ChargingProfile type by additional paramaters that are required in the
/// ReportChargingProfilesRequest (EvseId, ChargingLimitSource)
struct ReportedChargingProfile {
    ChargingProfile profile;
    int32_t evse_id;
    CiString<20> source;

    ReportedChargingProfile(const ChargingProfile& profile, const int32_t evse_id, const CiString<20> source) :
        profile(profile), evse_id(evse_id), source(source) {
    }
};

namespace conversions {
/// \brief Converts the given MessageType \p m to std::string
/// \returns a string representation of the MessageType
std::string messagetype_to_string(MessageType m);

/// \brief Converts the given std::string \p s to MessageType
/// \returns a MessageType from a string representation
MessageType string_to_messagetype(const std::string& s);

} // namespace conversions

/// \brief Writes the string representation of the given \p message_type to the given output stream \p os
/// \returns an output stream with the MessageType written to
std::ostream& operator<<(std::ostream& os, const MessageType& message_type);

namespace ChargingLimitSourceEnumStringType {
inline const CiString<20> EMS = "EMS";
inline const CiString<20> OTHER = "Other";
inline const CiString<20> SO = "SO";
inline const CiString<20> CSO = "CSO";
} // namespace ChargingLimitSourceEnumStringType

namespace IdTokenEnumStringType {
inline const CiString<20> Value = "Value";
inline const CiString<20> Central = "Central";
inline const CiString<20> DirectPayment = "DirectPayment";
inline const CiString<20> eMAID = "eMAID";
inline const CiString<20> EVCCID = "EVCCID";
inline const CiString<20> ISO14443 = "ISO14443";
inline const CiString<20> ISO15693 = "ISO15693";
inline const CiString<20> KeyCode = "KeyCode";
inline const CiString<20> Local = "Local";
inline const CiString<20> MacAddress = "MacAddress";
inline const CiString<20> NEMA = "NEMA";
inline const CiString<20> NoAuthorization = "NoAuthorization";
inline const CiString<20> VIN = "VIN";
} // namespace IdTokenEnumStringType

namespace ConnectorEnumStringType {
inline const CiString<20> cCCS1 = "cCCS1";     // Combined Charging System 1 (captive cabled) a.k.a. Combo 1
inline const CiString<20> cCCS2 = "cCCS2";     //  Combined Charging System 2 (captive cabled) a.k.a. Combo 2
inline const CiString<20> cChaoJi = "cChaoJi"; //  ChaoJi (captive cabled) a.k.a. CHAdeMO 3.0
inline const CiString<20> cG105 = "cG105";     //  JARI G105-1993 (captive cabled) a.k.a. CHAdeMO (captive cabled)
inline const CiString<20> cGBT_DC = "cGBT-DC"; //  GB/T 20234.3 DC connector (captive cabled)
inline const CiString<20> cLECCS = "cLECCS";   //  Light Equipment Combined Charging System IS17017 (captive cabled)
inline const CiString<20> cMCS = "cMCS";       //  Megawatt Charging System (captive cabled)
inline const CiString<20> cNACS = "cNACS";     //  North American Charging Standard (captive cabled)
inline const CiString<20> cNACS_CCS1 = "cNACS-CCS1"; //  Tesla MagicDock with built-in NACS to CCS1 adapter
inline const CiString<20> cTesla = "cTesla";         //  Tesla Connector (captive cabled)
inline const CiString<20> cType1 = "cType1";         //  IEC62196-2 Type 1 connector (captive cabled) a.k.a. J1772
inline const CiString<20> cType2 = "cType2"; //  IEC62196-2 Type 2 connector (captive cabled) a.k.a. Mennekes connector
inline const CiString<20> cUltraChaoJi = "cUltraChaoJi"; //  Ultra-ChaoJi for megawatt charging
inline const CiString<20> s309_1P_16A = "s309-1P-16A ";  //  16A 1 phase IEC60309 socket
inline const CiString<20> s309_1P_32A = "s309-1P-32A ";  //  32A 1 phase IEC60309 socket
inline const CiString<20> s309_3P_16A = "s309-3P-16A";   //  16A 3 phase IEC60309 socket
inline const CiString<20> s309_3P_32A = "s309-3P-32A";   //  32A 3 phase IEC60309 socket
inline const CiString<20> sBS1361 = "sBS1361";           //  UK domestic socket a.k.a. 13Amp
inline const CiString<20> sCEE_7_7 = "sCEE-7-7";         //  CEE 7/7 16A socket. May represent 7/4 and 7/5 a.k.a Schuko
inline const CiString<20> sType2 = "sType2";             //  IEC62196-2 Type 2 socket a.k.a. Mennekes connector
inline const CiString<20> sType3 = "sType3";             //  IEC62196-2 Type 3 socket a.k.a. Scame
inline const CiString<20> wInductive = "wInductive";     //  Wireless inductively coupled connection (generic)
inline const CiString<20> wResonant = "wResonant";       //  Wireless resonant coupled connection (generic)
inline const CiString<20> Other1PhMax16A =
    "Other1PhMax16A"; //  Other single phase (domestic) sockets not mentioned above, rated at no more than 16A. CEE7/17,
                      //  AS3112, NEMA 5-15, NEMA 5-20, JISC8303, TIS166, SI 32, CPCS-CCC, SEV1011, etc.
inline const CiString<20> Other1PhOver16A =
    "Other1PhOver16A";                           //  Other single phase sockets not mentioned above (over 16A)
inline const CiString<20> Other3Ph = "Other3Ph"; //  Other 3 phase sockets not mentioned above. NEMA14-30, NEMA14-50.
inline const CiString<20> Pan = "Pan";           //  Pantograph connector
inline const CiString<20> Undetermined = "Undetermined"; //  Yet to be determined (e.g. before plugged in)
inline const CiString<20> Unknown = "Unknown";           // Unknown/not determinable
} // namespace ConnectorEnumStringType
} // namespace v2
} // namespace ocpp

#endif
