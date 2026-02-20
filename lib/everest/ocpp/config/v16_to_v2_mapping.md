
# OCPP 1.6 to OCPP 2.x mapping

This documents how OCPP 1.6 configuration keys map to the OCPP 2.x Device Model.

It is intended as a **human-readable reference** for integrators and maintainers.
The implementation in code (for example `known_keys` conversion logic and patching behavior) remains authoritative.

## Core Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 1 | `AllowOfflineTxForUnknownId` | VariableAttribute | `AuthCtrlr` | `OfflineTxForUnknownIdEnabled` | `Actual` |
| 2 | `AuthorizationCacheEnabled` | VariableAttribute | `AuthCacheCtrlr` | `Enabled` | `Actual` |
| 3 | `AuthorizeRemoteTxRequests` | VariableAttribute | `AuthCtrlr` | `AuthorizeRemoteStart` | `Actual` |
| 4 | `BlinkRepeat` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `BlinkRepeat` | `Actual` |
| 5 | `ClockAlignedDataInterval` | VariableAttribute | `AlignedDataCtrlr` | `Interval` | `Actual` |
| 6 | `ConnectionTimeOut` | VariableAttribute | `TxCtrlr` | `EVConnectionTimeout` | `Actual` |
| 7 | `ConnectorPhaseRotation` | VariableAttribute | `*` | `PhaseRotation` | `Actual` |
| 8 | `ConnectorPhaseRotationMaxLength` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `ConnectorPhaseRotationMaxLength` | `Actual` |
| 9 | `GetConfigurationMaxKeys` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `GetConfigurationMaxKeys` | `Actual` |
| 10 | `HeartbeatInterval` | VariableAttribute | `OCPPCommCtrlr` | `HeartbeatInterval` | `Actual` |
| 11 | `LightIntensity` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `LightIntensity` | `Actual` |
| 12 | `LocalAuthorizeOffline` | VariableAttribute | `AuthCtrlr` | `LocalAuthorizeOffline` | `Actual` |
| 13 | `LocalPreAuthorize` | VariableAttribute | `AuthCtrlr` | `LocalPreAuthorize` | `Actual` |
| 14 | `MaxEnergyOnInvalidId` | VariableAttribute | `TxCtrlr` | `MaxEnergyOnInvalidId` | `Actual` |
| 15 | `MeterValuesAlignedData` | VariableAttribute | `AlignedDataCtrlr` | `Measurands` | `Actual` |
| 16 | `MeterValuesAlignedDataMaxLength` | VariableCharacteristics | `AlignedDataCtrlr` | `Measurands` | `maxLimit` |
| 17 | `MeterValuesSampledData` | VariableAttribute | `SampledDataCtrlr` | `TxUpdatedMeasurands` | `Actual` |
| 18 | `MeterValuesSampledDataMaxLength` | VariableCharacteristics | `SampledDataCtrlr` | `TxUpdatedMeasurands` | `maxLimit` |
| 19 | `MeterValueSampleInterval` | VariableAttribute | `SampledDataCtrlr` | `TxUpdatedInterval` | `Actual` |
| 20 | `MinimumStatusDuration` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `MinimumStatusDuration` | `Actual` |
| 21 | `NumberOfConnectors` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `NumberOfConnectors` | `Actual` |
| 22 | `ResetRetries` | VariableAttribute | `OCPPCommCtrlr` | `ResetRetries` | `Actual` |
| 23 | `StopTransactionOnEVSideDisconnect` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `StopTransactionOnEVSideDisconnect` | `Actual` |
| 24 | `StopTransactionOnInvalidId` | VariableAttribute | `TxCtrlr` | `StopTxOnInvalidId` | `Actual` |
| 25 | `StopTxnAlignedData` | VariableAttribute | `AlignedDataCtrlr` | `TxEndedMeasurands` | `Actual` |
| 26 | `StopTxnAlignedDataMaxLength` | VariableCharacteristics | `AlignedDataCtrlr` | `TxEndedMeasurands` | `maxLimit` |
| 27 | `StopTxnSampledData` | VariableAttribute | `SampledDataCtrlr` | `TxEndedMeasurands` | `Actual` |
| 28 | `StopTxnSampledDataMaxLength` | VariableCharacteristics | `SampledDataCtrlr` | `TxEndedMeasurands` | `maxLimit` |
| 29 | `SupportedFeatureProfiles` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `SupportedFeatureProfiles` | `Actual` |
| 30 | `SupportedFeatureProfilesMaxLength` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `SupportedFeatureProfilesMaxLength` | `Actual` |
| 31 | `TransactionMessageAttempts` | VariableAttribute | `OCPPCommCtrlr` | `MessageAttempts` | `Actual` |
| 32 | `TransactionMessageRetryInterval` | VariableAttribute | `OCPPCommCtrlr` | `MessageAttemptInterval` | `Actual` |
| 33 | `UnlockConnectorOnEVSideDisconnect` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `UnlockConnectorOnEVSideDisconnect` | `Actual` |
| 34 | `WebSocketPingInterval` | VariableAttribute | `OCPPCommCtrlr` | `WebSocketPingInterval` | `Actual` |
| 35 | `SupportedFileTransferProtocols` | VariableAttribute | `OCPPCommCtrlr` | `FileTransferProtocols` | `Actual` |

## Local Auth List Management Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 36 | `LocalAuthListEnabled` | VariableAttribute | `LocalAuthListCtrlr` | `Enabled` | `Actual` |
| 37 | `LocalAuthListMaxLength` | VariableCharacteristics | `LocalAuthListCtrlr` | `Entries` | `maxLimit` |
| 38 | `SendLocalListMaxLength` | VariableAttribute | `LocalAuthListCtrlr` | `ItemsPerMessage` | `Actual` |

## Reservation Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 39 | `ReserveConnectorZeroSupported` | TBD | `` | `` | `` |

## Smart Charging Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 40 | `ChargeProfileMaxStackLevel` | VariableAttribute | `SmartChargingCtrlr` | `ProfileStackLevel` | `Actual` |
| 41 | `ChargingScheduleAllowedChargingRateUnit` | VariableAttribute | `SmartChargingCtrlr` | `RateUnit` | `Actual` |
| 42 | `ChargingScheduleMaxPeriods` | VariableAttribute | `SmartChargingCtrlr` | `PeriodsPerSchedule` | `Actual` |
| 43 | `ConnectorSwitch3to1PhaseSupported` | VariableAttribute | `SmartChargingCtrlr` | `Phases3to1` | `Actual` |
| 44 | `MaxChargingProfilesInstalled` | VariableCharacteristics | `SmartChargingCtrlr` | `ChargingProfileEntries` | `maxLimit` |

## Internal Keys

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 45 | `ChargePointId` | VariableAttribute | `InternalCtrlr` | `ChargePointId` | `Actual` |
| 46 | `CentralSystemURI` | VariableAttribute | `InternalCtrlr` | `NetworkConnectionProfiles.ocppCsmsUrl` | `Actual` |
| 47 | `ChargeBoxSerialNumber` | VariableAttribute | `InternalCtrlr` | `ChargeBoxSerialNumber` | `Actual` |
| 48 | `ChargePointModel` | VariableAttribute | `InternalCtrlr` | `ChargePointModel` | `Actual` |
| 49 | `ChargePointSerialNumber` | VariableAttribute | `InternalCtrlr` | `ChargePointSerialNumber` | `Actual` |
| 50 | `ChargePointVendor` | VariableAttribute | `InternalCtrlr` | `ChargePointVendor` | `Actual` |
| 51 | `FirmwareVersion` | VariableAttribute | `InternalCtrlr` | `FirmwareVersion` | `Actual` |
| 52 | `ICCID` | VariableAttribute | `InternalCtrlr` | `ICCID` | `Actual` |
| 53 | `HostName` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `HostName` | `Actual` |
| 54 | `IFace` | VariableAttribute | `InternalCtrlr` | `IFace` | `Actual` |
| 55 | `IMSI` | VariableAttribute | `InternalCtrlr` | `IMSI` | `Actual` |
| 56 | `MeterSerialNumber` | VariableAttribute | `InternalCtrlr` | `MeterSerialNumber` | `Actual` |
| 57 | `MeterType` | VariableAttribute | `InternalCtrlr` | `MeterType` | `Actual` |
| 58 | `SupportedCiphers12` | VariableAttribute | `InternalCtrlr` | `SupportedCiphers12` | `Actual` |
| 59 | `SupportedCiphers13` | VariableAttribute | `InternalCtrlr` | `SupportedCiphers13` | `Actual` |
| 60 | `UseTPM` | VariableAttribute | `InternalCtrlr` | `UseTPM` | `Actual` |
| 61 | `UseTPMSeccLeafCertificate` | VariableAttribute | `InternalCtrlr` | `UseTPMSeccLeafCertificate` | `Actual` |
| 62 | `RetryBackoffRandomRange` | VariableAttribute | `OCPPCommCtrlr` | `RetryBackOffRandomRange` | `Actual` |
| 63 | `RetryBackoffRepeatTimes` | VariableAttribute | `OCPPCommCtrlr` | `RetryBackOffRepeatTimes` | `Actual` |
| 64 | `RetryBackoffWaitMinimum` | VariableAttribute | `OCPPCommCtrlr` | `RetryBackOffWaitMinimum` | `Actual` |
| 65 | `AuthorizeConnectorZeroOnConnectorOne` | VariableAttribute | `InternalCtrlr` | `AuthorizeConnectorZeroOnConnectorOne` | `Actual` |
| 66 | `LogMessages` | VariableAttribute | `InternalCtrlr` | `LogMessages` | `Actual` |
| 67 | `LogMessagesRaw` | VariableAttribute | `InternalCtrlr` | `LogMessagesRaw` | `Actual` |
| 68 | `LogMessagesFormat` | VariableAttribute | `InternalCtrlr` | `LogMessagesFormat` | `Actual` |
| 69 | `LogRotation` | VariableAttribute | `InternalCtrlr` | `LogRotation` | `Actual` |
| 70 | `LogRotationDateSuffix` | VariableAttribute | `InternalCtrlr` | `LogRotationDateSuffix` | `Actual` |
| 71 | `LogRotationMaximumFileSize` | VariableAttribute | `InternalCtrlr` | `LogRotationMaximumFileSize` | `Actual` |
| 72 | `LogRotationMaximumFileCount` | VariableAttribute | `InternalCtrlr` | `LogRotationMaximumFileCount` | `Actual` |
| 73 | `SupportedChargingProfilePurposeTypes` | VariableAttribute | `InternalCtrlr` | `SupportedChargingProfilePurposeTypes` | `Actual` |
| 74 | `IgnoredProfilePurposesOffline` | VariableAttribute | `SmartChargingCtrlr` | `IgnoredProfilePurposesOffline` | `Actual` |
| 75 | `MaxCompositeScheduleDuration` | VariableAttribute | `InternalCtrlr` | `MaxCompositeScheduleDuration` | `Actual` |
| 76 | `CompositeScheduleDefaultLimitAmps` | VariableAttribute | `SmartChargingCtrlr` | `CompositeScheduleDefaultLimitAmps` | `Actual` |
| 77 | `CompositeScheduleDefaultLimitWatts` | VariableAttribute | `SmartChargingCtrlr` | `CompositeScheduleDefaultLimitWatts` | `Actual` |
| 78 | `CompositeScheduleDefaultNumberPhases` | VariableAttribute | `SmartChargingCtrlr` | `CompositeScheduleDefaultNumberPhases` | `Actual` |
| 79 | `SupplyVoltage` | VariableAttribute | `SmartChargingCtrlr` | `SupplyVoltage` | `Actual` |
| 80 | `WebsocketPingPayload` | VariableAttribute | `InternalCtrlr` | `WebsocketPingPayload` | `Actual` |
| 81 | `WebsocketPongTimeout` | VariableAttribute | `InternalCtrlr` | `WebsocketPongTimeout` | `Actual` |
| 82 | `UseSslDefaultVerifyPaths` | VariableAttribute | `InternalCtrlr` | `UseSslDefaultVerifyPaths` | `Actual` |
| 83 | `VerifyCsmsCommonName` | VariableAttribute | `InternalCtrlr` | `VerifyCsmsCommonName` | `Actual` |
| 84 | `VerifyCsmsAllowWildcards` | VariableAttribute | `InternalCtrlr` | `VerifyCsmsAllowWildcards` | `Actual` |
| 85 | `OcspRequestInterval` | VariableAttribute | `InternalCtrlr` | `OcspRequestInterval` | `Actual` |
| 86 | `SeccLeafSubjectCommonName` | VariableAttribute | `ISO15118Ctrlr` | `SeccId` | `Actual` |
| 87 | `SeccLeafSubjectCountry` | VariableAttribute | `ISO15118Ctrlr` | `CountryName` | `Actual` |
| 88 | `SeccLeafSubjectOrganization` | VariableAttribute | `ISO15118Ctrlr` | `OrganizationName` | `Actual` |
| 89 | `ConnectorEvseIds` | VariableAttribute | `*EVSE` | `EvseId` | `Actual` |
| 90 | `AllowChargingProfileWithoutStartSchedule` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `AllowChargingProfileWithoutStartSchedule` | `Actual` |
| 91 | `WaitForStopTransactionsOnResetTimeout` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `WaitForStopTransactionsOnResetTimeout` | `Actual` |
| 92 | `QueueAllMessages` | VariableAttribute | `OCPPCommCtrlr` | `QueueAllMessages` | `Actual` |
| 93 | `MessageTypesDiscardForQueueing` | VariableAttribute | `OCPPCommCtrlr` | `MessageTypesDiscardForQueueing` | `Actual` |
| 94 | `MessageQueueSizeThreshold` | VariableAttribute | `InternalCtrlr` | `MessageQueueSizeThreshold` | `Actual` |
| 95 | `SupportedMeasurands` | VariableCharacteristics | `*` | `Measurands` | `valuesList` |
| 96 | `MaxMessageSize` | VariableAttribute | `InternalCtrlr` | `MaxMessageSize` | `Actual` |
| 97 | `TLSKeylogFile` | VariableAttribute | `InternalCtrlr` | `TLSKeylogFile` | `Actual` |
| 98 | `EnableTLSKeylog` | VariableAttribute | `InternalCtrlr` | `EnableTLSKeylog` | `Actual` |
| 99 | `StopTransactionIfUnlockNotSupported` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `StopTransactionIfUnlockNotSupported` | `Actual` |
| 100 | `MeterPublicKeys` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `MeterPublicKeys` | `Actual` |

## Security Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 101 | `AdditionalRootCertificateCheck` | VariableAttribute | `SecurityCtrlr` | `AdditionalRootCertificateCheck` | `Actual` |
| 102 | `AuthorizationKey` | VariableAttribute | `SecurityCtrlr` | `BasicAuthPassword` | `Actual` |
| 103 | `CertificateSignedMaxChainSize` | VariableAttribute | `SecurityCtrlr` | `MaxCertificateChainSize` | `Actual` |
| 104 | `CertificateStoreMaxLength` | VariableCharacteristics | `SecurityCtrlr` | `CertificateEntries` | `maxLimit` |
| 105 | `CpoName` | VariableAttribute | `SecurityCtrlr` | `OrganizationName` | `Actual` |
| 106 | `SecurityProfile` | VariableAttribute | `SecurityCtrlr` | `SecurityProfile` | `Actual` |
| 107 | `DisableSecurityEventNotifications` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `DisableSecurityEventNotifications` | `Actual` |

## PnC Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 108 | `ISO15118CertificateManagementEnabled` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `ISO15118CertificateManagementEnabled` | `Actual` |
| 109 | `ISO15118PnCEnabled` | VariableAttribute | `ISO15118Ctrlr` | `PnCEnabled` | `Actual` |
| 110 | `CentralContractValidationAllowed` | VariableAttribute | `ISO15118Ctrlr` | `CentralContractValidationAllowed` | `Actual` |
| 111 | `CertSigningWaitMinimum` | VariableAttribute | `SecurityCtrlr` | `CertSigningWaitMinimum` | `Actual` |
| 112 | `CertSigningRepeatTimes` | VariableAttribute | `SecurityCtrlr` | `CertSigningRepeatTimes` | `Actual` |
| 113 | `ContractValidationOffline` | VariableAttribute | `ISO15118Ctrlr` | `ContractValidationOffline` | `Actual` |

## CostAndPrice Profile

| ID | OCPP1.6 key | Mapping type | Component | Variable | Target field |
| ---: | --- | --- | --- | --- | --- |
| 114 | `CustomDisplayCostAndPrice` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `CustomDisplayCostAndPrice` | `Actual` |
| 115 | `NumberOfDecimalsForCostValues` | VariableAttribute | `TariffCostCtrlr` | `NumberOfDecimalsForCostValues` | `Actual` |
| 116 | `DefaultPrice` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `DefaultPrice` | `Actual` |
| 117 | `DefaultPriceText` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `DefaultPriceText` | `Actual` |
| 118 | `TimeOffset` | VariableAttribute | `ClockCtrlr` | `TimeOffset` | `Actual` |
| 119 | `NextTimeOffsetTransitionDateTime` | VariableAttribute | `ClockCtrlr` | `NextTimeOffsetTransitionDateTime` | `Actual` |
| 120 | `TimeOffsetNextTransition` | VariableAttribute | `ClockCtrlr` | `TimeOffset:NextTransition` | `Actual` |
| 121 | `CustomIdleFeeAfterStop` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `CustomIdleFeeAfterStop` | `Actual` |
| 122 | `SupportedLanguages` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `SupportedLanguages` | `Actual` |
| 123 | `CustomMultiLanguageMessages` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `CustomMultiLanguageMessages` | `Actual` |
| 124 | `Language` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `Language` | `Actual` |
| 125 | `WaitForSetUserPriceTimeout` | OCPP1.6-specific | `OCPP16LegacyCtrlr` | `WaitForSetUserPriceTimeout` | `Actual` |
