// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v2/types.hpp>

namespace ocpp {
namespace v2 {

namespace conversions {

std::string messagetype_to_string(MessageType m) {
    switch (m) {
    case MessageType::Authorize:
        return "Authorize";
    case MessageType::AuthorizeResponse:
        return "AuthorizeResponse";
    case MessageType::BootNotification:
        return "BootNotification";
    case MessageType::BootNotificationResponse:
        return "BootNotificationResponse";
    case MessageType::CancelReservation:
        return "CancelReservation";
    case MessageType::CancelReservationResponse:
        return "CancelReservationResponse";
    case MessageType::CertificateSigned:
        return "CertificateSigned";
    case MessageType::CertificateSignedResponse:
        return "CertificateSignedResponse";
    case MessageType::ChangeAvailability:
        return "ChangeAvailability";
    case MessageType::ChangeAvailabilityResponse:
        return "ChangeAvailabilityResponse";
    case MessageType::ClearCache:
        return "ClearCache";
    case MessageType::ClearCacheResponse:
        return "ClearCacheResponse";
    case MessageType::ClearChargingProfile:
        return "ClearChargingProfile";
    case MessageType::ClearChargingProfileResponse:
        return "ClearChargingProfileResponse";
    case MessageType::ClearDisplayMessage:
        return "ClearDisplayMessage";
    case MessageType::ClearDisplayMessageResponse:
        return "ClearDisplayMessageResponse";
    case MessageType::ClearedChargingLimit:
        return "ClearedChargingLimit";
    case MessageType::ClearedChargingLimitResponse:
        return "ClearedChargingLimitResponse";
    case MessageType::ClearVariableMonitoring:
        return "ClearVariableMonitoring";
    case MessageType::ClearVariableMonitoringResponse:
        return "ClearVariableMonitoringResponse";
    case MessageType::CostUpdated:
        return "CostUpdated";
    case MessageType::CostUpdatedResponse:
        return "CostUpdatedResponse";
    case MessageType::CustomerInformation:
        return "CustomerInformation";
    case MessageType::CustomerInformationResponse:
        return "CustomerInformationResponse";
    case MessageType::DataTransfer:
        return "DataTransfer";
    case MessageType::DataTransferResponse:
        return "DataTransferResponse";
    case MessageType::DeleteCertificate:
        return "DeleteCertificate";
    case MessageType::DeleteCertificateResponse:
        return "DeleteCertificateResponse";
    case MessageType::FirmwareStatusNotification:
        return "FirmwareStatusNotification";
    case MessageType::FirmwareStatusNotificationResponse:
        return "FirmwareStatusNotificationResponse";
    case MessageType::Get15118EVCertificate:
        return "Get15118EVCertificate";
    case MessageType::Get15118EVCertificateResponse:
        return "Get15118EVCertificateResponse";
    case MessageType::GetBaseReport:
        return "GetBaseReport";
    case MessageType::GetBaseReportResponse:
        return "GetBaseReportResponse";
    case MessageType::GetCertificateStatus:
        return "GetCertificateStatus";
    case MessageType::GetCertificateStatusResponse:
        return "GetCertificateStatusResponse";
    case MessageType::GetChargingProfiles:
        return "GetChargingProfiles";
    case MessageType::GetChargingProfilesResponse:
        return "GetChargingProfilesResponse";
    case MessageType::GetCompositeSchedule:
        return "GetCompositeSchedule";
    case MessageType::GetCompositeScheduleResponse:
        return "GetCompositeScheduleResponse";
    case MessageType::GetDisplayMessages:
        return "GetDisplayMessages";
    case MessageType::GetDisplayMessagesResponse:
        return "GetDisplayMessagesResponse";
    case MessageType::GetInstalledCertificateIds:
        return "GetInstalledCertificateIds";
    case MessageType::GetInstalledCertificateIdsResponse:
        return "GetInstalledCertificateIdsResponse";
    case MessageType::GetLocalListVersion:
        return "GetLocalListVersion";
    case MessageType::GetLocalListVersionResponse:
        return "GetLocalListVersionResponse";
    case MessageType::GetLog:
        return "GetLog";
    case MessageType::GetLogResponse:
        return "GetLogResponse";
    case MessageType::GetMonitoringReport:
        return "GetMonitoringReport";
    case MessageType::GetMonitoringReportResponse:
        return "GetMonitoringReportResponse";
    case MessageType::GetReport:
        return "GetReport";
    case MessageType::GetReportResponse:
        return "GetReportResponse";
    case MessageType::GetTransactionStatus:
        return "GetTransactionStatus";
    case MessageType::GetTransactionStatusResponse:
        return "GetTransactionStatusResponse";
    case MessageType::GetVariables:
        return "GetVariables";
    case MessageType::GetVariablesResponse:
        return "GetVariablesResponse";
    case MessageType::Heartbeat:
        return "Heartbeat";
    case MessageType::HeartbeatResponse:
        return "HeartbeatResponse";
    case MessageType::InstallCertificate:
        return "InstallCertificate";
    case MessageType::InstallCertificateResponse:
        return "InstallCertificateResponse";
    case MessageType::LogStatusNotification:
        return "LogStatusNotification";
    case MessageType::LogStatusNotificationResponse:
        return "LogStatusNotificationResponse";
    case MessageType::MeterValues:
        return "MeterValues";
    case MessageType::MeterValuesResponse:
        return "MeterValuesResponse";
    case MessageType::NotifyChargingLimit:
        return "NotifyChargingLimit";
    case MessageType::NotifyChargingLimitResponse:
        return "NotifyChargingLimitResponse";
    case MessageType::NotifyCustomerInformation:
        return "NotifyCustomerInformation";
    case MessageType::NotifyCustomerInformationResponse:
        return "NotifyCustomerInformationResponse";
    case MessageType::NotifyDisplayMessages:
        return "NotifyDisplayMessages";
    case MessageType::NotifyDisplayMessagesResponse:
        return "NotifyDisplayMessagesResponse";
    case MessageType::NotifyEVChargingNeeds:
        return "NotifyEVChargingNeeds";
    case MessageType::NotifyEVChargingNeedsResponse:
        return "NotifyEVChargingNeedsResponse";
    case MessageType::NotifyEVChargingSchedule:
        return "NotifyEVChargingSchedule";
    case MessageType::NotifyEVChargingScheduleResponse:
        return "NotifyEVChargingScheduleResponse";
    case MessageType::NotifyEvent:
        return "NotifyEvent";
    case MessageType::NotifyEventResponse:
        return "NotifyEventResponse";
    case MessageType::NotifyMonitoringReport:
        return "NotifyMonitoringReport";
    case MessageType::NotifyMonitoringReportResponse:
        return "NotifyMonitoringReportResponse";
    case MessageType::NotifyReport:
        return "NotifyReport";
    case MessageType::NotifyReportResponse:
        return "NotifyReportResponse";
    case MessageType::PublishFirmware:
        return "PublishFirmware";
    case MessageType::PublishFirmwareResponse:
        return "PublishFirmwareResponse";
    case MessageType::PublishFirmwareStatusNotification:
        return "PublishFirmwareStatusNotification";
    case MessageType::PublishFirmwareStatusNotificationResponse:
        return "PublishFirmwareStatusNotificationResponse";
    case MessageType::ReportChargingProfiles:
        return "ReportChargingProfiles";
    case MessageType::ReportChargingProfilesResponse:
        return "ReportChargingProfilesResponse";
    case MessageType::RequestStartTransaction:
        return "RequestStartTransaction";
    case MessageType::RequestStartTransactionResponse:
        return "RequestStartTransactionResponse";
    case MessageType::RequestStopTransaction:
        return "RequestStopTransaction";
    case MessageType::RequestStopTransactionResponse:
        return "RequestStopTransactionResponse";
    case MessageType::ReservationStatusUpdate:
        return "ReservationStatusUpdate";
    case MessageType::ReservationStatusUpdateResponse:
        return "ReservationStatusUpdateResponse";
    case MessageType::ReserveNow:
        return "ReserveNow";
    case MessageType::ReserveNowResponse:
        return "ReserveNowResponse";
    case MessageType::Reset:
        return "Reset";
    case MessageType::ResetResponse:
        return "ResetResponse";
    case MessageType::SecurityEventNotification:
        return "SecurityEventNotification";
    case MessageType::SecurityEventNotificationResponse:
        return "SecurityEventNotificationResponse";
    case MessageType::SendLocalList:
        return "SendLocalList";
    case MessageType::SendLocalListResponse:
        return "SendLocalListResponse";
    case MessageType::SetChargingProfile:
        return "SetChargingProfile";
    case MessageType::SetChargingProfileResponse:
        return "SetChargingProfileResponse";
    case MessageType::SetDisplayMessage:
        return "SetDisplayMessage";
    case MessageType::SetDisplayMessageResponse:
        return "SetDisplayMessageResponse";
    case MessageType::SetMonitoringBase:
        return "SetMonitoringBase";
    case MessageType::SetMonitoringBaseResponse:
        return "SetMonitoringBaseResponse";
    case MessageType::SetMonitoringLevel:
        return "SetMonitoringLevel";
    case MessageType::SetMonitoringLevelResponse:
        return "SetMonitoringLevelResponse";
    case MessageType::SetNetworkProfile:
        return "SetNetworkProfile";
    case MessageType::SetNetworkProfileResponse:
        return "SetNetworkProfileResponse";
    case MessageType::SetVariableMonitoring:
        return "SetVariableMonitoring";
    case MessageType::SetVariableMonitoringResponse:
        return "SetVariableMonitoringResponse";
    case MessageType::SetVariables:
        return "SetVariables";
    case MessageType::SetVariablesResponse:
        return "SetVariablesResponse";
    case MessageType::SignCertificate:
        return "SignCertificate";
    case MessageType::SignCertificateResponse:
        return "SignCertificateResponse";
    case MessageType::StatusNotification:
        return "StatusNotification";
    case MessageType::StatusNotificationResponse:
        return "StatusNotificationResponse";
    case MessageType::TransactionEvent:
        return "TransactionEvent";
    case MessageType::TransactionEventResponse:
        return "TransactionEventResponse";
    case MessageType::TriggerMessage:
        return "TriggerMessage";
    case MessageType::TriggerMessageResponse:
        return "TriggerMessageResponse";
    case MessageType::UnlockConnector:
        return "UnlockConnector";
    case MessageType::UnlockConnectorResponse:
        return "UnlockConnectorResponse";
    case MessageType::UnpublishFirmware:
        return "UnpublishFirmware";
    case MessageType::UnpublishFirmwareResponse:
        return "UnpublishFirmwareResponse";
    case MessageType::UpdateFirmware:
        return "UpdateFirmware";
    case MessageType::UpdateFirmwareResponse:
        return "UpdateFirmwareResponse";
    case MessageType::AdjustPeriodicEventStream:
        return "AdjustPeriodicEventStream";
    case MessageType::AdjustPeriodicEventStreamResponse:
        return "AdjustPeriodicEventStreamResponse";
    case MessageType::AFRRSignal:
        return "AFRRSignal";
    case MessageType::AFRRSignalResponse:
        return "AFRRSignalResponse";
    case MessageType::BatterySwap:
        return "BatterySwap";
    case MessageType::BatterySwapResponse:
        return "BatterySwapResponse";
    case MessageType::ChangeTransactionTariff:
        return "ChangeTransactionTariff";
    case MessageType::ChangeTransactionTariffResponse:
        return "ChangeTransactionTariffResponse";
    case MessageType::ClearDERControl:
        return "ClearDERControl";
    case MessageType::ClearDERControlResponse:
        return "ClearDERControlResponse";
    case MessageType::ClearTariffs:
        return "ClearTariffs";
    case MessageType::ClearTariffsResponse:
        return "ClearTariffsResponse";
    case MessageType::ClosePeriodicEventStream:
        return "ClosePeriodicEventStream";
    case MessageType::ClosePeriodicEventStreamResponse:
        return "ClosePeriodicEventStreamResponse";
    case MessageType::GetCRL:
        return "GetCRL";
    case MessageType::GetCRLResponse:
        return "GetCRLResponse";
    case MessageType::GetDERControl:
        return "GetDERControl";
    case MessageType::GetDERControlResponse:
        return "GetDERControlResponse";
    case MessageType::GetPeriodicEventStream:
        return "GetPeriodicEventStream";
    case MessageType::GetPeriodicEventStreamResponse:
        return "GetPeriodicEventStreamResponse";
    case MessageType::GetTariffs:
        return "GetTariffs";
    case MessageType::GetTariffsResponse:
        return "GetTariffsResponse";
    case MessageType::NotifyAllowedEnergyTransfer:
        return "NotifyAllowedEnergyTransfer";
    case MessageType::NotifyAllowedEnergyTransferResponse:
        return "NotifyAllowedEnergyTransferResponse";
    case MessageType::NotifyDERAlarm:
        return "NotifyDERAlarm";
    case MessageType::NotifyDERAlarmResponse:
        return "NotifyDERAlarmResponse";
    case MessageType::NotifyDERStartStop:
        return "NotifyDERStartStop";
    case MessageType::NotifyDERStartStopResponse:
        return "NotifyDERStartStopResponse";
    case MessageType::NotifyPeriodicEventStream:
        return "NotifyPeriodicEventStream";
    case MessageType::NotifyPeriodicEventStreamResponse:
        return "NotifyPeriodicEventStreamResponse";
    case MessageType::NotifyPriorityCharging:
        return "NotifyPriorityCharging";
    case MessageType::NotifyPriorityChargingResponse:
        return "NotifyPriorityChargingResponse";
    case MessageType::NotifySettlement:
        return "NotifySettlement";
    case MessageType::NotifySettlementResponse:
        return "NotifySettlementResponse";
    case MessageType::OpenPeriodicEventStream:
        return "OpenPeriodicEventStream";
    case MessageType::OpenPeriodicEventStreamResponse:
        return "OpenPeriodicEventStreamResponse";
    case MessageType::PullDynamicScheduleUpdate:
        return "PullDynamicScheduleUpdate";
    case MessageType::PullDynamicScheduleUpdateResponse:
        return "PullDynamicScheduleUpdateResponse";
    case MessageType::RequestBatterySwap:
        return "RequestBatterySwap";
    case MessageType::RequestBatterySwapResponse:
        return "RequestBatterySwapResponse";
    case MessageType::SetDefaultTariff:
        return "SetDefaultTariff";
    case MessageType::SetDefaultTariffResponse:
        return "SetDefaultTariffResponse";
    case MessageType::SetDERControl:
        return "SetDERControl";
    case MessageType::SetDERControlResponse:
        return "SetDERControlResponse";
    case MessageType::UpdateDynamicSchedule:
        return "UpdateDynamicSchedule";
    case MessageType::UpdateDynamicScheduleResponse:
        return "UpdateDynamicScheduleResponse";
    case MessageType::UsePriorityCharging:
        return "UsePriorityCharging";
    case MessageType::UsePriorityChargingResponse:
        return "UsePriorityChargingResponse";
    case MessageType::VatNumberValidation:
        return "VatNumberValidation";
    case MessageType::VatNumberValidationResponse:
        return "VatNumberValidationResponse";
    case MessageType::InternalError:
        return "InternalError";
    }
    throw EnumToStringException{m, "MessageType"};
}

MessageType string_to_messagetype(const std::string& s) {
    if (s == "Authorize") {
        return MessageType::Authorize;
    }
    if (s == "AuthorizeResponse") {
        return MessageType::AuthorizeResponse;
    }
    if (s == "BootNotification") {
        return MessageType::BootNotification;
    }
    if (s == "BootNotificationResponse") {
        return MessageType::BootNotificationResponse;
    }
    if (s == "CancelReservation") {
        return MessageType::CancelReservation;
    }
    if (s == "CancelReservationResponse") {
        return MessageType::CancelReservationResponse;
    }
    if (s == "CertificateSigned") {
        return MessageType::CertificateSigned;
    }
    if (s == "CertificateSignedResponse") {
        return MessageType::CertificateSignedResponse;
    }
    if (s == "ChangeAvailability") {
        return MessageType::ChangeAvailability;
    }
    if (s == "ChangeAvailabilityResponse") {
        return MessageType::ChangeAvailabilityResponse;
    }
    if (s == "ClearCache") {
        return MessageType::ClearCache;
    }
    if (s == "ClearCacheResponse") {
        return MessageType::ClearCacheResponse;
    }
    if (s == "ClearChargingProfile") {
        return MessageType::ClearChargingProfile;
    }
    if (s == "ClearChargingProfileResponse") {
        return MessageType::ClearChargingProfileResponse;
    }
    if (s == "ClearDisplayMessage") {
        return MessageType::ClearDisplayMessage;
    }
    if (s == "ClearDisplayMessageResponse") {
        return MessageType::ClearDisplayMessageResponse;
    }
    if (s == "ClearedChargingLimit") {
        return MessageType::ClearedChargingLimit;
    }
    if (s == "ClearedChargingLimitResponse") {
        return MessageType::ClearedChargingLimitResponse;
    }
    if (s == "ClearVariableMonitoring") {
        return MessageType::ClearVariableMonitoring;
    }
    if (s == "ClearVariableMonitoringResponse") {
        return MessageType::ClearVariableMonitoringResponse;
    }
    if (s == "CostUpdated") {
        return MessageType::CostUpdated;
    }
    if (s == "CostUpdatedResponse") {
        return MessageType::CostUpdatedResponse;
    }
    if (s == "CustomerInformation") {
        return MessageType::CustomerInformation;
    }
    if (s == "CustomerInformationResponse") {
        return MessageType::CustomerInformationResponse;
    }
    if (s == "DataTransfer") {
        return MessageType::DataTransfer;
    }
    if (s == "DataTransferResponse") {
        return MessageType::DataTransferResponse;
    }
    if (s == "DeleteCertificate") {
        return MessageType::DeleteCertificate;
    }
    if (s == "DeleteCertificateResponse") {
        return MessageType::DeleteCertificateResponse;
    }
    if (s == "FirmwareStatusNotification") {
        return MessageType::FirmwareStatusNotification;
    }
    if (s == "FirmwareStatusNotificationResponse") {
        return MessageType::FirmwareStatusNotificationResponse;
    }
    if (s == "Get15118EVCertificate") {
        return MessageType::Get15118EVCertificate;
    }
    if (s == "Get15118EVCertificateResponse") {
        return MessageType::Get15118EVCertificateResponse;
    }
    if (s == "GetBaseReport") {
        return MessageType::GetBaseReport;
    }
    if (s == "GetBaseReportResponse") {
        return MessageType::GetBaseReportResponse;
    }
    if (s == "GetCertificateStatus") {
        return MessageType::GetCertificateStatus;
    }
    if (s == "GetCertificateStatusResponse") {
        return MessageType::GetCertificateStatusResponse;
    }
    if (s == "GetChargingProfiles") {
        return MessageType::GetChargingProfiles;
    }
    if (s == "GetChargingProfilesResponse") {
        return MessageType::GetChargingProfilesResponse;
    }
    if (s == "GetCompositeSchedule") {
        return MessageType::GetCompositeSchedule;
    }
    if (s == "GetCompositeScheduleResponse") {
        return MessageType::GetCompositeScheduleResponse;
    }
    if (s == "GetDisplayMessages") {
        return MessageType::GetDisplayMessages;
    }
    if (s == "GetDisplayMessagesResponse") {
        return MessageType::GetDisplayMessagesResponse;
    }
    if (s == "GetInstalledCertificateIds") {
        return MessageType::GetInstalledCertificateIds;
    }
    if (s == "GetInstalledCertificateIdsResponse") {
        return MessageType::GetInstalledCertificateIdsResponse;
    }
    if (s == "GetLocalListVersion") {
        return MessageType::GetLocalListVersion;
    }
    if (s == "GetLocalListVersionResponse") {
        return MessageType::GetLocalListVersionResponse;
    }
    if (s == "GetLog") {
        return MessageType::GetLog;
    }
    if (s == "GetLogResponse") {
        return MessageType::GetLogResponse;
    }
    if (s == "GetMonitoringReport") {
        return MessageType::GetMonitoringReport;
    }
    if (s == "GetMonitoringReportResponse") {
        return MessageType::GetMonitoringReportResponse;
    }
    if (s == "GetReport") {
        return MessageType::GetReport;
    }
    if (s == "GetReportResponse") {
        return MessageType::GetReportResponse;
    }
    if (s == "GetTransactionStatus") {
        return MessageType::GetTransactionStatus;
    }
    if (s == "GetTransactionStatusResponse") {
        return MessageType::GetTransactionStatusResponse;
    }
    if (s == "GetVariables") {
        return MessageType::GetVariables;
    }
    if (s == "GetVariablesResponse") {
        return MessageType::GetVariablesResponse;
    }
    if (s == "Heartbeat") {
        return MessageType::Heartbeat;
    }
    if (s == "HeartbeatResponse") {
        return MessageType::HeartbeatResponse;
    }
    if (s == "InstallCertificate") {
        return MessageType::InstallCertificate;
    }
    if (s == "InstallCertificateResponse") {
        return MessageType::InstallCertificateResponse;
    }
    if (s == "LogStatusNotification") {
        return MessageType::LogStatusNotification;
    }
    if (s == "LogStatusNotificationResponse") {
        return MessageType::LogStatusNotificationResponse;
    }
    if (s == "MeterValues") {
        return MessageType::MeterValues;
    }
    if (s == "MeterValuesResponse") {
        return MessageType::MeterValuesResponse;
    }
    if (s == "NotifyChargingLimit") {
        return MessageType::NotifyChargingLimit;
    }
    if (s == "NotifyChargingLimitResponse") {
        return MessageType::NotifyChargingLimitResponse;
    }
    if (s == "NotifyCustomerInformation") {
        return MessageType::NotifyCustomerInformation;
    }
    if (s == "NotifyCustomerInformationResponse") {
        return MessageType::NotifyCustomerInformationResponse;
    }
    if (s == "NotifyDisplayMessages") {
        return MessageType::NotifyDisplayMessages;
    }
    if (s == "NotifyDisplayMessagesResponse") {
        return MessageType::NotifyDisplayMessagesResponse;
    }
    if (s == "NotifyEVChargingNeeds") {
        return MessageType::NotifyEVChargingNeeds;
    }
    if (s == "NotifyEVChargingNeedsResponse") {
        return MessageType::NotifyEVChargingNeedsResponse;
    }
    if (s == "NotifyEVChargingSchedule") {
        return MessageType::NotifyEVChargingSchedule;
    }
    if (s == "NotifyEVChargingScheduleResponse") {
        return MessageType::NotifyEVChargingScheduleResponse;
    }
    if (s == "NotifyEvent") {
        return MessageType::NotifyEvent;
    }
    if (s == "NotifyEventResponse") {
        return MessageType::NotifyEventResponse;
    }
    if (s == "NotifyMonitoringReport") {
        return MessageType::NotifyMonitoringReport;
    }
    if (s == "NotifyMonitoringReportResponse") {
        return MessageType::NotifyMonitoringReportResponse;
    }
    if (s == "NotifyReport") {
        return MessageType::NotifyReport;
    }
    if (s == "NotifyReportResponse") {
        return MessageType::NotifyReportResponse;
    }
    if (s == "PublishFirmware") {
        return MessageType::PublishFirmware;
    }
    if (s == "PublishFirmwareResponse") {
        return MessageType::PublishFirmwareResponse;
    }
    if (s == "PublishFirmwareStatusNotification") {
        return MessageType::PublishFirmwareStatusNotification;
    }
    if (s == "PublishFirmwareStatusNotificationResponse") {
        return MessageType::PublishFirmwareStatusNotificationResponse;
    }
    if (s == "ReportChargingProfiles") {
        return MessageType::ReportChargingProfiles;
    }
    if (s == "ReportChargingProfilesResponse") {
        return MessageType::ReportChargingProfilesResponse;
    }
    if (s == "RequestStartTransaction") {
        return MessageType::RequestStartTransaction;
    }
    if (s == "RequestStartTransactionResponse") {
        return MessageType::RequestStartTransactionResponse;
    }
    if (s == "RequestStopTransaction") {
        return MessageType::RequestStopTransaction;
    }
    if (s == "RequestStopTransactionResponse") {
        return MessageType::RequestStopTransactionResponse;
    }
    if (s == "ReservationStatusUpdate") {
        return MessageType::ReservationStatusUpdate;
    }
    if (s == "ReservationStatusUpdateResponse") {
        return MessageType::ReservationStatusUpdateResponse;
    }
    if (s == "ReserveNow") {
        return MessageType::ReserveNow;
    }
    if (s == "ReserveNowResponse") {
        return MessageType::ReserveNowResponse;
    }
    if (s == "Reset") {
        return MessageType::Reset;
    }
    if (s == "ResetResponse") {
        return MessageType::ResetResponse;
    }
    if (s == "SecurityEventNotification") {
        return MessageType::SecurityEventNotification;
    }
    if (s == "SecurityEventNotificationResponse") {
        return MessageType::SecurityEventNotificationResponse;
    }
    if (s == "SendLocalList") {
        return MessageType::SendLocalList;
    }
    if (s == "SendLocalListResponse") {
        return MessageType::SendLocalListResponse;
    }
    if (s == "SetChargingProfile") {
        return MessageType::SetChargingProfile;
    }
    if (s == "SetChargingProfileResponse") {
        return MessageType::SetChargingProfileResponse;
    }
    if (s == "SetDisplayMessage") {
        return MessageType::SetDisplayMessage;
    }
    if (s == "SetDisplayMessageResponse") {
        return MessageType::SetDisplayMessageResponse;
    }
    if (s == "SetMonitoringBase") {
        return MessageType::SetMonitoringBase;
    }
    if (s == "SetMonitoringBaseResponse") {
        return MessageType::SetMonitoringBaseResponse;
    }
    if (s == "SetMonitoringLevel") {
        return MessageType::SetMonitoringLevel;
    }
    if (s == "SetMonitoringLevelResponse") {
        return MessageType::SetMonitoringLevelResponse;
    }
    if (s == "SetNetworkProfile") {
        return MessageType::SetNetworkProfile;
    }
    if (s == "SetNetworkProfileResponse") {
        return MessageType::SetNetworkProfileResponse;
    }
    if (s == "SetVariableMonitoring") {
        return MessageType::SetVariableMonitoring;
    }
    if (s == "SetVariableMonitoringResponse") {
        return MessageType::SetVariableMonitoringResponse;
    }
    if (s == "SetVariables") {
        return MessageType::SetVariables;
    }
    if (s == "SetVariablesResponse") {
        return MessageType::SetVariablesResponse;
    }
    if (s == "SignCertificate") {
        return MessageType::SignCertificate;
    }
    if (s == "SignCertificateResponse") {
        return MessageType::SignCertificateResponse;
    }
    if (s == "StatusNotification") {
        return MessageType::StatusNotification;
    }
    if (s == "StatusNotificationResponse") {
        return MessageType::StatusNotificationResponse;
    }
    if (s == "TransactionEvent") {
        return MessageType::TransactionEvent;
    }
    if (s == "TransactionEventResponse") {
        return MessageType::TransactionEventResponse;
    }
    if (s == "TriggerMessage") {
        return MessageType::TriggerMessage;
    }
    if (s == "TriggerMessageResponse") {
        return MessageType::TriggerMessageResponse;
    }
    if (s == "UnlockConnector") {
        return MessageType::UnlockConnector;
    }
    if (s == "UnlockConnectorResponse") {
        return MessageType::UnlockConnectorResponse;
    }
    if (s == "UnpublishFirmware") {
        return MessageType::UnpublishFirmware;
    }
    if (s == "UnpublishFirmwareResponse") {
        return MessageType::UnpublishFirmwareResponse;
    }
    if (s == "UpdateFirmware") {
        return MessageType::UpdateFirmware;
    }
    if (s == "UpdateFirmwareResponse") {
        return MessageType::UpdateFirmwareResponse;
    }
    if (s == "InternalError") {
        return MessageType::InternalError;
    }
    if (s == "AdjustPeriodicEventStream") {
        return MessageType::AdjustPeriodicEventStream;
    }
    if (s == "AdjustPeriodicEventStreamResponse") {
        return MessageType::AdjustPeriodicEventStreamResponse;
    }
    if (s == "AFRRSignal") {
        return MessageType::AFRRSignal;
    }
    if (s == "AFRRSignalResponse") {
        return MessageType::AFRRSignalResponse;
    }
    if (s == "BatterySwap") {
        return MessageType::BatterySwap;
    }
    if (s == "BatterySwapResponse") {
        return MessageType::BatterySwapResponse;
    }
    if (s == "ChangeTransactionTariff") {
        return MessageType::ChangeTransactionTariff;
    }
    if (s == "ChangeTransactionTariffResponse") {
        return MessageType::ChangeTransactionTariffResponse;
    }
    if (s == "ClearDERControl") {
        return MessageType::ClearDERControl;
    }
    if (s == "ClearDERControlResponse") {
        return MessageType::ClearDERControlResponse;
    }
    if (s == "ClearTariffs") {
        return MessageType::ClearTariffs;
    }
    if (s == "ClearTariffsResponse") {
        return MessageType::ClearTariffsResponse;
    }
    if (s == "ClosePeriodicEventStream") {
        return MessageType::ClosePeriodicEventStream;
    }
    if (s == "ClosePeriodicEventStreamResponse") {
        return MessageType::ClosePeriodicEventStreamResponse;
    }
    if (s == "GetCRL") {
        return MessageType::GetCRL;
    }
    if (s == "GetCRLResponse") {
        return MessageType::GetCRLResponse;
    }
    if (s == "GetDERControl") {
        return MessageType::GetDERControl;
    }
    if (s == "GetDERControlResponse") {
        return MessageType::GetDERControlResponse;
    }
    if (s == "GetPeriodicEventStream") {
        return MessageType::GetPeriodicEventStream;
    }
    if (s == "GetPeriodicEventStreamResponse") {
        return MessageType::GetPeriodicEventStreamResponse;
    }
    if (s == "GetTariffs") {
        return MessageType::GetTariffs;
    }
    if (s == "GetTariffsResponse") {
        return MessageType::GetTariffsResponse;
    }
    if (s == "NotifyAllowedEnergyTransfer") {
        return MessageType::NotifyAllowedEnergyTransfer;
    }
    if (s == "NotifyAllowedEnergyTransferResponse") {
        return MessageType::NotifyAllowedEnergyTransferResponse;
    }
    if (s == "NotifyDERAlarm") {
        return MessageType::NotifyDERAlarm;
    }
    if (s == "NotifyDERAlarmResponse") {
        return MessageType::NotifyDERAlarmResponse;
    }
    if (s == "NotifyDERStartStop") {
        return MessageType::NotifyDERStartStop;
    }
    if (s == "NotifyDERStartStopResponse") {
        return MessageType::NotifyDERStartStopResponse;
    }
    if (s == "NotifyPeriodicEventStream") {
        return MessageType::NotifyPeriodicEventStream;
    }
    if (s == "NotifyPeriodicEventStreamResponse") {
        return MessageType::NotifyPeriodicEventStreamResponse;
    }
    if (s == "NotifyPriorityCharging") {
        return MessageType::NotifyPriorityCharging;
    }
    if (s == "NotifyPriorityChargingResponse") {
        return MessageType::NotifyPriorityChargingResponse;
    }
    if (s == "NotifySettlement") {
        return MessageType::NotifySettlement;
    }
    if (s == "NotifySettlementResponse") {
        return MessageType::NotifySettlementResponse;
    }
    if (s == "OpenPeriodicEventStream") {
        return MessageType::OpenPeriodicEventStream;
    }
    if (s == "OpenPeriodicEventStreamResponse") {
        return MessageType::OpenPeriodicEventStreamResponse;
    }
    if (s == "PullDynamicScheduleUpdate") {
        return MessageType::PullDynamicScheduleUpdate;
    }
    if (s == "PullDynamicScheduleUpdateResponse") {
        return MessageType::PullDynamicScheduleUpdateResponse;
    }
    if (s == "RequestBatterySwap") {
        return MessageType::RequestBatterySwap;
    }
    if (s == "RequestBatterySwapResponse") {
        return MessageType::RequestBatterySwapResponse;
    }
    if (s == "SetDefaultTariff") {
        return MessageType::SetDefaultTariff;
    }
    if (s == "SetDefaultTariffResponse") {
        return MessageType::SetDefaultTariffResponse;
    }
    if (s == "SetDERControl") {
        return MessageType::SetDERControl;
    }
    if (s == "SetDERControlResponse") {
        return MessageType::SetDERControlResponse;
    }
    if (s == "UpdateDynamicSchedule") {
        return MessageType::UpdateDynamicSchedule;
    }
    if (s == "UpdateDynamicScheduleResponse") {
        return MessageType::UpdateDynamicScheduleResponse;
    }
    if (s == "UsePriorityCharging") {
        return MessageType::UsePriorityCharging;
    }
    if (s == "UsePriorityChargingResponse") {
        return MessageType::UsePriorityChargingResponse;
    }
    if (s == "VatNumberValidation") {
        return MessageType::VatNumberValidation;
    }
    if (s == "VatNumberValidationResponse") {
        return MessageType::VatNumberValidationResponse;
    }
    throw StringToEnumException{s, "MessageType"};
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const MessageType& message_type) {
    os << conversions::messagetype_to_string(message_type);
    return os;
}

} // namespace v2
} // namespace ocpp
