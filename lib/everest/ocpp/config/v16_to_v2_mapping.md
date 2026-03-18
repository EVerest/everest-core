
# OCPP 1.6 to OCPP 2.x mapping

This documents how OCPP 1.6 configuration keys map to the OCPP 2.x Device Model.

It is intended as a **human-readable reference** for integrators and maintainers.
The implementation in code (for example `known_keys` conversion logic and patching behaviour) remains authoritative.

## Core Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 1 | `AllowOfflineTxForUnknownId` | VariableAttribute | `AuthCtrlr` | `OfflineTxForUnknownIdEnabled` | `Actual` |
| 2 | `AuthorizationCacheEnabled` | VariableAttribute | `AuthCacheCtrlr` | `Enabled` | `Actual` |
| 3 | `AuthorizeRemoteTxRequests` | VariableAttribute | `AuthCtrlr` | `AuthorizeRemoteStart` | `Actual` |
| 5 | `ClockAlignedDataInterval` | VariableAttribute | `AlignedDataCtrlr` | `Interval` | `Actual` |
| 6 | `ConnectionTimeOut` | VariableAttribute | `TxCtrlr` | `EVConnectionTimeout` | `Actual` |
| 7 | `HeartbeatInterval` | VariableAttribute | `OCPPCommCtrlr` | `HeartbeatInterval` | `Actual` |
| 8 | `LocalAuthorizeOffline` | VariableAttribute | `AuthCtrlr` | `LocalAuthorizeOffline` | `Actual` |
| 9 | `LocalPreAuthorize` | VariableAttribute | `AuthCtrlr` | `LocalPreAuthorize` | `Actual` |
| 10 | `MaxEnergyOnInvalidId` | VariableAttribute | `TxCtrlr` | `MaxEnergyOnInvalidId` | `Actual` |
| 11 | `MeterValuesAlignedData` | VariableAttribute | `AlignedDataCtrlr` | `Measurands` | `Actual` |
| 12 | `MeterValuesAlignedDataMaxLength` | VariableCharacteristics | `AlignedDataCtrlr` | `Measurands` | `maxLimit` |
| 13 | `MeterValuesSampledData` | VariableAttribute | `SampledDataCtrlr` | `TxUpdatedMeasurands` | `Actual` |
| 14 | `MeterValuesSampledDataMaxLength` | VariableCharacteristics | `SampledDataCtrlr` | `TxUpdatedMeasurands` | `maxLimit` |
| 15 | `MeterValueSampleInterval` | VariableAttribute | `SampledDataCtrlr` | `TxUpdatedInterval` | `Actual` |
| 16 | `ResetRetries` | VariableAttribute | `OCPPCommCtrlr` | `ResetRetries` | `Actual` |
| 17 | `StopTransactionOnInvalidId` | VariableAttribute | `TxCtrlr` | `StopTxOnInvalidId` | `Actual` |
| 18 | `StopTxnAlignedData` | VariableAttribute | `AlignedDataCtrlr` | `TxEndedMeasurands` | `Actual` |
| 19 | `StopTxnAlignedDataMaxLength` | VariableCharacteristics | `AlignedDataCtrlr` | `TxEndedMeasurands` | `maxLimit` |
| 20 | `StopTxnSampledData` | VariableAttribute | `SampledDataCtrlr` | `TxEndedMeasurands` | `Actual` |
| 21 | `StopTxnSampledDataMaxLength` | VariableCharacteristics | `SampledDataCtrlr` | `TxEndedMeasurands` | `maxLimit` |
| 22 | `TransactionMessageAttempts` | VariableAttribute | `OCPPCommCtrlr` | `MessageAttempts` | `Actual` |
| 23 | `TransactionMessageRetryInterval` | VariableAttribute | `OCPPCommCtrlr` | `MessageAttemptInterval` | `Actual` |
| 24 | `WebSocketPingInterval` | VariableAttribute | `OCPPCommCtrlr` | `WebSocketPingInterval` | `Actual` |
| 25 | `LocalAuthListEnabled` | VariableAttribute | `LocalAuthListCtrlr` | `Enabled` | `Actual` |
| 26 | `ChargeProfileMaxStackLevel` | VariableAttribute | `SmartChargingCtrlr` | `ProfileStackLevel` | `Actual` |
| 27 | `ChargingScheduleAllowedChargingRateUnit` | VariableAttribute | `SmartChargingCtrlr` | `RateUnit` | `Actual` |
| 28 | `ChargingScheduleMaxPeriods` | VariableAttribute | `SmartChargingCtrlr` | `PeriodsPerSchedule` | `Actual` |
| 29 | `ConnectorSwitch3to1PhaseSupported` | VariableAttribute | `SmartChargingCtrlr` | `Phases3to1` | `Actual` |
| 30 | `SupportedFileTransferProtocols` | VariableAttribute | `OCPPCommCtrlr` | `FileTransferProtocols` | `Actual` |

## Internal Keys

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 31 | `ChargePointId` | VariableAttribute | `InternalCtrlr` | `ChargePointId` | `Actual` |
| 32 | `ChargeBoxSerialNumber` | VariableAttribute | `InternalCtrlr` | `ChargeBoxSerialNumber` | `Actual` |
| 33 | `ChargePointModel` | VariableAttribute | `InternalCtrlr` | `ChargePointModel` | `Actual` |
| 34 | `ChargePointSerialNumber` | VariableAttribute | `InternalCtrlr` | `ChargePointSerialNumber` | `Actual` |
| 35 | `ChargePointVendor` | VariableAttribute | `InternalCtrlr` | `ChargePointVendor` | `Actual` |
| 36 | `FirmwareVersion` | VariableAttribute | `InternalCtrlr` | `FirmwareVersion` | `Actual` |
| 37 | `ICCID` | VariableAttribute | `InternalCtrlr` | `ICCID` | `Actual` |
| 38 | `IFace` | VariableAttribute | `InternalCtrlr` | `IFace` | `Actual` |
| 39 | `IMSI` | VariableAttribute | `InternalCtrlr` | `IMSI` | `Actual` |
| 40 | `MeterSerialNumber` | VariableAttribute | `InternalCtrlr` | `MeterSerialNumber` | `Actual` |
| 41 | `MeterType` | VariableAttribute | `InternalCtrlr` | `MeterType` | `Actual` |
| 42 | `SupportedCiphers12` | VariableAttribute | `InternalCtrlr` | `SupportedCiphers12` | `Actual` |
| 43 | `SupportedCiphers13` | VariableAttribute | `InternalCtrlr` | `SupportedCiphers13` | `Actual` |
| 44 | `UseTPM` | VariableAttribute | `InternalCtrlr` | `UseTPM` | `Actual` |
| 45 | `UseTPMSeccLeafCertificate` | VariableAttribute | `InternalCtrlr` | `UseTPMSeccLeafCertificate` | `Actual` |
| 46 | `RetryBackoffRandomRange` | VariableAttribute | `OCPPCommCtrlr` | `RetryBackOffRandomRange` | `Actual` |
| 47 | `RetryBackoffRepeatTimes` | VariableAttribute | `OCPPCommCtrlr` | `RetryBackOffRepeatTimes` | `Actual` |
| 48 | `AuthorizeConnectorZeroOnConnectorOne` | VariableAttribute | `InternalCtrlr` | `AuthorizeConnectorZeroOnConnectorOne` | `Actual` |
| 49 | `LogMessages` | VariableAttribute | `InternalCtrlr` | `LogMessages` | `Actual` |
| 50 | `LogMessagesRaw` | VariableAttribute | `InternalCtrlr` | `LogMessagesRaw` | `Actual` |
| 51 | `LogMessagesFormat` | VariableAttribute | `InternalCtrlr` | `LogMessagesFormat` | `Actual` |
| 52 | `LogRotation` | VariableAttribute | `InternalCtrlr` | `LogRotation` | `Actual` |
| 53 | `LogRotationDateSuffix` | VariableAttribute | `InternalCtrlr` | `LogRotationDateSuffix` | `Actual` |
| 54 | `LogRotationMaximumFileSize` | VariableAttribute | `InternalCtrlr` | `LogRotationMaximumFileSize` | `Actual` |
| 55 | `LogRotationMaximumFileCount` | VariableAttribute | `InternalCtrlr` | `LogRotationMaximumFileCount` | `Actual` |
| 56 | `SupportedChargingProfilePurposeTypes` | VariableAttribute | `InternalCtrlr` | `SupportedChargingProfilePurposeTypes` | `Actual` |
| 57 | `IgnoredProfilePurposesOffline` | VariableAttribute | `SmartChargingCtrlr` | `IgnoredProfilePurposesOffline` | `Actual` |
| 58 | `MaxCompositeScheduleDuration` | VariableAttribute | `InternalCtrlr` | `MaxCompositeScheduleDuration` | `Actual` |
| 59 | `CompositeScheduleDefaultLimitAmps` | VariableAttribute | `SmartChargingCtrlr` | `CompositeScheduleDefaultLimitAmps` | `Actual` |
| 60 | `CompositeScheduleDefaultLimitWatts` | VariableAttribute | `SmartChargingCtrlr` | `CompositeScheduleDefaultLimitWatts` | `Actual` |
| 61 | `CompositeScheduleDefaultNumberPhases` | VariableAttribute | `SmartChargingCtrlr` | `CompositeScheduleDefaultNumberPhases` | `Actual` |
| 62 | `SupplyVoltage` | VariableAttribute | `SmartChargingCtrlr` | `SupplyVoltage` | `Actual` |
| 63 | `WebsocketPingPayload` | VariableAttribute | `InternalCtrlr` | `WebsocketPingPayload` | `Actual` |
| 64 | `WebsocketPongTimeout` | VariableAttribute | `InternalCtrlr` | `WebsocketPongTimeout` | `Actual` |
| 65 | `UseSslDefaultVerifyPaths` | VariableAttribute | `InternalCtrlr` | `UseSslDefaultVerifyPaths` | `Actual` |
| 66 | `VerifyCsmsCommonName` | VariableAttribute | `InternalCtrlr` | `VerifyCsmsCommonName` | `Actual` |
| 67 | `VerifyCsmsAllowWildcards` | VariableAttribute | `InternalCtrlr` | `VerifyCsmsAllowWildcards` | `Actual` |
| 68 | `OcspRequestInterval` | VariableAttribute | `InternalCtrlr` | `OcspRequestInterval` | `Actual` |
| 69 | `SeccLeafSubjectCommonName` | VariableAttribute | `ISO15118Ctrlr` | `SeccId` | `Actual` |
| 70 | `SeccLeafSubjectCountry` | VariableAttribute | `ISO15118Ctrlr` | `CountryName` | `Actual` |
| 71 | `SeccLeafSubjectOrganization` | VariableAttribute | `ISO15118Ctrlr` | `OrganizationName` | `Actual` |
| 72 | `QueueAllMessages` | VariableAttribute | `OCPPCommCtrlr` | `QueueAllMessages` | `Actual` |
| 73 | `MessageTypesDiscardForQueueing` | VariableAttribute | `OCPPCommCtrlr` | `MessageTypesDiscardForQueueing` | `Actual` |
| 74 | `MessageQueueSizeThreshold` | VariableAttribute | `InternalCtrlr` | `MessageQueueSizeThreshold` | `Actual` |
| 75 | `MaxMessageSize` | VariableAttribute | `InternalCtrlr` | `MaxMessageSize` | `Actual` |
| 76 | `TLSKeylogFile` | VariableAttribute | `InternalCtrlr` | `TLSKeylogFile` | `Actual` |
| 77 | `EnableTLSKeylog` | VariableAttribute | `InternalCtrlr` | `EnableTLSKeylog` | `Actual` |
| 78 | `NumberOfConnectors` | OCPP1.6-specific | `InternalCtrlr` | `NumberOfConnectors` | `Actual` |
| 79 | `RetryBackoffWaitMinimum` | VariableAttribute | `OCPPCommCtrlr` | `RetryBackOffWaitMinimum` | `Actual` |

## Local Auth List Management Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 80 | `LocalAuthListMaxLength` | VariableCharacteristics | `LocalAuthListCtrlr` | `Entries` | `maxLimit` |
| 81 | `SendLocalListMaxLength` | VariableAttribute | `LocalAuthListCtrlr` | `ItemsPerMessage` | `maxLimit` |

## Smart Charging Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 82 | `MaxChargingProfilesInstalled` | VariableCharacteristics | `SmartChargingCtrlr` | `Entries` | `maxLimit` |

## Security Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 83 | `AdditionalRootCertificateCheck` | VariableAttribute | `SecurityCtrlr` | `AdditionalRootCertificateCheck` | `Actual` |
| 84 | `CertificateSignedMaxChainSize` | VariableAttribute | `SecurityCtrlr` | `MaxCertificateChainSize` | `Actual` |
| 85 | `CpoName` | VariableAttribute | `SecurityCtrlr` | `OrganizationName` | `Actual` |
| 86 | `CertSigningWaitMinimum` | VariableAttribute | `SecurityCtrlr` | `CertSigningWaitMinimum` | `Actual` |
| 87 | `CertSigningRepeatTimes` | VariableAttribute | `SecurityCtrlr` | `CertSigningRepeatTimes` | `Actual` |
| 88 | `CertificateStoreMaxLength` | VariableAttribute | `SecurityCtrlr` | `CertificateEntries` | `Actual` |

## PnC Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 89 | `ISO15118PnCEnabled` | VariableAttribute | `ISO15118Ctrlr` | `PnCEnabled` | `Actual` |
| 90 | `CentralContractValidationAllowed` | VariableAttribute | `ISO15118Ctrlr` | `CentralContractValidationAllowed` | `Actual` |
| 91 | `ContractValidationOffline` | VariableAttribute | `ISO15118Ctrlr` | `ContractValidationOffline` | `Actual` |

## CostAndPrice Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 92 | `NumberOfDecimalsForCostValues` | VariableAttribute | `TariffCostCtrlr` | `NumberOfDecimalsForCostValues` | `Actual` |
| 93 | `TimeOffset` | VariableAttribute | `ClockCtrlr` | `TimeOffset` | `Actual` |
| 94 | `NextTimeOffsetTransitionDateTime` | VariableAttribute | `ClockCtrlr` | `NextTimeOffsetTransitionDateTime` | `Actual` |
| 95 | `TimeOffsetNextTransition` | VariableAttribute | `ClockCtrlr` | `NextTransition` | `Actual` |

## Mavericks Section

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 96 | `BlinkRepeat` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `BlinkRepeat` | `Actual` |
| 97 | `ConnectorPhaseRotation` | VariableAttribute | `OCPP16LegacyCtrlr` | `PhaseRotation` | `Actual` |
| 98 | `ConnectorPhaseRotationMaxLength` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `ConnectorPhaseRotationMaxLength` | `Actual` |
| 99 | `GetConfigurationMaxKeys` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `GetConfigurationMaxKeys` | `Actual` |
| 100 | `LightIntensity` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `LightIntensity` | `Actual` |
| 101 | `MinimumStatusDuration` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `MinimumStatusDuration` | `Actual` |
| 102 | `StopTransactionOnEVSideDisconnect` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `StopTransactionOnEVSideDisconnect` | `Actual` |
| 103 | `SupportedFeatureProfiles` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `SupportedFeatureProfiles` | `Actual` |
| 104 | `SupportedFeatureProfilesMaxLength` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `SupportedFeatureProfilesMaxLength` | `Actual` |
| 105 | `UnlockConnectorOnEVSideDisconnect` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `UnlockConnectorOnEVSideDisconnect` | `Actual` |
| 106 | `ReserveConnectorZeroSupported` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `ReserveConnectorZeroSupported` | `Actual` |
| 107 | `HostName` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `HostName` | `Actual` |
| 108 | `AllowChargingProfileWithoutStartSchedule` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `AllowChargingProfileWithoutStartSchedule` | `Actual` |
| 109 | `WaitForStopTransactionsOnResetTimeout` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `WaitForStopTransactionsOnResetTimeout` | `Actual` |
| 110 | `StopTransactionIfUnlockNotSupported` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `StopTransactionIfUnlockNotSupported` | `Actual` |
| 111 | `MeterPublicKeys` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `MeterPublicKeys` | `Actual` |
| 112 | `DisableSecurityEventNotifications` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `DisableSecurityEventNotifications` | `Actual` |
| 113 | `ISO15118CertificateManagementEnabled` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `ISO15118CertificateManagementEnabled` | `Actual` |
| 114 | `CustomDisplayCostAndPrice` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `CustomDisplayCostAndPrice` | `Actual` |
| 115 | `DefaultPrice` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `DefaultPrice` | `Actual` |
| 116 | `DefaultPriceText` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `DefaultPriceText` | `Actual` |
| 117 | `CustomIdleFeeAfterStop` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `CustomIdleFeeAfterStop` | `Actual` |
| 118 | `SupportedLanguages` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `SupportedLanguages` | `Actual` |
| 119 | `CustomMultiLanguageMessages` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `CustomMultiLanguageMessages` | `Actual` |
| 120 | `Language` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `Language` | `Actual` |
| 121 | `WaitForSetUserPriceTimeout` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `WaitForSetUserPriceTimeout` | `Actual` |

## Mavericks Section

Direct mapping is not supported. An additional key is used instead.

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 122 | `AuthorizationKey` | VariableAttribute | `CustomLegacyController` | `AuthorizationKey` | `Actual` |
| 123 | `CentralSystemURI` | VariableAttribute | `CustomLegacyController` | `CentralSystemURI` | `Actual` |
| 124 | `SecurityProfile` | VariableAttribute | `CustomLegacyController` | `SecurityProfile` | `Actual` |

Special handling is required

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 125 | `ConnectorEvseIds` | VariableAttribute | `*EVSE` | `EvseId` | `Actual` |
| 126 | `SupportedMeasurands` | VariableCharacteristics | `*` | `Measurands` | `valuesList` |
