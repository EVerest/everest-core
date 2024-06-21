// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 -  Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V201_CTRLR_COMPONENT_VARIABLES
#define OCPP_V201_CTRLR_COMPONENT_VARIABLES

#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

namespace ControllerComponents {
extern const Component& InternalCtrlr;
extern const Component& AlignedDataCtrlr;
extern const Component& AuthCacheCtrlr;
extern const Component& AuthCtrlr;
extern const Component& ChargingStation;
extern const Component& ClockCtrlr;
extern const Component& Connector;
extern const Component& CustomizationCtrlr;
extern const Component& DeviceDataCtrlr;
extern const Component& DisplayMessageCtrlr;
extern const Component& EVSE;
extern const Component& ISO15118Ctrlr;
extern const Component& LocalAuthListCtrlr;
extern const Component& MonitoringCtrlr;
extern const Component& OCPPCommCtrlr;
extern const Component& ReservationCtrlr;
extern const Component& SampledDataCtrlr;
extern const Component& SecurityCtrlr;
extern const Component& SmartChargingCtrlr;
extern const Component& TariffCostCtrlr;
extern const Component& TxCtrlr;
} // namespace ControllerComponents

// Provides access to standardized variables of OCPP2.0.1 spec
namespace ControllerComponentVariables {
extern const ComponentVariable& InternalCtrlrEnabled;
extern const RequiredComponentVariable& ChargePointId;
extern const RequiredComponentVariable& NetworkConnectionProfiles;
extern const RequiredComponentVariable& ChargeBoxSerialNumber;
extern const RequiredComponentVariable& ChargePointModel;
extern const ComponentVariable& ChargePointSerialNumber;
extern const RequiredComponentVariable& ChargePointVendor;
extern const RequiredComponentVariable& FirmwareVersion;
extern const ComponentVariable& ICCID;
extern const ComponentVariable& IMSI;
extern const ComponentVariable& MeterSerialNumber;
extern const ComponentVariable& MeterType;
extern const RequiredComponentVariable& SupportedCiphers12;
extern const RequiredComponentVariable& SupportedCiphers13;
extern const ComponentVariable& AuthorizeConnectorZeroOnConnectorOne;
extern const ComponentVariable& LogMessages;
extern const RequiredComponentVariable& LogMessagesFormat;
extern const ComponentVariable& SupportedChargingProfilePurposeTypes;
extern const ComponentVariable& SupportedCriteria;
extern const ComponentVariable& RoundClockAlignedTimestamps;
extern const ComponentVariable& MaxCompositeScheduleDuration;
extern const RequiredComponentVariable& NumberOfConnectors;
extern const ComponentVariable& UseSslDefaultVerifyPaths;
extern const ComponentVariable& VerifyCsmsCommonName;
extern const ComponentVariable& UseTPM;
extern const ComponentVariable& VerifyCsmsAllowWildcards;
extern const ComponentVariable& IFace;
extern const ComponentVariable& OcspRequestInterval;
extern const ComponentVariable& WebsocketPingPayload;
extern const ComponentVariable& WebsocketPongTimeout;
extern const ComponentVariable& MaxCustomerInformationDataLength;
extern const ComponentVariable& V2GCertificateExpireCheckInitialDelaySeconds;
extern const ComponentVariable& V2GCertificateExpireCheckIntervalSeconds;
extern const ComponentVariable& ClientCertificateExpireCheckInitialDelaySeconds;
extern const ComponentVariable& ClientCertificateExpireCheckIntervalSeconds;
extern const ComponentVariable& MessageQueueSizeThreshold;
extern const ComponentVariable& MaxMessageSize;
extern const ComponentVariable& AlignedDataCtrlrEnabled;
extern const ComponentVariable& AlignedDataCtrlrAvailable;
extern const RequiredComponentVariable& AlignedDataInterval;
extern const RequiredComponentVariable& AlignedDataMeasurands;
extern const ComponentVariable& AlignedDataSendDuringIdle;
extern const ComponentVariable& AlignedDataSignReadings;
extern const RequiredComponentVariable& AlignedDataTxEndedInterval;
extern const RequiredComponentVariable& AlignedDataTxEndedMeasurands;
extern const ComponentVariable& AuthCacheCtrlrAvailable;
extern const ComponentVariable& AuthCacheCtrlrEnabled;
extern const ComponentVariable& AuthCacheDisablePostAuthorize;
extern const ComponentVariable& AuthCacheLifeTime;
extern const ComponentVariable& AuthCachePolicy;
extern const ComponentVariable& AuthCacheStorage;
extern const ComponentVariable& AuthCtrlrEnabled;
extern const ComponentVariable& AdditionalInfoItemsPerMessage;
extern const RequiredComponentVariable& AuthorizeRemoteStart;
extern const RequiredComponentVariable& LocalAuthorizeOffline;
extern const RequiredComponentVariable& LocalPreAuthorize;
extern const ComponentVariable& DisableRemoteAuthorization;
extern const ComponentVariable& MasterPassGroupId;
extern const ComponentVariable& OfflineTxForUnknownIdEnabled;
extern const ComponentVariable& AllowNewSessionsPendingFirmwareUpdate;
extern const RequiredComponentVariable& ChargingStationAvailabilityState;
extern const RequiredComponentVariable& ChargingStationAvailable;
extern const RequiredComponentVariable& ChargingStationSupplyPhases;
extern const RequiredComponentVariable& ClockCtrlrDateTime;
extern const ComponentVariable& NextTimeOffsetTransitionDateTime;
extern const ComponentVariable& NtpServerUri;
extern const ComponentVariable& NtpSource;
extern const ComponentVariable& TimeAdjustmentReportingThreshold;
extern const ComponentVariable& TimeOffset;
extern const ComponentVariable& TimeOffsetNextTransition;
extern const RequiredComponentVariable& TimeSource;
extern const ComponentVariable& TimeZone;
extern const ComponentVariable& CustomImplementationEnabled;
extern const RequiredComponentVariable& BytesPerMessageGetReport;
extern const RequiredComponentVariable& BytesPerMessageGetVariables;
extern const RequiredComponentVariable& BytesPerMessageSetVariables;
extern const ComponentVariable& ConfigurationValueSize;
extern const RequiredComponentVariable& ItemsPerMessageGetReport;
extern const RequiredComponentVariable& ItemsPerMessageGetVariables;
extern const RequiredComponentVariable& ItemsPerMessageSetVariables;
extern const ComponentVariable& ReportingValueSize;
extern const ComponentVariable& DisplayMessageCtrlrAvailable;
extern const RequiredComponentVariable& NumberOfDisplayMessages;
extern const RequiredComponentVariable& DisplayMessageSupportedFormats;
extern const RequiredComponentVariable& DisplayMessageSupportedPriorities;
extern const ComponentVariable& CentralContractValidationAllowed;
extern const RequiredComponentVariable& ContractValidationOffline;
extern const ComponentVariable& RequestMeteringReceipt;
extern const ComponentVariable& ISO15118CtrlrSeccId;
extern const ComponentVariable& ISO15118CtrlrCountryName;
extern const ComponentVariable& ISO15118CtrlrOrganizationName;
extern const ComponentVariable& PnCEnabled;
extern const ComponentVariable& V2GCertificateInstallationEnabled;
extern const ComponentVariable& ContractCertificateInstallationEnabled;
extern const ComponentVariable& LocalAuthListCtrlrAvailable;
extern const RequiredComponentVariable& BytesPerMessageSendLocalList;
extern const ComponentVariable& LocalAuthListCtrlrEnabled;
extern const RequiredComponentVariable& LocalAuthListCtrlrEntries;
extern const RequiredComponentVariable& ItemsPerMessageSendLocalList;
extern const ComponentVariable& LocalAuthListCtrlrStorage;
extern const ComponentVariable& MonitoringCtrlrAvailable;
extern const ComponentVariable& BytesPerMessageClearVariableMonitoring;
extern const RequiredComponentVariable& BytesPerMessageSetVariableMonitoring;
extern const ComponentVariable& MonitoringCtrlrEnabled;
extern const ComponentVariable& ActiveMonitoringBase;
extern const ComponentVariable& ActiveMonitoringLevel;
extern const ComponentVariable& ItemsPerMessageClearVariableMonitoring;
extern const RequiredComponentVariable& ItemsPerMessageSetVariableMonitoring;
extern const ComponentVariable& OfflineQueuingSeverity;
extern const ComponentVariable& ActiveNetworkProfile;
extern const RequiredComponentVariable& FileTransferProtocols;
extern const ComponentVariable& HeartbeatInterval;
extern const RequiredComponentVariable& MessageTimeout;
extern const RequiredComponentVariable& MessageAttemptInterval;
extern const RequiredComponentVariable& MessageAttempts;
extern const RequiredComponentVariable& NetworkConfigurationPriority;
extern const RequiredComponentVariable& NetworkProfileConnectionAttempts;
extern const RequiredComponentVariable& OfflineThreshold;
extern const ComponentVariable& QueueAllMessages;
extern const RequiredComponentVariable& ResetRetries;
extern const RequiredComponentVariable& RetryBackOffRandomRange;
extern const RequiredComponentVariable& RetryBackOffRepeatTimes;
extern const RequiredComponentVariable& RetryBackOffWaitMinimum;
extern const RequiredComponentVariable& UnlockOnEVSideDisconnect;
extern const RequiredComponentVariable& WebSocketPingInterval;
extern const ComponentVariable& ReservationCtrlrAvailable;
extern const ComponentVariable& ReservationCtrlrEnabled;
extern const ComponentVariable& ReservationCtrlrNonEvseSpecific;
extern const ComponentVariable& SampledDataCtrlrAvailable;
extern const ComponentVariable& SampledDataCtrlrEnabled;
extern const ComponentVariable& SampledDataSignReadings;
extern const RequiredComponentVariable& SampledDataTxEndedInterval;
extern const RequiredComponentVariable& SampledDataTxEndedMeasurands;
extern const RequiredComponentVariable& SampledDataTxStartedMeasurands;
extern const RequiredComponentVariable& SampledDataTxUpdatedInterval;
extern const RequiredComponentVariable& SampledDataTxUpdatedMeasurands;
extern const ComponentVariable& AdditionalRootCertificateCheck;
extern const ComponentVariable& BasicAuthPassword;
extern const RequiredComponentVariable& CertificateEntries;
extern const ComponentVariable& CertSigningRepeatTimes;
extern const ComponentVariable& CertSigningWaitMinimum;
extern const RequiredComponentVariable& SecurityCtrlrIdentity;
extern const ComponentVariable& MaxCertificateChainSize;
extern const ComponentVariable& UpdateCertificateSymlinks;
extern const RequiredComponentVariable& OrganizationName;
extern const RequiredComponentVariable& SecurityProfile;
extern const ComponentVariable& ACPhaseSwitchingSupported;
extern const ComponentVariable& SmartChargingCtrlrAvailable;
extern const ComponentVariable& SmartChargingCtrlrAvailableEnabled;
extern const RequiredComponentVariable& EntriesChargingProfiles;
extern const ComponentVariable& ExternalControlSignalsEnabled;
extern const RequiredComponentVariable& LimitChangeSignificance;
extern const ComponentVariable& NotifyChargingLimitWithSchedules;
extern const RequiredComponentVariable& PeriodsPerSchedule;
extern const ComponentVariable& Phases3to1;
extern const RequiredComponentVariable& ChargingProfileMaxStackLevel;
extern const RequiredComponentVariable& ChargingScheduleChargingRateUnit;
extern const ComponentVariable& TariffCostCtrlrAvailableTariff;
extern const ComponentVariable& TariffCostCtrlrAvailableCost;
extern const RequiredComponentVariable& TariffCostCtrlrCurrency;
extern const ComponentVariable& TariffCostCtrlrEnabledTariff;
extern const ComponentVariable& TariffCostCtrlrEnabledCost;
extern const RequiredComponentVariable& TariffFallbackMessage;
extern const RequiredComponentVariable& TotalCostFallbackMessage;
extern const RequiredComponentVariable& EVConnectionTimeOut;
extern const ComponentVariable& MaxEnergyOnInvalidId;
extern const RequiredComponentVariable& StopTxOnEVSideDisconnect;
extern const RequiredComponentVariable& StopTxOnInvalidId;
extern const ComponentVariable& TxBeforeAcceptedEnabled;
extern const RequiredComponentVariable& TxStartPoint;
extern const RequiredComponentVariable& TxStopPoint;
} // namespace ControllerComponentVariables

namespace EvseComponentVariables {
extern const Variable& Available;
extern const Variable& AvailabilityState;
extern const Variable& SupplyPhases;
extern const Variable& AllowReset;
extern const Variable& Power;
ComponentVariable get_component_variable(const int32_t evse_id, const Variable& variable);
} // namespace EvseComponentVariables

namespace ConnectorComponentVariables {
extern const Variable& Available;
extern const Variable& AvailabilityState;
extern const Variable& Type;
extern const Variable& SupplyPhases;
ComponentVariable get_component_variable(const int32_t evse_id, const int32_t connector_id, const Variable& variable);
} // namespace ConnectorComponentVariables

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_CTRLR_COMPONENT_VARIABLES
