// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_CHARGE_POINT_CONFIGURATION_HPP
#define OCPP_V16_CHARGE_POINT_CONFIGURATION_HPP

#include <mutex>
#include <set>

#include <ocpp/common/support_older_cpp_versions.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

/// \brief contains the configuration of the charge point
class ChargePointConfiguration {
private:
    json config;
    json custom_schema;
    fs::path user_config_path;

    std::set<SupportedFeatureProfiles> supported_feature_profiles;
    std::map<Measurand, std::vector<Phase>> supported_measurands;
    std::map<SupportedFeatureProfiles, std::set<MessageType>> supported_message_types_from_charge_point;
    std::map<SupportedFeatureProfiles, std::set<MessageType>> supported_message_types_from_central_system;
    std::set<MessageType> supported_message_types_sending;
    std::set<MessageType> supported_message_types_receiving;
    std::recursive_mutex configuration_mutex;

    std::vector<MeasurandWithPhase> csv_to_measurand_with_phase_vector(std::string csv);
    bool validate_measurands(const json& config);
    bool measurands_supported(std::string csv);
    json get_user_config();
    void setInUserConfig(std::string profile, std::string key, json value);
    void init_supported_measurands();

    bool isConnectorPhaseRotationValid(std::string str);

    bool checkTimeOffset(const std::string& offset);

public:
    ChargePointConfiguration(const std::string& config, const fs::path& ocpp_main_path,
                             const fs::path& user_config_path);

    // Internal config options
    std::string getChargePointId();
    KeyValue getChargePointIdKeyValue();
    std::string getCentralSystemURI();
    KeyValue getCentralSystemURIKeyValue();
    std::string getChargeBoxSerialNumber();
    KeyValue getChargeBoxSerialNumberKeyValue();
    CiString<20> getChargePointModel();
    KeyValue getChargePointModelKeyValue();
    std::optional<CiString<25>> getChargePointSerialNumber();
    std::optional<KeyValue> getChargePointSerialNumberKeyValue();
    CiString<20> getChargePointVendor();
    KeyValue getChargePointVendorKeyValue();
    CiString<50> getFirmwareVersion();
    KeyValue getFirmwareVersionKeyValue();
    std::optional<CiString<20>> getICCID();
    std::optional<KeyValue> getICCIDKeyValue();
    std::optional<CiString<20>> getIMSI();
    std::optional<KeyValue> getIMSIKeyValue();
    std::optional<CiString<25>> getMeterSerialNumber();
    std::optional<KeyValue> getMeterSerialNumberKeyValue();
    std::optional<CiString<25>> getMeterType();
    std::optional<KeyValue> getMeterTypeKeyValue();
    int32_t getWebsocketReconnectInterval();
    KeyValue getWebsocketReconnectIntervalKeyValue();
    bool getAuthorizeConnectorZeroOnConnectorOne();
    KeyValue getAuthorizeConnectorZeroOnConnectorOneKeyValue();
    bool getLogMessages();
    KeyValue getLogMessagesKeyValue();
    std::vector<std::string> getLogMessagesFormat();
    KeyValue getLogMessagesFormatKeyValue();
    std::vector<ChargingProfilePurposeType> getSupportedChargingProfilePurposeTypes();
    KeyValue getSupportedChargingProfilePurposeTypesKeyValue();
    int32_t getMaxCompositeScheduleDuration();
    KeyValue getMaxCompositeScheduleDurationKeyValue();
    std::string getSupportedCiphers12();
    KeyValue getSupportedCiphers12KeyValue();
    std::string getSupportedCiphers13();
    KeyValue getSupportedCiphers13KeyValue();
    bool getUseSslDefaultVerifyPaths();
    KeyValue getUseSslDefaultVerifyPathsKeyValue();
    bool getVerifyCsmsCommonName();
    KeyValue getVerifyCsmsCommonNameKeyValue();
    bool getVerifyCsmsAllowWildcards();
    void setVerifyCsmsAllowWildcards(bool verify_csms_allow_wildcards);
    KeyValue getVerifyCsmsAllowWildcardsKeyValue();
    bool getUseTPM();
    std::string getSupportedMeasurands();
    KeyValue getSupportedMeasurandsKeyValue();
    int getMaxMessageSize();
    KeyValue getMaxMessageSizeKeyValue();

    int32_t getRetryBackoffRandomRange();
    void setRetryBackoffRandomRange(int32_t retry_backoff_random_range);
    KeyValue getRetryBackoffRandomRangeKeyValue();

    int32_t getRetryBackoffRepeatTimes();
    void setRetryBackoffRepeatTimes(int32_t retry_backoff_repeat_times);
    KeyValue getRetryBackoffRepeatTimesKeyValue();

    int32_t getRetryBackoffWaitMinimum();
    void setRetryBackoffWaitMinimum(int32_t retry_backoff_wait_minimum);
    KeyValue getRetryBackoffWaitMinimumKeyValue();

    std::set<MessageType> getSupportedMessageTypesSending();
    std::set<MessageType> getSupportedMessageTypesReceiving();

    std::string getWebsocketPingPayload();
    KeyValue getWebsocketPingPayloadKeyValue();

    int32_t getWebsocketPongTimeout();
    KeyValue getWebsocketPongTimeoutKeyValue();

    std::optional<std::string> getHostName();
    std::optional<KeyValue> getHostNameKeyValue();

    std::optional<std::string> getIFace();
    std::optional<KeyValue> getIFaceKeyValue();

    std::optional<bool> getQueueAllMessages();
    std::optional<KeyValue> getQueueAllMessagesKeyValue();

    std::optional<int> getMessageQueueSizeThreshold();
    std::optional<KeyValue> getMessageQueueSizeThresholdKeyValue();

    // Core Profile - optional
    std::optional<bool> getAllowOfflineTxForUnknownId();
    void setAllowOfflineTxForUnknownId(bool enabled);
    std::optional<KeyValue> getAllowOfflineTxForUnknownIdKeyValue();

    // Core Profile - optional
    std::optional<bool> getAuthorizationCacheEnabled();
    void setAuthorizationCacheEnabled(bool enabled);
    std::optional<KeyValue> getAuthorizationCacheEnabledKeyValue();

    // Core Profile
    bool getAuthorizeRemoteTxRequests();
    void setAuthorizeRemoteTxRequests(bool enabled);
    KeyValue getAuthorizeRemoteTxRequestsKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getBlinkRepeat();
    void setBlinkRepeat(int32_t blink_repeat);
    std::optional<KeyValue> getBlinkRepeatKeyValue();

    // Core Profile
    int32_t getClockAlignedDataInterval();
    void setClockAlignedDataInterval(int32_t interval);
    KeyValue getClockAlignedDataIntervalKeyValue();

    // Core Profile
    int32_t getConnectionTimeOut();
    void setConnectionTimeOut(int32_t timeout);
    KeyValue getConnectionTimeOutKeyValue();

    // Core Profile
    std::string getConnectorPhaseRotation();
    void setConnectorPhaseRotation(std::string connector_phase_rotation);
    KeyValue getConnectorPhaseRotationKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getConnectorPhaseRotationMaxLength();
    std::optional<KeyValue> getConnectorPhaseRotationMaxLengthKeyValue();

    // Core Profile
    int32_t getGetConfigurationMaxKeys();
    KeyValue getGetConfigurationMaxKeysKeyValue();

    // Core Profile
    int32_t getHeartbeatInterval();
    void setHeartbeatInterval(int32_t interval);
    KeyValue getHeartbeatIntervalKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getLightIntensity();
    void setLightIntensity(int32_t light_intensity);
    std::optional<KeyValue> getLightIntensityKeyValue();

    // Core Profile
    bool getLocalAuthorizeOffline();
    void setLocalAuthorizeOffline(bool local_authorize_offline);
    KeyValue getLocalAuthorizeOfflineKeyValue();

    // Core Profile
    bool getLocalPreAuthorize();
    void setLocalPreAuthorize(bool local_pre_authorize);
    KeyValue getLocalPreAuthorizeKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getMaxEnergyOnInvalidId();
    void setMaxEnergyOnInvalidId(int32_t max_energy);
    std::optional<KeyValue> getMaxEnergyOnInvalidIdKeyValue();

    // Core Profile
    std::string getMeterValuesAlignedData();
    bool setMeterValuesAlignedData(std::string meter_values_aligned_data);
    KeyValue getMeterValuesAlignedDataKeyValue();
    std::vector<MeasurandWithPhase> getMeterValuesAlignedDataVector();

    // Core Profile - optional
    std::optional<int32_t> getMeterValuesAlignedDataMaxLength();
    std::optional<KeyValue> getMeterValuesAlignedDataMaxLengthKeyValue();

    // Core Profile
    std::string getMeterValuesSampledData();
    bool setMeterValuesSampledData(std::string meter_values_sampled_data);
    KeyValue getMeterValuesSampledDataKeyValue();
    std::vector<MeasurandWithPhase> getMeterValuesSampledDataVector();

    // Core Profile - optional
    std::optional<int32_t> getMeterValuesSampledDataMaxLength();
    std::optional<KeyValue> getMeterValuesSampledDataMaxLengthKeyValue();

    // Core Profile
    int32_t getMeterValueSampleInterval();
    void setMeterValueSampleInterval(int32_t interval);
    KeyValue getMeterValueSampleIntervalKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getMinimumStatusDuration();
    void setMinimumStatusDuration(int32_t minimum_status_duration);
    std::optional<KeyValue> getMinimumStatusDurationKeyValue();

    // Core Profile
    int32_t getNumberOfConnectors();
    KeyValue getNumberOfConnectorsKeyValue();

    // Reservation Profile
    std::optional<bool> getReserveConnectorZeroSupported();
    std::optional<KeyValue> getReserveConnectorZeroSupportedKeyValue();

    // Core Profile
    int32_t getResetRetries();
    void setResetRetries(int32_t retries);
    KeyValue getResetRetriesKeyValue();

    // Core Profile
    bool getStopTransactionOnEVSideDisconnect();
    void setStopTransactionOnEVSideDisconnect(bool stop_transaction_on_ev_side_disconnect);
    KeyValue getStopTransactionOnEVSideDisconnectKeyValue();

    // Core Profile
    bool getStopTransactionOnInvalidId();
    void setStopTransactionOnInvalidId(bool stop_transaction_on_invalid_id);
    KeyValue getStopTransactionOnInvalidIdKeyValue();

    // Core Profile
    std::string getStopTxnAlignedData();
    bool setStopTxnAlignedData(std::string stop_txn_aligned_data);
    KeyValue getStopTxnAlignedDataKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getStopTxnAlignedDataMaxLength();
    std::optional<KeyValue> getStopTxnAlignedDataMaxLengthKeyValue();

    // Core Profile
    std::string getStopTxnSampledData();
    bool setStopTxnSampledData(std::string stop_txn_sampled_data);
    KeyValue getStopTxnSampledDataKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getStopTxnSampledDataMaxLength();
    std::optional<KeyValue> getStopTxnSampledDataMaxLengthKeyValue();

    // Core Profile
    std::string getSupportedFeatureProfiles();
    KeyValue getSupportedFeatureProfilesKeyValue();
    std::set<SupportedFeatureProfiles> getSupportedFeatureProfilesSet();

    // Core Profile - optional
    std::optional<int32_t> getSupportedFeatureProfilesMaxLength();
    std::optional<KeyValue> getSupportedFeatureProfilesMaxLengthKeyValue();

    // Core Profile
    int32_t getTransactionMessageAttempts();
    void setTransactionMessageAttempts(int32_t attempts);
    KeyValue getTransactionMessageAttemptsKeyValue();

    // Core Profile
    int32_t getTransactionMessageRetryInterval();
    void setTransactionMessageRetryInterval(int32_t retry_interval);
    KeyValue getTransactionMessageRetryIntervalKeyValue();

    // Core Profile
    bool getUnlockConnectorOnEVSideDisconnect();
    void setUnlockConnectorOnEVSideDisconnect(bool unlock_connector_on_ev_side_disconnect);
    KeyValue getUnlockConnectorOnEVSideDisconnectKeyValue();

    // Core Profile - optional
    std::optional<int32_t> getWebsocketPingInterval();
    void setWebsocketPingInterval(int32_t websocket_ping_interval);
    std::optional<KeyValue> getWebsocketPingIntervalKeyValue();

    // Core Profile end

    // SmartCharging Profile
    int32_t getChargeProfileMaxStackLevel();
    KeyValue getChargeProfileMaxStackLevelKeyValue();

    // SmartCharging Profile
    std::string getChargingScheduleAllowedChargingRateUnit();
    KeyValue getChargingScheduleAllowedChargingRateUnitKeyValue();
    std::vector<ChargingRateUnit> getChargingScheduleAllowedChargingRateUnitVector();

    // SmartCharging Profile
    int32_t getChargingScheduleMaxPeriods();
    KeyValue getChargingScheduleMaxPeriodsKeyValue();

    // SmartCharging Profile - optional
    std::optional<bool> getConnectorSwitch3to1PhaseSupported();
    std::optional<KeyValue> getConnectorSwitch3to1PhaseSupportedKeyValue();

    // SmartCharging Profile
    int32_t getMaxChargingProfilesInstalled();
    KeyValue getMaxChargingProfilesInstalledKeyValue();

    // SmartCharging Profile end

    // Security profile - optional
    std::optional<bool> getAdditionalRootCertificateCheck();
    std::optional<KeyValue> getAdditionalRootCertificateCheckKeyValue();

    // Security profile - optional
    std::optional<std::string> getAuthorizationKey();
    void setAuthorizationKey(std::string authorization_key);
    std::optional<KeyValue> getAuthorizationKeyKeyValue();

    // Security profile - optional
    std::optional<int32_t> getCertificateSignedMaxChainSize();
    std::optional<KeyValue> getCertificateSignedMaxChainSizeKeyValue();

    // Security profile - optional
    std::optional<int32_t> getCertificateStoreMaxLength();
    std::optional<KeyValue> getCertificateStoreMaxLengthKeyValue();

    // Security profile - optional
    std::optional<std::string> getCpoName();
    void setCpoName(std::string cpo_name);
    std::optional<KeyValue> getCpoNameKeyValue();

    // // Security profile - optional in ocpp but mandatory websocket connection
    int32_t getSecurityProfile();
    void setSecurityProfile(int32_t security_profile);
    KeyValue getSecurityProfileKeyValue();

    // // Security profile - optional with default
    bool getDisableSecurityEventNotifications();
    void setDisableSecurityEventNotifications(bool disable_security_event_notifications);
    KeyValue getDisableSecurityEventNotificationsKeyValue();

    // Local Auth List Management Profile
    bool getLocalAuthListEnabled();
    void setLocalAuthListEnabled(bool local_auth_list_enabled);
    KeyValue getLocalAuthListEnabledKeyValue();

    // Local Auth List Management Profile
    int32_t getLocalAuthListMaxLength();
    KeyValue getLocalAuthListMaxLengthKeyValue();

    // Local Auth List Management Profile
    int32_t getSendLocalListMaxLength();
    KeyValue getSendLocalListMaxLengthKeyValue();

    // PnC
    bool getISO15118PnCEnabled();
    void setISO15118PnCEnabled(const bool iso15118_pnc_enabled);
    KeyValue getISO15118PnCEnabledKeyValue();

    std::optional<bool> getCentralContractValidationAllowed();
    void setCentralContractValidationAllowed(const bool central_contract_validation_allowed);
    std::optional<KeyValue> getCentralContractValidationAllowedKeyValue();

    std::optional<int32_t> getCertSigningWaitMinimum();
    void setCertSigningWaitMinimum(const int32_t cert_signing_wait_minimum);
    std::optional<KeyValue> getCertSigningWaitMinimumKeyValue();

    std::optional<int32_t> getCertSigningRepeatTimes();
    void setCertSigningRepeatTimes(const int32_t cert_signing_repeat_times);
    std::optional<KeyValue> getCertSigningRepeatTimesKeyValue();

    bool getContractValidationOffline();
    void setContractValidationOffline(const bool contract_validation_offline);
    KeyValue getContractValidationOfflineKeyValue();

    int32_t getOcspRequestInterval();
    void setOcspRequestInterval(const int32_t ocsp_request_interval);
    KeyValue getOcspRequestIntervalKeyValue();

    std::optional<std::string> getSeccLeafSubjectCommonName();
    void setSeccLeafSubjectCommonName(const std::string& secc_leaf_subject_common_name);
    std::optional<KeyValue> getSeccLeafSubjectCommonNameKeyValue();

    std::optional<std::string> getSeccLeafSubjectCountry();
    void setSeccLeafSubjectCountry(const std::string& secc_leaf_subject_country);
    std::optional<KeyValue> getSeccLeafSubjectCountryKeyValue();

    std::optional<std::string> getSeccLeafSubjectOrganization();
    void setSeccLeafSubjectOrganization(const std::string& secc_leaf_subject_organization);
    std::optional<KeyValue> getSeccLeafSubjectOrganizationKeyValue();

    std::optional<std::string> getConnectorEvseIds();
    void setConnectorEvseIds(const std::string& connector_evse_ids);
    std::optional<KeyValue> getConnectorEvseIdsKeyValue();

    std::optional<bool> getAllowChargingProfileWithoutStartSchedule();
    void setAllowChargingProfileWithoutStartSchedule(const bool allow);
    std::optional<KeyValue> getAllowChargingProfileWithoutStartScheduleKeyValue();

    int32_t getWaitForStopTransactionsOnResetTimeout();
    void setWaitForStopTransactionsOnResetTimeout(const int32_t wait_for_stop_transactions_on_reset_timeout);
    KeyValue getWaitForStopTransactionsOnResetTimeoutKeyValue();

    // California Pricing Requirements
    bool getCustomDisplayCostAndPriceEnabled();
    KeyValue getCustomDisplayCostAndPriceEnabledKeyValue();

    std::optional<uint32_t> getPriceNumberOfDecimalsForCostValues();
    std::optional<KeyValue> getPriceNumberOfDecimalsForCostValuesKeyValue();

    std::optional<std::string> getDefaultPriceText(const std::string& language);
    ConfigurationStatus setDefaultPriceText(const CiString<50>& key, const CiString<500>& value);
    KeyValue getDefaultPriceTextKeyValue(const std::string& language);
    std::optional<std::vector<KeyValue>> getAllDefaultPriceTextKeyValues();

    std::optional<std::string> getDefaultPrice();
    ConfigurationStatus setDefaultPrice(const std::string& value);
    std::optional<KeyValue> getDefaultPriceKeyValue();

    std::optional<std::string> getDisplayTimeOffset();
    ConfigurationStatus setDisplayTimeOffset(const std::string& offset);
    std::optional<KeyValue> getDisplayTimeOffsetKeyValue();

    std::optional<std::string> getNextTimeOffsetTransitionDateTime();
    ConfigurationStatus setNextTimeOffsetTransitionDateTime(const std::string& date_time);
    std::optional<KeyValue> getNextTimeOffsetTransitionDateTimeKeyValue();

    std::optional<std::string> getTimeOffsetNextTransition();
    ConfigurationStatus setTimeOffsetNextTransition(const std::string& offset);
    std::optional<KeyValue> getTimeOffsetNextTransitionKeyValue();

    std::optional<bool> getCustomIdleFeeAfterStop();
    void setCustomIdleFeeAfterStop(const bool& value);
    std::optional<KeyValue> getCustomIdleFeeAfterStopKeyValue();

    std::optional<bool> getCustomMultiLanguageMessagesEnabled();
    std::optional<KeyValue> getCustomMultiLanguageMessagesEnabledKeyValue();

    std::optional<std::string> getMultiLanguageSupportedLanguages();
    std::optional<KeyValue> getMultiLanguageSupportedLanguagesKeyValue();

    std::optional<std::string> getLanguage();
    void setLanguage(const std::string& language);
    std::optional<KeyValue> getLanguageKeyValue();

    // custom
    std::optional<KeyValue> getCustomKeyValue(CiString<50> key);
    ConfigurationStatus setCustomKey(CiString<50> key, CiString<500> value, bool force);

    std::optional<KeyValue> get(CiString<50> key);

    std::vector<KeyValue> get_all_key_value();

    ConfigurationStatus set(CiString<50> key, CiString<500> value);
};

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_CHARGE_POINT_CONFIGURATION_HPP
