// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_CHARGE_POINT_CONFIGURATION_INTERFACE_HPP
#define OCPP_V16_CHARGE_POINT_CONFIGURATION_INTERFACE_HPP

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <ocpp/common/cistring.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp::v16 {

struct KeyValue;

/// \brief contains the configuration of the charge point
class ChargePointConfigurationInterface {
public:
    virtual ~ChargePointConfigurationInterface() = default;
    // UserConfig and Internal
    virtual std::string getChargeBoxSerialNumber() = 0;
    virtual std::optional<CiString<25>> getChargePointSerialNumber() = 0;
    virtual CiString<50> getFirmwareVersion() = 0;
    virtual std::optional<CiString<20>> getICCID() = 0;
    virtual std::optional<CiString<20>> getIMSI() = 0;
    virtual std::optional<CiString<25>> getMeterSerialNumber() = 0;
    virtual std::optional<CiString<25>> getMeterType() = 0;

    virtual KeyValue getChargeBoxSerialNumberKeyValue() = 0;
    virtual KeyValue getFirmwareVersionKeyValue() = 0;

    virtual std::optional<KeyValue> getChargePointSerialNumberKeyValue() = 0;
    virtual std::optional<KeyValue> getICCIDKeyValue() = 0;
    virtual std::optional<KeyValue> getIMSIKeyValue() = 0;
    virtual std::optional<KeyValue> getMeterSerialNumberKeyValue() = 0;
    virtual std::optional<KeyValue> getMeterTypeKeyValue() = 0;

    virtual void setChargepointInformation(const std::string& chargePointVendor, const std::string& chargePointModel,
                                           const std::optional<std::string>& chargePointSerialNumber,
                                           const std::optional<std::string>& chargeBoxSerialNumber,
                                           const std::optional<std::string>& firmwareVersion) = 0;
    virtual void setChargepointMeterInformation(const std::optional<std::string>& meterSerialNumber,
                                                const std::optional<std::string>& meterType) = 0;
    virtual void setChargepointModemInformation(const std::optional<std::string>& ICCID,
                                                const std::optional<std::string>& IMSI) = 0;

    // Internal
    virtual std::string getCentralSystemURI() = 0;
    virtual std::string getChargePointId() = 0;
    virtual std::string getSupportedCiphers12() = 0;
    virtual std::string getSupportedCiphers13() = 0;
    virtual std::string getSupportedMeasurands() = 0;
    virtual std::string getTLSKeylogFile() = 0;
    virtual std::string getWebsocketPingPayload() = 0;

    virtual CiString<20> getChargePointModel() = 0;
    virtual CiString<20> getChargePointVendor() = 0;

    virtual bool getAuthorizeConnectorZeroOnConnectorOne() = 0;
    virtual bool getEnableTLSKeylog() = 0;
    virtual bool getLogMessages() = 0;
    virtual bool getLogMessagesRaw() = 0;
    virtual bool getLogRotation() = 0;
    virtual bool getLogRotationDateSuffix() = 0;
    virtual bool getStopTransactionIfUnlockNotSupported() = 0;
    virtual bool getUseSslDefaultVerifyPaths() = 0;
    virtual bool getUseTPM() = 0;
    virtual bool getUseTPMSeccLeafCertificate() = 0;
    virtual bool getVerifyCsmsAllowWildcards() = 0;
    virtual bool getVerifyCsmsCommonName() = 0;

    virtual int getMaxMessageSize() = 0;

    virtual std::int32_t getMaxCompositeScheduleDuration() = 0;
    virtual std::int32_t getOcspRequestInterval() = 0;
    virtual std::int32_t getRetryBackoffRandomRange() = 0;
    virtual std::int32_t getRetryBackoffRepeatTimes() = 0;
    virtual std::int32_t getRetryBackoffWaitMinimum() = 0;
    virtual std::int32_t getWaitForStopTransactionsOnResetTimeout() = 0;
    virtual std::int32_t getWebsocketPongTimeout() = 0;

    virtual std::uint64_t getLogRotationMaximumFileCount() = 0;
    virtual std::uint64_t getLogRotationMaximumFileSize() = 0;

    virtual std::vector<std::string> getLogMessagesFormat() = 0;
    virtual std::vector<ChargingProfilePurposeType> getIgnoredProfilePurposesOffline() = 0;
    virtual std::vector<ChargingProfilePurposeType> getSupportedChargingProfilePurposeTypes() = 0;

    virtual std::optional<std::string> getConnectorEvseIds() = 0;
    virtual std::optional<std::string> getHostName() = 0;
    virtual std::optional<std::string> getIFace() = 0;
    virtual std::optional<std::string> getMessageTypesDiscardForQueueing() = 0;
    virtual std::optional<std::string> getSeccLeafSubjectCommonName() = 0;
    virtual std::optional<std::string> getSeccLeafSubjectCountry() = 0;
    virtual std::optional<std::string> getSeccLeafSubjectOrganization() = 0;
    virtual std::optional<bool> getAllowChargingProfileWithoutStartSchedule() = 0;
    virtual std::optional<bool> getQueueAllMessages() = 0;
    virtual std::optional<int> getMessageQueueSizeThreshold() = 0;
    virtual std::optional<std::int32_t> getCompositeScheduleDefaultLimitAmps() = 0;
    virtual std::optional<std::int32_t> getCompositeScheduleDefaultLimitWatts() = 0;
    virtual std::optional<std::int32_t> getCompositeScheduleDefaultNumberPhases() = 0;
    virtual std::optional<std::int32_t> getSupplyVoltage() = 0;
    virtual std::optional<std::vector<KeyValue>> getAllMeterPublicKeyKeyValues() = 0;

    virtual std::set<MessageType> getSupportedMessageTypesSending() = 0;
    virtual std::set<MessageType> getSupportedMessageTypesReceiving() = 0;

    virtual KeyValue getAuthorizeConnectorZeroOnConnectorOneKeyValue() = 0;
    virtual KeyValue getCentralSystemURIKeyValue() = 0;
    virtual KeyValue getChargePointIdKeyValue() = 0;
    virtual KeyValue getChargePointModelKeyValue() = 0;
    virtual KeyValue getChargePointVendorKeyValue() = 0;
    virtual KeyValue getEnableTLSKeylogKeyValue() = 0;
    virtual KeyValue getLogMessagesFormatKeyValue() = 0;
    virtual KeyValue getLogMessagesKeyValue() = 0;
    virtual KeyValue getLogMessagesRawKeyValue() = 0;
    virtual KeyValue getLogRotationDateSuffixKeyValue() = 0;
    virtual KeyValue getLogRotationKeyValue() = 0;
    virtual KeyValue getLogRotationMaximumFileCountKeyValue() = 0;
    virtual KeyValue getLogRotationMaximumFileSizeKeyValue() = 0;
    virtual KeyValue getMaxCompositeScheduleDurationKeyValue() = 0;
    virtual KeyValue getMaxMessageSizeKeyValue() = 0;
    virtual KeyValue getOcspRequestIntervalKeyValue() = 0;
    virtual KeyValue getRetryBackoffRandomRangeKeyValue() = 0;
    virtual KeyValue getRetryBackoffRepeatTimesKeyValue() = 0;
    virtual KeyValue getRetryBackoffWaitMinimumKeyValue() = 0;
    virtual KeyValue getStopTransactionIfUnlockNotSupportedKeyValue() = 0;
    virtual KeyValue getSupportedChargingProfilePurposeTypesKeyValue() = 0;
    virtual KeyValue getSupportedCiphers12KeyValue() = 0;
    virtual KeyValue getSupportedCiphers13KeyValue() = 0;
    virtual KeyValue getSupportedMeasurandsKeyValue() = 0;
    virtual KeyValue getTLSKeylogFileKeyValue() = 0;
    virtual KeyValue getUseSslDefaultVerifyPathsKeyValue() = 0;
    virtual KeyValue getUseTPMKeyValue() = 0;
    virtual KeyValue getUseTPMSeccLeafCertificateKeyValue() = 0;
    virtual KeyValue getVerifyCsmsAllowWildcardsKeyValue() = 0;
    virtual KeyValue getVerifyCsmsCommonNameKeyValue() = 0;
    virtual KeyValue getWaitForStopTransactionsOnResetTimeoutKeyValue() = 0;
    virtual KeyValue getWebsocketPingPayloadKeyValue() = 0;
    virtual KeyValue getWebsocketPongTimeoutKeyValue() = 0;

    virtual std::optional<KeyValue> getAllowChargingProfileWithoutStartScheduleKeyValue() = 0;
    virtual std::optional<KeyValue> getCompositeScheduleDefaultLimitAmpsKeyValue() = 0;
    virtual std::optional<KeyValue> getCompositeScheduleDefaultLimitWattsKeyValue() = 0;
    virtual std::optional<KeyValue> getCompositeScheduleDefaultNumberPhasesKeyValue() = 0;
    virtual std::optional<KeyValue> getConnectorEvseIdsKeyValue() = 0;
    virtual std::optional<KeyValue> getHostNameKeyValue() = 0;
    virtual std::optional<KeyValue> getIFaceKeyValue() = 0;
    virtual std::optional<KeyValue> getIgnoredProfilePurposesOfflineKeyValue() = 0;
    virtual std::optional<KeyValue> getMessageTypesDiscardForQueueingKeyValue() = 0;
    virtual std::optional<KeyValue> getMessageQueueSizeThresholdKeyValue() = 0;
    virtual std::optional<KeyValue> getPublicKeyKeyValue(std::uint32_t connector_id) = 0;
    virtual std::optional<KeyValue> getQueueAllMessagesKeyValue() = 0;
    virtual std::optional<KeyValue> getSeccLeafSubjectCommonNameKeyValue() = 0;
    virtual std::optional<KeyValue> getSeccLeafSubjectCountryKeyValue() = 0;
    virtual std::optional<KeyValue> getSeccLeafSubjectOrganizationKeyValue() = 0;
    virtual std::optional<KeyValue> getSupplyVoltageKeyValue() = 0;

    virtual void setAllowChargingProfileWithoutStartSchedule(bool allow) = 0;
    virtual void setCentralSystemURI(const std::string& ocpp_uri) = 0;
    virtual void setCompositeScheduleDefaultLimitAmps(std::int32_t limit_amps) = 0;
    virtual void setCompositeScheduleDefaultLimitWatts(std::int32_t limit_watts) = 0;
    virtual void setCompositeScheduleDefaultNumberPhases(std::int32_t number_phases) = 0;
    virtual void setConnectorEvseIds(const std::string& connector_evse_ids) = 0;
    virtual bool setIgnoredProfilePurposesOffline(const std::string& ignored_profile_purposes_offline) = 0;
    virtual bool setMeterPublicKey(std::int32_t connector_id, const std::string& public_key_pem) = 0;
    virtual void setOcspRequestInterval(std::int32_t ocsp_request_interval) = 0;
    virtual void setRetryBackoffRandomRange(std::int32_t retry_backoff_random_range) = 0;
    virtual void setRetryBackoffRepeatTimes(std::int32_t retry_backoff_repeat_times) = 0;
    virtual void setRetryBackoffWaitMinimum(std::int32_t retry_backoff_wait_minimum) = 0;
    virtual void setSeccLeafSubjectCommonName(const std::string& secc_leaf_subject_common_name) = 0;
    virtual void setSeccLeafSubjectCountry(const std::string& secc_leaf_subject_country) = 0;
    virtual void setSeccLeafSubjectOrganization(const std::string& secc_leaf_subject_organization) = 0;
    virtual void setStopTransactionIfUnlockNotSupported(bool stop_transaction_if_unlock_not_supported) = 0;
    virtual void setSupplyVoltage(std::int32_t supply_voltage) = 0;
    virtual void setVerifyCsmsAllowWildcards(bool verify_csms_allow_wildcards) = 0;
    virtual void setWaitForStopTransactionsOnResetTimeout(std::int32_t wait_for_stop_transactions_on_reset_timeout) = 0;

    // Core Profile
    virtual std::string getConnectorPhaseRotation() = 0;
    virtual std::string getMeterValuesAlignedData() = 0;
    virtual std::string getMeterValuesSampledData() = 0;
    virtual std::string getStopTxnAlignedData() = 0;
    virtual std::string getStopTxnSampledData() = 0;
    virtual std::string getSupportedFeatureProfiles() = 0;

    virtual bool getAuthorizeRemoteTxRequests() = 0;
    virtual bool getLocalAuthorizeOffline() = 0;
    virtual bool getLocalPreAuthorize() = 0;
    virtual bool getStopTransactionOnInvalidId() = 0;
    virtual bool getUnlockConnectorOnEVSideDisconnect() = 0;

    virtual std::int32_t getClockAlignedDataInterval() = 0;
    virtual std::int32_t getConnectionTimeOut() = 0;
    virtual std::int32_t getGetConfigurationMaxKeys() = 0;
    virtual std::int32_t getHeartbeatInterval() = 0;
    virtual std::int32_t getMeterValueSampleInterval() = 0;
    virtual std::int32_t getNumberOfConnectors() = 0;
    virtual std::int32_t getResetRetries() = 0;
    virtual std::int32_t getTransactionMessageAttempts() = 0;
    virtual std::int32_t getTransactionMessageRetryInterval() = 0;

    virtual std::vector<MeasurandWithPhase> getMeterValuesAlignedDataVector() = 0;
    virtual std::vector<MeasurandWithPhase> getMeterValuesSampledDataVector() = 0;

    virtual std::optional<bool> getAllowOfflineTxForUnknownId() = 0;
    virtual std::optional<bool> getAuthorizationCacheEnabled() = 0;
    virtual std::optional<bool> getReserveConnectorZeroSupported() = 0;
    virtual std::optional<bool> getStopTransactionOnEVSideDisconnect() = 0;
    virtual std::optional<std::int32_t> getBlinkRepeat() = 0;
    virtual std::optional<std::int32_t> getConnectorPhaseRotationMaxLength() = 0;
    virtual std::optional<std::int32_t> getLightIntensity() = 0;
    virtual std::optional<std::int32_t> getMaxEnergyOnInvalidId() = 0;
    virtual std::optional<std::int32_t> getMeterValuesAlignedDataMaxLength() = 0;
    virtual std::optional<std::int32_t> getMeterValuesSampledDataMaxLength() = 0;
    virtual std::optional<std::int32_t> getMinimumStatusDuration() = 0;
    virtual std::optional<std::int32_t> getStopTxnAlignedDataMaxLength() = 0;
    virtual std::optional<std::int32_t> getStopTxnSampledDataMaxLength() = 0;
    virtual std::optional<std::int32_t> getSupportedFeatureProfilesMaxLength() = 0;
    virtual std::optional<std::int32_t> getWebsocketPingInterval() = 0;

    virtual std::set<SupportedFeatureProfiles> getSupportedFeatureProfilesSet() = 0;

    virtual KeyValue getAuthorizeRemoteTxRequestsKeyValue() = 0;
    virtual KeyValue getClockAlignedDataIntervalKeyValue() = 0;
    virtual KeyValue getConnectionTimeOutKeyValue() = 0;
    virtual KeyValue getConnectorPhaseRotationKeyValue() = 0;
    virtual KeyValue getGetConfigurationMaxKeysKeyValue() = 0;
    virtual KeyValue getHeartbeatIntervalKeyValue() = 0;
    virtual KeyValue getLocalAuthorizeOfflineKeyValue() = 0;
    virtual KeyValue getLocalPreAuthorizeKeyValue() = 0;
    virtual KeyValue getMeterValuesAlignedDataKeyValue() = 0;
    virtual KeyValue getMeterValueSampleIntervalKeyValue() = 0;
    virtual KeyValue getMeterValuesSampledDataKeyValue() = 0;
    virtual KeyValue getNumberOfConnectorsKeyValue() = 0;
    virtual KeyValue getResetRetriesKeyValue() = 0;
    virtual KeyValue getStopTransactionOnInvalidIdKeyValue() = 0;
    virtual KeyValue getStopTxnAlignedDataKeyValue() = 0;
    virtual KeyValue getStopTxnSampledDataKeyValue() = 0;
    virtual KeyValue getSupportedFeatureProfilesKeyValue() = 0;
    virtual KeyValue getTransactionMessageAttemptsKeyValue() = 0;
    virtual KeyValue getTransactionMessageRetryIntervalKeyValue() = 0;
    virtual KeyValue getUnlockConnectorOnEVSideDisconnectKeyValue() = 0;

    virtual std::optional<KeyValue> getAllowOfflineTxForUnknownIdKeyValue() = 0;
    virtual std::optional<KeyValue> getAuthorizationCacheEnabledKeyValue() = 0;
    virtual std::optional<KeyValue> getBlinkRepeatKeyValue() = 0;
    virtual std::optional<KeyValue> getConnectorPhaseRotationMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getLightIntensityKeyValue() = 0;
    virtual std::optional<KeyValue> getMaxEnergyOnInvalidIdKeyValue() = 0;
    virtual std::optional<KeyValue> getMeterValuesAlignedDataMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getMeterValuesSampledDataMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getMinimumStatusDurationKeyValue() = 0;
    virtual std::optional<KeyValue> getReserveConnectorZeroSupportedKeyValue() = 0;
    virtual std::optional<KeyValue> getStopTransactionOnEVSideDisconnectKeyValue() = 0;
    virtual std::optional<KeyValue> getStopTxnAlignedDataMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getStopTxnSampledDataMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getSupportedFeatureProfilesMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getWebsocketPingIntervalKeyValue() = 0;

    virtual void setAllowOfflineTxForUnknownId(bool enabled) = 0;
    virtual void setAuthorizationCacheEnabled(bool enabled) = 0;
    virtual void setAuthorizeRemoteTxRequests(bool enabled) = 0;
    virtual void setBlinkRepeat(std::int32_t blink_repeat) = 0;
    virtual void setClockAlignedDataInterval(std::int32_t interval) = 0;
    virtual void setConnectionTimeOut(std::int32_t timeout) = 0;
    virtual void setConnectorPhaseRotation(const std::string& connector_phase_rotation) = 0;
    virtual void setHeartbeatInterval(std::int32_t interval) = 0;
    virtual void setLightIntensity(std::int32_t light_intensity) = 0;
    virtual void setLocalAuthorizeOffline(bool local_authorize_offline) = 0;
    virtual void setLocalPreAuthorize(bool local_pre_authorize) = 0;
    virtual void setMaxEnergyOnInvalidId(std::int32_t max_energy) = 0;
    virtual bool setMeterValuesAlignedData(const std::string& meter_values_aligned_data) = 0;
    virtual bool setMeterValuesSampledData(const std::string& meter_values_sampled_data) = 0;
    virtual void setMeterValueSampleInterval(std::int32_t interval) = 0;
    virtual void setMinimumStatusDuration(std::int32_t minimum_status_duration) = 0;
    virtual void setResetRetries(std::int32_t retries) = 0;
    virtual void setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id) = 0;
    virtual bool setStopTxnAlignedData(const std::string& stop_txn_aligned_data) = 0;
    virtual bool setStopTxnSampledData(const std::string& stop_txn_sampled_data) = 0;
    virtual void setTransactionMessageAttempts(std::int32_t attempts) = 0;
    virtual void setTransactionMessageRetryInterval(std::int32_t retry_interval) = 0;
    virtual void setUnlockConnectorOnEVSideDisconnect(bool unlock_connector_on_ev_side_disconnect) = 0;
    virtual void setWebsocketPingInterval(std::int32_t websocket_ping_interval) = 0;

    // Firmware Management Profile
    virtual std::optional<std::string> getSupportedFileTransferProtocols() = 0;
    virtual std::optional<KeyValue> getSupportedFileTransferProtocolsKeyValue() = 0;

    // Smart Charging Profile
    virtual std::string getChargingScheduleAllowedChargingRateUnit() = 0;
    virtual std::int32_t getChargeProfileMaxStackLevel() = 0;
    virtual std::int32_t getChargingScheduleMaxPeriods() = 0;
    virtual std::int32_t getMaxChargingProfilesInstalled() = 0;

    virtual std::optional<bool> getConnectorSwitch3to1PhaseSupported() = 0;

    virtual std::vector<ChargingRateUnit> getChargingScheduleAllowedChargingRateUnitVector() = 0;

    virtual KeyValue getChargeProfileMaxStackLevelKeyValue() = 0;
    virtual KeyValue getChargingScheduleAllowedChargingRateUnitKeyValue() = 0;
    virtual KeyValue getChargingScheduleMaxPeriodsKeyValue() = 0;
    virtual KeyValue getMaxChargingProfilesInstalledKeyValue() = 0;

    virtual std::optional<KeyValue> getConnectorSwitch3to1PhaseSupportedKeyValue() = 0;

    // Security Profile
    virtual bool getDisableSecurityEventNotifications() = 0;
    virtual std::int32_t getSecurityProfile() = 0;

    virtual std::optional<std::string> getAuthorizationKey() = 0;
    virtual std::optional<std::string> getCpoName() = 0;
    virtual std::optional<bool> getAdditionalRootCertificateCheck() = 0;
    virtual std::optional<std::int32_t> getCertificateSignedMaxChainSize() = 0;
    virtual std::optional<std::int32_t> getCertificateStoreMaxLength() = 0;

    virtual KeyValue getDisableSecurityEventNotificationsKeyValue() = 0;
    virtual KeyValue getSecurityProfileKeyValue() = 0;

    virtual std::optional<KeyValue> getAdditionalRootCertificateCheckKeyValue() = 0;
    virtual std::optional<KeyValue> getAuthorizationKeyKeyValue() = 0;
    virtual std::optional<KeyValue> getCertificateSignedMaxChainSizeKeyValue() = 0;
    virtual std::optional<KeyValue> getCertificateStoreMaxLengthKeyValue() = 0;
    virtual std::optional<KeyValue> getCpoNameKeyValue() = 0;

    virtual void setAuthorizationKey(const std::string& authorization_key) = 0;
    virtual void setCpoName(const std::string& cpo_name) = 0;
    virtual void setDisableSecurityEventNotifications(bool disable_security_event_notifications) = 0;
    virtual void setSecurityProfile(std::int32_t security_profile) = 0;

    // Local Auth List Management Profile
    virtual bool getLocalAuthListEnabled() = 0;
    virtual std::int32_t getLocalAuthListMaxLength() = 0;
    virtual std::int32_t getSendLocalListMaxLength() = 0;

    virtual KeyValue getLocalAuthListEnabledKeyValue() = 0;
    virtual KeyValue getLocalAuthListMaxLengthKeyValue() = 0;
    virtual KeyValue getSendLocalListMaxLengthKeyValue() = 0;

    virtual void setLocalAuthListEnabled(bool local_auth_list_enabled) = 0;

    // PnC
    virtual bool getContractValidationOffline() = 0;
    virtual bool getISO15118CertificateManagementEnabled() = 0;
    virtual bool getISO15118PnCEnabled() = 0;

    virtual std::optional<bool> getCentralContractValidationAllowed() = 0;
    virtual std::optional<std::int32_t> getCertSigningRepeatTimes() = 0;
    virtual std::optional<std::int32_t> getCertSigningWaitMinimum() = 0;

    virtual KeyValue getContractValidationOfflineKeyValue() = 0;
    virtual KeyValue getISO15118CertificateManagementEnabledKeyValue() = 0;
    virtual KeyValue getISO15118PnCEnabledKeyValue() = 0;

    virtual std::optional<KeyValue> getCentralContractValidationAllowedKeyValue() = 0;
    virtual std::optional<KeyValue> getCertSigningRepeatTimesKeyValue() = 0;
    virtual std::optional<KeyValue> getCertSigningWaitMinimumKeyValue() = 0;

    virtual void setContractValidationOffline(bool contract_validation_offline) = 0;
    virtual void setCentralContractValidationAllowed(bool central_contract_validation_allowed) = 0;
    virtual void setCertSigningRepeatTimes(std::int32_t cert_signing_repeat_times) = 0;
    virtual void setCertSigningWaitMinimum(std::int32_t cert_signing_wait_minimum) = 0;
    virtual void setISO15118CertificateManagementEnabled(bool iso15118_certificate_management_enabled) = 0;
    virtual void setISO15118PnCEnabled(bool iso15118_pnc_enabled) = 0;

    // California Pricing Requirements
    virtual bool getCustomDisplayCostAndPriceEnabled() = 0;

    virtual TariffMessage getDefaultTariffMessage(bool offline) = 0;

    virtual std::optional<std::string> getDefaultPrice() = 0;
    virtual std::optional<std::string> getDefaultPriceText(const std::string& language) = 0;
    virtual std::optional<std::string> getDisplayTimeOffset() = 0;
    virtual std::optional<std::string> getLanguage() = 0;
    virtual std::optional<std::string> getMultiLanguageSupportedLanguages() = 0;
    virtual std::optional<std::string> getNextTimeOffsetTransitionDateTime() = 0;
    virtual std::optional<std::string> getTimeOffsetNextTransition() = 0;
    virtual std::optional<bool> getCustomIdleFeeAfterStop() = 0;
    virtual std::optional<bool> getCustomMultiLanguageMessagesEnabled() = 0;
    virtual std::optional<std::int32_t> getWaitForSetUserPriceTimeout() = 0;
    virtual std::optional<std::uint32_t> getPriceNumberOfDecimalsForCostValues() = 0;

    virtual KeyValue getCustomDisplayCostAndPriceEnabledKeyValue() = 0;
    virtual KeyValue getDefaultPriceTextKeyValue(const std::string& language) = 0;

    virtual std::optional<KeyValue> getCustomIdleFeeAfterStopKeyValue() = 0;
    virtual std::optional<KeyValue> getCustomMultiLanguageMessagesEnabledKeyValue() = 0;
    virtual std::optional<KeyValue> getDefaultPriceKeyValue() = 0;
    virtual std::optional<KeyValue> getDisplayTimeOffsetKeyValue() = 0;
    virtual std::optional<KeyValue> getLanguageKeyValue() = 0;
    virtual std::optional<KeyValue> getMultiLanguageSupportedLanguagesKeyValue() = 0;
    virtual std::optional<KeyValue> getNextTimeOffsetTransitionDateTimeKeyValue() = 0;
    virtual std::optional<KeyValue> getPriceNumberOfDecimalsForCostValuesKeyValue() = 0;
    virtual std::optional<KeyValue> getTimeOffsetNextTransitionKeyValue() = 0;
    virtual std::optional<KeyValue> getWaitForSetUserPriceTimeoutKeyValue() = 0;

    virtual std::optional<std::vector<KeyValue>> getAllDefaultPriceTextKeyValues() = 0;

    virtual void setCustomIdleFeeAfterStop(bool value) = 0;
    virtual ConfigurationStatus setDefaultPrice(const std::string& value) = 0;
    virtual ConfigurationStatus setDefaultPriceText(const CiString<50>& key, const CiString<500>& value) = 0;
    virtual ConfigurationStatus setDisplayTimeOffset(const std::string& offset) = 0;
    virtual void setLanguage(const std::string& language) = 0;
    virtual ConfigurationStatus setNextTimeOffsetTransitionDateTime(const std::string& date_time) = 0;
    virtual ConfigurationStatus setTimeOffsetNextTransition(const std::string& offset) = 0;
    virtual void setWaitForSetUserPriceTimeout(std::int32_t wait_for_set_user_price_timeout) = 0;

    // Signed Meter Values

    // Custom
    virtual std::optional<KeyValue> getCustomKeyValue(const CiString<50>& key) = 0;
    virtual std::optional<KeyValue> get(const CiString<50>& key) = 0;
    virtual std::vector<KeyValue> get_all_key_value() = 0;

    virtual ConfigurationStatus setCustomKey(const CiString<50>& key, const CiString<500>& value, bool force) = 0;
    virtual std::optional<ConfigurationStatus> set(const CiString<50>& key, const CiString<500>& value) = 0;
};

} // namespace ocpp::v16

#endif // OCPP_V16_CHARGE_POINT_CONFIGURATION_INTERFACE_HPP
