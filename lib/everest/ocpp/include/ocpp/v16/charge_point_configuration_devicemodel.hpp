// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_CHARGE_POINT_CONFIGURATION_DEVICEMODEL_HPP
#define OCPP_V16_CHARGE_POINT_CONFIGURATION_DEVICEMODEL_HPP

#include <ocpp/v16/charge_point_configuration_base.hpp>
#include <ocpp/v16/charge_point_configuration_interface.hpp>
#include <ocpp/v16/types.hpp>

#include <map>
#include <set>
#include <vector>

namespace ocpp {
namespace v2 {
class DeviceModelInterface;
}
namespace v16 {

/// \brief contains the configuration of the charge point
class ChargePointConfigurationDeviceModel : private ChargePointConfigurationBase,
                                            public ChargePointConfigurationInterface {
public:
    using SetResult = v2::SetVariableStatusEnum;

protected:
    std::unique_ptr<v2::DeviceModelInterface> storage;

    SetResult setInternalAllowChargingProfileWithoutStartSchedule(const std::string& value);
    SetResult setInternalCentralSystemURI(const std::string& value);
    SetResult setInternalCompositeScheduleDefaultLimitAmps(const std::string& value);
    SetResult setInternalCompositeScheduleDefaultLimitWatts(const std::string& value);
    SetResult setInternalCompositeScheduleDefaultNumberPhases(const std::string& value);
    SetResult setInternalConnectorEvseIds(const std::string& value);
    SetResult setInternalIgnoredProfilePurposesOffline(const std::string& value);
    SetResult setInternalOcspRequestInterval(const std::string& value);
    SetResult setInternalRetryBackoffRandomRange(const std::string& value);
    SetResult setInternalRetryBackoffRepeatTimes(const std::string& value);
    SetResult setInternalRetryBackoffWaitMinimum(const std::string& value);
    SetResult setInternalSeccLeafSubjectCommonName(const std::string& value);
    SetResult setInternalSeccLeafSubjectCountry(const std::string& value);
    SetResult setInternalSeccLeafSubjectOrganization(const std::string& value);
    SetResult setInternalStopTransactionIfUnlockNotSupported(const std::string& value);
    SetResult setInternalSupplyVoltage(const std::string& value);
    SetResult setInternalVerifyCsmsAllowWildcards(const std::string& value);
    SetResult setInternalWaitForStopTransactionsOnResetTimeout(const std::string& value);

    SetResult setInternalAllowOfflineTxForUnknownId(const std::string& value);
    SetResult setInternalAuthorizationCacheEnabled(const std::string& value);
    SetResult setInternalAuthorizeRemoteTxRequests(const std::string& value);
    SetResult setInternalBlinkRepeat(const std::string& value);
    SetResult setInternalClockAlignedDataInterval(const std::string& value);
    SetResult setInternalConnectionTimeOut(const std::string& value);
    SetResult setInternalConnectorPhaseRotation(const std::string& value);
    SetResult setInternalHeartbeatInterval(const std::string& value);
    SetResult setInternalLightIntensity(const std::string& value);
    SetResult setInternalLocalAuthorizeOffline(const std::string& value);
    SetResult setInternalLocalPreAuthorize(const std::string& value);
    SetResult setInternalMaxEnergyOnInvalidId(const std::string& value);
    SetResult setInternalMeterValuesAlignedData(const std::string& value);
    SetResult setInternalMeterValuesSampledData(const std::string& value);
    SetResult setInternalMeterValueSampleInterval(const std::string& value);
    SetResult setInternalMinimumStatusDuration(const std::string& value);
    SetResult setInternalResetRetries(const std::string& value);
    SetResult setInternalStopTransactionOnInvalidId(const std::string& value);
    SetResult setInternalStopTxnAlignedData(const std::string& value);
    SetResult setInternalStopTxnSampledData(const std::string& value);
    SetResult setInternalTransactionMessageAttempts(const std::string& value);
    SetResult setInternalTransactionMessageRetryInterval(const std::string& value);
    SetResult setInternalUnlockConnectorOnEVSideDisconnect(const std::string& value);
    SetResult setInternalWebsocketPingInterval(const std::string& value);

    SetResult setInternalAuthorizationKey(const std::string& value);
    SetResult setInternalCpoName(const std::string& value);
    SetResult setInternalDisableSecurityEventNotifications(const std::string& value);
    SetResult setInternalSecurityProfile(const std::string& value);

    SetResult setInternalISO15118CertificateManagementEnabled(const std::string& value);
    SetResult setInternalLocalAuthListEnabled(const std::string& value);

    SetResult setInternalContractValidationOffline(const std::string& value);
    SetResult setInternalCentralContractValidationAllowed(const std::string& value);
    SetResult setInternalCertSigningRepeatTimes(const std::string& value);
    SetResult setInternalCertSigningWaitMinimum(const std::string& value);
    SetResult setInternalISO15118PnCEnabled(const std::string& value);

    SetResult setInternalCustomIdleFeeAfterStop(const std::string& value);
    SetResult setInternalDefaultPrice(const std::string& value);
    SetResult setInternalDefaultPriceText(const std::string& key, const std::string& value);
    SetResult setInternalDisplayTimeOffset(const std::string& value);
    SetResult setInternalLanguage(const std::string& value);
    SetResult setInternalNextTimeOffsetTransitionDateTime(const std::string& value);
    SetResult setInternalTimeOffsetNextTransition(const std::string& value);
    SetResult setInternalWaitForSetUserPriceTimeout(const std::string& value);

public:
    explicit ChargePointConfigurationDeviceModel(const std::string_view& ocpp_main_path,
                                                 std::unique_ptr<v2::DeviceModelInterface> device_model_interface);
    virtual ~ChargePointConfigurationDeviceModel() = default;

    // UserConfig and Internal
    std::string getChargeBoxSerialNumber() override;
    std::optional<CiString<25>> getChargePointSerialNumber() override;
    CiString<50> getFirmwareVersion() override;
    std::optional<CiString<20>> getICCID() override;
    std::optional<CiString<20>> getIMSI() override;
    std::optional<CiString<25>> getMeterSerialNumber() override;
    std::optional<CiString<25>> getMeterType() override;

    KeyValue getChargeBoxSerialNumberKeyValue() override;
    KeyValue getFirmwareVersionKeyValue() override;

    std::optional<KeyValue> getChargePointSerialNumberKeyValue() override;
    std::optional<KeyValue> getICCIDKeyValue() override;
    std::optional<KeyValue> getIMSIKeyValue() override;
    std::optional<KeyValue> getMeterSerialNumberKeyValue() override;
    std::optional<KeyValue> getMeterTypeKeyValue() override;

    void setChargepointInformation(const std::string& chargePointVendor, const std::string& chargePointModel,
                                   const std::optional<std::string>& chargePointSerialNumber,
                                   const std::optional<std::string>& chargeBoxSerialNumber,
                                   const std::optional<std::string>& firmwareVersion) override;
    void setChargepointMeterInformation(const std::optional<std::string>& meterSerialNumber,
                                        const std::optional<std::string>& meterType) override;
    void setChargepointModemInformation(const std::optional<std::string>& ICCID,
                                        const std::optional<std::string>& IMSI) override;

    // Internal
    std::string getCentralSystemURI() override;
    std::string getChargePointId() override;
    std::string getSupportedCiphers12() override;
    std::string getSupportedCiphers13() override;
    std::string getSupportedMeasurands() override;
    std::string getTLSKeylogFile() override;
    std::string getWebsocketPingPayload() override;

    CiString<20> getChargePointModel() override;
    CiString<20> getChargePointVendor() override;

    bool getAuthorizeConnectorZeroOnConnectorOne() override;
    bool getEnableTLSKeylog() override;
    bool getLogMessages() override;
    bool getLogMessagesRaw() override;
    bool getLogRotation() override;
    bool getLogRotationDateSuffix() override;
    bool getStopTransactionIfUnlockNotSupported() override;
    bool getUseSslDefaultVerifyPaths() override;
    bool getUseTPM() override;
    bool getUseTPMSeccLeafCertificate() override;
    bool getVerifyCsmsAllowWildcards() override;
    bool getVerifyCsmsCommonName() override;

    int getMaxMessageSize() override;

    std::int32_t getMaxCompositeScheduleDuration() override;
    std::int32_t getOcspRequestInterval() override;
    std::int32_t getRetryBackoffRandomRange() override;
    std::int32_t getRetryBackoffRepeatTimes() override;
    std::int32_t getRetryBackoffWaitMinimum() override;
    std::int32_t getWaitForStopTransactionsOnResetTimeout() override;
    std::int32_t getWebsocketPongTimeout() override;

    std::uint64_t getLogRotationMaximumFileCount() override;
    std::uint64_t getLogRotationMaximumFileSize() override;

    std::vector<std::string> getLogMessagesFormat() override;
    std::vector<ChargingProfilePurposeType> getIgnoredProfilePurposesOffline() override;
    std::vector<ChargingProfilePurposeType> getSupportedChargingProfilePurposeTypes() override;

    std::optional<std::string> getConnectorEvseIds() override;
    std::optional<std::string> getHostName() override;
    std::optional<std::string> getIFace() override;
    std::optional<std::string> getMessageTypesDiscardForQueueing() override;
    std::optional<std::string> getSeccLeafSubjectCommonName() override;
    std::optional<std::string> getSeccLeafSubjectCountry() override;
    std::optional<std::string> getSeccLeafSubjectOrganization() override;
    std::optional<bool> getAllowChargingProfileWithoutStartSchedule() override;
    std::optional<bool> getQueueAllMessages() override;
    std::optional<int> getMessageQueueSizeThreshold() override;
    std::optional<std::int32_t> getCompositeScheduleDefaultLimitAmps() override;
    std::optional<std::int32_t> getCompositeScheduleDefaultLimitWatts() override;
    std::optional<std::int32_t> getCompositeScheduleDefaultNumberPhases() override;
    std::optional<std::int32_t> getSupplyVoltage() override;
    std::optional<std::vector<KeyValue>> getAllMeterPublicKeyKeyValues() override;

    std::set<MessageType> getSupportedMessageTypesSending() override;
    std::set<MessageType> getSupportedMessageTypesReceiving() override;

    KeyValue getAuthorizeConnectorZeroOnConnectorOneKeyValue() override;
    KeyValue getCentralSystemURIKeyValue() override;
    KeyValue getChargePointIdKeyValue() override;
    KeyValue getChargePointModelKeyValue() override;
    KeyValue getChargePointVendorKeyValue() override;
    KeyValue getEnableTLSKeylogKeyValue() override;
    KeyValue getLogMessagesFormatKeyValue() override;
    KeyValue getLogMessagesKeyValue() override;
    KeyValue getLogMessagesRawKeyValue() override;
    KeyValue getLogRotationDateSuffixKeyValue() override;
    KeyValue getLogRotationKeyValue() override;
    KeyValue getLogRotationMaximumFileCountKeyValue() override;
    KeyValue getLogRotationMaximumFileSizeKeyValue() override;
    KeyValue getMaxCompositeScheduleDurationKeyValue() override;
    KeyValue getMaxMessageSizeKeyValue() override;
    KeyValue getOcspRequestIntervalKeyValue() override;
    KeyValue getRetryBackoffRandomRangeKeyValue() override;
    KeyValue getRetryBackoffRepeatTimesKeyValue() override;
    KeyValue getRetryBackoffWaitMinimumKeyValue() override;
    KeyValue getStopTransactionIfUnlockNotSupportedKeyValue() override;
    KeyValue getSupportedChargingProfilePurposeTypesKeyValue() override;
    KeyValue getSupportedCiphers12KeyValue() override;
    KeyValue getSupportedCiphers13KeyValue() override;
    KeyValue getSupportedMeasurandsKeyValue() override;
    KeyValue getTLSKeylogFileKeyValue() override;
    KeyValue getUseSslDefaultVerifyPathsKeyValue() override;
    KeyValue getUseTPMKeyValue() override;
    KeyValue getUseTPMSeccLeafCertificateKeyValue() override;
    KeyValue getVerifyCsmsAllowWildcardsKeyValue() override;
    KeyValue getVerifyCsmsCommonNameKeyValue() override;
    KeyValue getWaitForStopTransactionsOnResetTimeoutKeyValue() override;
    KeyValue getWebsocketPingPayloadKeyValue() override;
    KeyValue getWebsocketPongTimeoutKeyValue() override;

    std::optional<KeyValue> getAllowChargingProfileWithoutStartScheduleKeyValue() override;
    std::optional<KeyValue> getCompositeScheduleDefaultLimitAmpsKeyValue() override;
    std::optional<KeyValue> getCompositeScheduleDefaultLimitWattsKeyValue() override;
    std::optional<KeyValue> getCompositeScheduleDefaultNumberPhasesKeyValue() override;
    std::optional<KeyValue> getConnectorEvseIdsKeyValue() override;
    std::optional<KeyValue> getHostNameKeyValue() override;
    std::optional<KeyValue> getIFaceKeyValue() override;
    std::optional<KeyValue> getIgnoredProfilePurposesOfflineKeyValue() override;
    std::optional<KeyValue> getMessageTypesDiscardForQueueingKeyValue() override;
    std::optional<KeyValue> getMessageQueueSizeThresholdKeyValue() override;
    std::optional<KeyValue> getPublicKeyKeyValue(std::uint32_t connector_id) override;
    std::optional<KeyValue> getQueueAllMessagesKeyValue() override;
    std::optional<KeyValue> getSeccLeafSubjectCommonNameKeyValue() override;
    std::optional<KeyValue> getSeccLeafSubjectCountryKeyValue() override;
    std::optional<KeyValue> getSeccLeafSubjectOrganizationKeyValue() override;
    std::optional<KeyValue> getSupplyVoltageKeyValue() override;

    void setAllowChargingProfileWithoutStartSchedule(bool allow) override;
    void setCentralSystemURI(const std::string& ocpp_uri) override;
    void setCompositeScheduleDefaultLimitAmps(std::int32_t limit_amps) override;
    void setCompositeScheduleDefaultLimitWatts(std::int32_t limit_watts) override;
    void setCompositeScheduleDefaultNumberPhases(std::int32_t number_phases) override;
    void setConnectorEvseIds(const std::string& connector_evse_ids) override;
    bool setIgnoredProfilePurposesOffline(const std::string& ignored_profile_purposes_offline) override;
    bool setMeterPublicKey(std::int32_t connector_id, const std::string& public_key_pem) override;
    void setOcspRequestInterval(std::int32_t ocsp_request_interval) override;
    void setRetryBackoffRandomRange(std::int32_t retry_backoff_random_range) override;
    void setRetryBackoffRepeatTimes(std::int32_t retry_backoff_repeat_times) override;
    void setRetryBackoffWaitMinimum(std::int32_t retry_backoff_wait_minimum) override;
    void setSeccLeafSubjectCommonName(const std::string& secc_leaf_subject_common_name) override;
    void setSeccLeafSubjectCountry(const std::string& secc_leaf_subject_country) override;
    void setSeccLeafSubjectOrganization(const std::string& secc_leaf_subject_organization) override;
    void setStopTransactionIfUnlockNotSupported(bool stop_transaction_if_unlock_not_supported) override;
    void setSupplyVoltage(std::int32_t supply_voltage) override;
    void setVerifyCsmsAllowWildcards(bool verify_csms_allow_wildcards) override;
    void setWaitForStopTransactionsOnResetTimeout(std::int32_t wait_for_stop_transactions_on_reset_timeout) override;

    // Core Profile
    std::string getConnectorPhaseRotation() override;
    std::string getMeterValuesAlignedData() override;
    std::string getMeterValuesSampledData() override;
    std::string getStopTxnAlignedData() override;
    std::string getStopTxnSampledData() override;
    std::string getSupportedFeatureProfiles() override;

    bool getAuthorizeRemoteTxRequests() override;
    bool getLocalAuthorizeOffline() override;
    bool getLocalPreAuthorize() override;
    bool getStopTransactionOnInvalidId() override;
    bool getUnlockConnectorOnEVSideDisconnect() override;

    std::int32_t getClockAlignedDataInterval() override;
    std::int32_t getConnectionTimeOut() override;
    std::int32_t getGetConfigurationMaxKeys() override;
    std::int32_t getHeartbeatInterval() override;
    std::int32_t getMeterValueSampleInterval() override;
    std::int32_t getNumberOfConnectors() override;
    std::int32_t getResetRetries() override;
    std::int32_t getTransactionMessageAttempts() override;
    std::int32_t getTransactionMessageRetryInterval() override;

    std::vector<MeasurandWithPhase> getMeterValuesAlignedDataVector() override;
    std::vector<MeasurandWithPhase> getMeterValuesSampledDataVector() override;

    std::optional<bool> getAllowOfflineTxForUnknownId() override;
    std::optional<bool> getAuthorizationCacheEnabled() override;
    std::optional<bool> getReserveConnectorZeroSupported() override;
    std::optional<bool> getStopTransactionOnEVSideDisconnect() override;
    std::optional<std::int32_t> getBlinkRepeat() override;
    std::optional<std::int32_t> getConnectorPhaseRotationMaxLength() override;
    std::optional<std::int32_t> getLightIntensity() override;
    std::optional<std::int32_t> getMaxEnergyOnInvalidId() override;
    std::optional<std::int32_t> getMeterValuesAlignedDataMaxLength() override;
    std::optional<std::int32_t> getMeterValuesSampledDataMaxLength() override;
    std::optional<std::int32_t> getMinimumStatusDuration() override;
    std::optional<std::int32_t> getStopTxnAlignedDataMaxLength() override;
    std::optional<std::int32_t> getStopTxnSampledDataMaxLength() override;
    std::optional<std::int32_t> getSupportedFeatureProfilesMaxLength() override;
    std::optional<std::int32_t> getWebsocketPingInterval() override;

    std::set<SupportedFeatureProfiles> getSupportedFeatureProfilesSet() override;

    KeyValue getAuthorizeRemoteTxRequestsKeyValue() override;
    KeyValue getClockAlignedDataIntervalKeyValue() override;
    KeyValue getConnectionTimeOutKeyValue() override;
    KeyValue getConnectorPhaseRotationKeyValue() override;
    KeyValue getGetConfigurationMaxKeysKeyValue() override;
    KeyValue getHeartbeatIntervalKeyValue() override;
    KeyValue getLocalAuthorizeOfflineKeyValue() override;
    KeyValue getLocalPreAuthorizeKeyValue() override;
    KeyValue getMeterValuesAlignedDataKeyValue() override;
    KeyValue getMeterValueSampleIntervalKeyValue() override;
    KeyValue getMeterValuesSampledDataKeyValue() override;
    KeyValue getNumberOfConnectorsKeyValue() override;
    KeyValue getResetRetriesKeyValue() override;
    KeyValue getStopTransactionOnInvalidIdKeyValue() override;
    KeyValue getStopTxnAlignedDataKeyValue() override;
    KeyValue getStopTxnSampledDataKeyValue() override;
    KeyValue getSupportedFeatureProfilesKeyValue() override;
    KeyValue getTransactionMessageAttemptsKeyValue() override;
    KeyValue getTransactionMessageRetryIntervalKeyValue() override;
    KeyValue getUnlockConnectorOnEVSideDisconnectKeyValue() override;

    std::optional<KeyValue> getAllowOfflineTxForUnknownIdKeyValue() override;
    std::optional<KeyValue> getAuthorizationCacheEnabledKeyValue() override;
    std::optional<KeyValue> getBlinkRepeatKeyValue() override;
    std::optional<KeyValue> getConnectorPhaseRotationMaxLengthKeyValue() override;
    std::optional<KeyValue> getLightIntensityKeyValue() override;
    std::optional<KeyValue> getMaxEnergyOnInvalidIdKeyValue() override;
    std::optional<KeyValue> getMeterValuesAlignedDataMaxLengthKeyValue() override;
    std::optional<KeyValue> getMeterValuesSampledDataMaxLengthKeyValue() override;
    std::optional<KeyValue> getMinimumStatusDurationKeyValue() override;
    std::optional<KeyValue> getReserveConnectorZeroSupportedKeyValue() override;
    std::optional<KeyValue> getStopTransactionOnEVSideDisconnectKeyValue() override;
    std::optional<KeyValue> getStopTxnAlignedDataMaxLengthKeyValue() override;
    std::optional<KeyValue> getStopTxnSampledDataMaxLengthKeyValue() override;
    std::optional<KeyValue> getSupportedFeatureProfilesMaxLengthKeyValue() override;
    std::optional<KeyValue> getWebsocketPingIntervalKeyValue() override;

    void setAllowOfflineTxForUnknownId(bool enabled) override;
    void setAuthorizationCacheEnabled(bool enabled) override;
    void setAuthorizeRemoteTxRequests(bool enabled) override;
    void setBlinkRepeat(std::int32_t blink_repeat) override;
    void setClockAlignedDataInterval(std::int32_t interval) override;
    void setConnectionTimeOut(std::int32_t timeout) override;
    void setConnectorPhaseRotation(const std::string& connector_phase_rotation) override;
    void setHeartbeatInterval(std::int32_t interval) override;
    void setLightIntensity(std::int32_t light_intensity) override;
    void setLocalAuthorizeOffline(bool local_authorize_offline) override;
    void setLocalPreAuthorize(bool local_pre_authorize) override;
    void setMaxEnergyOnInvalidId(std::int32_t max_energy) override;
    bool setMeterValuesAlignedData(const std::string& meter_values_aligned_data) override;
    bool setMeterValuesSampledData(const std::string& meter_values_sampled_data) override;
    void setMeterValueSampleInterval(std::int32_t interval) override;
    void setMinimumStatusDuration(std::int32_t minimum_status_duration) override;
    void setResetRetries(std::int32_t retries) override;
    void setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id) override;
    bool setStopTxnAlignedData(const std::string& stop_txn_aligned_data) override;
    bool setStopTxnSampledData(const std::string& stop_txn_sampled_data) override;
    void setTransactionMessageAttempts(std::int32_t attempts) override;
    void setTransactionMessageRetryInterval(std::int32_t retry_interval) override;
    void setUnlockConnectorOnEVSideDisconnect(bool unlock_connector_on_ev_side_disconnect) override;
    void setWebsocketPingInterval(std::int32_t websocket_ping_interval) override;

    // Firmware Management Profile
    std::optional<std::string> getSupportedFileTransferProtocols() override;
    std::optional<KeyValue> getSupportedFileTransferProtocolsKeyValue() override;

    // Smart Charging Profile
    std::string getChargingScheduleAllowedChargingRateUnit() override;
    std::int32_t getChargeProfileMaxStackLevel() override;
    std::int32_t getChargingScheduleMaxPeriods() override;
    std::int32_t getMaxChargingProfilesInstalled() override;

    std::optional<bool> getConnectorSwitch3to1PhaseSupported() override;

    std::vector<ChargingRateUnit> getChargingScheduleAllowedChargingRateUnitVector() override;

    KeyValue getChargeProfileMaxStackLevelKeyValue() override;
    KeyValue getChargingScheduleAllowedChargingRateUnitKeyValue() override;
    KeyValue getChargingScheduleMaxPeriodsKeyValue() override;
    KeyValue getMaxChargingProfilesInstalledKeyValue() override;

    std::optional<KeyValue> getConnectorSwitch3to1PhaseSupportedKeyValue() override;

    // Security Profile
    bool getDisableSecurityEventNotifications() override;
    std::int32_t getSecurityProfile() override;

    std::optional<std::string> getAuthorizationKey() override;
    std::optional<std::string> getCpoName() override;
    std::optional<bool> getAdditionalRootCertificateCheck() override;
    std::optional<std::int32_t> getCertificateSignedMaxChainSize() override;
    std::optional<std::int32_t> getCertificateStoreMaxLength() override;

    KeyValue getDisableSecurityEventNotificationsKeyValue() override;
    KeyValue getSecurityProfileKeyValue() override;

    std::optional<KeyValue> getAdditionalRootCertificateCheckKeyValue() override;
    std::optional<KeyValue> getAuthorizationKeyKeyValue() override;
    std::optional<KeyValue> getCertificateSignedMaxChainSizeKeyValue() override;
    std::optional<KeyValue> getCertificateStoreMaxLengthKeyValue() override;
    std::optional<KeyValue> getCpoNameKeyValue() override;

    void setAuthorizationKey(const std::string& authorization_key) override;
    void setCpoName(const std::string& cpo_name) override;
    void setDisableSecurityEventNotifications(bool disable_security_event_notifications) override;
    void setSecurityProfile(std::int32_t security_profile) override;

    // Local Auth List Management Profile
    bool getLocalAuthListEnabled() override;
    std::int32_t getLocalAuthListMaxLength() override;
    std::int32_t getSendLocalListMaxLength() override;

    KeyValue getLocalAuthListEnabledKeyValue() override;
    KeyValue getLocalAuthListMaxLengthKeyValue() override;
    KeyValue getSendLocalListMaxLengthKeyValue() override;

    void setLocalAuthListEnabled(bool local_auth_list_enabled) override;

    // PnC
    bool getContractValidationOffline() override;
    bool getISO15118CertificateManagementEnabled() override;
    bool getISO15118PnCEnabled() override;

    std::optional<bool> getCentralContractValidationAllowed() override;
    std::optional<std::int32_t> getCertSigningRepeatTimes() override;
    std::optional<std::int32_t> getCertSigningWaitMinimum() override;

    KeyValue getContractValidationOfflineKeyValue() override;
    KeyValue getISO15118CertificateManagementEnabledKeyValue() override;
    KeyValue getISO15118PnCEnabledKeyValue() override;

    std::optional<KeyValue> getCentralContractValidationAllowedKeyValue() override;
    std::optional<KeyValue> getCertSigningRepeatTimesKeyValue() override;
    std::optional<KeyValue> getCertSigningWaitMinimumKeyValue() override;

    void setContractValidationOffline(bool contract_validation_offline) override;
    void setCentralContractValidationAllowed(bool central_contract_validation_allowed) override;
    void setCertSigningRepeatTimes(std::int32_t cert_signing_repeat_times) override;
    void setCertSigningWaitMinimum(std::int32_t cert_signing_wait_minimum) override;
    void setISO15118CertificateManagementEnabled(bool iso15118_certificate_management_enabled) override;
    void setISO15118PnCEnabled(bool iso15118_pnc_enabled) override;

    // California Pricing Requirements
    bool getCustomDisplayCostAndPriceEnabled() override;

    TariffMessage getDefaultTariffMessage(bool offline) override;

    std::optional<std::string> getDefaultPrice() override;
    std::optional<std::string> getDefaultPriceText(const std::string& language) override;
    std::optional<std::string> getDisplayTimeOffset() override;
    std::optional<std::string> getLanguage() override;
    std::optional<std::string> getMultiLanguageSupportedLanguages() override;
    std::optional<std::string> getNextTimeOffsetTransitionDateTime() override;
    std::optional<std::string> getTimeOffsetNextTransition() override;
    std::optional<bool> getCustomIdleFeeAfterStop() override;
    std::optional<bool> getCustomMultiLanguageMessagesEnabled() override;
    std::optional<std::int32_t> getWaitForSetUserPriceTimeout() override;
    std::optional<std::uint32_t> getPriceNumberOfDecimalsForCostValues() override;

    KeyValue getCustomDisplayCostAndPriceEnabledKeyValue() override;
    KeyValue getDefaultPriceTextKeyValue(const std::string& language) override;

    std::optional<KeyValue> getCustomIdleFeeAfterStopKeyValue() override;
    std::optional<KeyValue> getCustomMultiLanguageMessagesEnabledKeyValue() override;
    std::optional<KeyValue> getDefaultPriceKeyValue() override;
    std::optional<KeyValue> getDisplayTimeOffsetKeyValue() override;
    std::optional<KeyValue> getLanguageKeyValue() override;
    std::optional<KeyValue> getMultiLanguageSupportedLanguagesKeyValue() override;
    std::optional<KeyValue> getNextTimeOffsetTransitionDateTimeKeyValue() override;
    std::optional<KeyValue> getPriceNumberOfDecimalsForCostValuesKeyValue() override;
    std::optional<KeyValue> getTimeOffsetNextTransitionKeyValue() override;
    std::optional<KeyValue> getWaitForSetUserPriceTimeoutKeyValue() override;

    std::optional<std::vector<KeyValue>> getAllDefaultPriceTextKeyValues() override;

    void setCustomIdleFeeAfterStop(bool value) override;
    ConfigurationStatus setDefaultPrice(const std::string& value) override;
    ConfigurationStatus setDefaultPriceText(const CiString<50>& key, const CiString<500>& value) override;
    ConfigurationStatus setDisplayTimeOffset(const std::string& offset) override;
    void setLanguage(const std::string& language) override;
    ConfigurationStatus setNextTimeOffsetTransitionDateTime(const std::string& date_time) override;
    ConfigurationStatus setTimeOffsetNextTransition(const std::string& offset) override;
    void setWaitForSetUserPriceTimeout(std::int32_t wait_for_set_user_price_timeout) override;

    // Custom
    std::optional<KeyValue> getCustomKeyValue(const CiString<50>& key) override;
    std::optional<KeyValue> get(const CiString<50>& key) override;
    std::vector<KeyValue> get_all_key_value() override;

    ConfigurationStatus setCustomKey(const CiString<50>& key, const CiString<500>& value, bool force) override;
    std::optional<ConfigurationStatus> set(const CiString<50>& key, const CiString<500>& value) override;
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CHARGE_POINT_CONFIGURATION_DEVICEMODEL_HPP
