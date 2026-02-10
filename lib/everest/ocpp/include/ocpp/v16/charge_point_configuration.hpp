// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_CHARGE_POINT_CONFIGURATION_HPP
#define OCPP_V16_CHARGE_POINT_CONFIGURATION_HPP

#include <map>
#include <mutex>
#include <optional>
#include <set>

#include <ocpp/common/support_older_cpp_versions.hpp>
#include <ocpp/v16/charge_point_configuration_base.hpp>
#include <ocpp/v16/charge_point_configuration_interface.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

/// \brief contains the configuration of the charge point
class ChargePointConfiguration : private ChargePointConfigurationBase, public ChargePointConfigurationInterface {
private:
    json config;
    json custom_schema;
    json internal_schema;
    bool core_schema_unlock_connector_on_ev_side_disconnect_ro_value;
    fs::path user_config_path;

    std::recursive_mutex configuration_mutex;

    bool validate_measurands(const json& config);
    json get_user_config();
    void setInUserConfig(const std::string& profile, const std::string& key, json value);

    void setChargepointInformationProperty(json& user_config, const std::string& key,
                                           const std::optional<std::string>& value);

public:
    ChargePointConfiguration(const std::string& config, const fs::path& ocpp_main_path,
                             const fs::path& user_config_path);
    ChargePointConfiguration() = delete;
    ChargePointConfiguration(const ChargePointConfiguration&) = delete;
    ChargePointConfiguration(ChargePointConfiguration&&) = delete;
    ChargePointConfiguration& operator=(const ChargePointConfiguration&) = delete;
    ChargePointConfiguration& operator=(ChargePointConfiguration&&) = delete;
    virtual ~ChargePointConfiguration() = default;

    void setChargepointInformation(const std::string& chargePointVendor, const std::string& chargePointModel,
                                   const std::optional<std::string>& chargePointSerialNumber,
                                   const std::optional<std::string>& chargeBoxSerialNumber,
                                   const std::optional<std::string>& firmwareVersion) override;
    void setChargepointModemInformation(const std::optional<std::string>& ICCID,
                                        const std::optional<std::string>& IMSI) override;
    void setChargepointMeterInformation(const std::optional<std::string>& meterSerialNumber,
                                        const std::optional<std::string>& meterType) override;
    // Internal config options
    std::string getChargePointId() override;
    KeyValue getChargePointIdKeyValue() override;
    std::string getCentralSystemURI() override;
    void setCentralSystemURI(const std::string& ocpp_uri) override;
    KeyValue getCentralSystemURIKeyValue() override;
    std::string getChargeBoxSerialNumber() override;
    KeyValue getChargeBoxSerialNumberKeyValue() override;
    CiString<20> getChargePointModel() override;
    KeyValue getChargePointModelKeyValue() override;
    std::optional<CiString<25>> getChargePointSerialNumber() override;
    std::optional<KeyValue> getChargePointSerialNumberKeyValue() override;
    CiString<20> getChargePointVendor() override;
    KeyValue getChargePointVendorKeyValue() override;
    CiString<50> getFirmwareVersion() override;
    KeyValue getFirmwareVersionKeyValue() override;
    std::optional<CiString<20>> getICCID() override;
    std::optional<KeyValue> getICCIDKeyValue() override;
    std::optional<CiString<20>> getIMSI() override;
    std::optional<KeyValue> getIMSIKeyValue() override;
    std::optional<CiString<25>> getMeterSerialNumber() override;
    std::optional<KeyValue> getMeterSerialNumberKeyValue() override;
    std::optional<CiString<25>> getMeterType() override;
    std::optional<KeyValue> getMeterTypeKeyValue() override;
    bool getAuthorizeConnectorZeroOnConnectorOne() override;
    KeyValue getAuthorizeConnectorZeroOnConnectorOneKeyValue() override;
    bool getLogMessages() override;
    KeyValue getLogMessagesKeyValue() override;
    bool getLogMessagesRaw() override;
    KeyValue getLogMessagesRawKeyValue() override;
    std::vector<std::string> getLogMessagesFormat() override;
    KeyValue getLogMessagesFormatKeyValue() override;
    bool getLogRotation() override;
    KeyValue getLogRotationKeyValue() override;
    bool getLogRotationDateSuffix() override;
    KeyValue getLogRotationDateSuffixKeyValue() override;
    uint64_t getLogRotationMaximumFileSize() override;
    KeyValue getLogRotationMaximumFileSizeKeyValue() override;
    uint64_t getLogRotationMaximumFileCount() override;
    KeyValue getLogRotationMaximumFileCountKeyValue() override;
    std::vector<ChargingProfilePurposeType> getSupportedChargingProfilePurposeTypes() override;
    KeyValue getSupportedChargingProfilePurposeTypesKeyValue() override;
    std::vector<ChargingProfilePurposeType> getIgnoredProfilePurposesOffline() override;
    std::optional<KeyValue> getIgnoredProfilePurposesOfflineKeyValue() override;
    bool setIgnoredProfilePurposesOffline(const std::string& ignored_profile_purposes_offline) override;
    std::int32_t getMaxCompositeScheduleDuration() override;
    KeyValue getMaxCompositeScheduleDurationKeyValue() override;
    std::optional<std::int32_t> getCompositeScheduleDefaultLimitAmps() override;
    std::optional<KeyValue> getCompositeScheduleDefaultLimitAmpsKeyValue() override;
    void setCompositeScheduleDefaultLimitAmps(std::int32_t limit_amps) override;
    std::optional<std::int32_t> getCompositeScheduleDefaultLimitWatts() override;
    std::optional<KeyValue> getCompositeScheduleDefaultLimitWattsKeyValue() override;
    void setCompositeScheduleDefaultLimitWatts(std::int32_t limit_watts) override;
    std::optional<std::int32_t> getCompositeScheduleDefaultNumberPhases() override;
    std::optional<KeyValue> getCompositeScheduleDefaultNumberPhasesKeyValue() override;
    void setCompositeScheduleDefaultNumberPhases(std::int32_t number_phases) override;
    std::optional<std::int32_t> getSupplyVoltage() override;
    std::optional<KeyValue> getSupplyVoltageKeyValue() override;
    void setSupplyVoltage(std::int32_t supply_voltage) override;
    std::string getSupportedCiphers12() override;
    KeyValue getSupportedCiphers12KeyValue() override;
    std::string getSupportedCiphers13() override;
    KeyValue getSupportedCiphers13KeyValue() override;
    bool getUseSslDefaultVerifyPaths() override;
    KeyValue getUseSslDefaultVerifyPathsKeyValue() override;
    bool getVerifyCsmsCommonName() override;
    KeyValue getVerifyCsmsCommonNameKeyValue() override;
    bool getVerifyCsmsAllowWildcards() override;
    void setVerifyCsmsAllowWildcards(bool verify_csms_allow_wildcards) override;
    KeyValue getVerifyCsmsAllowWildcardsKeyValue() override;
    bool getUseTPM() override;
    KeyValue getUseTPMKeyValue() override;
    bool getUseTPMSeccLeafCertificate() override;
    KeyValue getUseTPMSeccLeafCertificateKeyValue() override;

    std::string getSupportedMeasurands() override;
    KeyValue getSupportedMeasurandsKeyValue() override;
    int getMaxMessageSize() override;
    KeyValue getMaxMessageSizeKeyValue() override;

    bool getEnableTLSKeylog() override;
    KeyValue getEnableTLSKeylogKeyValue() override;
    std::string getTLSKeylogFile() override;
    KeyValue getTLSKeylogFileKeyValue() override;

    bool getStopTransactionIfUnlockNotSupported() override;
    void setStopTransactionIfUnlockNotSupported(bool stop_transaction_if_unlock_not_supported) override;
    KeyValue getStopTransactionIfUnlockNotSupportedKeyValue() override;

    std::int32_t getRetryBackoffRandomRange() override;
    void setRetryBackoffRandomRange(std::int32_t retry_backoff_random_range) override;
    KeyValue getRetryBackoffRandomRangeKeyValue() override;

    std::int32_t getRetryBackoffRepeatTimes() override;
    void setRetryBackoffRepeatTimes(std::int32_t retry_backoff_repeat_times) override;
    KeyValue getRetryBackoffRepeatTimesKeyValue() override;

    std::int32_t getRetryBackoffWaitMinimum() override;
    void setRetryBackoffWaitMinimum(std::int32_t retry_backoff_wait_minimum) override;
    KeyValue getRetryBackoffWaitMinimumKeyValue() override;

    std::set<MessageType> getSupportedMessageTypesSending() override;
    std::set<MessageType> getSupportedMessageTypesReceiving() override;

    std::string getWebsocketPingPayload() override;
    KeyValue getWebsocketPingPayloadKeyValue() override;

    std::int32_t getWebsocketPongTimeout() override;
    KeyValue getWebsocketPongTimeoutKeyValue() override;

    std::optional<std::string> getHostName() override;
    std::optional<KeyValue> getHostNameKeyValue() override;

    std::optional<std::string> getIFace() override;
    std::optional<KeyValue> getIFaceKeyValue() override;

    std::optional<bool> getQueueAllMessages() override;
    std::optional<KeyValue> getQueueAllMessagesKeyValue() override;

    std::optional<std::string> getMessageTypesDiscardForQueueing() override;
    std::optional<KeyValue> getMessageTypesDiscardForQueueingKeyValue() override;

    std::optional<int> getMessageQueueSizeThreshold() override;
    std::optional<KeyValue> getMessageQueueSizeThresholdKeyValue() override;

    // Core Profile - optional
    std::optional<bool> getAllowOfflineTxForUnknownId() override;
    void setAllowOfflineTxForUnknownId(bool enabled) override;
    std::optional<KeyValue> getAllowOfflineTxForUnknownIdKeyValue() override;

    // Core Profile - optional
    std::optional<bool> getAuthorizationCacheEnabled() override;
    void setAuthorizationCacheEnabled(bool enabled) override;
    std::optional<KeyValue> getAuthorizationCacheEnabledKeyValue() override;

    // Core Profile
    bool getAuthorizeRemoteTxRequests() override;
    void setAuthorizeRemoteTxRequests(bool enabled) override;
    KeyValue getAuthorizeRemoteTxRequestsKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getBlinkRepeat() override;
    void setBlinkRepeat(std::int32_t blink_repeat) override;
    std::optional<KeyValue> getBlinkRepeatKeyValue() override;

    // Core Profile
    std::int32_t getClockAlignedDataInterval() override;
    void setClockAlignedDataInterval(std::int32_t interval) override;
    KeyValue getClockAlignedDataIntervalKeyValue() override;

    // Core Profile
    std::int32_t getConnectionTimeOut() override;
    void setConnectionTimeOut(std::int32_t timeout) override;
    KeyValue getConnectionTimeOutKeyValue() override;

    // Core Profile
    std::string getConnectorPhaseRotation() override;
    void setConnectorPhaseRotation(const std::string& connector_phase_rotation) override;
    KeyValue getConnectorPhaseRotationKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getConnectorPhaseRotationMaxLength() override;
    std::optional<KeyValue> getConnectorPhaseRotationMaxLengthKeyValue() override;

    // Core Profile
    std::int32_t getGetConfigurationMaxKeys() override;
    KeyValue getGetConfigurationMaxKeysKeyValue() override;

    // Core Profile
    std::int32_t getHeartbeatInterval() override;
    void setHeartbeatInterval(std::int32_t interval) override;
    KeyValue getHeartbeatIntervalKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getLightIntensity() override;
    void setLightIntensity(std::int32_t light_intensity) override;
    std::optional<KeyValue> getLightIntensityKeyValue() override;

    // Core Profile
    bool getLocalAuthorizeOffline() override;
    void setLocalAuthorizeOffline(bool local_authorize_offline) override;
    KeyValue getLocalAuthorizeOfflineKeyValue() override;

    // Core Profile
    bool getLocalPreAuthorize() override;
    void setLocalPreAuthorize(bool local_pre_authorize) override;
    KeyValue getLocalPreAuthorizeKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getMaxEnergyOnInvalidId() override;
    void setMaxEnergyOnInvalidId(std::int32_t max_energy) override;
    std::optional<KeyValue> getMaxEnergyOnInvalidIdKeyValue() override;

    // Core Profile
    std::string getMeterValuesAlignedData() override;
    bool setMeterValuesAlignedData(const std::string& meter_values_aligned_data) override;
    KeyValue getMeterValuesAlignedDataKeyValue() override;
    std::vector<MeasurandWithPhase> getMeterValuesAlignedDataVector() override;

    // Core Profile - optional
    std::optional<std::int32_t> getMeterValuesAlignedDataMaxLength() override;
    std::optional<KeyValue> getMeterValuesAlignedDataMaxLengthKeyValue() override;

    // Core Profile
    std::string getMeterValuesSampledData() override;
    bool setMeterValuesSampledData(const std::string& meter_values_sampled_data) override;
    KeyValue getMeterValuesSampledDataKeyValue() override;
    std::vector<MeasurandWithPhase> getMeterValuesSampledDataVector() override;

    // Core Profile - optional
    std::optional<std::int32_t> getMeterValuesSampledDataMaxLength() override;
    std::optional<KeyValue> getMeterValuesSampledDataMaxLengthKeyValue() override;

    // Core Profile
    std::int32_t getMeterValueSampleInterval() override;
    void setMeterValueSampleInterval(std::int32_t interval) override;
    KeyValue getMeterValueSampleIntervalKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getMinimumStatusDuration() override;
    void setMinimumStatusDuration(std::int32_t minimum_status_duration) override;
    std::optional<KeyValue> getMinimumStatusDurationKeyValue() override;

    // Core Profile
    std::int32_t getNumberOfConnectors() override;
    KeyValue getNumberOfConnectorsKeyValue() override;

    // Reservation Profile
    std::optional<bool> getReserveConnectorZeroSupported() override;
    std::optional<KeyValue> getReserveConnectorZeroSupportedKeyValue() override;

    // Core Profile
    std::int32_t getResetRetries() override;
    void setResetRetries(std::int32_t retries) override;
    KeyValue getResetRetriesKeyValue() override;

    // Core Profile - optional
    std::optional<bool> getStopTransactionOnEVSideDisconnect() override;
    std::optional<KeyValue> getStopTransactionOnEVSideDisconnectKeyValue() override;

    // Core Profile
    bool getStopTransactionOnInvalidId() override;
    void setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id) override;
    KeyValue getStopTransactionOnInvalidIdKeyValue() override;

    // Core Profile
    std::string getStopTxnAlignedData() override;
    bool setStopTxnAlignedData(const std::string& stop_txn_aligned_data) override;
    KeyValue getStopTxnAlignedDataKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getStopTxnAlignedDataMaxLength() override;
    std::optional<KeyValue> getStopTxnAlignedDataMaxLengthKeyValue() override;

    // Core Profile
    std::string getStopTxnSampledData() override;
    bool setStopTxnSampledData(const std::string& stop_txn_sampled_data) override;
    KeyValue getStopTxnSampledDataKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getStopTxnSampledDataMaxLength() override;
    std::optional<KeyValue> getStopTxnSampledDataMaxLengthKeyValue() override;

    // Core Profile
    std::string getSupportedFeatureProfiles() override;
    KeyValue getSupportedFeatureProfilesKeyValue() override;
    std::set<SupportedFeatureProfiles> getSupportedFeatureProfilesSet() override;

    // Core Profile - optional
    std::optional<std::int32_t> getSupportedFeatureProfilesMaxLength() override;
    std::optional<KeyValue> getSupportedFeatureProfilesMaxLengthKeyValue() override;

    // Core Profile
    std::int32_t getTransactionMessageAttempts() override;
    void setTransactionMessageAttempts(std::int32_t attempts) override;
    KeyValue getTransactionMessageAttemptsKeyValue() override;

    // Core Profile
    std::int32_t getTransactionMessageRetryInterval() override;
    void setTransactionMessageRetryInterval(std::int32_t retry_interval) override;
    KeyValue getTransactionMessageRetryIntervalKeyValue() override;

    // Core Profile
    bool getUnlockConnectorOnEVSideDisconnect() override;
    void setUnlockConnectorOnEVSideDisconnect(bool unlock_connector_on_ev_side_disconnect) override;
    KeyValue getUnlockConnectorOnEVSideDisconnectKeyValue() override;

    // Core Profile - optional
    std::optional<std::int32_t> getWebsocketPingInterval() override;
    void setWebsocketPingInterval(std::int32_t websocket_ping_interval) override;
    std::optional<KeyValue> getWebsocketPingIntervalKeyValue() override;

    // Core Profile end

    // Firmware Management Profile

    std::optional<std::string> getSupportedFileTransferProtocols() override;
    std::optional<KeyValue> getSupportedFileTransferProtocolsKeyValue() override;

    // SmartCharging Profile
    std::int32_t getChargeProfileMaxStackLevel() override;
    KeyValue getChargeProfileMaxStackLevelKeyValue() override;

    // SmartCharging Profile
    std::string getChargingScheduleAllowedChargingRateUnit() override;
    KeyValue getChargingScheduleAllowedChargingRateUnitKeyValue() override;
    std::vector<ChargingRateUnit> getChargingScheduleAllowedChargingRateUnitVector() override;

    // SmartCharging Profile
    std::int32_t getChargingScheduleMaxPeriods() override;
    KeyValue getChargingScheduleMaxPeriodsKeyValue() override;

    // SmartCharging Profile - optional
    std::optional<bool> getConnectorSwitch3to1PhaseSupported() override;
    std::optional<KeyValue> getConnectorSwitch3to1PhaseSupportedKeyValue() override;

    // SmartCharging Profile
    std::int32_t getMaxChargingProfilesInstalled() override;
    KeyValue getMaxChargingProfilesInstalledKeyValue() override;

    // SmartCharging Profile end

    // Security profile - optional
    std::optional<bool> getAdditionalRootCertificateCheck() override;
    std::optional<KeyValue> getAdditionalRootCertificateCheckKeyValue() override;

    // Security profile - optional
    std::optional<std::string> getAuthorizationKey() override;
    void setAuthorizationKey(const std::string& authorization_key) override;
    std::optional<KeyValue> getAuthorizationKeyKeyValue() override;

    // Security profile - optional
    std::optional<std::int32_t> getCertificateSignedMaxChainSize() override;
    std::optional<KeyValue> getCertificateSignedMaxChainSizeKeyValue() override;

    // Security profile - optional
    std::optional<std::int32_t> getCertificateStoreMaxLength() override;
    std::optional<KeyValue> getCertificateStoreMaxLengthKeyValue() override;

    // Security profile - optional
    std::optional<std::string> getCpoName() override;
    void setCpoName(const std::string& cpo_name) override;
    std::optional<KeyValue> getCpoNameKeyValue() override;

    // // Security profile - optional in ocpp but mandatory websocket connection
    std::int32_t getSecurityProfile() override;
    void setSecurityProfile(std::int32_t security_profile) override;
    KeyValue getSecurityProfileKeyValue() override;

    // // Security profile - optional with default
    bool getDisableSecurityEventNotifications() override;
    void setDisableSecurityEventNotifications(bool disable_security_event_notifications) override;
    KeyValue getDisableSecurityEventNotificationsKeyValue() override;

    // Local Auth List Management Profile
    bool getLocalAuthListEnabled() override;
    void setLocalAuthListEnabled(bool local_auth_list_enabled) override;
    KeyValue getLocalAuthListEnabledKeyValue() override;

    // Local Auth List Management Profile
    std::int32_t getLocalAuthListMaxLength() override;
    KeyValue getLocalAuthListMaxLengthKeyValue() override;

    // Local Auth List Management Profile
    std::int32_t getSendLocalListMaxLength() override;
    KeyValue getSendLocalListMaxLengthKeyValue() override;

    // PnC
    bool getISO15118CertificateManagementEnabled() override;
    void setISO15118CertificateManagementEnabled(bool iso15118_certificate_management_enabled) override;
    KeyValue getISO15118CertificateManagementEnabledKeyValue() override;

    bool getISO15118PnCEnabled() override;
    void setISO15118PnCEnabled(bool iso15118_pnc_enabled) override;
    KeyValue getISO15118PnCEnabledKeyValue() override;

    std::optional<bool> getCentralContractValidationAllowed() override;
    void setCentralContractValidationAllowed(bool central_contract_validation_allowed) override;
    std::optional<KeyValue> getCentralContractValidationAllowedKeyValue() override;

    std::optional<std::int32_t> getCertSigningWaitMinimum() override;
    void setCertSigningWaitMinimum(std::int32_t cert_signing_wait_minimum) override;
    std::optional<KeyValue> getCertSigningWaitMinimumKeyValue() override;

    std::optional<std::int32_t> getCertSigningRepeatTimes() override;
    void setCertSigningRepeatTimes(std::int32_t cert_signing_repeat_times) override;
    std::optional<KeyValue> getCertSigningRepeatTimesKeyValue() override;

    bool getContractValidationOffline() override;
    void setContractValidationOffline(bool contract_validation_offline) override;
    KeyValue getContractValidationOfflineKeyValue() override;

    std::int32_t getOcspRequestInterval() override;
    void setOcspRequestInterval(std::int32_t ocsp_request_interval) override;
    KeyValue getOcspRequestIntervalKeyValue() override;

    std::optional<std::string> getSeccLeafSubjectCommonName() override;
    void setSeccLeafSubjectCommonName(const std::string& secc_leaf_subject_common_name) override;
    std::optional<KeyValue> getSeccLeafSubjectCommonNameKeyValue() override;

    std::optional<std::string> getSeccLeafSubjectCountry() override;
    void setSeccLeafSubjectCountry(const std::string& secc_leaf_subject_country) override;
    std::optional<KeyValue> getSeccLeafSubjectCountryKeyValue() override;

    std::optional<std::string> getSeccLeafSubjectOrganization() override;
    void setSeccLeafSubjectOrganization(const std::string& secc_leaf_subject_organization) override;
    std::optional<KeyValue> getSeccLeafSubjectOrganizationKeyValue() override;

    std::optional<std::string> getConnectorEvseIds() override;
    void setConnectorEvseIds(const std::string& connector_evse_ids) override;
    std::optional<KeyValue> getConnectorEvseIdsKeyValue() override;

    std::optional<bool> getAllowChargingProfileWithoutStartSchedule() override;
    void setAllowChargingProfileWithoutStartSchedule(bool allow) override;
    std::optional<KeyValue> getAllowChargingProfileWithoutStartScheduleKeyValue() override;

    std::int32_t getWaitForStopTransactionsOnResetTimeout() override;
    void setWaitForStopTransactionsOnResetTimeout(std::int32_t wait_for_stop_transactions_on_reset_timeout) override;
    KeyValue getWaitForStopTransactionsOnResetTimeoutKeyValue() override;

    // California Pricing Requirements
    bool getCustomDisplayCostAndPriceEnabled() override;
    KeyValue getCustomDisplayCostAndPriceEnabledKeyValue() override;

    std::optional<std::uint32_t> getPriceNumberOfDecimalsForCostValues() override;
    std::optional<KeyValue> getPriceNumberOfDecimalsForCostValuesKeyValue() override;

    std::optional<std::string> getDefaultPriceText(const std::string& language) override;
    TariffMessage getDefaultTariffMessage(bool offline) override;
    ConfigurationStatus setDefaultPriceText(const CiString<50>& key, const CiString<500>& value) override;
    KeyValue getDefaultPriceTextKeyValue(const std::string& language) override;
    std::optional<std::vector<KeyValue>> getAllDefaultPriceTextKeyValues() override;

    std::optional<std::string> getDefaultPrice() override;
    ConfigurationStatus setDefaultPrice(const std::string& value) override;
    std::optional<KeyValue> getDefaultPriceKeyValue() override;

    std::optional<std::string> getDisplayTimeOffset() override;
    ConfigurationStatus setDisplayTimeOffset(const std::string& offset) override;
    std::optional<KeyValue> getDisplayTimeOffsetKeyValue() override;

    std::optional<std::string> getNextTimeOffsetTransitionDateTime() override;
    ConfigurationStatus setNextTimeOffsetTransitionDateTime(const std::string& date_time) override;
    std::optional<KeyValue> getNextTimeOffsetTransitionDateTimeKeyValue() override;

    std::optional<std::string> getTimeOffsetNextTransition() override;
    ConfigurationStatus setTimeOffsetNextTransition(const std::string& offset) override;
    std::optional<KeyValue> getTimeOffsetNextTransitionKeyValue() override;

    std::optional<bool> getCustomIdleFeeAfterStop() override;
    void setCustomIdleFeeAfterStop(bool value) override;
    std::optional<KeyValue> getCustomIdleFeeAfterStopKeyValue() override;

    std::optional<bool> getCustomMultiLanguageMessagesEnabled() override;
    std::optional<KeyValue> getCustomMultiLanguageMessagesEnabledKeyValue() override;

    std::optional<std::string> getMultiLanguageSupportedLanguages() override;
    std::optional<KeyValue> getMultiLanguageSupportedLanguagesKeyValue() override;

    std::optional<std::string> getLanguage() override;
    void setLanguage(const std::string& language) override;
    std::optional<KeyValue> getLanguageKeyValue() override;

    std::optional<std::int32_t> getWaitForSetUserPriceTimeout() override;
    void setWaitForSetUserPriceTimeout(std::int32_t wait_for_set_user_price_timeout) override;
    std::optional<KeyValue> getWaitForSetUserPriceTimeoutKeyValue() override;

    // Signed Meter Values
    std::optional<KeyValue> getPublicKeyKeyValue(std::uint32_t connector_id) override;
    std::optional<std::vector<KeyValue>> getAllMeterPublicKeyKeyValues() override;
    bool setMeterPublicKey(std::int32_t connector_id, const std::string& public_key_pem) override;

    // custom
    std::optional<KeyValue> getCustomKeyValue(const CiString<50>& key) override;
    ConfigurationStatus setCustomKey(const CiString<50>& key, const CiString<500>& value, bool force) override;

    std::optional<KeyValue> get(const CiString<50>& key) override;

    std::vector<KeyValue> get_all_key_value() override;

    std::optional<ConfigurationStatus> set(const CiString<50>& key, const CiString<500>& value) override;
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CHARGE_POINT_CONFIGURATION_HPP
