// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_CHARGE_POINT_CONFIGURATION_HPP
#define OCPP1_6_CHARGE_POINT_CONFIGURATION_HPP

#include <set>

#include <sqlite3.h>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief contains the configuration of the charge point
class ChargePointConfiguration {
private:
    json config;
    sqlite3* db;
    std::set<SupportedFeatureProfiles> supported_feature_profiles;
    std::map<Measurand, std::vector<Phase>> supported_measurands;
    std::map<SupportedFeatureProfiles, std::set<MessageType>> supported_message_types_from_charge_point;
    std::map<SupportedFeatureProfiles, std::set<MessageType>> supported_message_types_from_central_system;
    std::set<MessageType> supported_message_types_sending;
    std::set<MessageType> supported_message_types_receiving;

    std::vector<MeasurandWithPhase> csv_to_measurand_with_phase_vector(std::string csv);
    bool measurands_supported(std::string csv);

public:
    ChargePointConfiguration(json config, std::string schemas_path, std::string database_path);
    void close();

    // Internal config options
    std::string getChargePointId();
    std::string getCentralSystemURI();
    boost::optional<CiString25Type> getChargeBoxSerialNumber();
    CiString20Type getChargePointModel();
    boost::optional<CiString25Type> getChargePointSerialNumber();
    CiString20Type getChargePointVendor();
    CiString50Type getFirmwareVersion();
    boost::optional<CiString20Type> getICCID();
    boost::optional<CiString20Type> getIMSI();
    boost::optional<CiString25Type> getMeterSerialNumber();
    boost::optional<CiString25Type> getMeterType();
    int32_t getWebsocketReconnectInterval();

    std::string getSupportedCiphers();
    // move this to a new "Security" profile with the other OCPP 1.6 security extension config options?
    boost::optional<std::string> getAuthorizationKey();

    // Internal
    std::set<MessageType> getSupportedMessageTypesSending();
    std::set<MessageType> getSupportedMessageTypesReceiving();

    bool setConnectorAvailability(int32_t connectorId, AvailabilityType availability);
    AvailabilityType getConnectorAvailability(int32_t connectorId);

    std::map<int32_t, ocpp1_6::AvailabilityType> getConnectorAvailability();

    // Core Profile - Authorization Cache
    bool updateAuthorizationCacheEntry(CiString20Type idTag, IdTagInfo idTagInfo);
    bool clearAuthorizationCache();
    boost::optional<IdTagInfo> getAuthorizationCacheEntry(CiString20Type idTag);

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

    boost::optional<KeyValue> get(CiString50Type key);

    std::vector<KeyValue> get_all_key_value();

    ConfigurationStatus set(CiString50Type key, CiString500Type value);
};
} // namespace ocpp1_6

#endif // OCPP1_6_CHARGE_POINT_CONFIGURATION_HPP
