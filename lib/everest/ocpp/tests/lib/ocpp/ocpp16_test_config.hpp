// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <initializer_list>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

namespace ocpp::tests {

inline constexpr char OCPP16_TEST_CONFIG_FULL[] = R"({
    "Internal": {
        "ChargePointId": "cp002",
        "CentralSystemURI": "127.0.0.1:8181/steve/websocket/CentralSystemService/",
        "ChargeBoxSerialNumber": "cp002",
        "ChargePointModel": "Model",
        "ChargePointSerialNumber": "serial",
        "ChargePointVendor": "EVerest",
        "FirmwareVersion": "0.2",
        "ICCID": "8914800000000000000",
        "IMSI": "262 012345678901",
        "MeterSerialNumber": "meter-serial",
        "MeterType": "DC",
        "SupportedCiphers12": ["ECDHE-ECDSA-AES128-GCM-SHA256", "AES256-GCM-SHA384"],
        "SupportedCiphers13": ["TLS_AES_256_GCM_SHA384", "TLS_AES_128_GCM_SHA256", "TLS_CHACHA20_POLY1305_SHA256"],
        "UseTPM": true,
        "UseTPMSeccLeafCertificate": true,
        "RetryBackoffRandomRange": 12,
        "RetryBackoffRepeatTimes": 4,
        "RetryBackoffWaitMinimum": 4,
        "AuthorizeConnectorZeroOnConnectorOne": false,
        "LogMessages": false,
        "LogMessagesFormat": ["log", "html", "session_logging", "security"],
        "LogRotation": true,
        "LogRotationDateSuffix": true,
        "LogRotationMaximumFileSize": 1,
        "LogRotationMaximumFileCount": 2,
        "SupportedChargingProfilePurposeTypes": ["ChargePointMaxProfile", "TxProfile"],
        "MaxCompositeScheduleDuration": 31536000,
        "WebsocketPingPayload": "Hello from EVerest!",
        "WebsocketPongTimeout": 6,
        "UseSslDefaultVerifyPaths": false,
        "VerifyCsmsCommonName": false,
        "VerifyCsmsAllowWildcards": false,
        "OcspRequestInterval": 604800,
        "SeccLeafSubjectCommonName": "DEPNX100002",
        "SeccLeafSubjectCountry": "EN",
        "SeccLeafSubjectOrganization": "Pionix",
        "ConnectorEvseIds": "DE*EV*100001,DE*EV*100002",
        "AllowChargingProfileWithoutStartSchedule": true,
        "WaitForStopTransactionsOnResetTimeout": 30,
        "QueueAllMessages": false,
        "MessageTypesDiscardForQueueing": "Heartbeat,StatusNotification",
        "MessageQueueSizeThreshold": 6000,
        "SupportedMeasurands": "Energy.Active.Import.Register,Energy.Active.Export.Register,Power.Active.Import,Voltage,Current.Import,Frequency,Current.Offered,Power.Offered,SoC",
        "MaxMessageSize": 65000,
        "TLSKeylogFile": "/tmp/ocpp_tls_keylog.txt",
        "EnableTLSKeylog": true,
        "HostName": "cp002.local",
        "StopTransactionIfUnlockNotSupported": false,
        "MeterPublicKeys": ["PUBLIC_KEY_1", "PUBLIC_KEY_2"],
        "IFace": "eth0",
        "LogMessagesRaw": false,
        "IgnoredProfilePurposesOffline": "ChargePointMaxProfile,TxProfile",
        "CompositeScheduleDefaultLimitAmps": 92,
        "CompositeScheduleDefaultLimitWatts": 42,
        "CompositeScheduleDefaultNumberPhases": 2,
        "SupplyVoltage": 220
    },
    "Core": {
        "AllowOfflineTxForUnknownId": false,
        "AuthorizationCacheEnabled": false,
        "AuthorizeRemoteTxRequests": false,
        "BlinkRepeat": 10,
        "ClockAlignedDataInterval": 700,
        "ConnectionTimeOut": 123,
        "ConnectorPhaseRotation": "RST0.RST,1.RST,2.RTS",
        "ConnectorPhaseRotationMaxLength": 101,
        "GetConfigurationMaxKeys": 512,
        "HeartbeatInterval": 905,
        "LightIntensity": 12,
        "LocalAuthorizeOffline": false,
        "LocalPreAuthorize": false,
        "MaxEnergyOnInvalidId": 503,
        "MeterValuesAlignedData": "Energy.Active.Import.Register,Frequency",
        "MeterValuesAlignedDataMaxLength": 611,
        "MeterValuesSampledData": "Energy.Active.Import.Register,Power.Active.Import",
        "MeterValuesSampledDataMaxLength": 622,
        "MeterValueSampleInterval": 301,
        "MinimumStatusDuration": 2,
        "NumberOfConnectors": 3,
        "ResetRetries": 2,
        "StopTransactionOnEVSideDisconnect": false,
        "StopTransactionOnInvalidId": false,
        "StopTxnAlignedData": "Energy.Active.Import.Register",
        "StopTxnAlignedDataMaxLength": 633,
        "StopTxnSampledData": "Energy.Active.Import.Register",
        "StopTxnSampledDataMaxLength": 644,
        "SupportedFeatureProfiles": "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging",
        "SupportedFeatureProfilesMaxLength": 512,
        "TransactionMessageAttempts": 2,
        "TransactionMessageRetryInterval": 11,
        "UnlockConnectorOnEVSideDisconnect": false,
        "WebSocketPingInterval": 11
    },
    "LocalAuthListManagement": {
        "LocalAuthListEnabled": false,
        "LocalAuthListMaxLength": 512,
        "SendLocalListMaxLength": 512
    },
    "SmartCharging": {
        "ChargeProfileMaxStackLevel": 1001,
        "ChargingScheduleAllowedChargingRateUnit": "Current,Power",
        "ChargingScheduleMaxPeriods": 1001,
        "ConnectorSwitch3to1PhaseSupported": false,
        "MaxChargingProfilesInstalled": 1001
    },
    "FirmwareManagement": {
        "SupportedFileTransferProtocols": "FTP,FTPS"
    },
    "Reservation": {
        "ReserveConnectorZeroSupported": true
    },
    "Security": {
        "AdditionalRootCertificateCheck": true,
        "AuthorizationKey": "SECRETSECRETSECRET",
        "CertificateSignedMaxChainSize": 6000,
        "CertificateStoreMaxLength": 700,
        "CpoName": "EVerest",
        "SecurityProfile": 1,
        "DisableSecurityEventNotifications": true
    },
    "PnC": {
        "ISO15118CertificateManagementEnabled": false,
        "ISO15118PnCEnabled": false,
        "CentralContractValidationAllowed": false,
        "CertSigningWaitMinimum": 15,
        "CertSigningRepeatTimes": 3,
        "ContractValidationOffline": false
    },
    "CostAndPrice": {
        "CustomDisplayCostAndPrice": false,
        "NumberOfDecimalsForCostValues": 5,
        "DefaultPrice": {
            "priceText": "This is the price",
            "priceTextOffline": "Show this price text when offline!",
            "chargingPrice": {"kWhPrice": 3.14, "hourPrice": 0.42}
        },
        "DefaultPriceText": {
            "priceTexts": [
                {"priceText": "This is the price", "priceTextOffline": "Show this price text when offline!", "language": "en"},
                {"priceText": "Dit is de prijs", "priceTextOffline": "Laat dit zien wanneer de charging station offline is!", "language": "nl"},
                {"priceText": "Dette er prisen", "priceTextOffline": "Vis denne pristeksten når du er frakoblet", "language": "nb_NO"}
            ]
        },
        "TimeOffset": "02:00",
        "NextTimeOffsetTransitionDateTime": "2024-01-01T00:00:00",
        "TimeOffsetNextTransition": "01:00",
        "CustomIdleFeeAfterStop": false,
        "SupportedLanguages": "en, nl, de, nb_NO",
        "CustomMultiLanguageMessages": true,
        "Language": "en",
        "WaitForSetUserPriceTimeout": 5000
    }
})";

inline std::string make_ocpp16_test_config_full() {
    return OCPP16_TEST_CONFIG_FULL;
}

inline std::string make_ocpp16_test_config_missing_optionals() {
    auto config = nlohmann::json::parse(OCPP16_TEST_CONFIG_FULL);

    const std::initializer_list<std::pair<const char*, const char*>> removed_keys = {
        {"Internal", "ChargePointSerialNumber"},
        {"Internal", "ICCID"},
        {"Internal", "IMSI"},
        {"Internal", "MeterSerialNumber"},
        {"Internal", "MeterType"},
        {"Internal", "HostName"},
        {"Internal", "IFace"},
        {"Internal", "QueueAllMessages"},
        {"Internal", "MessageTypesDiscardForQueueing"},
        {"Internal", "MessageQueueSizeThreshold"},
        {"Internal", "IgnoredProfilePurposesOffline"},
        {"Core", "BlinkRepeat"},
        {"Core", "LightIntensity"},
        {"Core", "MaxEnergyOnInvalidId"},
        {"Security", "AuthorizationKey"},
        {"Security", "AdditionalRootCertificateCheck"},
        {"Security", "CertificateSignedMaxChainSize"},
        {"Security", "CertificateStoreMaxLength"},
        {"Security", "CpoName"},
        {"PnC", "CentralContractValidationAllowed"},
        {"PnC", "CertSigningWaitMinimum"},
        {"PnC", "CertSigningRepeatTimes"},
        {"CostAndPrice", "DefaultPrice"},
        {"CostAndPrice", "DefaultPriceText"},
        {"CostAndPrice", "TimeOffset"},
        {"CostAndPrice", "NextTimeOffsetTransitionDateTime"},
        {"CostAndPrice", "TimeOffsetNextTransition"},
        {"CostAndPrice", "WaitForSetUserPriceTimeout"}};

    for (const auto& [section, key] : removed_keys) {
        if (config.contains(section)) {
            config[section].erase(key);
        }
    }

    return config.dump();
}

} // namespace ocpp::tests
