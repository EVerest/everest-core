// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHARGE_POINT_CONFIGURATION_HPP
#define OCPP1_6_CHARGE_POINT_CONFIGURATION_HPP

#include <set>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/pki_handler.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief contains the configuration of the charge point
class ChargePointConfiguration {
private:
    json config;
    boost::filesystem::path user_config_path;
    std::shared_ptr<PkiHandler> pki_handler;

    std::set<SupportedFeatureProfiles> supported_feature_profiles;
    std::map<Measurand, std::vector<Phase>> supported_measurands;
    std::map<SupportedFeatureProfiles, std::set<MessageType>> supported_message_types_from_charge_point;
    std::map<SupportedFeatureProfiles, std::set<MessageType>> supported_message_types_from_central_system;
    std::set<MessageType> supported_message_types_sending;
    std::set<MessageType> supported_message_types_receiving;

    std::vector<MeasurandWithPhase> csv_to_measurand_with_phase_vector(std::string csv);
    bool validate_measurands(const json &config);
    bool measurands_supported(std::string csv);
    json get_user_config();
    void setInUserConfig(std::string profile, std::string key, json value);

    bool isConnectorPhaseRotationValid(std::string str);

public:
    ChargePointConfiguration(const json& config, const std::string& ocpp_main_path,
                             const std::string& user_config_path);

    std::shared_ptr<PkiHandler> getPkiHandler();

    // Internal config options
    std::string getChargePointId();
    std::string getCentralSystemURI();
    std::string getChargeBoxSerialNumber();
    CiString20Type getChargePointModel();
    boost::optional<CiString25Type> getChargePointSerialNumber();
    CiString20Type getChargePointVendor();
    CiString50Type getFirmwareVersion();
    boost::optional<CiString20Type> getICCID();
    boost::optional<CiString20Type> getIMSI();
    boost::optional<CiString25Type> getMeterSerialNumber();
    boost::optional<CiString25Type> getMeterType();
    int32_t getWebsocketReconnectInterval();
    bool getAuthorizeConnectorZeroOnConnectorOne();
    bool getLogMessages();
    std::vector<std::string> getLogMessagesFormat();
    std::vector<ChargingProfilePurposeType> getSupportedChargingProfilePurposeTypes();
    int32_t getMaxCompositeScheduleDuration();

    std::string getSupportedCiphers12();
    std::string getSupportedCiphers13();

    std::set<MessageType> getSupportedMessageTypesSending();
    std::set<MessageType> getSupportedMessageTypesReceiving();


    // Core Profile - optional
    boost::optional<bool> getAllowOfflineTxForUnknownId();
    void setAllowOfflineTxForUnknownId(bool enabled);
    boost::optional<KeyValue> getAllowOfflineTxForUnknownIdKeyValue();

    // Core Profile - optional
    boost::optional<bool> getAuthorizationCacheEnabled();
    void setAuthorizationCacheEnabled(bool enabled);
    boost::optional<KeyValue> getAuthorizationCacheEnabledKeyValue();

    // Core Profile
    bool getAuthorizeRemoteTxRequests();
    void setAuthorizeRemoteTxRequests(bool enabled);
    KeyValue getAuthorizeRemoteTxRequestsKeyValue();

    // Core Profile - optional
    boost::optional<int32_t> getBlinkRepeat();
    void setBlinkRepeat(int32_t blink_repeat);
    boost::optional<KeyValue> getBlinkRepeatKeyValue();

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
    boost::optional<int32_t> getConnectorPhaseRotationMaxLength();
    boost::optional<KeyValue> getConnectorPhaseRotationMaxLengthKeyValue();

    // Core Profile
    int32_t getGetConfigurationMaxKeys();
    KeyValue getGetConfigurationMaxKeysKeyValue();

    // Core Profile
    int32_t getHeartbeatInterval();
    void setHeartbeatInterval(int32_t interval);
    KeyValue getHeartbeatIntervalKeyValue();

    // Core Profile - optional
    boost::optional<int32_t> getLightIntensity();
    void setLightIntensity(int32_t light_intensity);
    boost::optional<KeyValue> getLightIntensityKeyValue();

    // Core Profile
    bool getLocalAuthorizeOffline();
    void setLocalAuthorizeOffline(bool local_authorize_offline);
    KeyValue getLocalAuthorizeOfflineKeyValue();

    // Core Profile
    bool getLocalPreAuthorize();
    void setLocalPreAuthorize(bool local_pre_authorize);
    KeyValue getLocalPreAuthorizeKeyValue();

    // Core Profile - optional
    boost::optional<int32_t> getMaxEnergyOnInvalidId();
    void setMaxEnergyOnInvalidId(int32_t max_energy);
    boost::optional<KeyValue> getMaxEnergyOnInvalidIdKeyValue();

    // Core Profile
    std::string getMeterValuesAlignedData();
    bool setMeterValuesAlignedData(std::string meter_values_aligned_data);
    KeyValue getMeterValuesAlignedDataKeyValue();
    std::vector<MeasurandWithPhase> getMeterValuesAlignedDataVector();

    // Core Profile - optional
    boost::optional<int32_t> getMeterValuesAlignedDataMaxLength();
    boost::optional<KeyValue> getMeterValuesAlignedDataMaxLengthKeyValue();

    // Core Profile
    std::string getMeterValuesSampledData();
    bool setMeterValuesSampledData(std::string meter_values_sampled_data);
    KeyValue getMeterValuesSampledDataKeyValue();
    std::vector<MeasurandWithPhase> getMeterValuesSampledDataVector();

    // Core Profile - optional
    boost::optional<int32_t> getMeterValuesSampledDataMaxLength();
    boost::optional<KeyValue> getMeterValuesSampledDataMaxLengthKeyValue();

    // Core Profile
    int32_t getMeterValueSampleInterval();
    void setMeterValueSampleInterval(int32_t interval);
    KeyValue getMeterValueSampleIntervalKeyValue();

    // Core Profile - optional
    boost::optional<int32_t> getMinimumStatusDuration();
    void setMinimumStatusDuration(int32_t minimum_status_duration);
    boost::optional<KeyValue> getMinimumStatusDurationKeyValue();

    // Core Profile
    int32_t getNumberOfConnectors();
    KeyValue getNumberOfConnectorsKeyValue();

    // Reservation Profile
    boost::optional<bool> getReserveConnectorZeroSupported();
    boost::optional<KeyValue> getReserveConnectorZeroSupportedKeyValue();

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
    std::vector<MeasurandWithPhase> getStopTxnAlignedDataVector();

    // Core Profile - optional
    boost::optional<int32_t> getStopTxnAlignedDataMaxLength();
    boost::optional<KeyValue> getStopTxnAlignedDataMaxLengthKeyValue();

    // Core Profile
    std::string getStopTxnSampledData();
    bool setStopTxnSampledData(std::string stop_txn_sampled_data);
    KeyValue getStopTxnSampledDataKeyValue();
    std::vector<MeasurandWithPhase> getStopTxnSampledDataVector();

    // Core Profile - optional
    boost::optional<int32_t> getStopTxnSampledDataMaxLength();
    boost::optional<KeyValue> getStopTxnSampledDataMaxLengthKeyValue();

    // Core Profile
    std::string getSupportedFeatureProfiles();
    KeyValue getSupportedFeatureProfilesKeyValue();
    std::set<SupportedFeatureProfiles> getSupportedFeatureProfilesSet();

    // Core Profile - optional
    boost::optional<int32_t> getSupportedFeatureProfilesMaxLength();
    boost::optional<KeyValue> getSupportedFeatureProfilesMaxLengthKeyValue();

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
    boost::optional<int32_t> getWebsocketPingInterval();
    void setWebsocketPingInterval(int32_t websocket_ping_interval);
    boost::optional<KeyValue> getWebsocketPingIntervalKeyValue();

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
    boost::optional<bool> getConnectorSwitch3to1PhaseSupported();
    boost::optional<KeyValue> getConnectorSwitch3to1PhaseSupportedKeyValue();

    // SmartCharging Profile
    int32_t getMaxChargingProfilesInstalled();
    KeyValue getMaxChargingProfilesInstalledKeyValue();

    // SmartCharging Profile end

    // Security profile - optional
    boost::optional<bool> getAdditionalRootCertificateCheck();
    boost::optional<KeyValue> getAdditionalRootCertificateCheckKeyValue();

    // Security profile - optional
    boost::optional<std::string> getAuthorizationKey();
    void setAuthorizationKey(std::string authorization_key);
    boost::optional<KeyValue> getAuthorizationKeyKeyValue();

    // Security profile - optional
    boost::optional<int32_t> getCertificateSignedMaxChainSize();
    boost::optional<KeyValue> getCertificateSignedMaxChainSizeKeyValue();

    // Security profile - optional
    boost::optional<int32_t> getCertificateStoreMaxLength();
    boost::optional<KeyValue> getCertificateStoreMaxLengthKeyValue();

    // Security profile - optional
    boost::optional<std::string> getCpoName();
    void setCpoName(std::string cpo_name);
    boost::optional<KeyValue> getCpoNameKeyValue();

    // // Security profile - optional in ocpp but mandatory websocket connection
    int32_t getSecurityProfile();
    void setSecurityProfile(int32_t security_profile);
    KeyValue getSecurityProfileKeyValue();

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

    boost::optional<KeyValue> get(CiString50Type key);

    std::vector<KeyValue> get_all_key_value();

    ConfigurationStatus set(CiString50Type key, CiString500Type value);
};
} // namespace ocpp1_6

#endif // OCPP1_6_CHARGE_POINT_CONFIGURATION_HPP
