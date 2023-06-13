// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

// AUTO GENERATED - DO NOT EDIT!

#include <map>

#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/common/pki_handler.hpp>

namespace ocpp {
namespace v201 {

enum class StandardizedComponent {
    AuthCtrlr,
    InternalCtrlr,
    SampledDataCtrlr,
    OCPPCommCtrlr,
    FiscalMetering,
    ChargingStation,
    DistributionPanel,
    Controller,
    MonitoringCtrlr,
    ClockCtrlr,
    SmartChargingCtrlr,
    LocalEnergyStorage,
    AlignedDataCtrlr,
    EVSE,
    TariffCostCtrlr,
    DisplayMessageCtrlr,
    SecurityCtrlr,
    ConnectedEV,
    CustomizationCtrlr,
    Connector,
    DeviceDataCtrlr,
    LocalAuthListCtrlr,
    TokenReader,
    CPPWMController,
    ISO15118Ctrlr,
    TxCtrlr,
    ReservationCtrlr,
    AuthCacheCtrlr,
    CHAdeMOCtrlr
};

namespace conversions {
/// \brief Converts the given StandardizedComponent \p e to human readable string
/// \returns a string representation of the StandardizedComponent
std::string standardized_component_to_string(StandardizedComponent e);

/// \brief Converts the given std::string \p s to StandardizedComponent
/// \returns a StandardizedComponent from a string representation
StandardizedComponent string_to_standardized_component(const std::string& s);
} //namespace conversion

/// \brief Writes the string representation of the given StandardizedComponent \p standardized_component to the given output stream \p os
/// \returns an output stream with the StandardizedComponent written to
std::ostream& operator<<(std::ostream& os, const StandardizedComponent& standardized_component);

struct EnhancedVariable : Variable {
    std::map<AttributeEnum, VariableAttribute> attributes;
    std::optional<VariableCharacteristics> characteristics;
    std::vector<VariableMonitoring> monitorings;

    bool operator==(const Variable& other) const {
        // variables are equal when they have an equal name and equal or no instance
        return this->name == other.name and ((!this->instance.has_value() and !other.instance.has_value()) or
                                             this->instance.value() == other.instance.value());
    };
};

struct EnhancedComponent : Component {
    explicit EnhancedComponent(const std::string &name) {
        this->name = name;
    }
    std::map<std::string, EnhancedVariable> variables;

    bool operator==(const Component& other) const {
        // components are equal when they have an equal name and equal or no instance
        return this->name == other.name and ((!this->instance.has_value() and !other.instance.has_value()) or
                                             this->instance.value() == other.instance.value());
    };
};

class DeviceModelManagerBase {
protected:
    std::map<StandardizedComponent, EnhancedComponent> components;

    bool exists(const StandardizedComponent &standardized_component, const std::string &variable_name, const AttributeEnum &attribute_enum);

public:
    DeviceModelManagerBase() {};

    /// \brief Returns Enabled
    std::optional<bool> get_internal_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p internal_ctrlr_enabled
    void set_internal_ctrlr_enabled(const bool &internal_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargePointId
    std::string get_charge_point_id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargePointId to given \p charge_point_id
    void set_charge_point_id(const std::string &charge_point_id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CentralSystemURI
    std::string get_central_system_uri(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CentralSystemURI to given \p central_system_uri
    void set_central_system_uri(const std::string &central_system_uri, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargeBoxSerialNumber
    std::string get_charge_box_serial_number(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargeBoxSerialNumber to given \p charge_box_serial_number
    void set_charge_box_serial_number(const std::string &charge_box_serial_number, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargePointModel
    std::string get_charge_point_model(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargePointModel to given \p charge_point_model
    void set_charge_point_model(const std::string &charge_point_model, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargePointSerialNumber
    std::optional<std::string> get_charge_point_serial_number(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargePointSerialNumber to given \p charge_point_serial_number
    void set_charge_point_serial_number(const std::string &charge_point_serial_number, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargePointVendor
    std::string get_charge_point_vendor(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargePointVendor to given \p charge_point_vendor
    void set_charge_point_vendor(const std::string &charge_point_vendor, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns FirmwareVersion
    std::string get_firmware_version(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets FirmwareVersion to given \p firmware_version
    void set_firmware_version(const std::string &firmware_version, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ICCID
    std::optional<std::string> get_iccid(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ICCID to given \p iccid
    void set_iccid(const std::string &iccid, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns IMSI
    std::optional<std::string> get_imsi(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets IMSI to given \p imsi
    void set_imsi(const std::string &imsi, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MeterSerialNumber
    std::optional<std::string> get_meter_serial_number(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MeterSerialNumber to given \p meter_serial_number
    void set_meter_serial_number(const std::string &meter_serial_number, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MeterType
    std::optional<std::string> get_meter_type(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MeterType to given \p meter_type
    void set_meter_type(const std::string &meter_type, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SupportedCiphers12
    std::string get_supported_ciphers12(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SupportedCiphers12 to given \p supported_ciphers12
    void set_supported_ciphers12(const std::string &supported_ciphers12, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SupportedCiphers13
    std::string get_supported_ciphers13(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SupportedCiphers13 to given \p supported_ciphers13
    void set_supported_ciphers13(const std::string &supported_ciphers13, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns WebsocketReconnectInterval
    int get_websocket_reconnect_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets WebsocketReconnectInterval to given \p websocket_reconnect_interval
    void set_websocket_reconnect_interval(const int &websocket_reconnect_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AuthorizeConnectorZeroOnConnectorOne
    std::optional<bool> get_authorize_connector_zero_on_connector_one(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AuthorizeConnectorZeroOnConnectorOne to given \p authorize_connector_zero_on_connector_one
    void set_authorize_connector_zero_on_connector_one(const bool &authorize_connector_zero_on_connector_one, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns LogMessages
    std::optional<bool> get_log_messages(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets LogMessages to given \p log_messages
    void set_log_messages(const bool &log_messages, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns LogMessagesFormat
    std::optional<std::string> get_log_messages_format(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets LogMessagesFormat to given \p log_messages_format
    void set_log_messages_format(const std::string &log_messages_format, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SupportedChargingProfilePurposeTypes
    std::optional<std::string> get_supported_charging_profile_purpose_types(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SupportedChargingProfilePurposeTypes to given \p supported_charging_profile_purpose_types
    void set_supported_charging_profile_purpose_types(const std::string &supported_charging_profile_purpose_types, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MaxCompositeScheduleDuration
    std::optional<int> get_max_composite_schedule_duration(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MaxCompositeScheduleDuration to given \p max_composite_schedule_duration
    void set_max_composite_schedule_duration(const int &max_composite_schedule_duration, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NumberOfConnectors
    int get_number_of_connectors(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NumberOfConnectors to given \p number_of_connectors
    void set_number_of_connectors(const int &number_of_connectors, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns UseSslDefaultVerifyPaths
    std::optional<bool> get_use_ssl_default_verify_paths(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets UseSslDefaultVerifyPaths to given \p use_ssl_default_verify_paths
    void set_use_ssl_default_verify_paths(const bool &use_ssl_default_verify_paths, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns OcspRequestInterval
    std::optional<int> get_ocsp_request_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets OcspRequestInterval to given \p ocsp_request_interval
    void set_ocsp_request_interval(const int &ocsp_request_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns WebsocketPingPayload
    std::optional<std::string> get_websocket_ping_payload(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets WebsocketPingPayload to given \p websocket_ping_payload
    void set_websocket_ping_payload(const std::string &websocket_ping_payload, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_aligned_data_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p aligned_data_ctrlr_enabled
    void set_aligned_data_ctrlr_enabled(const bool &aligned_data_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_aligned_data_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p aligned_data_ctrlr_available
    void set_aligned_data_ctrlr_available(const bool &aligned_data_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AlignedDataInterval
    int get_aligned_data_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AlignedDataInterval to given \p aligned_data_interval
    void set_aligned_data_interval(const int &aligned_data_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AlignedDataMeasurands
    std::string get_aligned_data_measurands(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AlignedDataMeasurands to given \p aligned_data_measurands
    void set_aligned_data_measurands(const std::string &aligned_data_measurands, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AlignedDataSendDuringIdle
    std::optional<bool> get_aligned_data_send_during_idle(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AlignedDataSendDuringIdle to given \p aligned_data_send_during_idle
    void set_aligned_data_send_during_idle(const bool &aligned_data_send_during_idle, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AlignedDataSignReadings
    std::optional<bool> get_aligned_data_sign_readings(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AlignedDataSignReadings to given \p aligned_data_sign_readings
    void set_aligned_data_sign_readings(const bool &aligned_data_sign_readings, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AlignedDataTxEndedInterval
    int get_aligned_data_tx_ended_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AlignedDataTxEndedInterval to given \p aligned_data_tx_ended_interval
    void set_aligned_data_tx_ended_interval(const int &aligned_data_tx_ended_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AlignedDataTxEndedMeasurands
    std::string get_aligned_data_tx_ended_measurands(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AlignedDataTxEndedMeasurands to given \p aligned_data_tx_ended_measurands
    void set_aligned_data_tx_ended_measurands(const std::string &aligned_data_tx_ended_measurands, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_auth_cache_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p auth_cache_ctrlr_available
    void set_auth_cache_ctrlr_available(const bool &auth_cache_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_auth_cache_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p auth_cache_ctrlr_enabled
    void set_auth_cache_ctrlr_enabled(const bool &auth_cache_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AuthCacheLifeTime
    std::optional<int> get_auth_cache_life_time(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AuthCacheLifeTime to given \p auth_cache_life_time
    void set_auth_cache_life_time(const int &auth_cache_life_time, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AuthCachePolicy
    std::optional<std::string> get_auth_cache_policy(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AuthCachePolicy to given \p auth_cache_policy
    void set_auth_cache_policy(const std::string &auth_cache_policy, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AuthCacheStorage
    std::optional<int> get_auth_cache_storage(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AuthCacheStorage to given \p auth_cache_storage
    void set_auth_cache_storage(const int &auth_cache_storage, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AuthCacheDisablePostAuthorize
    std::optional<bool> get_auth_cache_disable_post_authorize(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AuthCacheDisablePostAuthorize to given \p auth_cache_disable_post_authorize
    void set_auth_cache_disable_post_authorize(const bool &auth_cache_disable_post_authorize, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_auth_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p auth_ctrlr_enabled
    void set_auth_ctrlr_enabled(const bool &auth_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AdditionalInfoItemsPerMessage
    std::optional<int> get_additional_info_items_per_message(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AdditionalInfoItemsPerMessage to given \p additional_info_items_per_message
    void set_additional_info_items_per_message(const int &additional_info_items_per_message, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AuthorizeRemoteStart
    bool get_authorize_remote_start(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AuthorizeRemoteStart to given \p authorize_remote_start
    void set_authorize_remote_start(const bool &authorize_remote_start, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns LocalAuthorizeOffline
    bool get_local_authorize_offline(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets LocalAuthorizeOffline to given \p local_authorize_offline
    void set_local_authorize_offline(const bool &local_authorize_offline, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns LocalPreAuthorize
    bool get_local_pre_authorize(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets LocalPreAuthorize to given \p local_pre_authorize
    void set_local_pre_authorize(const bool &local_pre_authorize, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MasterPassGroupId
    std::optional<std::string> get_master_pass_group_id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MasterPassGroupId to given \p master_pass_group_id
    void set_master_pass_group_id(const std::string &master_pass_group_id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns OfflineTxForUnknownIdEnabled
    std::optional<bool> get_offline_tx_for_unknown_id_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets OfflineTxForUnknownIdEnabled to given \p offline_tx_for_unknown_id_enabled
    void set_offline_tx_for_unknown_id_enabled(const bool &offline_tx_for_unknown_id_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DisableRemoteAuthorization
    std::optional<bool> get_disable_remote_authorization(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DisableRemoteAuthorization to given \p disable_remote_authorization
    void set_disable_remote_authorization(const bool &disable_remote_authorization, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_chade_moctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p chade_moctrlr_enabled
    void set_chade_moctrlr_enabled(const bool &chade_moctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SelftestActive
    std::optional<bool> get_selftest_active(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SelftestActive to given \p selftest_active
    void set_selftest_active(const bool &selftest_active, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CHAdeMOProtocolNumber
    std::optional<int> get_chade_moprotocol_number(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CHAdeMOProtocolNumber to given \p chade_moprotocol_number
    void set_chade_moprotocol_number(const int &chade_moprotocol_number, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns VehicleStatus
    std::optional<bool> get_vehicle_status(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets VehicleStatus to given \p vehicle_status
    void set_vehicle_status(const bool &vehicle_status, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DynamicControl
    std::optional<bool> get_dynamic_control(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DynamicControl to given \p dynamic_control
    void set_dynamic_control(const bool &dynamic_control, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns HighCurrentControl
    std::optional<bool> get_high_current_control(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets HighCurrentControl to given \p high_current_control
    void set_high_current_control(const bool &high_current_control, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns HighVoltageControl
    std::optional<bool> get_high_voltage_control(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets HighVoltageControl to given \p high_voltage_control
    void set_high_voltage_control(const bool &high_voltage_control, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AutoManufacturerCode
    std::optional<int> get_auto_manufacturer_code(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AutoManufacturerCode to given \p auto_manufacturer_code
    void set_auto_manufacturer_code(const int &auto_manufacturer_code, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AllowNewSessionsPendingFirmwareUpdate
    std::optional<bool> get_allow_new_sessions_pending_firmware_update(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AllowNewSessionsPendingFirmwareUpdate to given \p allow_new_sessions_pending_firmware_update
    void set_allow_new_sessions_pending_firmware_update(const bool &allow_new_sessions_pending_firmware_update, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AvailabilityState
    std::string get_charging_station_availability_state(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AvailabilityState to given \p charging_station_availability_state
    void set_charging_station_availability_state(const std::string &charging_station_availability_state, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    bool get_charging_station_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p charging_station_available
    void set_charging_station_available(const bool &charging_station_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Model
    std::optional<std::string> get_model(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Model to given \p model
    void set_model(const std::string &model, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SupplyPhases
    int get_charging_station_supply_phases(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SupplyPhases to given \p charging_station_supply_phases
    void set_charging_station_supply_phases(const int &charging_station_supply_phases, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns VendorName
    std::optional<std::string> get_vendor_name(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets VendorName to given \p vendor_name
    void set_vendor_name(const std::string &vendor_name, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_clock_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p clock_ctrlr_enabled
    void set_clock_ctrlr_enabled(const bool &clock_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DateTime
    ocpp::DateTime get_date_time(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DateTime to given \p date_time
    void set_date_time(const ocpp::DateTime &date_time, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NextTimeOffsetTransitionDateTime
    std::optional<ocpp::DateTime> get_next_time_offset_transition_date_time(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NextTimeOffsetTransitionDateTime to given \p next_time_offset_transition_date_time
    void set_next_time_offset_transition_date_time(const ocpp::DateTime &next_time_offset_transition_date_time, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NtpServerUri
    std::optional<std::string> get_ntp_server_uri(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NtpServerUri to given \p ntp_server_uri
    void set_ntp_server_uri(const std::string &ntp_server_uri, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NtpSource
    std::optional<std::string> get_ntp_source(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NtpSource to given \p ntp_source
    void set_ntp_source(const std::string &ntp_source, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TimeAdjustmentReportingThreshold
    std::optional<int> get_time_adjustment_reporting_threshold(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TimeAdjustmentReportingThreshold to given \p time_adjustment_reporting_threshold
    void set_time_adjustment_reporting_threshold(const int &time_adjustment_reporting_threshold, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TimeOffset
    std::optional<std::string> get_time_offset(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TimeOffset to given \p time_offset
    void set_time_offset(const std::string &time_offset, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TimeOffset
    std::optional<std::string> get_time_offset_next_transition(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TimeOffset to given \p time_offset_next_transition
    void set_time_offset_next_transition(const std::string &time_offset_next_transition, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TimeSource
    std::string get_time_source(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TimeSource to given \p time_source
    void set_time_source(const std::string &time_source, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TimeZone
    std::optional<std::string> get_time_zone(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TimeZone to given \p time_zone
    void set_time_zone(const std::string &time_zone, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ProtocolAgreed
    std::optional<std::string> get_protocol_agreed(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ProtocolAgreed to given \p protocol_agreed
    void set_protocol_agreed(const std::string &protocol_agreed, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ProtocolSupportedByEV
    std::optional<std::string> get_protocol_supported_by_ev_priority_(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ProtocolSupportedByEV to given \p protocol_supported_by_ev_priority_
    void set_protocol_supported_by_ev_priority_(const std::string &protocol_supported_by_ev_priority_, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns VehicleID
    std::optional<std::string> get_vehicle_id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets VehicleID to given \p vehicle_id
    void set_vehicle_id(const std::string &vehicle_id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AvailabilityState
    std::optional<std::string> get_connector_availability_state(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AvailabilityState to given \p connector_availability_state
    void set_connector_availability_state(const std::string &connector_availability_state, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    bool get_connector_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p connector_available
    void set_connector_available(const bool &connector_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargeProtocol
    std::optional<std::string> get_charge_protocol(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargeProtocol to given \p charge_protocol
    void set_charge_protocol(const std::string &charge_protocol, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ConnectorType
    std::string get_connector_type(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ConnectorType to given \p connector_type
    void set_connector_type(const std::string &connector_type, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SupplyPhases
    int get_connector_supply_phases(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SupplyPhases to given \p connector_supply_phases
    void set_connector_supply_phases(const int &connector_supply_phases, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MaxMsgElements
    std::optional<int> get_max_msg_elements(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MaxMsgElements to given \p max_msg_elements
    void set_max_msg_elements(const int &max_msg_elements, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_cppwmcontroller_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p cppwmcontroller_enabled
    void set_cppwmcontroller_enabled(const bool &cppwmcontroller_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns State
    std::optional<std::string> get_state(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets State to given \p state
    void set_state(const std::string &state, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_customization_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p customization_ctrlr_enabled
    void set_customization_ctrlr_enabled(const bool &customization_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CustomImplementationEnabled
    std::optional<bool> get_custom_implementation_enabled_vendor_id_(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CustomImplementationEnabled to given \p custom_implementation_enabled_vendor_id_
    void set_custom_implementation_enabled_vendor_id_(const bool &custom_implementation_enabled_vendor_id_, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_device_data_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p device_data_ctrlr_enabled
    void set_device_data_ctrlr_enabled(const bool &device_data_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BytesPerMessage
    int get_bytes_per_message_get_report(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BytesPerMessage to given \p bytes_per_message_get_report
    void set_bytes_per_message_get_report(const int &bytes_per_message_get_report, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BytesPerMessage
    int get_bytes_per_message_get_variables(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BytesPerMessage to given \p bytes_per_message_get_variables
    void set_bytes_per_message_get_variables(const int &bytes_per_message_get_variables, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BytesPerMessage
    int get_bytes_per_message_set_variables(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BytesPerMessage to given \p bytes_per_message_set_variables
    void set_bytes_per_message_set_variables(const int &bytes_per_message_set_variables, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ConfigurationValueSize
    std::optional<int> get_configuration_value_size(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ConfigurationValueSize to given \p configuration_value_size
    void set_configuration_value_size(const int &configuration_value_size, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ItemsPerMessage
    int get_items_per_message_get_report(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ItemsPerMessage to given \p items_per_message_get_report
    void set_items_per_message_get_report(const int &items_per_message_get_report, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ItemsPerMessage
    int get_items_per_message_get_variables(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ItemsPerMessage to given \p items_per_message_get_variables
    void set_items_per_message_get_variables(const int &items_per_message_get_variables, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ItemsPerMessage
    int get_items_per_message_set_variables(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ItemsPerMessage to given \p items_per_message_set_variables
    void set_items_per_message_set_variables(const int &items_per_message_set_variables, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ReportingValueSize
    std::optional<int> get_reporting_value_size(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ReportingValueSize to given \p reporting_value_size
    void set_reporting_value_size(const int &reporting_value_size, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ValueSize
    std::optional<int> get_value_size(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ValueSize to given \p value_size
    void set_value_size(const int &value_size, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_display_message_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p display_message_ctrlr_available
    void set_display_message_ctrlr_available(const bool &display_message_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NumberOfDisplayMessages
    int get_number_of_display_messages(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NumberOfDisplayMessages to given \p number_of_display_messages
    void set_number_of_display_messages(const int &number_of_display_messages, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_display_message_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p display_message_ctrlr_enabled
    void set_display_message_ctrlr_enabled(const bool &display_message_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns PersonalMessageSize
    std::optional<int> get_personal_message_size(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets PersonalMessageSize to given \p personal_message_size
    void set_personal_message_size(const int &personal_message_size, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DisplayMessageSupportedFormats
    std::string get_display_message_supported_formats(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DisplayMessageSupportedFormats to given \p display_message_supported_formats
    void set_display_message_supported_formats(const std::string &display_message_supported_formats, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DisplayMessageSupportedPriorities
    std::string get_display_message_supported_priorities(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DisplayMessageSupportedPriorities to given \p display_message_supported_priorities
    void set_display_message_supported_priorities(const std::string &display_message_supported_priorities, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargingStation
    std::optional<std::string> get_charging_station(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargingStation to given \p charging_station
    void set_charging_station(const std::string &charging_station, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DistributionPanel
    std::optional<std::string> get_distribution_panel(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DistributionPanel to given \p distribution_panel
    void set_distribution_panel(const std::string &distribution_panel, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Fuse
    std::optional<int> get_fuse_n_(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Fuse to given \p fuse_n_
    void set_fuse_n_(const int &fuse_n_, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AllowReset
    std::optional<bool> get_allow_reset(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AllowReset to given \p allow_reset
    void set_allow_reset(const bool &allow_reset, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AvailabilityState
    std::string get_evse_availability_state(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AvailabilityState to given \p evse_availability_state
    void set_evse_availability_state(const std::string &evse_availability_state, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    bool get_evse_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p evse_available
    void set_evse_available(const bool &evse_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns EvseId
    std::optional<std::string> get_evse__id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets EvseId to given \p evse__id
    void set_evse__id(const std::string &evse__id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Power
    double get_power(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Power to given \p power
    void set_power(const double &power, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SupplyPhases
    int get_evse_supply_phases(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SupplyPhases to given \p evse_supply_phases
    void set_evse_supply_phases(const int &evse_supply_phases, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ISO15118EvseId
    std::optional<std::string> get_iso15118_evse__id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ISO15118EvseId to given \p iso15118_evse__id
    void set_iso15118_evse__id(const std::string &iso15118_evse__id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns EnergyExport
    std::optional<double> get_energy_export(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets EnergyExport to given \p energy_export
    void set_energy_export(const double &energy_export, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns EnergyExportRegister
    std::optional<double> get_energy_export_register(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets EnergyExportRegister to given \p energy_export_register
    void set_energy_export_register(const double &energy_export_register, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns EnergyImport
    std::optional<double> get_energy_import(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets EnergyImport to given \p energy_import
    void set_energy_import(const double &energy_import, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns EnergyImportRegister
    std::optional<double> get_energy_import_register(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets EnergyImportRegister to given \p energy_import_register
    void set_energy_import_register(const double &energy_import_register, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_iso15118ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p iso15118ctrlr_enabled
    void set_iso15118ctrlr_enabled(const bool &iso15118ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CentralContractValidationAllowed
    std::optional<bool> get_central_contract_validation_allowed(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CentralContractValidationAllowed to given \p central_contract_validation_allowed
    void set_central_contract_validation_allowed(const bool &central_contract_validation_allowed, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ContractValidationOffline
    bool get_contract_validation_offline(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ContractValidationOffline to given \p contract_validation_offline
    void set_contract_validation_offline(const bool &contract_validation_offline, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SeccId
    std::optional<std::string> get_secc_id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SeccId to given \p secc_id
    void set_secc_id(const std::string &secc_id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MaxScheduleEntries
    std::optional<int> get_max_schedule_entries(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MaxScheduleEntries to given \p max_schedule_entries
    void set_max_schedule_entries(const int &max_schedule_entries, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns RequestedEnergyTransferMode
    std::optional<std::string> get_requested_energy_transfer_mode(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets RequestedEnergyTransferMode to given \p requested_energy_transfer_mode
    void set_requested_energy_transfer_mode(const std::string &requested_energy_transfer_mode, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns RequestMeteringReceipt
    std::optional<bool> get_request_metering_receipt(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets RequestMeteringReceipt to given \p request_metering_receipt
    void set_request_metering_receipt(const bool &request_metering_receipt, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CountryName
    std::optional<std::string> get_country_name(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CountryName to given \p country_name
    void set_country_name(const std::string &country_name, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns OrganizationName
    std::optional<std::string> get_iso15118ctrlr_organization_name(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets OrganizationName to given \p iso15118ctrlr_organization_name
    void set_iso15118ctrlr_organization_name(const std::string &iso15118ctrlr_organization_name, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns PnCEnabled
    std::optional<bool> get_pn_cenabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets PnCEnabled to given \p pn_cenabled
    void set_pn_cenabled(const bool &pn_cenabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns V2GCertificateInstallationEnabled
    std::optional<bool> get_v2gcertificate_installation_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets V2GCertificateInstallationEnabled to given \p v2gcertificate_installation_enabled
    void set_v2gcertificate_installation_enabled(const bool &v2gcertificate_installation_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ContractCertificateInstallationEnabled
    std::optional<bool> get_contract_certificate_installation_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ContractCertificateInstallationEnabled to given \p contract_certificate_installation_enabled
    void set_contract_certificate_installation_enabled(const bool &contract_certificate_installation_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_local_auth_list_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p local_auth_list_ctrlr_available
    void set_local_auth_list_ctrlr_available(const bool &local_auth_list_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BytesPerMessageSendLocalList
    int get_bytes_per_message_send_local_list(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BytesPerMessageSendLocalList to given \p bytes_per_message_send_local_list
    void set_bytes_per_message_send_local_list(const int &bytes_per_message_send_local_list, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_local_auth_list_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p local_auth_list_ctrlr_enabled
    void set_local_auth_list_ctrlr_enabled(const bool &local_auth_list_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Entries
    int get_entries(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Entries to given \p entries
    void set_entries(const int &entries, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ItemsPerMessageSendLocalList
    int get_items_per_message_send_local_list(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ItemsPerMessageSendLocalList to given \p items_per_message_send_local_list
    void set_items_per_message_send_local_list(const int &items_per_message_send_local_list, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Storage
    std::optional<int> get_storage(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Storage to given \p storage
    void set_storage(const int &storage, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns DisablePostAuthorize
    std::optional<bool> get_disable_post_authorize(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets DisablePostAuthorize to given \p disable_post_authorize
    void set_disable_post_authorize(const bool &disable_post_authorize, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Capacity
    std::optional<double> get_capacity(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Capacity to given \p capacity
    void set_capacity(const double &capacity, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_monitoring_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p monitoring_ctrlr_available
    void set_monitoring_ctrlr_available(const bool &monitoring_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BytesPerMessage
    std::optional<int> get_bytes_per_message_clear_variable_monitoring(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BytesPerMessage to given \p bytes_per_message_clear_variable_monitoring
    void set_bytes_per_message_clear_variable_monitoring(const int &bytes_per_message_clear_variable_monitoring, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BytesPerMessage
    int get_bytes_per_message_set_variable_monitoring(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BytesPerMessage to given \p bytes_per_message_set_variable_monitoring
    void set_bytes_per_message_set_variable_monitoring(const int &bytes_per_message_set_variable_monitoring, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_monitoring_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p monitoring_ctrlr_enabled
    void set_monitoring_ctrlr_enabled(const bool &monitoring_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ItemsPerMessage
    std::optional<int> get_items_per_message_clear_variable_monitoring(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ItemsPerMessage to given \p items_per_message_clear_variable_monitoring
    void set_items_per_message_clear_variable_monitoring(const int &items_per_message_clear_variable_monitoring, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ItemsPerMessage
    int get_items_per_message_set_variable_monitoring(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ItemsPerMessage to given \p items_per_message_set_variable_monitoring
    void set_items_per_message_set_variable_monitoring(const int &items_per_message_set_variable_monitoring, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns OfflineQueuingSeverity
    std::optional<int> get_offline_queuing_severity(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets OfflineQueuingSeverity to given \p offline_queuing_severity
    void set_offline_queuing_severity(const int &offline_queuing_severity, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MonitoringBase
    std::optional<std::string> get_monitoring_base(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MonitoringBase to given \p monitoring_base
    void set_monitoring_base(const std::string &monitoring_base, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MonitoringLevel
    std::optional<int> get_monitoring_level(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MonitoringLevel to given \p monitoring_level
    void set_monitoring_level(const int &monitoring_level, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ActiveMonitoringBase
    std::optional<std::string> get_active_monitoring_base(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ActiveMonitoringBase to given \p active_monitoring_base
    void set_active_monitoring_base(const std::string &active_monitoring_base, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ActiveMonitoringLevel
    std::optional<int> get_active_monitoring_level(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ActiveMonitoringLevel to given \p active_monitoring_level
    void set_active_monitoring_level(const int &active_monitoring_level, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_ocppcomm_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p ocppcomm_ctrlr_enabled
    void set_ocppcomm_ctrlr_enabled(const bool &ocppcomm_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ActiveNetworkProfile
    std::optional<std::string> get_active_network_profile(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ActiveNetworkProfile to given \p active_network_profile
    void set_active_network_profile(const std::string &active_network_profile, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns FileTransferProtocols
    std::string get_file_transfer_protocols(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets FileTransferProtocols to given \p file_transfer_protocols
    void set_file_transfer_protocols(const std::string &file_transfer_protocols, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns HeartbeatInterval
    std::optional<int> get_heartbeat_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets HeartbeatInterval to given \p heartbeat_interval
    void set_heartbeat_interval(const int &heartbeat_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MessageTimeout
    int get_message_timeout_default(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MessageTimeout to given \p message_timeout_default
    void set_message_timeout_default(const int &message_timeout_default, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MessageAttemptInterval
    int get_message_attempt_interval_transaction_event(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MessageAttemptInterval to given \p message_attempt_interval_transaction_event
    void set_message_attempt_interval_transaction_event(const int &message_attempt_interval_transaction_event, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MessageAttempts
    int get_message_attempts_transaction_event(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MessageAttempts to given \p message_attempts_transaction_event
    void set_message_attempts_transaction_event(const int &message_attempts_transaction_event, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NetworkConfigurationPriority
    std::string get_network_configuration_priority(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NetworkConfigurationPriority to given \p network_configuration_priority
    void set_network_configuration_priority(const std::string &network_configuration_priority, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NetworkProfileConnectionAttempts
    int get_network_profile_connection_attempts(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NetworkProfileConnectionAttempts to given \p network_profile_connection_attempts
    void set_network_profile_connection_attempts(const int &network_profile_connection_attempts, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns OfflineThreshold
    int get_offline_threshold(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets OfflineThreshold to given \p offline_threshold
    void set_offline_threshold(const int &offline_threshold, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns PublicKeyWithSignedMeterValue
    std::optional<std::string> get_public_key_with_signed_meter_value(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets PublicKeyWithSignedMeterValue to given \p public_key_with_signed_meter_value
    void set_public_key_with_signed_meter_value(const std::string &public_key_with_signed_meter_value, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns QueueAllMessages
    std::optional<bool> get_queue_all_messages(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets QueueAllMessages to given \p queue_all_messages
    void set_queue_all_messages(const bool &queue_all_messages, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ResetRetries
    int get_reset_retries(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ResetRetries to given \p reset_retries
    void set_reset_retries(const int &reset_retries, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns RetryBackOffRandomRange
    std::optional<int> get_retry_back_off_random_range(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets RetryBackOffRandomRange to given \p retry_back_off_random_range
    void set_retry_back_off_random_range(const int &retry_back_off_random_range, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns RetryBackOffRepeatTimes
    std::optional<int> get_retry_back_off_repeat_times(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets RetryBackOffRepeatTimes to given \p retry_back_off_repeat_times
    void set_retry_back_off_repeat_times(const int &retry_back_off_repeat_times, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns RetryBackOffWaitMinimum
    std::optional<int> get_retry_back_off_wait_minimum(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets RetryBackOffWaitMinimum to given \p retry_back_off_wait_minimum
    void set_retry_back_off_wait_minimum(const int &retry_back_off_wait_minimum, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns UnlockOnEVSideDisconnect
    bool get_unlock_on_evside_disconnect(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets UnlockOnEVSideDisconnect to given \p unlock_on_evside_disconnect
    void set_unlock_on_evside_disconnect(const bool &unlock_on_evside_disconnect, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns WebSocketPingInterval
    std::optional<int> get_web_socket_ping_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets WebSocketPingInterval to given \p web_socket_ping_interval
    void set_web_socket_ping_interval(const int &web_socket_ping_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns FieldLength
    std::optional<int> get_field_length(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets FieldLength to given \p field_length
    void set_field_length(const int &field_length, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_reservation_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p reservation_ctrlr_available
    void set_reservation_ctrlr_available(const bool &reservation_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_reservation_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p reservation_ctrlr_enabled
    void set_reservation_ctrlr_enabled(const bool &reservation_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NonEvseSpecific
    std::optional<bool> get_non__evse__specific(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NonEvseSpecific to given \p non__evse__specific
    void set_non__evse__specific(const bool &non__evse__specific, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_sampled_data_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p sampled_data_ctrlr_available
    void set_sampled_data_ctrlr_available(const bool &sampled_data_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_sampled_data_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p sampled_data_ctrlr_enabled
    void set_sampled_data_ctrlr_enabled(const bool &sampled_data_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SampledDataSignReadings
    std::optional<bool> get_sampled_data_sign_readings(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SampledDataSignReadings to given \p sampled_data_sign_readings
    void set_sampled_data_sign_readings(const bool &sampled_data_sign_readings, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SampledDataTxEndedInterval
    int get_sampled_data_tx_ended_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SampledDataTxEndedInterval to given \p sampled_data_tx_ended_interval
    void set_sampled_data_tx_ended_interval(const int &sampled_data_tx_ended_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SampledDataTxEndedMeasurands
    std::string get_sampled_data_tx_ended_measurands(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SampledDataTxEndedMeasurands to given \p sampled_data_tx_ended_measurands
    void set_sampled_data_tx_ended_measurands(const std::string &sampled_data_tx_ended_measurands, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SampledDataTxStartedMeasurands
    std::string get_sampled_data_tx_started_measurands(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SampledDataTxStartedMeasurands to given \p sampled_data_tx_started_measurands
    void set_sampled_data_tx_started_measurands(const std::string &sampled_data_tx_started_measurands, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SampledDataTxUpdatedInterval
    int get_sampled_data_tx_updated_interval(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SampledDataTxUpdatedInterval to given \p sampled_data_tx_updated_interval
    void set_sampled_data_tx_updated_interval(const int &sampled_data_tx_updated_interval, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SampledDataTxUpdatedMeasurands
    std::string get_sampled_data_tx_updated_measurands(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SampledDataTxUpdatedMeasurands to given \p sampled_data_tx_updated_measurands
    void set_sampled_data_tx_updated_measurands(const std::string &sampled_data_tx_updated_measurands, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns RegisterValuesWithoutPhases
    std::optional<bool> get_register_values_without_phases(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets RegisterValuesWithoutPhases to given \p register_values_without_phases
    void set_register_values_without_phases(const bool &register_values_without_phases, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_security_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p security_ctrlr_enabled
    void set_security_ctrlr_enabled(const bool &security_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns AdditionalRootCertificateCheck
    std::optional<bool> get_additional_root_certificate_check(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets AdditionalRootCertificateCheck to given \p additional_root_certificate_check
    void set_additional_root_certificate_check(const bool &additional_root_certificate_check, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns BasicAuthPassword
    std::optional<std::string> get_basic_auth_password(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets BasicAuthPassword to given \p basic_auth_password
    void set_basic_auth_password(const std::string &basic_auth_password, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CertificateEntries
    int get_certificate_entries(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CertificateEntries to given \p certificate_entries
    void set_certificate_entries(const int &certificate_entries, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CertSigningRepeatTimes
    std::optional<int> get_cert_signing_repeat_times(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CertSigningRepeatTimes to given \p cert_signing_repeat_times
    void set_cert_signing_repeat_times(const int &cert_signing_repeat_times, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns CertSigningWaitMinimum
    std::optional<int> get_cert_signing_wait_minimum(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets CertSigningWaitMinimum to given \p cert_signing_wait_minimum
    void set_cert_signing_wait_minimum(const int &cert_signing_wait_minimum, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Identity
    std::optional<std::string> get_identity(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Identity to given \p identity
    void set_identity(const std::string &identity, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MaxCertificateChainSize
    std::optional<int> get_max_certificate_chain_size(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MaxCertificateChainSize to given \p max_certificate_chain_size
    void set_max_certificate_chain_size(const int &max_certificate_chain_size, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns OrganizationName
    std::string get_security_ctrlr_organization_name(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets OrganizationName to given \p security_ctrlr_organization_name
    void set_security_ctrlr_organization_name(const std::string &security_ctrlr_organization_name, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns SecurityProfile
    int get_security_profile(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets SecurityProfile to given \p security_profile
    void set_security_profile(const int &security_profile, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ACPhaseSwitchingSupported
    std::optional<bool> get_acphase_switching_supported(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ACPhaseSwitchingSupported to given \p acphase_switching_supported
    void set_acphase_switching_supported(const bool &acphase_switching_supported, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_smart_charging_ctrlr_available(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p smart_charging_ctrlr_available
    void set_smart_charging_ctrlr_available(const bool &smart_charging_ctrlr_available, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_smart_charging_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p smart_charging_ctrlr_enabled
    void set_smart_charging_ctrlr_enabled(const bool &smart_charging_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Entries
    int get_entries_charging_profiles(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Entries to given \p entries_charging_profiles
    void set_entries_charging_profiles(const int &entries_charging_profiles, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ExternalControlSignalsEnabled
    std::optional<bool> get_external_control_signals_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ExternalControlSignalsEnabled to given \p external_control_signals_enabled
    void set_external_control_signals_enabled(const bool &external_control_signals_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns LimitChangeSignificance
    double get_limit_change_significance(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets LimitChangeSignificance to given \p limit_change_significance
    void set_limit_change_significance(const double &limit_change_significance, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns NotifyChargingLimitWithSchedules
    std::optional<bool> get_notify_charging_limit_with_schedules(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets NotifyChargingLimitWithSchedules to given \p notify_charging_limit_with_schedules
    void set_notify_charging_limit_with_schedules(const bool &notify_charging_limit_with_schedules, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns PeriodsPerSchedule
    int get_periods_per_schedule(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets PeriodsPerSchedule to given \p periods_per_schedule
    void set_periods_per_schedule(const int &periods_per_schedule, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Phases3to1
    std::optional<bool> get_phases3to1(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Phases3to1 to given \p phases3to1
    void set_phases3to1(const bool &phases3to1, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargingProfileMaxStackLevel
    int get_charging_profile_max_stack_level(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargingProfileMaxStackLevel to given \p charging_profile_max_stack_level
    void set_charging_profile_max_stack_level(const int &charging_profile_max_stack_level, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargingScheduleChargingRateUnit
    std::string get_charging_schedule_charging_rate_unit(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargingScheduleChargingRateUnit to given \p charging_schedule_charging_rate_unit
    void set_charging_schedule_charging_rate_unit(const std::string &charging_schedule_charging_rate_unit, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_available_tariff(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p available_tariff
    void set_available_tariff(const bool &available_tariff, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Available
    std::optional<bool> get_available_cost(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Available to given \p available_cost
    void set_available_cost(const bool &available_cost, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Currency
    std::string get_currency(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Currency to given \p currency
    void set_currency(const std::string &currency, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_enabled_tariff(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p enabled_tariff
    void set_enabled_tariff(const bool &enabled_tariff, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_enabled_cost(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p enabled_cost
    void set_enabled_cost(const bool &enabled_cost, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TariffFallbackMessage
    std::string get_tariff_fallback_message(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TariffFallbackMessage to given \p tariff_fallback_message
    void set_tariff_fallback_message(const std::string &tariff_fallback_message, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TotalCostFallbackMessage
    std::string get_total_cost_fallback_message(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TotalCostFallbackMessage to given \p total_cost_fallback_message
    void set_total_cost_fallback_message(const std::string &total_cost_fallback_message, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Token
    std::optional<std::string> get_token(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Token to given \p token
    void set_token(const std::string &token, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TokenType
    std::optional<std::string> get_token_type(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TokenType to given \p token_type
    void set_token_type(const std::string &token_type, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns Enabled
    std::optional<bool> get_tx_ctrlr_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets Enabled to given \p tx_ctrlr_enabled
    void set_tx_ctrlr_enabled(const bool &tx_ctrlr_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns ChargingTime
    std::optional<double> get_charging_time(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets ChargingTime to given \p charging_time
    void set_charging_time(const double &charging_time, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns EVConnectionTimeOut
    int get_evconnection_time_out(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets EVConnectionTimeOut to given \p evconnection_time_out
    void set_evconnection_time_out(const int &evconnection_time_out, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns MaxEnergyOnInvalidId
    std::optional<int> get_max_energy_on_invalid_id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets MaxEnergyOnInvalidId to given \p max_energy_on_invalid_id
    void set_max_energy_on_invalid_id(const int &max_energy_on_invalid_id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns StopTxOnEVSideDisconnect
    bool get_stop_tx_on_evside_disconnect(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets StopTxOnEVSideDisconnect to given \p stop_tx_on_evside_disconnect
    void set_stop_tx_on_evside_disconnect(const bool &stop_tx_on_evside_disconnect, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns StopTxOnInvalidId
    bool get_stop_tx_on_invalid_id(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets StopTxOnInvalidId to given \p stop_tx_on_invalid_id
    void set_stop_tx_on_invalid_id(const bool &stop_tx_on_invalid_id, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TxBeforeAcceptedEnabled
    std::optional<bool> get_tx_before_accepted_enabled(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TxBeforeAcceptedEnabled to given \p tx_before_accepted_enabled
    void set_tx_before_accepted_enabled(const bool &tx_before_accepted_enabled, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TxStartPoint
    std::string get_tx_start_point(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TxStartPoint to given \p tx_start_point
    void set_tx_start_point(const std::string &tx_start_point, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    /// \brief Returns TxStopPoint
    std::string get_tx_stop_point(const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    
    /// \brief Sets TxStopPoint to given \p tx_stop_point
    void set_tx_stop_point(const std::string &tx_stop_point, const AttributeEnum &attribute_enum = AttributeEnum::Actual);
    

};

} //namespace v201
} // namespace ocpp
