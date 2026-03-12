// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 -  Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/ctrlr_component_variables.hpp>

#include <everest/logging.hpp>
#include <nlohmann/json.hpp>
#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/utils.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

namespace ControllerComponents {
const Component InternalCtrlr = {"InternalCtrlr"};
const Component AlignedDataCtrlr = {"AlignedDataCtrlr"};
const Component AuthCacheCtrlr = {"AuthCacheCtrlr"};
const Component AuthCtrlr = {"AuthCtrlr"};
const Component ChargingStation = {"ChargingStation"};
const Component ClockCtrlr = {"ClockCtrlr"};
const Component ConnectedEV = {"ConnectedEV"};
const Component CustomizationCtrlr = {"CustomizationCtrlr"};
const Component DeviceDataCtrlr = {"DeviceDataCtrlr"};
const Component DisplayMessageCtrlr = {"DisplayMessageCtrlr"};
const Component ISO15118Ctrlr = {"ISO15118Ctrlr"};
const Component LocalAuthListCtrlr = {"LocalAuthListCtrlr"};
const Component MonitoringCtrlr = {"MonitoringCtrlr"};
const Component OCPPCommCtrlr = {"OCPPCommCtrlr"};
const Component ReservationCtrlr = {"ReservationCtrlr"};
const Component SampledDataCtrlr = {"SampledDataCtrlr"};
const Component SecurityCtrlr = {"SecurityCtrlr"};
const Component SmartChargingCtrlr = {"SmartChargingCtrlr"};
const Component TariffCostCtrlr = {"TariffCostCtrlr"};
const Component TxCtrlr = {"TxCtrlr"};
const Component OCPP16LegacyCtrlr = {"OCPP16LegacyCtrlr"};
const Component CustomLegacyController = {"CustomLegacyController"};
} // namespace ControllerComponents

namespace StandardizedVariables {
const Variable Problem = {"Problem"};
const Variable Tripped = {"Tripped"};
const Variable Overload = {"Overload"};
const Variable Fallback = {"Fallback"};
}; // namespace StandardizedVariables

namespace ControllerComponentVariables {
const ComponentVariable InternalCtrlrEnabled = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const RequiredComponentVariable ChargePointId = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ChargePointId",
    }),
};
const RequiredComponentVariable NetworkConnectionProfiles = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "NetworkConnectionProfiles",
    }),
};
const RequiredComponentVariable ChargeBoxSerialNumber = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ChargeBoxSerialNumber",
    }),
};
const RequiredComponentVariable ChargePointModel = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ChargePointModel",
    }),
};
const ComponentVariable ChargePointSerialNumber = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ChargePointSerialNumber",
    }),
};
const RequiredComponentVariable ChargePointVendor = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ChargePointVendor",
    }),
};
const RequiredComponentVariable FirmwareVersion = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "FirmwareVersion",
    }),
};
const ComponentVariable ICCID = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ICCID",
    }),
};
const ComponentVariable IMSI = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "IMSI",
    }),
};
const ComponentVariable MeterSerialNumber = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MeterSerialNumber",
    }),
};
const ComponentVariable MeterType = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MeterType",
    }),
};
const RequiredComponentVariable SupportedCiphers12 = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "SupportedCiphers12",
    }),
};
const RequiredComponentVariable SupportedCiphers13 = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "SupportedCiphers13",
    }),
};
const ComponentVariable AuthorizeConnectorZeroOnConnectorOne = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "AuthorizeConnectorZeroOnConnectorOne",
    }),
};
const ComponentVariable LogMessages = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogMessages",
    }),
};
const ComponentVariable LogMessagesRaw = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogMessagesRaw",
    }),
};
const RequiredComponentVariable LogMessagesFormat = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogMessagesFormat",
    }),
};
const ComponentVariable LogRotation = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogRotation",
    }),
};
const ComponentVariable LogRotationDateSuffix = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogRotationDateSuffix",
    }),
};
const ComponentVariable LogRotationMaximumFileSize = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogRotationMaximumFileSize",
    }),
};
const ComponentVariable LogRotationMaximumFileCount = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "LogRotationMaximumFileCount",
    }),
};
const ComponentVariable SupportedCriteria = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "SupportedCriteria",
    }),
};
const ComponentVariable RoundClockAlignedTimestamps = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "RoundClockAlignedTimestamps",
    }),
};
const ComponentVariable NetworkConfigTimeout = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "NetworkConfigTimeout",
    }),
};
const ComponentVariable SupportedChargingProfilePurposeTypes = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "SupportedChargingProfilePurposeTypes",
    }),
};
const ComponentVariable MaxCompositeScheduleDuration = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MaxCompositeScheduleDuration",
    }),
};
const RequiredComponentVariable NumberOfConnectors = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "NumberOfConnectors",
    }),
};
const ComponentVariable UseSslDefaultVerifyPaths = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "UseSslDefaultVerifyPaths",
    }),
};
const ComponentVariable VerifyCsmsCommonName = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "VerifyCsmsCommonName",
    }),
};
const ComponentVariable UseTPM = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "UseTPM",
    }),
};
const ComponentVariable UseTPMSeccLeafCertificate = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "UseTPMSeccLeafCertificate",
    }),
};
const ComponentVariable VerifyCsmsAllowWildcards = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "VerifyCsmsAllowWildcards",
    }),
};
const ComponentVariable IFace = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "IFace",
    }),
};
const ComponentVariable EnableTLSKeylog = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "EnableTLSKeylog",
    }),
};
const ComponentVariable TLSKeylogFile = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "TLSKeylogFile",
    }),
};
const ComponentVariable OcspRequestInterval = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "OcspRequestInterval",
    }),
};
const ComponentVariable WebsocketPingPayload = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "WebsocketPingPayload",
    }),
};
const ComponentVariable WebsocketPongTimeout = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "WebsocketPongTimeout",
    }),
};
const ComponentVariable MonitorsProcessingInterval = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MonitorsProcessingInterval",
    }),
};
const ComponentVariable MaxCustomerInformationDataLength = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MaxCustomerInformationDataLength",
    }),
};
const ComponentVariable V2GCertificateExpireCheckInitialDelaySeconds = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "V2GCertificateExpireCheckInitialDelaySeconds",
    }),
};
const ComponentVariable V2GCertificateExpireCheckIntervalSeconds = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "V2GCertificateExpireCheckIntervalSeconds",
    }),
};
const ComponentVariable ClientCertificateExpireCheckInitialDelaySeconds = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ClientCertificateExpireCheckInitialDelaySeconds",
    }),
};
const ComponentVariable ClientCertificateExpireCheckIntervalSeconds = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ClientCertificateExpireCheckIntervalSeconds",
    }),
};
const ComponentVariable MessageQueueSizeThreshold = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MessageQueueSizeThreshold",
    }),
};
const ComponentVariable MaxMessageSize = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "MaxMessageSize",
    }),
};
const ComponentVariable ResumeTransactionsOnBoot = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "ResumeTransactionsOnBoot",
    }),
};
const ComponentVariable AllowCSMSRootCertInstallWithUnsecureConnection = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "AllowCSMSRootCertInstallWithUnsecureConnection",
    }),
};
const ComponentVariable AllowMFRootCertInstallWithUnsecureConnection = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "AllowMFRootCertInstallWithUnsecureConnection",
    }),
};
const ComponentVariable AllowSecurityLevelZeroConnections = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "AllowSecurityLevelZeroConnections",
    }),
};
const RequiredComponentVariable SupportedOcppVersions = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({"SupportedOcppVersions"}),
};
const ComponentVariable AlignedDataCtrlrEnabled = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable AlignedDataCtrlrAvailable = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const RequiredComponentVariable AlignedDataInterval = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "Interval",
    }),
};
const RequiredComponentVariable AlignedDataMeasurands = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "Measurands",
    }),
};
const ComponentVariable AlignedDataSendDuringIdle = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "SendDuringIdle",
    }),
};
const ComponentVariable AlignedDataSignReadings = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "SignReadings",
    }),
};
const RequiredComponentVariable AlignedDataTxEndedInterval = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "TxEndedInterval",
    }),
};
const RequiredComponentVariable AlignedDataTxEndedMeasurands = {
    ControllerComponents::AlignedDataCtrlr,
    std::optional<Variable>({
        "TxEndedMeasurands",
    }),
};
const ComponentVariable AuthCacheCtrlrAvailable = {
    ControllerComponents::AuthCacheCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable AuthCacheDisablePostAuthorize = {
    ControllerComponents::AuthCacheCtrlr,
    std::optional<Variable>({
        "DisablePostAuthorize",
    }),
};
const ComponentVariable AuthCacheCtrlrEnabled = {
    ControllerComponents::AuthCacheCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable AuthCacheLifeTime = {
    ControllerComponents::AuthCacheCtrlr,
    std::optional<Variable>({
        "LifeTime",
    }),
};
const ComponentVariable AuthCachePolicy = {
    ControllerComponents::AuthCacheCtrlr,
    std::optional<Variable>({
        "Policy",
    }),
};
const ComponentVariable AuthCacheStorage = {
    ControllerComponents::AuthCacheCtrlr,
    std::optional<Variable>({
        "Storage",
    }),
};
const ComponentVariable AuthCtrlrEnabled = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable AdditionalInfoItemsPerMessage = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "AdditionalInfoItemsPerMessage",
    }),
};
const RequiredComponentVariable AuthorizeRemoteStart = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "AuthorizeRemoteStart",
    }),
};
const RequiredComponentVariable LocalAuthorizeOffline = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "LocalAuthorizeOffline",
    }),
};
const RequiredComponentVariable LocalPreAuthorize = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "LocalPreAuthorize",
    }),
};
const ComponentVariable DisableRemoteAuthorization = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "DisableRemoteAuthorization",
    }),
};
const ComponentVariable MasterPassGroupId = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "MasterPassGroupId",
    }),
};
const ComponentVariable OfflineTxForUnknownIdEnabled = {
    ControllerComponents::AuthCtrlr,
    std::optional<Variable>({
        "OfflineTxForUnknownIdEnabled",
    }),
};
const ComponentVariable AllowNewSessionsPendingFirmwareUpdate = {
    ControllerComponents::ChargingStation,
    std::optional<Variable>({"AllowNewSessionsPendingFirmwareUpdate", "BytesPerMessage"}),
};
const RequiredComponentVariable ChargingStationAvailabilityState = {
    ControllerComponents::ChargingStation,
    std::optional<Variable>({
        "AvailabilityState",
    }),
};
const RequiredComponentVariable ChargingStationAvailable = {
    ControllerComponents::ChargingStation,
    std::optional<Variable>({
        "Available",
    }),
};
const RequiredComponentVariable ChargingStationSupplyPhases = {
    ControllerComponents::ChargingStation,
    std::optional<Variable>({
        "SupplyPhases",
    }),
};
const RequiredComponentVariable ClockCtrlrDateTime = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "DateTime",
    }),
};
const ComponentVariable NextTimeOffsetTransitionDateTime = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "NextTimeOffsetTransitionDateTime",
    }),
};
const ComponentVariable NtpServerUri = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "NtpServerUri",
    }),
};
const ComponentVariable NtpSource = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "NtpSource",
    }),
};
const ComponentVariable TimeAdjustmentReportingThreshold = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "TimeAdjustmentReportingThreshold",
    }),
};
const ComponentVariable TimeOffset = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "TimeOffset",
    }),
};
const ComponentVariable TimeOffsetNextTransition = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({"TimeOffset", "NextTransition"}),
};
const RequiredComponentVariable TimeSource = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "TimeSource",
    }),
};
const ComponentVariable TimeZone = {
    ControllerComponents::ClockCtrlr,
    std::optional<Variable>({
        "TimeZone",
    }),
};
const ComponentVariable CustomImplementationEnabled = {
    ControllerComponents::CustomizationCtrlr,
    std::optional<Variable>({
        "CustomImplementationEnabled",
    }),
};
const ComponentVariable CustomImplementationCaliforniaPricingEnabled = {
    ControllerComponents::CustomizationCtrlr,
    std::optional<Variable>({"CustomImplementationEnabled", "org.openchargealliance.costmsg"}),
};
const ComponentVariable CustomImplementationMultiLanguageEnabled = {
    ControllerComponents::CustomizationCtrlr,
    std::optional<Variable>({"CustomImplementationEnabled", "org.openchargealliance.multilanguage"}),
};
const RequiredComponentVariable BytesPerMessageGetReport = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({"BytesPerMessage", "GetReport"}),
};
const RequiredComponentVariable BytesPerMessageGetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({"BytesPerMessage", "GetVariables"}),
};
const RequiredComponentVariable BytesPerMessageSetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({"BytesPerMessage", "SetVariables"}),
};
const ComponentVariable ConfigurationValueSize = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({
        "ConfigurationValueSize",
    }),
};
const RequiredComponentVariable ItemsPerMessageGetReport = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({"ItemsPerMessage", "GetReport"}),
};
const RequiredComponentVariable ItemsPerMessageGetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({"ItemsPerMessage", "GetVariables"}),
};
const RequiredComponentVariable ItemsPerMessageSetVariables = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({"ItemsPerMessage", "SetVariables"}),
};
const ComponentVariable ReportingValueSize = {
    ControllerComponents::DeviceDataCtrlr,
    std::optional<Variable>({
        "ReportingValueSize",
    }),
};
const ComponentVariable DisplayMessageCtrlrAvailable = {
    ControllerComponents::DisplayMessageCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const RequiredComponentVariable NumberOfDisplayMessages = {
    ControllerComponents::DisplayMessageCtrlr,
    std::optional<Variable>({
        "DisplayMessages",
    }),
};
const RequiredComponentVariable DisplayMessageSupportedFormats = {
    ControllerComponents::DisplayMessageCtrlr,
    std::optional<Variable>({
        "SupportedFormats",
    }),
};
const RequiredComponentVariable DisplayMessageSupportedPriorities = {
    ControllerComponents::DisplayMessageCtrlr,
    std::optional<Variable>({
        "SupportedPriorities",
    }),
};
const ComponentVariable DisplayMessageSupportedStates = {ControllerComponents::DisplayMessageCtrlr,
                                                         std::optional<Variable>({"SupportedStates"})};

const ComponentVariable DisplayMessageQRCodeDisplayCapable = {ControllerComponents::DisplayMessageCtrlr,
                                                              std::optional<Variable>({"QRCodeDisplayCapable"})};

const ComponentVariable DisplayMessageLanguage = {ControllerComponents::DisplayMessageCtrlr,
                                                  std::optional<Variable>({"Language"})};

const ComponentVariable CentralContractValidationAllowed = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "CentralContractValidationAllowed",
    }),
};
const RequiredComponentVariable ContractValidationOffline = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "ContractValidationOffline",
    }),
};
const ComponentVariable RequestMeteringReceipt = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "RequestMeteringReceipt",
    }),
};
const ComponentVariable ISO15118CtrlrSeccId = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "SeccId",
    }),
};
const ComponentVariable ISO15118CtrlrCountryName = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "CountryName",
    }),
};
const ComponentVariable ISO15118CtrlrOrganizationName = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "OrganizationName",
    }),
};
const ComponentVariable PnCEnabled = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "PnCEnabled",
    }),
};
const ComponentVariable V2GCertificateInstallationEnabled = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "V2GCertificateInstallationEnabled",
    }),
};
const ComponentVariable ContractCertificateInstallationEnabled = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "ContractCertificateInstallationEnabled",
    }),
};
const ComponentVariable LocalAuthListCtrlrAvailable = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const RequiredComponentVariable BytesPerMessageSendLocalList = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "BytesPerMessage",
    }),
};
const ComponentVariable LocalAuthListCtrlrEnabled = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const RequiredComponentVariable LocalAuthListCtrlrEntries = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "Entries",
    }),
};
const RequiredComponentVariable ItemsPerMessageSendLocalList = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "ItemsPerMessage",
    }),
};
const ComponentVariable LocalAuthListCtrlrStorage = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "Storage",
    }),
};
const ComponentVariable LocalAuthListDisablePostAuthorize = {
    ControllerComponents::LocalAuthListCtrlr,
    std::optional<Variable>({
        "DisablePostAuthorize",
    }),
};
const ComponentVariable MonitoringCtrlrAvailable = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable BytesPerMessageClearVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({"BytesPerMessage", "ClearVariableMonitoring"}),
};
const RequiredComponentVariable BytesPerMessageSetVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({"BytesPerMessage", "SetVariableMonitoring"}),
};
const ComponentVariable MonitoringCtrlrEnabled = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable ActiveMonitoringBase = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({"ActiveMonitoringBase"}),
};
const ComponentVariable ActiveMonitoringLevel = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({"ActiveMonitoringLevel"}),
};
const ComponentVariable ItemsPerMessageClearVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({"ItemsPerMessage", "ClearVariableMonitoring"}),
};
const RequiredComponentVariable ItemsPerMessageSetVariableMonitoring = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({"ItemsPerMessage", "SetVariableMonitoring"}),
};
const ComponentVariable OfflineQueuingSeverity = {
    ControllerComponents::MonitoringCtrlr,
    std::optional<Variable>({
        "OfflineQueuingSeverity",
    }),
};
const ComponentVariable ActiveNetworkProfile = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "ActiveNetworkProfile",
    }),
};
const RequiredComponentVariable FileTransferProtocols = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "FileTransferProtocols",
    }),
};
const ComponentVariable HeartbeatInterval = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "HeartbeatInterval",
    }),
};
const RequiredComponentVariable MessageTimeout = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({"MessageTimeout", "Default"}),
};
const RequiredComponentVariable MessageAttemptInterval = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({"MessageAttemptInterval", "TransactionEvent"}),
};
const RequiredComponentVariable MessageAttempts = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({"MessageAttempts", "TransactionEvent"}),
};
const RequiredComponentVariable NetworkConfigurationPriority = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "NetworkConfigurationPriority",
    }),
};
const RequiredComponentVariable NetworkProfileConnectionAttempts = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "NetworkProfileConnectionAttempts",
    }),
};
const RequiredComponentVariable OfflineThreshold = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "OfflineThreshold",
    }),
};
const ComponentVariable QueueAllMessages = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "QueueAllMessages",
    }),
};
const ComponentVariable MessageTypesDiscardForQueueing = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "MessageTypesDiscardForQueueing",
    }),
};
const RequiredComponentVariable ResetRetries = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "ResetRetries",
    }),
};
const RequiredComponentVariable RetryBackOffRandomRange = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "RetryBackOffRandomRange",
    }),
};
const RequiredComponentVariable RetryBackOffRepeatTimes = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "RetryBackOffRepeatTimes",
    }),
};
const RequiredComponentVariable RetryBackOffWaitMinimum = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "RetryBackOffWaitMinimum",
    }),
};
const RequiredComponentVariable UnlockOnEVSideDisconnect = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "UnlockOnEVSideDisconnect",
    }),
};
const RequiredComponentVariable WebSocketPingInterval = {
    ControllerComponents::OCPPCommCtrlr,
    std::optional<Variable>({
        "WebSocketPingInterval",
    }),
};
const ComponentVariable ReservationCtrlrAvailable = {
    ControllerComponents::ReservationCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable ReservationCtrlrEnabled = {
    ControllerComponents::ReservationCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable ReservationCtrlrNonEvseSpecific = {
    ControllerComponents::ReservationCtrlr,
    std::optional<Variable>({
        "NonEvseSpecific",
    }),
};
const ComponentVariable SampledDataCtrlrAvailable = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable SampledDataCtrlrEnabled = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const ComponentVariable SampledDataSignReadings = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "SignReadings",
    }),
};
const RequiredComponentVariable SampledDataTxEndedInterval = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "TxEndedInterval",
    }),
};
const RequiredComponentVariable SampledDataTxEndedMeasurands = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "TxEndedMeasurands",
    }),
};
const RequiredComponentVariable SampledDataTxStartedMeasurands = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "TxStartedMeasurands",
    }),
};
const RequiredComponentVariable SampledDataTxUpdatedInterval = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "TxUpdatedInterval",
    }),
};
const RequiredComponentVariable SampledDataTxUpdatedMeasurands = {
    ControllerComponents::SampledDataCtrlr,
    std::optional<Variable>({
        "TxUpdatedMeasurands",
    }),
};
const ComponentVariable AdditionalRootCertificateCheck = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "AdditionalRootCertificateCheck",
    }),
};
const ComponentVariable BasicAuthPassword = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "BasicAuthPassword",
    }),
};
const RequiredComponentVariable CertificateEntries = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "CertificateEntries",
    }),
};
const ComponentVariable CertSigningRepeatTimes = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "CertSigningRepeatTimes",
    }),
};
const ComponentVariable CertSigningWaitMinimum = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "CertSigningWaitMinimum",
    }),
};
const RequiredComponentVariable SecurityCtrlrIdentity = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "Identity",
    }),
};
const ComponentVariable MaxCertificateChainSize = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "MaxCertificateChainSize",
    }),
};
const ComponentVariable UpdateCertificateSymlinks = {
    ControllerComponents::InternalCtrlr,
    std::optional<Variable>({
        "UpdateCertificateSymlinks",
    }),
};
const RequiredComponentVariable OrganizationName = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "OrganizationName",
    }),
};
const RequiredComponentVariable SecurityProfile = {
    ControllerComponents::SecurityCtrlr,
    std::optional<Variable>({
        "SecurityProfile",
    }),
};
const ComponentVariable ACPhaseSwitchingSupported = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "ACPhaseSwitchingSupported",
    }),
};
const ComponentVariable SmartChargingCtrlrAvailable = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "Available",
    }),
};
const ComponentVariable SmartChargingCtrlrEnabled = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "Enabled",
    }),
};
const RequiredComponentVariable EntriesChargingProfiles = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"Entries", "ChargingProfiles"}),
};
const ComponentVariable ExternalControlSignalsEnabled = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "ExternalControlSignalsEnabled",
    }),
};
const RequiredComponentVariable LimitChangeSignificance = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "LimitChangeSignificance",
    }),
};
const ComponentVariable NotifyChargingLimitWithSchedules = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "NotifyChargingLimitWithSchedules",
    }),
};
const RequiredComponentVariable PeriodsPerSchedule = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "PeriodsPerSchedule",
    }),
};
const RequiredComponentVariable CompositeScheduleDefaultLimitAmps = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"CompositeScheduleDefaultLimitAmps"}),
};
const RequiredComponentVariable CompositeScheduleDefaultLimitWatts = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"CompositeScheduleDefaultLimitWatts"}),
};
const RequiredComponentVariable CompositeScheduleDefaultNumberPhases = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"CompositeScheduleDefaultNumberPhases"}),
};
const RequiredComponentVariable SupplyVoltage = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"SupplyVoltage"}),
};
const ComponentVariable Phases3to1 = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "Phases3to1",
    }),
};
const RequiredComponentVariable ChargingProfileMaxStackLevel = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "ProfileStackLevel",
    }),
};
const RequiredComponentVariable ChargingScheduleChargingRateUnit = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "RateUnit",
    }),
};
const ComponentVariable IgnoredProfilePurposesOffline = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({
        "IgnoredProfilePurposesOffline",
    }),
};
const ComponentVariable ChargingProfilePersistenceTxProfile = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"ChargingProfilePersistence", "TxProfile", std::nullopt}), std::nullopt};

const ComponentVariable ChargingProfilePersistenceChargingStationExternalConstraints = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"ChargingProfilePersistence", "ChargingStationExternalConstraints", std::nullopt}),
    std::nullopt};

const ComponentVariable ChargingProfilePersistenceLocalGeneration = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"ChargingProfilePersistence", "LocalGeneration", std::nullopt}), std::nullopt};

const ComponentVariable ChargingProfileUpdateRateLimit = {
    ControllerComponents::SmartChargingCtrlr, std::optional<Variable>({"UpdateRateLimit", std::nullopt, std::nullopt}),
    std::nullopt};

const ComponentVariable MaxExternalConstraintsId = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"MaxExternalConstraintsId", std::nullopt, std::nullopt}), std::nullopt};

const ComponentVariable SupportedAdditionalPurposes = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"SupportedAdditionalPurposes", std::nullopt, std::nullopt}), std::nullopt};

const ComponentVariable SupportsDynamicProfiles = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"SupportsFeature", "DynamicProfiles", std::nullopt}), std::nullopt};

const ComponentVariable SupportsUseLocalTime = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"SupportsFeature", "UseLocalTime", std::nullopt}), std::nullopt};

const ComponentVariable SupportsRandomizedDelay = {
    ControllerComponents::SmartChargingCtrlr,
    std::optional<Variable>({"SupportsFeature", "RandomizedDelay", std::nullopt}), std::nullopt};

const ComponentVariable SupportsLimitAtSoC = {ControllerComponents::SmartChargingCtrlr,
                                              std::optional<Variable>({"SupportsFeature", "LimitAtSoC", std::nullopt}),
                                              std::nullopt};

const ComponentVariable SupportsEvseSleep = {ControllerComponents::SmartChargingCtrlr,
                                             std::optional<Variable>({"SupportsFeature", "EvseSleep", std::nullopt}),
                                             std::nullopt};

const ComponentVariable TariffCostCtrlrAvailableTariff = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({"Available", "Tariff"}),
};
const ComponentVariable TariffCostCtrlrAvailableCost = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({"Available", "Cost"}),
};
const RequiredComponentVariable TariffCostCtrlrCurrency = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({
        "Currency",
    }),
};
const ComponentVariable TariffCostCtrlrEnabledTariff = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({"Enabled", "Tariff"}),
};
const ComponentVariable TariffCostCtrlrEnabledCost = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({"Enabled", "Cost"}),
};
const RequiredComponentVariable TariffFallbackMessage = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({
        "TariffFallbackMessage",
    }),
};
const ComponentVariable OfflineTariffFallbackMessage = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({
        "OfflineTariffFallbackMessage",
    }),
};
const RequiredComponentVariable TotalCostFallbackMessage = {
    ControllerComponents::TariffCostCtrlr,
    std::optional<Variable>({
        "TotalCostFallbackMessage",
    }),
};

const ComponentVariable NumberOfDecimalsForCostValues = {ControllerComponents::TariffCostCtrlr,
                                                         std::optional<Variable>({"NumberOfDecimalsForCostValues"})};

const RequiredComponentVariable EVConnectionTimeOut = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "EVConnectionTimeOut",
    }),
};
const ComponentVariable MaxEnergyOnInvalidId = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "MaxEnergyOnInvalidId",
    }),
};
const RequiredComponentVariable StopTxOnEVSideDisconnect = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "StopTxOnEVSideDisconnect",
    }),
};
const RequiredComponentVariable StopTxOnInvalidId = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "StopTxOnInvalidId",
    }),
};
const ComponentVariable TxBeforeAcceptedEnabled = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "TxBeforeAcceptedEnabled",
    }),
};
const RequiredComponentVariable TxStartPoint = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "TxStartPoint",
    }),
};
const RequiredComponentVariable TxStopPoint = {
    ControllerComponents::TxCtrlr,
    std::optional<Variable>({
        "TxStopPoint",
    }),
};

const ComponentVariable ISO15118CtrlrAvailable = {
    ControllerComponents::ISO15118Ctrlr,
    std::optional<Variable>({
        "Available",
    }),
};

// OCPP1.6 Mavericks - Configuration keys without direct OCPP 2.x equivalents
const ComponentVariable BlinkRepeat = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "BlinkRepeat",
    }),
};
const ComponentVariable ConnectorPhaseRotation = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "ConnectorPhaseRotation",
    }),
};
const ComponentVariable ConnectorPhaseRotationMaxLength = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "ConnectorPhaseRotationMaxLength",
    }),
};
const RequiredComponentVariable GetConfigurationMaxKeys = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "GetConfigurationMaxKeys",
    }),
};
const ComponentVariable LightIntensity = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "LightIntensity",
    }),
};
const ComponentVariable MinimumStatusDuration = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "MinimumStatusDuration",
    }),
};
const ComponentVariable StopTransactionOnEVSideDisconnect = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "StopTransactionOnEVSideDisconnect",
    }),
};
const RequiredComponentVariable SupportedFeatureProfiles = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "SupportedFeatureProfiles",
    }),
};
const ComponentVariable SupportedFeatureProfilesMaxLength = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "SupportedFeatureProfilesMaxLength",
    }),
};
const RequiredComponentVariable UnlockConnectorOnEVSideDisconnect = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "UnlockConnectorOnEVSideDisconnect",
    }),
};
const ComponentVariable ReserveConnectorZeroSupported = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "ReserveConnectorZeroSupported",
    }),
};
const ComponentVariable HostName = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "HostName",
    }),
};
const ComponentVariable AllowChargingProfileWithoutStartSchedule = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "AllowChargingProfileWithoutStartSchedule",
    }),
};
const ComponentVariable WaitForStopTransactionsOnResetTimeout = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "WaitForStopTransactionsOnResetTimeout",
    }),
};
const ComponentVariable StopTransactionIfUnlockNotSupported = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "StopTransactionIfUnlockNotSupported",
    }),
};
const ComponentVariable MeterPublicKeys = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "MeterPublicKeys",
    }),
};
const ComponentVariable DisableSecurityEventNotifications = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "DisableSecurityEventNotifications",
    }),
};
const ComponentVariable ISO15118CertificateManagementEnabled = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "ISO15118CertificateManagementEnabled",
    }),
};
const ComponentVariable CustomDisplayCostAndPrice = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "CustomDisplayCostAndPrice",
    }),
};
const ComponentVariable DefaultPrice = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "DefaultPrice",
    }),
};
const ComponentVariable DefaultPriceText = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "DefaultPriceText",
    }),
};
const ComponentVariable CustomIdleFeeAfterStop = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "CustomIdleFeeAfterStop",
    }),
};
const ComponentVariable SupportedLanguages = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "SupportedLanguages",
    }),
};
const ComponentVariable CustomMultiLanguageMessages = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "CustomMultiLanguageMessages",
    }),
};
const ComponentVariable Language = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "Language",
    }),
};
const ComponentVariable WaitForSetUserPriceTimeout = {
    ControllerComponents::OCPP16LegacyCtrlr,
    std::optional<Variable>({
        "WaitForSetUserPriceTimeout",
    }),
};
const ComponentVariable AuthorizationKey16 = {
    ControllerComponents::CustomLegacyController,
    std::optional<Variable>({
        "AuthorizationKey",
    }),
};
const RequiredComponentVariable CentralSystemURI16 = {
    ControllerComponents::CustomLegacyController,
    std::optional<Variable>({
        "CentralSystemURI",
    }),
};
const RequiredComponentVariable SecurityProfile16 = {
    ControllerComponents::CustomLegacyController,
    std::optional<Variable>({
        "SecurityProfile",
    }),
};
} // namespace ControllerComponentVariables

namespace EvseComponentVariables {

const Variable Available = {"Available"};
const Variable AvailabilityState = {"AvailabilityState"};
const Variable SupplyPhases = {"SupplyPhases"};
const Variable AllowReset = {"AllowReset"};
const Variable Power = {"Power"};
const Variable DCInputPhaseControl = {"DCInputPhaseControl", std::nullopt, std::nullopt};
const Variable ISO15118EvseId = {"ISO15118EvseId"};

ComponentVariable get_component_variable(const std::int32_t evse_id, const Variable& variable) {
    EVSE evse = {evse_id};
    const Component component = {"EVSE", evse};
    ComponentVariable component_variable;
    component_variable.component = component;
    component_variable.variable = variable;
    return component_variable;
}
} // namespace EvseComponentVariables

namespace ConnectorComponentVariables {

const Variable Available = {"Available"};
const Variable AvailabilityState = {"AvailabilityState"};
const Variable Type = {"ConnectorType"};
const Variable SupplyPhases = {"SupplyPhases"};

ComponentVariable get_component_variable(const std::int32_t evse_id, const std::int32_t connector_id,
                                         const Variable& variable) {
    EVSE evse = {evse_id, connector_id};
    const Component component = {"Connector", evse};
    ComponentVariable component_variable;
    component_variable.component = component;
    component_variable.variable = variable;
    return component_variable;
}
} // namespace ConnectorComponentVariables

namespace V2xComponentVariables {

const Variable Available = {"Available"};
const Variable Enabled = {"Enabled"};
const Variable SupportedEnergyTransferModes = {"SupportedEnergyTransferModes"};
const Variable SupportedOperationModes = {"SupportedOperationModes"};
const Variable TxStartedMeasurands = {"TxStartedMeasurands"};
const Variable TxEndedMeasurands = {"TxEndedMeasurands"};
const Variable TxEndedInterval = {"TxEndedInterval"};
const Variable TxUpdatedMeasurands = {"TxUpdatedMeasurands"};
const Variable TxUpdatedInterval = {"TxUpdatedInterval"};

Variable get_v2x_tx_started_measurands(const OperationModeEnum& mode) {
    Variable cv = V2xComponentVariables::TxStartedMeasurands;
    cv.instance = conversions::operation_mode_enum_to_string(mode);
    return cv;
}

Variable get_v2x_tx_ended_measurands(const OperationModeEnum& mode) {
    Variable cv = V2xComponentVariables::TxEndedMeasurands;
    cv.instance = conversions::operation_mode_enum_to_string(mode);
    return cv;
}

Variable get_v2x_tx_ended_interval(const OperationModeEnum& mode) {
    Variable cv = V2xComponentVariables::TxEndedInterval;
    cv.instance = conversions::operation_mode_enum_to_string(mode);
    return cv;
}

Variable get_v2x_tx_updated_measurands(const OperationModeEnum& mode) {
    Variable cv = V2xComponentVariables::TxUpdatedMeasurands;
    cv.instance = conversions::operation_mode_enum_to_string(mode);
    return cv;
}

Variable get_v2x_tx_updated_interval(const OperationModeEnum& mode) {
    Variable cv = V2xComponentVariables::TxUpdatedInterval;
    cv.instance = conversions::operation_mode_enum_to_string(mode);
    return cv;
}

ComponentVariable get_component_variable(const std::int32_t evse_id, const Variable& variable) {
    EVSE evse = {evse_id};
    const Component component = {"V2XChargingCtrlr", evse};
    ComponentVariable component_variable;
    component_variable.component = component;
    component_variable.variable = variable;
    return component_variable;
}
} // namespace V2xComponentVariables

namespace ISO15118ComponentVariables {

const Variable Available = {"Available"};
const Variable Enabled = {"Enabled"};
const Variable ServiceRenegotiationSupport = {"ServiceRenegotiationSupport"};
const Variable ProtocolSupported = {"ProtocolSupported"};
const Variable SeccId = {"SeccId"};
const Variable CountryName = {"CountryName"};
const Variable OrganizationName = {"OrganizationName"};

ComponentVariable get_component_variable(const std::int32_t evse_id, const Variable& variable) {
    EVSE evse = {evse_id};
    const Component component = {"ISO15118Ctrlr", evse};
    ComponentVariable component_variable;
    component_variable.component = component;
    component_variable.variable = variable;
    return component_variable;
}
} // namespace ISO15118ComponentVariables

namespace ConnectedEvComponentVariables {

const Variable Available = {"Available"};
const Variable VehicleId = {"VehicleId"};
const Variable ProtocolAgreed = {"ProtocolAgreed"};
const Variable VehicleCertificateLeaf = {"VehicleCertificate", "Leaf"};
const Variable VehicleCertificateSubCa1 = {"VehicleCertificate", "SubCA1"};
const Variable VehicleCertificateSubCa2 = {"VehicleCertificate", "SubCA2"};
const Variable VehicleCertificateRoot = {"VehicleCertificate", "Root"};

Variable get_protocol_supported_by_ev(const std::int32_t priority) {
    Variable protocol_supported_by_ev = {"ProtocolSupportedByEV", std::to_string(priority)};
    return protocol_supported_by_ev;
}

ComponentVariable get_component_variable(const std::int32_t evse_id, const Variable& variable) {
    EVSE evse = {evse_id};
    const Component component = {"ConnectedEV", evse};
    ComponentVariable component_variable;
    component_variable.component = component;
    component_variable.variable = variable;
    return component_variable;
}

} // namespace ConnectedEvComponentVariables

const std::vector<std::pair<ComponentVariable, std::vector<RequiredComponentVariable>>>
    required_component_available_variables{
        {ControllerComponentVariables::LocalAuthListCtrlrAvailable,
         {ControllerComponentVariables::LocalAuthListCtrlrEntries,
          ControllerComponentVariables::ItemsPerMessageSendLocalList,
          ControllerComponentVariables::BytesPerMessageSendLocalList}},
        {ControllerComponentVariables::SmartChargingCtrlrAvailable,
         {ControllerComponentVariables::ChargingProfileMaxStackLevel,
          ControllerComponentVariables::ChargingScheduleChargingRateUnit,
          ControllerComponentVariables::PeriodsPerSchedule, ControllerComponentVariables::EntriesChargingProfiles,
          ControllerComponentVariables::LimitChangeSignificance,
          ControllerComponentVariables::CompositeScheduleDefaultLimitAmps,
          ControllerComponentVariables::CompositeScheduleDefaultLimitWatts,
          ControllerComponentVariables::CompositeScheduleDefaultNumberPhases,
          ControllerComponentVariables::SupplyVoltage}},
        {ControllerComponentVariables::TariffCostCtrlrAvailableTariff,
         {ControllerComponentVariables::TariffFallbackMessage}},
        {ControllerComponentVariables::TariffCostCtrlrAvailableCost,
         {ControllerComponentVariables::TotalCostFallbackMessage,
          ControllerComponentVariables::TariffCostCtrlrCurrency}},
        {ControllerComponentVariables::MonitoringCtrlrAvailable,
         {ControllerComponentVariables::ItemsPerMessageSetVariableMonitoring,
          ControllerComponentVariables::BytesPerMessageSetVariableMonitoring}},
        {ControllerComponentVariables::DisplayMessageCtrlrAvailable,
         {ControllerComponentVariables::NumberOfDisplayMessages,
          ControllerComponentVariables::DisplayMessageSupportedFormats,
          ControllerComponentVariables::DisplayMessageSupportedPriorities}}};

const std::vector<RequiredComponentVariable> required_variables{
    ControllerComponentVariables::AlignedDataInterval,
    ControllerComponentVariables::AlignedDataMeasurands,
    ControllerComponentVariables::AlignedDataTxEndedInterval,
    ControllerComponentVariables::AlignedDataTxEndedMeasurands,
    ControllerComponentVariables::SampledDataTxEndedMeasurands,
    ControllerComponentVariables::SampledDataTxEndedInterval,
    ControllerComponentVariables::SampledDataTxStartedMeasurands,
    ControllerComponentVariables::SampledDataTxUpdatedMeasurands,
    ControllerComponentVariables::SampledDataTxUpdatedInterval,
    ControllerComponentVariables::ChargePointId,
    ControllerComponentVariables::NetworkConnectionProfiles,
    ControllerComponentVariables::ChargeBoxSerialNumber,
    ControllerComponentVariables::ChargePointModel,
    ControllerComponentVariables::ChargePointVendor,
    ControllerComponentVariables::FirmwareVersion,
    ControllerComponentVariables::SupportedCiphers12,
    ControllerComponentVariables::SupportedCiphers13,
    ControllerComponentVariables::LogMessagesFormat,
    ControllerComponentVariables::NumberOfConnectors,
    ControllerComponentVariables::SupportedOcppVersions,
    ControllerComponentVariables::AuthorizeRemoteStart,
    ControllerComponentVariables::LocalAuthorizeOffline,
    ControllerComponentVariables::LocalPreAuthorize,
    ControllerComponentVariables::ChargingStationAvailabilityState,
    ControllerComponentVariables::ChargingStationAvailable,
    ControllerComponentVariables::ChargingStationSupplyPhases,
    ControllerComponentVariables::ClockCtrlrDateTime,
    ControllerComponentVariables::TimeSource,
    ControllerComponentVariables::BytesPerMessageGetReport,
    ControllerComponentVariables::BytesPerMessageGetVariables,
    ControllerComponentVariables::BytesPerMessageSetVariables,
    ControllerComponentVariables::ItemsPerMessageGetReport,
    ControllerComponentVariables::ItemsPerMessageGetVariables,
    ControllerComponentVariables::ItemsPerMessageSetVariables,
    ControllerComponentVariables::ContractValidationOffline,
    ControllerComponentVariables::FileTransferProtocols,
    ControllerComponentVariables::MessageTimeout,
    ControllerComponentVariables::MessageAttemptInterval,
    ControllerComponentVariables::MessageAttempts,
    ControllerComponentVariables::NetworkConfigurationPriority,
    ControllerComponentVariables::NetworkProfileConnectionAttempts,
    ControllerComponentVariables::OfflineThreshold,
    ControllerComponentVariables::ResetRetries,
    ControllerComponentVariables::RetryBackOffRandomRange,
    ControllerComponentVariables::RetryBackOffRepeatTimes,
    ControllerComponentVariables::RetryBackOffWaitMinimum,
    ControllerComponentVariables::UnlockOnEVSideDisconnect,
    ControllerComponentVariables::WebSocketPingInterval,
    ControllerComponentVariables::CertificateEntries,
    ControllerComponentVariables::SecurityCtrlrIdentity,
    ControllerComponentVariables::OrganizationName,
    ControllerComponentVariables::SecurityProfile,
    ControllerComponentVariables::EVConnectionTimeOut,
    ControllerComponentVariables::StopTxOnEVSideDisconnect,
    ControllerComponentVariables::StopTxOnInvalidId,
    ControllerComponentVariables::TxStartPoint,
    ControllerComponentVariables::TxStopPoint,
    ControllerComponentVariables::TxStartPoint,
    ControllerComponentVariables::TxStopPoint};

// Note: Power is also required, but the value is not required but the maxLimit. So that is why it is not added here.
const std::vector<Variable> required_evse_variables{
    EvseComponentVariables::Available, EvseComponentVariables::AvailabilityState, EvseComponentVariables::SupplyPhases};

const std::vector<Variable> required_connector_variables{
    ConnectorComponentVariables::Available, ConnectorComponentVariables::AvailabilityState,
    ConnectorComponentVariables::SupplyPhases, ConnectorComponentVariables::Type};

const std::vector<Variable> required_v2x_variables{V2xComponentVariables::Available, V2xComponentVariables::Enabled,
                                                   V2xComponentVariables::SupportedEnergyTransferModes,
                                                   V2xComponentVariables::SupportedOperationModes};

namespace NetworkConfigurationComponentVariables {

const Variable OcppCsmsUrl = {"OcppCsmsUrl"};
const Variable SecurityProfile = {"SecurityProfile"};
const Variable OcppInterface = {"OcppInterface"};
const Variable OcppTransport = {"OcppTransport"};
const Variable MessageTimeout = {"MessageTimeout"};
const Variable Identity = {"Identity"};
const Variable BasicAuthPassword = {"BasicAuthPassword"};
const Variable ApnEnabled = {"ApnEnabled"};
const Variable VpnEnabled = {"VpnEnabled"};
const Variable Apn = {"Apn"};
const Variable ApnUserName = {"ApnUserName"};
const Variable ApnPassword = {"ApnPassword"};
const Variable SimPin = {"SimPin"};
const Variable PreferredNetwork = {"PreferredNetwork"};
const Variable UseOnlyPreferredNetwork = {"UseOnlyPreferredNetwork"};
const Variable ApnAuthentication = {"ApnAuthentication"};
const Variable VpnServer = {"VpnServer"};
const Variable VpnUser = {"VpnUser"};
const Variable VpnPassword = {"VpnPassword"};
const Variable VpnKey = {"VpnKey"};
const Variable VpnType = {"VpnType"};
const Variable VpnGroup = {"VpnGroup"};
const Variable OcppVersion = {"OcppVersion"};
const Variable CsmsRootCertificateHashAlgorithm = {"CsmsRootCertificateHashAlgorithm"};
const Variable CsmsRootCertificateIssuerKeyHash = {"CsmsRootCertificateIssuerKeyHash"};
const Variable CsmsRootCertificateIssuerNameHash = {"CsmsRootCertificateIssuerNameHash"};
const Variable CsmsRootCertificateSerialNumber = {"CsmsRootCertificateSerialNumber"};

ComponentVariable get_component_variable(const std::int32_t slot, const Variable& variable) {
    Component component;
    component.name = "NetworkConfiguration";
    component.instance = std::to_string(slot);
    ComponentVariable component_variable;
    component_variable.component = component;
    component_variable.variable = variable;
    return component_variable;
}

std::optional<NetworkConnectionProfile> read_profile_from_device_model(DeviceModelInterface& dm, int32_t slot) {
    try {
        NetworkConnectionProfile profile;

        const auto url_cv = get_component_variable(slot, OcppCsmsUrl);
        const auto url_opt = dm.get_optional_value<std::string>(url_cv);
        if (!url_opt.has_value() || url_opt.value().empty()) {
            return std::nullopt;
        }
        profile.ocppCsmsUrl = url_opt.value();

        const auto sec_cv = get_component_variable(slot, SecurityProfile);
        const auto sec_opt = dm.get_optional_value<int>(sec_cv);
        if (!sec_opt.has_value()) {
            return std::nullopt;
        }
        profile.securityProfile = sec_opt.value();

        const auto iface_cv = get_component_variable(slot, OcppInterface);
        const auto iface_opt = dm.get_optional_value<std::string>(iface_cv);
        if (!iface_opt.has_value()) {
            return std::nullopt;
        }
        profile.ocppInterface = conversions::string_to_ocppinterface_enum(iface_opt.value());

        const auto trans_cv = get_component_variable(slot, OcppTransport);
        const auto trans_opt = dm.get_optional_value<std::string>(trans_cv);
        if (!trans_opt.has_value()) {
            return std::nullopt;
        }
        profile.ocppTransport = conversions::string_to_ocpptransport_enum(trans_opt.value());

        const auto timeout_cv = get_component_variable(slot, MessageTimeout);
        const auto timeout_opt = dm.get_optional_value<int>(timeout_cv);
        if (!timeout_opt.has_value()) {
            return std::nullopt;
        }
        profile.messageTimeout = timeout_opt.value();

        const auto identity_cv = get_component_variable(slot, Identity);
        if (const auto identity_opt = dm.get_optional_value<std::string>(identity_cv);
            identity_opt.has_value() && !identity_opt.value().empty()) {
            profile.identity = identity_opt.value();
        }

        const auto pwd_cv = get_component_variable(slot, BasicAuthPassword);
        if (const auto pwd_opt = dm.get_optional_value<std::string>(pwd_cv);
            pwd_opt.has_value() && !pwd_opt.value().empty()) {
            profile.basicAuthPassword = pwd_opt.value();
        }

        const auto apn_enabled_cv = get_component_variable(slot, ApnEnabled);
        const auto apn_enabled = dm.get_optional_value<bool>(apn_enabled_cv).value_or(false);
        if (apn_enabled) {
            APN apn;
            const auto apn_cv = get_component_variable(slot, Apn);
            if (const auto apn_opt = dm.get_optional_value<std::string>(apn_cv)) {
                apn.apn = apn_opt.value();
            } else {
                EVLOG_warning << "APN enabled but APN value not set for slot " << slot;
                return std::nullopt;
            }
            const auto auth_cv = get_component_variable(slot, ApnAuthentication);
            if (const auto auth_opt = dm.get_optional_value<std::string>(auth_cv)) {
                apn.apnAuthentication = conversions::string_to_apnauthentication_enum(auth_opt.value());
            } else {
                apn.apnAuthentication = APNAuthenticationEnum::AUTO;
            }
            if (const auto user_opt = dm.get_optional_value<std::string>(get_component_variable(slot, ApnUserName))) {
                apn.apnUserName = user_opt.value();
            }
            if (const auto pwd_opt = dm.get_optional_value<std::string>(get_component_variable(slot, ApnPassword))) {
                apn.apnPassword = pwd_opt.value();
            }
            if (const auto pin_opt = dm.get_optional_value<int>(get_component_variable(slot, SimPin))) {
                apn.simPin = pin_opt.value();
            }
            if (const auto net_opt =
                    dm.get_optional_value<std::string>(get_component_variable(slot, PreferredNetwork))) {
                apn.preferredNetwork = net_opt.value();
            }
            if (const auto only_opt =
                    dm.get_optional_value<bool>(get_component_variable(slot, UseOnlyPreferredNetwork))) {
                apn.useOnlyPreferredNetwork = only_opt.value();
            }
            profile.apn = apn;
        }

        const auto vpn_enabled_cv = get_component_variable(slot, VpnEnabled);
        const auto vpn_enabled = dm.get_optional_value<bool>(vpn_enabled_cv).value_or(false);
        if (vpn_enabled) {
            VPN vpn;
            if (const auto server_opt = dm.get_optional_value<std::string>(get_component_variable(slot, VpnServer))) {
                vpn.server = server_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN server value not set for slot " << slot;
                return std::nullopt;
            }
            if (const auto user_opt = dm.get_optional_value<std::string>(get_component_variable(slot, VpnUser))) {
                vpn.user = user_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN user value not set for slot " << slot;
                return std::nullopt;
            }
            if (const auto pwd_opt = dm.get_optional_value<std::string>(get_component_variable(slot, VpnPassword))) {
                vpn.password = pwd_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN password value not set for slot " << slot;
                return std::nullopt;
            }
            if (const auto key_opt = dm.get_optional_value<std::string>(get_component_variable(slot, VpnKey))) {
                vpn.key = key_opt.value();
            } else {
                EVLOG_warning << "VPN enabled but VPN key value not set for slot " << slot;
                return std::nullopt;
            }
            if (const auto type_opt = dm.get_optional_value<std::string>(get_component_variable(slot, VpnType))) {
                vpn.type = conversions::string_to_vpnenum(type_opt.value());
            } else {
                EVLOG_warning << "VPN enabled but VPN type value not set for slot " << slot;
                return std::nullopt;
            }
            if (const auto group_opt = dm.get_optional_value<std::string>(get_component_variable(slot, VpnGroup))) {
                vpn.group = group_opt.value();
            }
            profile.vpn = vpn;
        }

        return profile;
    } catch (const std::exception& e) {
        EVLOG_error << "Error reading profile from device model for slot " << slot << ": " << e.what();
        return std::nullopt;
    }
}

bool write_profile_to_device_model(DeviceModelInterface& dm, int32_t slot, const NetworkConnectionProfile& profile,
                                   const std::string& source) {
    try {
        auto set = [&](const Variable& var, const std::string& val) {
            const auto cv = get_component_variable(slot, var);
            return dm.set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, val, source);
        };

        if (set(OcppCsmsUrl, profile.ocppCsmsUrl.get()) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set OcppCsmsUrl for slot " << slot;
            return false;
        }
        if (set(SecurityProfile, std::to_string(profile.securityProfile)) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set SecurityProfile for slot " << slot;
            return false;
        }
        if (set(OcppInterface, conversions::ocppinterface_enum_to_string(profile.ocppInterface)) !=
            SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set OcppInterface for slot " << slot;
            return false;
        }
        if (set(OcppTransport, conversions::ocpptransport_enum_to_string(profile.ocppTransport)) !=
            SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set OcppTransport for slot " << slot;
            return false;
        }
        if (set(MessageTimeout, std::to_string(profile.messageTimeout)) != SetVariableStatusEnum::Accepted) {
            EVLOG_error << "Failed to set MessageTimeout for slot " << slot;
            return false;
        }

        if (set(Identity, profile.identity.has_value() ? profile.identity.value().get() : "") !=
            SetVariableStatusEnum::Accepted) {
            EVLOG_warning << "Failed to set Identity for slot " << slot;
        }
        if (set(BasicAuthPassword, profile.basicAuthPassword.has_value() ? profile.basicAuthPassword.value().get()
                                                                         : "") != SetVariableStatusEnum::Accepted) {
            EVLOG_warning << "Failed to set BasicAuthPassword for slot " << slot;
        }

        if (profile.apn.has_value()) {
            const auto& apn = profile.apn.value();
            set(ApnEnabled, "true");
            set(Apn, apn.apn.get());
            set(ApnAuthentication, conversions::apnauthentication_enum_to_string(apn.apnAuthentication));
            set(ApnUserName, apn.apnUserName.has_value() ? apn.apnUserName.value().get() : "");
            set(ApnPassword, apn.apnPassword.has_value() ? apn.apnPassword.value().get() : "");
            set(SimPin, apn.simPin.has_value() ? std::to_string(apn.simPin.value()) : "");
            set(PreferredNetwork, apn.preferredNetwork.has_value() ? apn.preferredNetwork.value().get() : "");
            set(UseOnlyPreferredNetwork, apn.useOnlyPreferredNetwork.has_value()
                                             ? (apn.useOnlyPreferredNetwork.value() ? "true" : "false")
                                             : "");
        } else {
            set(ApnEnabled, "false");
        }

        if (profile.vpn.has_value()) {
            const auto& vpn = profile.vpn.value();
            set(VpnEnabled, "true");
            set(VpnServer, vpn.server.get());
            set(VpnUser, vpn.user.get());
            set(VpnPassword, vpn.password.get());
            set(VpnKey, vpn.key.get());
            set(VpnType, conversions::vpnenum_to_string(vpn.type));
            set(VpnGroup, vpn.group.has_value() ? vpn.group.value().get() : "");
        } else {
            set(VpnEnabled, "false");
        }

        return true;
    } catch (const std::exception& e) {
        EVLOG_error << "Error writing profile to device model for slot " << slot << ": " << e.what();
        return false;
    }
}

void migrate_from_blob_if_needed(DeviceModelInterface& dm) {
    try {
        const auto blob_opt =
            dm.get_optional_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles);
        if (!blob_opt.has_value() || blob_opt.value().empty()) {
            EVLOG_debug << "NetworkConnectionProfiles blob is empty, no migration needed";
            return;
        }
        const auto profiles = json::parse(blob_opt.value());
        if (profiles.empty()) {
            dm.set_value(ControllerComponentVariables::NetworkConnectionProfiles.component,
                         ControllerComponentVariables::NetworkConnectionProfiles.variable.value(),
                         AttributeEnum::Actual, "", "internal");
            return;
        }

        int imported = 0;
        for (const auto& profile_json : profiles) {
            const int slot = profile_json.at("configurationSlot").get<int>();
            const NetworkConnectionProfile profile = profile_json.at("connectionData");
            if (write_profile_to_device_model(dm, slot, profile, "internal")) {
                ++imported;
            } else {
                EVLOG_warning << "Failed to import NetworkConfiguration[" << slot << "] from blob";
            }
        }

        // Clear the blob so this migration does not run again on the next boot
        dm.set_value(ControllerComponentVariables::NetworkConnectionProfiles.component,
                     ControllerComponentVariables::NetworkConnectionProfiles.variable.value(), AttributeEnum::Actual,
                     "", "internal");

        EVLOG_info << "Imported " << imported
                   << " profile(s) from NetworkConnectionProfiles blob into NetworkConfiguration DM components";
    } catch (const std::exception& e) {
        EVLOG_error << "Error importing from NetworkConnectionProfiles blob: " << e.what();
    }
}

void clear_slot_in_device_model(DeviceModelInterface& dm, int32_t slot) {
    static const std::vector<const Variable*> all_vars = {
        &OcppCsmsUrl,       &SecurityProfile,   &OcppInterface, &OcppTransport,    &MessageTimeout,
        &Identity,          &BasicAuthPassword, &ApnEnabled,    &VpnEnabled,       &Apn,
        &ApnUserName,       &ApnPassword,       &SimPin,        &PreferredNetwork, &UseOnlyPreferredNetwork,
        &ApnAuthentication, &VpnServer,         &VpnUser,       &VpnPassword,      &VpnKey,
        &VpnType,           &VpnGroup,
    };
    for (const auto* var : all_vars) {
        const auto cv = get_component_variable(slot, *var);
        dm.set_value(cv.component, cv.variable.value(), AttributeEnum::Actual, "", "internal");
    }
    EVLOG_debug << "Cleared NetworkConfiguration[" << slot << "] from device model";
}
} // namespace NetworkConfigurationComponentVariables

} // namespace v2
} // namespace ocpp
