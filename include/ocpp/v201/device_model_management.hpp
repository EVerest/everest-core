// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <boost/optional.hpp>
#include <map>

#include <ocpp/common/pki_handler.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Enum for all standardized components for OCPP2.0.1 (from appendices)
enum class StandardizedComponent {
    ConnectedEV,
    SecurityCtrlr,
    DeviceDataCtrlr,
    ReservationCtrlr,
    CustomizationCtrlr,
    OCPPCommCtrlr,
    DistributionPanel,
    MonitoringCtrlr,
    TxCtrlr,
    EVSE,
    InternalCtrlr,
    SmartChargingCtrlr,
    AlignedDataCtrlr,
    ChargingStation,
    Controller,
    FiscalMetering,
    ClockCtrlr,
    CHAdeMOCtrlr,
    TokenReader,
    LocalEnergyStorage,
    Connector,
    LocalAuthListCtrlr,
    AuthCacheCtrlr,
    AuthCtrlr,
    CPPWMController,
    DisplayMessageCtrlr,
    ISO15118Ctrlr,
    TariffCostCtrlr,
    SampledDataCtrlr
};

namespace conversions {
/// \brief Converts the given StandardizedComponent \p e to human readable string
/// \returns a string representation of the StandardizedComponent
std::string standardized_component_to_string(StandardizedComponent e);

/// \brief Converts the given std::string \p s to StandardizedComponent
/// \returns a StandardizedComponent from a string representation
StandardizedComponent string_to_standardized_component(const std::string& s);
} // namespace conversions

/// \brief Writes the string representation of the given StandardizedComponent \p standardized_component to the given
/// output stream \p os \returns an output stream with the StandardizedComponent written to
std::ostream& operator<<(std::ostream& os, const StandardizedComponent& standardized_component);

/// \brief Variable extended by VariableCharacteristics, VariableAttribute and VariableMonitoring
struct EnhancedVariable : Variable {
    std::map<AttributeEnum, VariableAttribute> attributes;
    boost::optional<VariableCharacteristics> characteristics;
    std::vector<VariableMonitoring> monitorings;

    bool operator==(const Variable& other) const {
        // variables are equal when they have an equal name and equal or no instance
        return this->name == other.name and ((!this->instance.has_value() and !other.instance.has_value()) or
                                             this->instance.value() == other.instance.value());
    };
};

/// \brief Component extended by a collection of EnhancedVariables
struct EnhancedComponent : Component {
    EnhancedComponent(const std::string& name) {
        this->name = name;
    }
    std::map<std::string, EnhancedVariable> variables; // key is variable name

    bool operator==(const Component& other) const {
        // components are equal when they have an equal name and equal or no instance
        return this->name == other.name and ((!this->instance.has_value() and !other.instance.has_value()) or
                                             this->instance.value() == other.instance.value());
    };
};

/// \brief Class for configuration and monitoring of OCPP components and variables
class DeviceModelManager {
private:
    std::map<StandardizedComponent, EnhancedComponent> components;

public:
    /// \brief Construct a new DeviceModelManager object
    /// \param config OCPP json config
    /// \param ocpp_main_path path where utility files for OCPP are read and written to
    DeviceModelManager(const json& config, const std::string& ocpp_main_path);

    /// \brief Set the variable specified by \p set_variable_data
    /// \param set_variable_data specifies the variable to be set
    /// \return SetVariableStatusEnum indicating the result of the operation
    SetVariableStatusEnum set_variable(const SetVariableData& set_variable_data);

    /// \brief Get the variable specified by \p get_variable_data
    /// \param get_variable_data specifies the variable to get
    /// \return std::pair<GetVariableStatusEnum, boost::optional<CiString<2500>>> first item of the pair indicates the
    /// result of the operation and the second item optionally contains the value
    std::pair<GetVariableStatusEnum, boost::optional<CiString<2500>>>
    get_variable(const GetVariableData& get_variable_data);

    /// \brief This function returns an std::vector<ReportData> based on the options specified by the arguments \p
    /// report_base, \p component_variables and \p component_criteria
    /// \param report_base optionally specifies the type of inventory data that should be included in the result
    /// \param component_variables optionally specifies std::vector<ComponentVariables> that should be included in the
    /// result
    /// \param component_criteria optionally specifies std::vector<ComponentCriterionEnum> that should be
    /// included in the result
    /// \return std::vector<ReportData>
    std::vector<ReportData>
    get_report_data(const boost::optional<ReportBaseEnum>& report_base = boost::none,
                    const boost::optional<std::vector<ComponentVariable>>& component_variables = boost::none,
                    const boost::optional<std::vector<ComponentCriterionEnum>>& component_criteria = boost::none);

    /// \brief Returns ChargePointId
    std::string get_charge_point_id();

    /// \brief Sets ChargePointId to given \p charge_point_id
    void set_charge_point_id(const std::string& charge_point_id);

    /// \brief Returns CentralSystemURI
    std::string get_central_system_uri();

    /// \brief Sets CentralSystemURI to given \p central_system_uri
    void set_central_system_uri(const std::string& central_system_uri);

    /// \brief Returns ChargeBoxSerialNumber
    std::string get_charge_box_serial_number();

    /// \brief Sets ChargeBoxSerialNumber to given \p charge_box_serial_number
    void set_charge_box_serial_number(const std::string& charge_box_serial_number);

    /// \brief Returns ChargePointModel
    std::string get_charge_point_model();

    /// \brief Sets ChargePointModel to given \p charge_point_model
    void set_charge_point_model(const std::string& charge_point_model);

    /// \brief Returns ChargePointVendor
    std::string get_charge_point_vendor();

    /// \brief Sets ChargePointVendor to given \p charge_point_vendor
    void set_charge_point_vendor(const std::string& charge_point_vendor);

    /// \brief Returns FirmwareVersion
    std::string get_firmware_version();

    /// \brief Sets FirmwareVersion to given \p firmware_version
    void set_firmware_version(const std::string& firmware_version);

    /// \brief Returns SupportedCiphers12
    std::string get_supported_ciphers12();

    /// \brief Sets SupportedCiphers12 to given \p supported_ciphers12
    void set_supported_ciphers12(const std::string& supported_ciphers12);

    /// \brief Returns SupportedCiphers13
    std::string get_supported_ciphers13();

    /// \brief Sets SupportedCiphers13 to given \p supported_ciphers13
    void set_supported_ciphers13(const std::string& supported_ciphers13);

    /// \brief Returns WebsocketReconnectInterval
    int32_t get_websocket_reconnect_interval();

    /// \brief Sets WebsocketReconnectInterval to given \p websocket_reconnect_interval
    void set_websocket_reconnect_interval(const int32_t& websocket_reconnect_interval);

    /// \brief Returns NumberOfConnectors
    int32_t get_number_of_connectors();

    /// \brief Sets NumberOfConnectors to given \p number_of_connectors
    void set_number_of_connectors(const int32_t& number_of_connectors);

    /// \brief Returns Interval
    int32_t get_interval();

    /// \brief Sets Interval to given \p interval
    void set_interval(const int32_t& interval);

    /// \brief Returns Measurands
    std::string get_measurands();

    /// \brief Sets Measurands to given \p measurands
    void set_measurands(const std::string& measurands);

    /// \brief Returns TxEndedInterval
    int32_t get_aligned_data_ctrlr_tx_ended_interval();

    /// \brief Sets TxEndedInterval to given \p aligned_data_ctrlr_tx_ended_interval
    void set_aligned_data_ctrlr_tx_ended_interval(const int32_t& aligned_data_ctrlr_tx_ended_interval);

    /// \brief Returns TxEndedMeasurands
    std::string get_aligned_data_ctrlr_tx_ended_measurands();

    /// \brief Sets TxEndedMeasurands to given \p aligned_data_ctrlr_tx_ended_measurands
    void set_aligned_data_ctrlr_tx_ended_measurands(const std::string& aligned_data_ctrlr_tx_ended_measurands);

    /// \brief Returns AuthorizeRemoteStart
    bool get_authorize_remote_start();

    /// \brief Sets AuthorizeRemoteStart to given \p authorize_remote_start
    void set_authorize_remote_start(const bool& authorize_remote_start);

    /// \brief Returns LocalAuthorizeOffline
    bool get_local_authorize_offline();

    /// \brief Sets LocalAuthorizeOffline to given \p local_authorize_offline
    void set_local_authorize_offline(const bool& local_authorize_offline);

    /// \brief Returns LocalPreAuthorize
    bool get_local_pre_authorize();

    /// \brief Sets LocalPreAuthorize to given \p local_pre_authorize
    void set_local_pre_authorize(const bool& local_pre_authorize);

    /// \brief Returns AvailabilityState
    std::string get_charging_station_availability_state();

    /// \brief Sets AvailabilityState to given \p charging_station_availability_state
    void set_charging_station_availability_state(const std::string& charging_station_availability_state);

    /// \brief Returns Available
    bool get_charging_station_available();

    /// \brief Sets Available to given \p charging_station_available
    void set_charging_station_available(const bool& charging_station_available);

    /// \brief Returns SupplyPhases
    int32_t get_charging_station_supply_phases();

    /// \brief Sets SupplyPhases to given \p charging_station_supply_phases
    void set_charging_station_supply_phases(const int32_t& charging_station_supply_phases);

    /// \brief Returns DateTime
    ocpp::DateTime get_date_time();

    /// \brief Sets DateTime to given \p date_time
    void set_date_time(const ocpp::DateTime& date_time);

    /// \brief Returns TimeSource
    std::string get_time_source();

    /// \brief Sets TimeSource to given \p time_source
    void set_time_source(const std::string& time_source);

    /// \brief Returns Available
    bool get_connector_available();

    /// \brief Sets Available to given \p connector_available
    void set_connector_available(const bool& connector_available);

    /// \brief Returns ConnectorType
    std::string get_connector_type();

    /// \brief Sets ConnectorType to given \p connector_type
    void set_connector_type(const std::string& connector_type);

    /// \brief Returns SupplyPhases
    int32_t get_connector_supply_phases();

    /// \brief Sets SupplyPhases to given \p connector_supply_phases
    void set_connector_supply_phases(const int32_t& connector_supply_phases);

    /// \brief Returns BytesPerMessage
    int32_t get_bytes_per_message_get_report();

    /// \brief Sets BytesPerMessage to given \p bytes_per_message_get_report
    void set_bytes_per_message_get_report(const int32_t& bytes_per_message_get_report);

    /// \brief Returns BytesPerMessage
    int32_t get_bytes_per_message_get_variables();

    /// \brief Sets BytesPerMessage to given \p bytes_per_message_get_variables
    void set_bytes_per_message_get_variables(const int32_t& bytes_per_message_get_variables);

    /// \brief Returns BytesPerMessage
    int32_t get_bytes_per_message_set_variables();

    /// \brief Sets BytesPerMessage to given \p bytes_per_message_set_variables
    void set_bytes_per_message_set_variables(const int32_t& bytes_per_message_set_variables);

    /// \brief Returns ItemsPerMessage
    int32_t get_items_per_message_get_report();

    /// \brief Sets ItemsPerMessage to given \p items_per_message_get_report
    void set_items_per_message_get_report(const int32_t& items_per_message_get_report);

    /// \brief Returns ItemsPerMessage
    int32_t get_items_per_message_get_variables();

    /// \brief Sets ItemsPerMessage to given \p items_per_message_get_variables
    void set_items_per_message_get_variables(const int32_t& items_per_message_get_variables);

    /// \brief Returns ItemsPerMessage
    int32_t get_items_per_message_set_variables();

    /// \brief Sets ItemsPerMessage to given \p items_per_message_set_variables
    void set_items_per_message_set_variables(const int32_t& items_per_message_set_variables);

    /// \brief Returns DisplayMessages
    int32_t get_display_messages();

    /// \brief Sets DisplayMessages to given \p display_messages
    void set_display_messages(const int32_t& display_messages);

    /// \brief Returns SupportedFormats
    std::string get_supported_formats();

    /// \brief Sets SupportedFormats to given \p supported_formats
    void set_supported_formats(const std::string& supported_formats);

    /// \brief Returns SupportedPriorities
    std::string get_supported_priorities();

    /// \brief Sets SupportedPriorities to given \p supported_priorities
    void set_supported_priorities(const std::string& supported_priorities);

    /// \brief Returns AvailabilityState
    std::string get_evse_availability_state();

    /// \brief Sets AvailabilityState to given \p evse_availability_state
    void set_evse_availability_state(const std::string& evse_availability_state);

    /// \brief Returns Available
    bool get_evse_available();

    /// \brief Sets Available to given \p evse_available
    void set_evse_available(const bool& evse_available);

    /// \brief Returns Power
    double get_power();

    /// \brief Sets Power to given \p power
    void set_power(const double& power);

    /// \brief Returns SupplyPhases
    int32_t get_evse_supply_phases();

    /// \brief Sets SupplyPhases to given \p evse_supply_phases
    void set_evse_supply_phases(const int32_t& evse_supply_phases);

    /// \brief Returns ContractValidationOffline
    bool get_contract_validation_offline();

    /// \brief Sets ContractValidationOffline to given \p contract_validation_offline
    void set_contract_validation_offline(const bool& contract_validation_offline);

    /// \brief Returns BytesPerMessage
    int32_t get_bytes_per_message();

    /// \brief Sets BytesPerMessage to given \p bytes_per_message
    void set_bytes_per_message(const int32_t& bytes_per_message);

    /// \brief Returns Entries
    int32_t get_entries();

    /// \brief Sets Entries to given \p entries
    void set_entries(const int32_t& entries);

    /// \brief Returns ItemsPerMessage
    int32_t get_items_per_message();

    /// \brief Sets ItemsPerMessage to given \p items_per_message
    void set_items_per_message(const int32_t& items_per_message);

    /// \brief Returns BytesPerMessage
    int32_t get_bytes_per_message_set_variable_monitoring();

    /// \brief Sets BytesPerMessage to given \p bytes_per_message_set_variable_monitoring
    void set_bytes_per_message_set_variable_monitoring(const int32_t& bytes_per_message_set_variable_monitoring);

    /// \brief Returns ItemsPerMessage
    int32_t get_items_per_message_set_variable_monitoring();

    /// \brief Sets ItemsPerMessage to given \p items_per_message_set_variable_monitoring
    void set_items_per_message_set_variable_monitoring(const int32_t& items_per_message_set_variable_monitoring);

    /// \brief Returns FileTransferProtocols
    std::string get_file_transfer_protocols();

    /// \brief Sets FileTransferProtocols to given \p file_transfer_protocols
    void set_file_transfer_protocols(const std::string& file_transfer_protocols);

    /// \brief Returns MessageTimeout
    int32_t get_message_timeout_default();

    /// \brief Sets MessageTimeout to given \p message_timeout_default
    void set_message_timeout_default(const int32_t& message_timeout_default);

    /// \brief Returns MessageAttemptInterval
    int32_t get_message_attempt_interval_transaction_event();

    /// \brief Sets MessageAttemptInterval to given \p message_attempt_interval_transaction_event
    void set_message_attempt_interval_transaction_event(const int32_t& message_attempt_interval_transaction_event);

    /// \brief Returns MessageAttempts
    int32_t get_message_attempts_transaction_event();

    /// \brief Sets MessageAttempts to given \p message_attempts_transaction_event
    void set_message_attempts_transaction_event(const int32_t& message_attempts_transaction_event);

    /// \brief Returns NetworkConfigurationPriority
    std::string get_network_configuration_priority();

    /// \brief Sets NetworkConfigurationPriority to given \p network_configuration_priority
    void set_network_configuration_priority(const std::string& network_configuration_priority);

    /// \brief Returns NetworkProfileConnectionAttempts
    int32_t get_network_profile_connection_attempts();

    /// \brief Sets NetworkProfileConnectionAttempts to given \p network_profile_connection_attempts
    void set_network_profile_connection_attempts(const int32_t& network_profile_connection_attempts);

    /// \brief Returns OfflineThreshold
    int32_t get_offline_threshold();

    /// \brief Sets OfflineThreshold to given \p offline_threshold
    void set_offline_threshold(const int32_t& offline_threshold);

    /// \brief Returns ResetRetries
    int32_t get_reset_retries();

    /// \brief Sets ResetRetries to given \p reset_retries
    void set_reset_retries(const int32_t& reset_retries);

    /// \brief Returns UnlockOnEVSideDisconnect
    bool get_unlock_on_evside_disconnect();

    /// \brief Sets UnlockOnEVSideDisconnect to given \p unlock_on_evside_disconnect
    void set_unlock_on_evside_disconnect(const bool& unlock_on_evside_disconnect);

    /// \brief Returns TxEndedInterval
    int32_t get_sampled_data_ctrlr_tx_ended_interval();

    /// \brief Sets TxEndedInterval to given \p sampled_data_ctrlr_tx_ended_interval
    void set_sampled_data_ctrlr_tx_ended_interval(const int32_t& sampled_data_ctrlr_tx_ended_interval);

    /// \brief Returns TxEndedMeasurands
    std::string get_sampled_data_ctrlr_tx_ended_measurands();

    /// \brief Sets TxEndedMeasurands to given \p sampled_data_ctrlr_tx_ended_measurands
    void set_sampled_data_ctrlr_tx_ended_measurands(const std::string& sampled_data_ctrlr_tx_ended_measurands);

    /// \brief Returns TxStartedMeasurands
    std::string get_tx_started_measurands();

    /// \brief Sets TxStartedMeasurands to given \p tx_started_measurands
    void set_tx_started_measurands(const std::string& tx_started_measurands);

    /// \brief Returns TxUpdatedInterval
    int32_t get_tx_updated_interval();

    /// \brief Sets TxUpdatedInterval to given \p tx_updated_interval
    void set_tx_updated_interval(const int32_t& tx_updated_interval);

    /// \brief Returns TxUpdatedMeasurands
    std::string get_tx_updated_measurands();

    /// \brief Sets TxUpdatedMeasurands to given \p tx_updated_measurands
    void set_tx_updated_measurands(const std::string& tx_updated_measurands);

    /// \brief Returns CertificateEntries
    int32_t get_certificate_entries();

    /// \brief Sets CertificateEntries to given \p certificate_entries
    void set_certificate_entries(const int32_t& certificate_entries);

    /// \brief Returns OrganizationName
    std::string get_security_ctrlr_organization_name();

    /// \brief Sets OrganizationName to given \p security_ctrlr_organization_name
    void set_security_ctrlr_organization_name(const std::string& security_ctrlr_organization_name);

    /// \brief Returns SecurityProfile
    int32_t get_security_profile();

    /// \brief Sets SecurityProfile to given \p security_profile
    void set_security_profile(const int32_t& security_profile);

    /// \brief Returns Entries
    int32_t get_entries_charging_profiles();

    /// \brief Sets Entries to given \p entries_charging_profiles
    void set_entries_charging_profiles(const int32_t& entries_charging_profiles);

    /// \brief Returns LimitChangeSignificance
    double get_limit_change_significance();

    /// \brief Sets LimitChangeSignificance to given \p limit_change_significance
    void set_limit_change_significance(const double& limit_change_significance);

    /// \brief Returns PeriodsPerSchedule
    int32_t get_periods_per_schedule();

    /// \brief Sets PeriodsPerSchedule to given \p periods_per_schedule
    void set_periods_per_schedule(const int32_t& periods_per_schedule);

    /// \brief Returns ProfileStackLevel
    int32_t get_profile_stack_level();

    /// \brief Sets ProfileStackLevel to given \p profile_stack_level
    void set_profile_stack_level(const int32_t& profile_stack_level);

    /// \brief Returns RateUnit
    std::string get_rate_unit();

    /// \brief Sets RateUnit to given \p rate_unit
    void set_rate_unit(const std::string& rate_unit);

    /// \brief Returns Currency
    std::string get_currency();

    /// \brief Sets Currency to given \p currency
    void set_currency(const std::string& currency);

    /// \brief Returns TariffFallbackMessage
    std::string get_tariff_fallback_message();

    /// \brief Sets TariffFallbackMessage to given \p tariff_fallback_message
    void set_tariff_fallback_message(const std::string& tariff_fallback_message);

    /// \brief Returns TotalCostFallbackMessage
    std::string get_total_cost_fallback_message();

    /// \brief Sets TotalCostFallbackMessage to given \p total_cost_fallback_message
    void set_total_cost_fallback_message(const std::string& total_cost_fallback_message);

    /// \brief Returns EVConnectionTimeOut
    int32_t get_evconnection_time_out();

    /// \brief Sets EVConnectionTimeOut to given \p evconnection_time_out
    void set_evconnection_time_out(const int32_t& evconnection_time_out);

    /// \brief Returns StopTxOnEVSideDisconnect
    bool get_stop_tx_on_evside_disconnect();

    /// \brief Sets StopTxOnEVSideDisconnect to given \p stop_tx_on_evside_disconnect
    void set_stop_tx_on_evside_disconnect(const bool& stop_tx_on_evside_disconnect);

    /// \brief Returns StopTxOnInvalidId
    bool get_stop_tx_on_invalid_id();

    /// \brief Sets StopTxOnInvalidId to given \p stop_tx_on_invalid_id
    void set_stop_tx_on_invalid_id(const bool& stop_tx_on_invalid_id);

    /// \brief Returns TxStartPoint
    std::string get_tx_start_point();

    /// \brief Sets TxStartPoint to given \p tx_start_point
    void set_tx_start_point(const std::string& tx_start_point);

    /// \brief Returns TxStopPoint
    std::string get_tx_stop_point();

    /// \brief Sets TxStopPoint to given \p tx_stop_point
    void set_tx_stop_point(const std::string& tx_stop_point);
};

} // namespace v201
} // namespace ocpp
