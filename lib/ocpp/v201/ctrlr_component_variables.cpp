// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 -  Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/ctrlr_component_variables.hpp>

namespace ocpp {
namespace v201 {

namespace ControllerComponents {
const Component& InternalCtrlr = {"InternalCtrlr"};
const Component& AlignedDataCtrlr = {"AlignedDataCtrlr"};
const Component& AuthCacheCtrlr = {"AuthCacheCtrlr"};
const Component& AuthCtrlr = {"AuthCtrlr"};
const Component& ChargingStation = {"ChargingStation"};
const Component& ClockCtrlr = {"ClockCtrlr"};
const Component& Connector = {"Connector"};
const Component& CustomizationCtrlr = {"CustomizationCtrlr"};
const Component& DeviceDataCtrlr = {"DeviceDataCtrlr"};
const Component& DisplayMessageCtrlr = {"DisplayMessageCtrlr"};
const Component& EVSE = {"EVSE"};
const Component& ISO15118Ctrlr = {"ISO15118Ctrlr"};
const Component& LocalAuthListCtrlr = {"LocalAuthListCtrlr"};
const Component& MonitoringCtrlr = {"MonitoringCtrlr"};
const Component& OCPPCommCtrlr = {"OCPPCommCtrlr"};
const Component& ReservationCtrlr = {"ReservationCtrlr"};
const Component& SampledDataCtrlr = {"SampledDataCtrlr"};
const Component& SecurityCtrlr = {"SecurityCtrlr"};
const Component& SmartChargingCtrlr = {"SmartChargingCtrlr"};
const Component& TariffCostCtrlr = {"TariffCostCtrlr"};
const Component& TxCtrlr = {"TxCtrlr"};
} // namespace ControllerComponents

namespace ControllerComponentVariables {

const ComponentVariable& InternalCtrlrEnabled = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& ChargePointId = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ChargePointId",
    }),
};
const ComponentVariable& CentralSystemURI = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "CentralSystemURI",
    }),
};
const ComponentVariable& ChargeBoxSerialNumber = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ChargeBoxSerialNumber",
    }),
};
const ComponentVariable& ChargePointModel = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ChargePointModel",
    }),
};
const ComponentVariable& ChargePointSerialNumber = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ChargePointSerialNumber",
    }),
};
const ComponentVariable& ChargePointVendor = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ChargePointVendor",
    }),
};
const ComponentVariable& FirmwareVersion = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "FirmwareVersion",
    }),
};
const ComponentVariable& ICCID = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ICCID",
    }),
};
const ComponentVariable& IMSI = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "IMSI",
    }),
};
const ComponentVariable& MeterSerialNumber = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "MeterSerialNumber",
    }),
};
const ComponentVariable& MeterType = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "MeterType",
    }),
};
const ComponentVariable& SupportedCiphers12 = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SupportedCiphers12",
    }),
};
const ComponentVariable& SupportedCiphers13 = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SupportedCiphers13",
    }),
};
const ComponentVariable& WebsocketReconnectInterval = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "WebsocketReconnectInterval",
    }),
};
const ComponentVariable& AuthorizeConnectorZeroOnConnectorOne = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "AuthorizeConnectorZeroOnConnectorOne",
    }),
};
const ComponentVariable& LogMessages = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "LogMessages",
    }),
};
const ComponentVariable& LogMessagesFormat = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "LogMessagesFormat",
    }),
};
const ComponentVariable& SupportedChargingProfilePurposeTypes = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SupportedChargingProfilePurposeTypes",
    }),
};
const ComponentVariable& MaxCompositeScheduleDuration = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "MaxCompositeScheduleDuration",
    }),
};
const ComponentVariable& NumberOfConnectors = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NumberOfConnectors",
    }),
};
const ComponentVariable& UseSslDefaultVerifyPaths = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "UseSslDefaultVerifyPaths",
    }),
};
const ComponentVariable& OcspRequestInterval = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "OcspRequestInterval",
    }),
};
const ComponentVariable& WebsocketPingPayload = {
    ControllerComponents::InternalCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "WebsocketPingPayload",
    }),
};
const ComponentVariable& AlignedDataCtrlrEnabled = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& AlignedDataCtrlrAvailable = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& AlignedDataInterval = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Interval",
    }),
};
const ComponentVariable& AlignedDataMeasurands = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Measurands",
    }),
};
const ComponentVariable& AlignedDataSendDuringIdle = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SendDuringIdle",
    }),
};
const ComponentVariable& AlignedDataSignReadings = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SignReadings",
    }),
};
const ComponentVariable& AlignedDataTxEndedInterval = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxEndedInterval",
    }),
};
const ComponentVariable& AlignedDataTxEndedMeasurands = {
    ControllerComponents::AlignedDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxEndedMeasurands",
    }),
};
const ComponentVariable& AuthCacheCtrlrAvailable = {
    ControllerComponents::AuthCacheCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& AuthCacheCtrlrEnabled = {
    ControllerComponents::AuthCacheCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& AuthCacheLifeTime = {
    ControllerComponents::AuthCacheCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "LifeTime",
    }),
};
const ComponentVariable& AuthCachePolicy = {
    ControllerComponents::AuthCacheCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Policy",
    }),
};
const ComponentVariable& AuthCacheStorage = {
    ControllerComponents::AuthCacheCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Storage",
    }),
};
const ComponentVariable& AuthCtrlrEnabled = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& AdditionalInfoItemsPerMessage = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "AdditionalInfoItemsPerMessage",
    }),
};
const ComponentVariable& AuthorizeRemoteStart = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "AuthorizeRemoteStart",
    }),
};
const ComponentVariable& LocalAuthorizeOffline = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "LocalAuthorizeOffline",
    }),
};
const ComponentVariable& LocalPreAuthorize = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "LocalPreAuthorize",
    }),
};
const ComponentVariable& MasterPassGroupId = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "MasterPassGroupId",
    }),
};
const ComponentVariable& OfflineTxForUnknownIdEnabled = {
    ControllerComponents::AuthCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "OfflineTxForUnknownIdEnabled",
    }),
};
const ComponentVariable& AllowNewSessionsPendingFirmwareUpdate = {
    ControllerComponents::ChargingStation,
    std::nullopt,
    std::optional<Variable>({"AllowNewSessionsPendingFirmwareUpdate", std::nullopt, "BytesPerMessage"}),
};
const ComponentVariable& ChargingStationAvailabilityState = {
    ControllerComponents::ChargingStation,
    std::nullopt,
    std::optional<Variable>({
        "AvailabilityState",
    }),
};
const ComponentVariable& ChargingStationAvailable = {
    ControllerComponents::ChargingStation,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& ChargingStationSupplyPhases = {
    ControllerComponents::ChargingStation,
    std::nullopt,
    std::optional<Variable>({
        "SupplyPhases",
    }),
};
const ComponentVariable& ClockCtrlrDateTime = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "DateTime",
    }),
};
const ComponentVariable& NextTimeOffsetTransitionDateTime = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NextTimeOffsetTransitionDateTime",
    }),
};
const ComponentVariable& NtpServerUri = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NtpServerUri",
    }),
};
const ComponentVariable& NtpSource = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NtpSource",
    }),
};
const ComponentVariable& TimeAdjustmentReportingThreshold = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TimeAdjustmentReportingThreshold",
    }),
};
const ComponentVariable& TimeOffset = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TimeOffset",
    }),
};
const ComponentVariable& TimeOffsetNextTransition = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({"TimeOffset", std::nullopt, "NextTransition"}),
};
const ComponentVariable& TimeSource = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TimeSource",
    }),
};
const ComponentVariable& TimeZone = {
    ControllerComponents::ClockCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TimeZone",
    }),
};
const ComponentVariable& ConnectorAvailable = {
    ControllerComponents::Connector,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& ConnectorType = {
    ControllerComponents::Connector,
    std::nullopt,
    std::optional<Variable>({
        "ConnectorType",
    }),
};
const ComponentVariable& ConnectorSupplyPhases = {
    ControllerComponents::Connector,
    std::nullopt,
    std::optional<Variable>({
        "SupplyPhases",
    }),
};
const ComponentVariable& CustomImplementationEnabled = {
    ControllerComponents::CustomizationCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "CustomImplementationEnabled",
    }),
};
const ComponentVariable& BytesPerMessageGetReport = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({"BytesPerMessage", std::nullopt, "GetReport"}),
};
const ComponentVariable& BytesPerMessageGetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({"BytesPerMessage", std::nullopt, "GetVariables"}),
};
const ComponentVariable& BytesPerMessageSetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({"BytesPerMessage", std::nullopt, "SetVariables"}),
};
const ComponentVariable& ConfigurationValueSize = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ConfigurationValueSize",
    }),
};
const ComponentVariable& ItemsPerMessageGetReport = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({"ItemsPerMessage", std::nullopt, "GetReport"}),
};
const ComponentVariable& ItemsPerMessageGetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({"ItemsPerMessage", std::nullopt, "GetVariables"}),
};
const ComponentVariable& ItemsPerMessageSetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({"ItemsPerMessage", std::nullopt, "SetVariables"}),
};
const ComponentVariable& ReportingValueSize = {
    ControllerComponents::DeviceDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ReportingValueSize",
    }),
};
const ComponentVariable& DisplayMessageCtrlrAvailable = {
    ControllerComponents::DisplayMessageCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& NumberOfDisplayMessages = {
    ControllerComponents::DisplayMessageCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NumberOfDisplayMessages",
    }),
};
const ComponentVariable& DisplayMessageSupportedFormats = {
    ControllerComponents::DisplayMessageCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SupportedFormats",
    }),
};
const ComponentVariable& DisplayMessageSupportedPriorities = {
    ControllerComponents::DisplayMessageCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SupportedPriorities",
    }),
};
const ComponentVariable& EVSEAllowReset = {
    ControllerComponents::EVSE,
    std::nullopt,
    std::optional<Variable>({
        "AllowReset",
    }),
};
const ComponentVariable& EVSEAvailable = {
    ControllerComponents::EVSE,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& EVSEPower = {
    ControllerComponents::EVSE,
    std::nullopt,
    std::optional<Variable>({
        "Power",
    }),
};
const ComponentVariable& EVSESupplyPhases = {
    ControllerComponents::EVSE,
    std::nullopt,
    std::optional<Variable>({
        "SupplyPhases",
    }),
};
const ComponentVariable& CentralContractValidationAllowed = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "CentralContractValidationAllowed",
    }),
};
const ComponentVariable& ContractValidationOffline = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "ContractValidationOffline",
    }),
};
const ComponentVariable& RequestMeteringReceipt = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "RequestMeteringReceipt",
    }),
};
const ComponentVariable& ISO15118CtrlrCountryName = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "CountryName",
    }),
};
const ComponentVariable& ISO15118CtrlrOrganizationName = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "OrganizationName",
    }),
};
const ComponentVariable& PnCEnabled = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "PnCEnabled",
    }),
};
const ComponentVariable& V2GCertificateInstallationEnabled = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "V2GCertificateInstallationEnabled",
    }),
};
const ComponentVariable& ContractCertificateInstallationEnabled = {
    ControllerComponents::ISO15118Ctrlr,
    std::nullopt,
    std::optional<Variable>({
        "ContractCertificateInstallationEnabled",
    }),
};
const ComponentVariable& LocalAuthListCtrlrAvailable = {
    ControllerComponents::LocalAuthListCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& BytesPerMessageSendLocalList = {
    ControllerComponents::LocalAuthListCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "BytesPerMessageSendLocalList",
    }),
};
const ComponentVariable& LocalAuthListCtrlrEnabled = {
    ControllerComponents::LocalAuthListCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& LocalAuthListCtrlrEntries = {
    ControllerComponents::LocalAuthListCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Entries",
    }),
};
const ComponentVariable& ItemsPerMessageSendLocalList = {
    ControllerComponents::LocalAuthListCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ItemsPerMessageSendLocalList",
    }),
};
const ComponentVariable& LocalAuthListCtrlrStorage = {
    ControllerComponents::LocalAuthListCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Storage",
    }),
};
const ComponentVariable& MonitoringCtrlrAvailable = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& BytesPerMessageClearVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({"BytesPerMessage", std::nullopt, "ClearVariableMonitoring"}),
};
const ComponentVariable& BytesPerMessageSetVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({"BytesPerMessage", std::nullopt, "SetVariableMonitoring"}),
};
const ComponentVariable& MonitoringCtrlrEnabled = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& ItemsPerMessageClearVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({"ItemsPerMessage", std::nullopt, "ClearVariableMonitoring"}),
};
const ComponentVariable& ItemsPerMessageSetVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({"ItemsPerMessage", std::nullopt, "SetVariableMonitoring"}),
};
const ComponentVariable& OfflineQueuingSeverity = {
    ControllerComponents::MonitoringCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "OfflineQueuingSeverity",
    }),
};
const ComponentVariable& ActiveNetworkProfile = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ActiveNetworkProfile",
    }),
};
const ComponentVariable& FileTransferProtocols = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "FileTransferProtocols",
    }),
};
const ComponentVariable& HeartbeatInterval = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "HeartbeatInterval",
    }),
};
const ComponentVariable& MessageTimeout = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({"MessageTimeout", std::nullopt, "Default"}),
};
const ComponentVariable& MessageAttemptInterval = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({"MessageAttemptInterval", std::nullopt, "TransactionEvent"}),
};
const ComponentVariable& MessageAttempts = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({"MessageAttempts", std::nullopt, "TransactionEvent"}),
};
const ComponentVariable& NetworkConfigurationPriority = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NetworkConfigurationPriority",
    }),
};
const ComponentVariable& NetworkProfileConnectionAttempts = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NetworkProfileConnectionAttempts",
    }),
};
const ComponentVariable& OfflineThreshold = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "OfflineThreshold",
    }),
};
const ComponentVariable& QueueAllMessages = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "QueueAllMessages",
    }),
};
const ComponentVariable& ResetRetries = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ResetRetries",
    }),
};
const ComponentVariable& UnlockOnEVSideDisconnect = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "UnlockOnEVSideDisconnect",
    }),
};
const ComponentVariable& WebSocketPingInterval = {
    ControllerComponents::OCPPCommCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "WebSocketPingInterval",
    }),
};
const ComponentVariable& ReservationCtrlrAvailable = {
    ControllerComponents::ReservationCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& ReservationCtrlrEnabled = {
    ControllerComponents::ReservationCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& ReservationCtrlrNonEvseSpecific = {
    ControllerComponents::ReservationCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NonEvseSpecific",
    }),
};
const ComponentVariable& SampledDataCtrlrAvailable = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& SampledDataCtrlrEnabled = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& SampledDataSignReadings = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SignReadings",
    }),
};
const ComponentVariable& SampledDataTxEndedInterval = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxEndedInterval",
    }),
};
const ComponentVariable& SampledDataTxEndedMeasurands = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxEndedMeasurands",
    }),
};
const ComponentVariable& SampledDataTxStartedMeasurands = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxStartedMeasurands",
    }),
};
const ComponentVariable& SampledDataTxUpdatedInterval = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxUpdatedInterval",
    }),
};
const ComponentVariable& SampledDataTxUpdatedMeasurands = {
    ControllerComponents::SampledDataCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxUpdatedMeasurands",
    }),
};
const ComponentVariable& AdditionalRootCertificateCheck = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "AdditionalRootCertificateCheck",
    }),
};
const ComponentVariable& BasicAuthPassword = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "BasicAuthPassword",
    }),
};
const ComponentVariable& CertificateEntries = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "CertificateEntries",
    }),
};
const ComponentVariable& SecurityCtrlrIdentity = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Identity",
    }),
};
const ComponentVariable& MaxCertificateChainSize = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "MaxCertificateChainSize",
    }),
};
const ComponentVariable& OrganizationName = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "OrganizationName",
    }),
};
const ComponentVariable& SecurityProfile = {
    ControllerComponents::SecurityCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "SecurityProfile",
    }),
};
const ComponentVariable& ACPhaseSwitchingSupported = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ACPhaseSwitchingSupported",
    }),
};
const ComponentVariable& SmartChargingCtrlrAvailable = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable& SmartChargingCtrlrAvailableEnabled = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable& EntriesChargingProfiles = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({"Entries", std::nullopt, "ChargingProfiles"}),
};
const ComponentVariable& ExternalControlSignalsEnabled = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ExternalControlSignalsEnabled",
    }),
};
const ComponentVariable& LimitChangeSignificance = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "LimitChangeSignificance",
    }),
};
const ComponentVariable& NotifyChargingLimitWithSchedules = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "NotifyChargingLimitWithSchedules",
    }),
};
const ComponentVariable& PeriodsPerSchedule = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "PeriodsPerSchedule",
    }),
};
const ComponentVariable& Phases3to1 = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Phases3to1",
    }),
};
const ComponentVariable& ChargingProfileMaxStackLevel = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "ProfileStackLevel",
    }),
};
const ComponentVariable& ChargingScheduleChargingRateUnit = {
    ControllerComponents::SmartChargingCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "RateUnit",
    }),
};
const ComponentVariable& TariffCostCtrlrAvailableTariff = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({"Available", std::nullopt, "Tariff"}),
};
const ComponentVariable& TariffCostCtrlrAvailableCost = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({"Available", std::nullopt, "Cost"}),
};
const ComponentVariable& TariffCostCtrlrCurrency = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "Currency",
    }),
};
const ComponentVariable& TariffCostCtrlrEnabledTariff = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({"Enabled", std::nullopt, "Tariff"}),
};
const ComponentVariable& TariffCostCtrlrEnabledCost = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({"Enabled", std::nullopt, "Cost"}),
};
const ComponentVariable& TariffFallbackMessage = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TariffFallbackMessage",
    }),
};
const ComponentVariable& TotalCostFallbackMessage = {
    ControllerComponents::TariffCostCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TotalCostFallbackMessage",
    }),
};
const ComponentVariable& EVConnectionTimeOut = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "EVConnectionTimeOut",
    }),
};
const ComponentVariable& MaxEnergyOnInvalidId = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "MaxEnergyOnInvalidId",
    }),
};
const ComponentVariable& StopTxOnEVSideDisconnect = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "StopTxOnEVSideDisconnect",
    }),
};
const ComponentVariable& StopTxOnInvalidId = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "StopTxOnInvalidId",
    }),
};
const ComponentVariable& TxBeforeAcceptedEnabled = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxBeforeAcceptedEnabled",
    }),
};
const ComponentVariable& TxStartPoint = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxStartPoint",
    }),
};
const ComponentVariable& TxStopPoint = {
    ControllerComponents::TxCtrlr,
    std::nullopt,
    std::optional<Variable>({
        "TxStopPoint",
    }),
};

} // namespace ControllerComponentVariables
} // namespace v201
} // namespace ocpp
