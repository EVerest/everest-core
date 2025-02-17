// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <future>
#include <memory>
#include <set>

#include <ocpp/common/message_dispatcher.hpp>
#include <ocpp/v201/functional_blocks/authorization.hpp>
#include <ocpp/v201/functional_blocks/availability.hpp>
#include <ocpp/v201/functional_blocks/data_transfer.hpp>
#include <ocpp/v201/functional_blocks/diagnostics.hpp>
#include <ocpp/v201/functional_blocks/display_message.hpp>
#include <ocpp/v201/functional_blocks/firmware_update.hpp>
#include <ocpp/v201/functional_blocks/meter_values.hpp>
#include <ocpp/v201/functional_blocks/reservation.hpp>
#include <ocpp/v201/functional_blocks/security.hpp>
#include <ocpp/v201/functional_blocks/smart_charging.hpp>
#include <ocpp/v201/functional_blocks/tariff_and_cost.hpp>
#include <ocpp/v201/functional_blocks/transaction.hpp>

#include <ocpp/common/charging_station_base.hpp>

#include <ocpp/v201/average_meter_values.hpp>
#include <ocpp/v201/charge_point_callbacks.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage_interface.hpp>
#include <ocpp/v201/evse_manager.hpp>
#include <ocpp/v201/monitoring_updater.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/ocsp_updater.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/utils.hpp>

#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/ClearVariableMonitoring.hpp>
#include <ocpp/v201/messages/CustomerInformation.hpp>
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/Get15118EVCertificate.hpp>
#include <ocpp/v201/messages/GetBaseReport.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/GetMonitoringReport.hpp>
#include <ocpp/v201/messages/GetReport.hpp>
#include <ocpp/v201/messages/GetVariables.hpp>
#include <ocpp/v201/messages/NotifyCustomerInformation.hpp>
#include <ocpp/v201/messages/NotifyEvent.hpp>
#include <ocpp/v201/messages/NotifyMonitoringReport.hpp>
#include <ocpp/v201/messages/NotifyReport.hpp>
#include <ocpp/v201/messages/RequestStartTransaction.hpp>
#include <ocpp/v201/messages/RequestStopTransaction.hpp>
#include <ocpp/v201/messages/Reset.hpp>
#include <ocpp/v201/messages/SetMonitoringBase.hpp>
#include <ocpp/v201/messages/SetMonitoringLevel.hpp>
#include <ocpp/v201/messages/SetNetworkProfile.hpp>
#include <ocpp/v201/messages/SetVariableMonitoring.hpp>
#include <ocpp/v201/messages/SetVariables.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>
#include <ocpp/v201/messages/UnlockConnector.hpp>

#include "component_state_manager.hpp"

namespace ocpp {
namespace v201 {

class UnexpectedMessageTypeFromCSMS : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/// \brief Interface class for OCPP2.0.1 Charging Station
class ChargePointInterface {
public:
    virtual ~ChargePointInterface() = default;

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    /// \param bootreason  Optional bootreason (default: PowerUp).
    /// \param start_connecting Optional, set to false to initialize but not start connecting. Otherwise will connect to
    /// the first network profile. (default: true)
    virtual void start(BootReasonEnum bootreason = BootReasonEnum::PowerUp, bool start_connecting = true) = 0;

    /// \brief Stops the ChargePoint. Disconnects the websocket connection and stops MessageQueue and all timers
    virtual void stop() = 0;

    /// \brief Initializes the websocket and connects to a CSMS. Provide a network_profile_slot to connect to that
    /// specific slot.
    ///
    /// \param network_profile_slot Optional slot to use when connecting. std::nullopt means the slot will be determined
    /// automatically.
    virtual void connect_websocket(std::optional<int32_t> network_profile_slot = std::nullopt) = 0;

    /// \brief Disconnects the the websocket connection to the CSMS if it is connected
    virtual void disconnect_websocket() = 0;

    /// \addtogroup ocpp201_handlers OCPP 2.0.1 handlers
    /// Handlers that can be called from the implementing class
    /// @{

    /// @name Handlers
    /// The handlers
    /// @{

    ///
    /// \brief Can be called when a network is disconnected, for example when an ethernet cable is removed.
    ///
    /// This is introduced because the websocket can take several minutes to timeout when a network interface becomes
    /// unavailable, whereas the system can detect this sooner.
    ///
    /// \param ocpp_interface       The interface that is disconnected.
    ///
    virtual void on_network_disconnected(OCPPInterfaceEnum ocpp_interface) = 0;

    /// \brief Chargepoint notifies about new firmware update status firmware_update_status. This function should be
    ///        called during a Firmware Update to indicate the current firmware_update_status.
    /// \param request_id   The request_id. When it is -1, it will not be included in the request.
    /// \param firmware_update_status The firmware_update_status
    virtual void on_firmware_update_status_notification(int32_t request_id,
                                                        const FirmwareStatusEnum& firmware_update_status) = 0;

    /// \brief Event handler that should be called when a session has started
    /// \param evse_id
    /// \param connector_id
    virtual void on_session_started(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the EV sends a certificate request (for update or installation)
    /// \param request
    virtual Get15118EVCertificateResponse
    on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request) = 0;

    /// \brief Event handler that should be called when a transaction has started
    /// \param evse_id
    /// \param connector_id
    /// \param session_id
    /// \param timestamp
    /// \param trigger_reason
    /// \param meter_start
    /// \param id_token
    /// \param group_id_token   Optional group id token
    /// \param reservation_id
    /// \param remote_start_id
    /// \param charging_state   The new charging state
    virtual void
    on_transaction_started(const int32_t evse_id, const int32_t connector_id, const std::string& session_id,
                           const DateTime& timestamp, const ocpp::v201::TriggerReasonEnum trigger_reason,
                           const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                           const std::optional<IdToken>& group_id_token, const std::optional<int32_t>& reservation_id,
                           const std::optional<int32_t>& remote_start_id, const ChargingStateEnum charging_state) = 0;

    /// \brief Event handler that should be called when a transaction has finished
    /// \param evse_id
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    /// \param id_token
    /// \param signed_meter_value
    /// \param charging_state
    virtual void on_transaction_finished(const int32_t evse_id, const DateTime& timestamp, const MeterValue& meter_stop,
                                         const ReasonEnum reason, const TriggerReasonEnum trigger_reason,
                                         const std::optional<IdToken>& id_token,
                                         const std::optional<std::string>& signed_meter_value,
                                         const ChargingStateEnum charging_state) = 0;

    /// \brief Event handler that should be called when a session has finished
    /// \param evse_id
    /// \param connector_id
    virtual void on_session_finished(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the given \p id_token is authorized
    virtual void on_authorized(const int32_t evse_id, const int32_t connector_id, const IdToken& id_token) = 0;

    /// \brief Event handler that should be called when a new meter value is present
    /// \param evse_id
    /// \param meter_value
    virtual void on_meter_value(const int32_t evse_id, const MeterValue& meter_value) = 0;

    /// \brief Event handler that should be called when the connector on the given \p evse_id and \p connector_id
    /// becomes unavailable
    virtual void on_unavailable(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the connector returns from unavailable on the given \p evse_id
    /// and \p connector_id .
    virtual void on_enabled(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the connector on the given evse_id and connector_id is faulted.
    /// \param evse_id          Faulted EVSE id
    /// \param connector_id     Faulted connector id
    virtual void on_faulted(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the fault on the connector on the given evse_id is cleared.
    /// \param evse_id          EVSE id where fault was cleared
    /// \param connector_id     Connector id where fault was cleared
    virtual void on_fault_cleared(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the connector on the given evse_id and connector_id is reserved.
    /// \param evse_id          Reserved EVSE id
    /// \param connector_id     Reserved connector id
    virtual void on_reserved(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that should be called when the reservation of the connector is cleared.
    /// \param evse_id          Cleared EVSE id
    /// \param connector_id     Cleared connector id.
    virtual void on_reservation_cleared(const int32_t evse_id, const int32_t connector_id) = 0;

    /// \brief Event handler that will update the charging state internally when it has been changed.
    /// \param evse_id          The evse id of which the charging state has changed.
    /// \param charging_state   The new charging state.
    /// \param trigger_reason   The trigger reason of the event. Defaults to ChargingStateChanged
    /// \return True on success. False if evse id does not exist.
    virtual bool
    on_charging_state_changed(const uint32_t evse_id, const ChargingStateEnum charging_state,
                              const TriggerReasonEnum trigger_reason = TriggerReasonEnum::ChargingStateChanged) = 0;

    /// \brief Event handler that can be called to trigger a NotifyEvent.req with the given \p events
    /// \param events
    virtual void on_event(const std::vector<EventData>& events) = 0;

    ///
    /// \brief Event handler that can be called to notify about the log status.
    ///
    /// This function should be called curing a Diagnostics / Log upload to indicate the current log status.
    ///
    /// \param status       Log status.
    /// \param requestId    Request id that was provided in GetLogRequest.
    ///
    virtual void on_log_status_notification(UploadLogStatusEnum status, int32_t requestId) = 0;

    // \brief Notifies chargepoint that a SecurityEvent has occured. This will send a SecurityEventNotification.req to
    // the
    /// CSMS
    /// \param type type of the security event
    /// \param tech_info additional info of the security event
    /// \param critical if set this overwrites the default criticality recommended in the OCPP 2.0.1 appendix. A
    /// critical security event is transmitted as a message to the CSMS, a non-critical one is just written to the
    /// security log
    /// \param timestamp when this security event occured, if absent the current datetime is assumed
    virtual void on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                   const std::optional<bool>& critical = std::nullopt,
                                   const std::optional<DateTime>& timestamp = std::nullopt) = 0;

    /// \brief Event handler that will update the variable internally when it has been changed on the fly.
    /// \param set_variable_data contains data of the variable to set
    ///
    virtual void on_variable_changed(const SetVariableData& set_variable_data) = 0;

    /// \brief Event handler that will send a ReservationStatusUpdate request.
    /// \param reservation_id   The reservation id.
    /// \param status           The status.
    virtual void on_reservation_status(const int32_t reservation_id, const ReservationUpdateStatusEnum status) = 0;

    /// @}  // End handlers group

    /// @}

    /// \brief Validates provided \p id_token \p certificate and \p ocsp_request_data using CSMS, AuthCache or AuthList
    /// \param id_token
    /// \param certificate
    /// \param ocsp_request_data
    /// \return AuthorizeResponse containing the result of the validation
    virtual AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                             const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) = 0;

    /// \brief Data transfer mechanism initiated by charger
    /// \param vendorId
    /// \param messageId
    /// \param data
    /// \return DataTransferResponse containing the result from CSMS
    virtual std::optional<DataTransferResponse> data_transfer_req(const CiString<255>& vendorId,
                                                                  const std::optional<CiString<50>>& messageId,
                                                                  const std::optional<json>& data) = 0;

    /// \brief Data transfer mechanism initiated by charger
    /// \param request
    /// \return DataTransferResponse containing the result from CSMS. In case no response is received from the CSMS
    /// because the message timed out or the charging station is offline, std::nullopt is returned
    virtual std::optional<DataTransferResponse> data_transfer_req(const DataTransferRequest& request) = 0;

    /// \brief Delay draining the message queue after reconnecting, so the CSMS can perform post-reconnect checks first
    /// \param delay The delay period (seconds)
    virtual void set_message_queue_resume_delay(std::chrono::seconds delay) = 0;

    /// \brief Gets variables specified within \p get_variable_data_vector from the device model and returns the result.
    /// This function is used internally in order to handle GetVariables.req messages and it can be used to get
    /// variables externally.
    /// \param get_variable_data_vector contains data of the variables to get
    /// \return Vector containing a result for each requested variable
    virtual std::vector<GetVariableResult>
    get_variables(const std::vector<GetVariableData>& get_variable_data_vector) = 0;

    /// \brief Sets variables specified within \p set_variable_data_vector in the device model and returns the result.
    /// \param set_variable_data_vector contains data of the variables to set
    /// \return Map containing the SetVariableData as a key and the  SetVariableResult as a value for each requested
    /// change
    virtual std::map<SetVariableData, SetVariableResult>
    set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) = 0;

    /// \brief Gets a composite schedule based on the given \p request
    /// \param request specifies different options for the request
    /// \return GetCompositeScheduleResponse containing the status of the operation and the composite schedule if the
    /// operation was successful
    virtual GetCompositeScheduleResponse get_composite_schedule(const GetCompositeScheduleRequest& request) = 0;

    /// \brief Gets a composite schedule based on the given parameters.
    /// \note This will ignore TxDefaultProfiles and TxProfiles if no transaction is active on \p evse_id
    /// \param evse_id Evse to get the schedule for
    /// \param duration How long the schedule should be
    /// \param unit ChargingRateUnit to thet the schedule for
    /// \return the composite schedule if the operation was successful, otherwise nullopt
    virtual std::optional<CompositeSchedule> get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                                    ChargingRateUnitEnum unit) = 0;

    /// \brief Gets composite schedules for all evse_ids (including 0) for the given \p duration and \p unit . If no
    /// valid profiles are given for an evse for the specified period, the composite schedule will be empty for this
    /// evse.
    /// \param duration of the request from. Composite schedules will be retrieved from now to (now + duration)
    /// \param unit of the period entries of the composite schedules
    /// \return vector of composite schedules, one for each evse_id including 0.
    virtual std::vector<CompositeSchedule> get_all_composite_schedules(const int32_t duration,
                                                                       const ChargingRateUnitEnum& unit) = 0;

    /// \brief Gets the configured NetworkConnectionProfile based on the given \p configuration_slot . The
    /// central system uri of the connection options will not contain ws:// or wss:// because this method removes it if
    /// present. This returns the value from the cached network connection profiles. \param
    /// network_configuration_priority \return
    virtual std::optional<NetworkConnectionProfile>
    get_network_connection_profile(const int32_t configuration_slot) const = 0;

    /// \brief Get the priority of the given configuration slot.
    /// \param configuration_slot   The configuration slot to get the priority from.
    /// \return The priority if the configuration slot exists.
    ///
    virtual std::optional<int> get_priority_from_configuration_slot(const int configuration_slot) const = 0;

    /// @brief Get the network connection slots sorted by priority.
    /// Each item in the vector contains the configured configuration slots, where the slot with index 0 has the highest
    /// priority.
    /// @return The network connection slots
    ///
    virtual const std::vector<int>& get_network_connection_slots() const = 0;
};

/// \brief Class implements OCPP2.0.1 Charging Station
class ChargePoint : public ChargePointInterface, private ocpp::ChargingStationBase {

private:
    std::shared_ptr<DeviceModel> device_model;
    std::unique_ptr<EvseManager> evse_manager;
    std::unique_ptr<ConnectivityManager> connectivity_manager;

    std::unique_ptr<MessageDispatcherInterface<MessageType>> message_dispatcher;
    std::unique_ptr<DataTransferInterface> data_transfer;
    std::unique_ptr<ReservationInterface> reservation;
    std::unique_ptr<AvailabilityInterface> availability;
    std::unique_ptr<AuthorizationInterface> authorization;
    std::unique_ptr<DiagnosticsInterface> diagnostics;
    std::unique_ptr<SecurityInterface> security;
    std::unique_ptr<DisplayMessageInterface> display_message;
    std::unique_ptr<FirmwareUpdateInterface> firmware_update;
    std::unique_ptr<MeterValuesInterface> meter_values;
    std::unique_ptr<SmartCharging> smart_charging;
    std::unique_ptr<TariffAndCostInterface> tariff_and_cost;
    std::unique_ptr<TransactionInterface> transaction;

    // utility
    std::shared_ptr<MessageQueue<v201::MessageType>> message_queue;
    std::shared_ptr<DatabaseHandler> database_handler;

    // timers
    Everest::SteadyTimer boot_notification_timer;

    // states
    std::atomic<RegistrationStatusEnum> registration_status;
    UploadLogStatusEnum upload_log_status;
    int32_t upload_log_status_id;
    BootReasonEnum bootreason;
    bool skip_invalid_csms_certificate_notifications;

    /// \brief Component responsible for maintaining and persisting the operational status of CS, EVSEs, and connectors.
    std::shared_ptr<ComponentStateManagerInterface> component_state_manager;

    // store the connector status
    struct EvseConnectorPair {
        int32_t evse_id;
        int32_t connector_id;

        // Define a comparison operator for the struct
        bool operator<(const EvseConnectorPair& other) const {
            // Compare based on name, then age
            if (evse_id != other.evse_id) {
                return evse_id < other.evse_id;
            }
            return connector_id < other.connector_id;
        }
    };

    std::chrono::time_point<std::chrono::steady_clock> time_disconnected;

    // callback struct
    Callbacks callbacks;

    /// \brief Handler for automatic or explicit OCSP cache updates
    OcspUpdater ocsp_updater;

    /// \brief optional delay to resumption of message queue after reconnecting to the CSMS
    std::chrono::seconds message_queue_resume_delay = std::chrono::seconds(0);

    // internal helper functions
    void initialize(const std::map<int32_t, int32_t>& evse_connector_structure, const std::string& message_log_path);
    void websocket_connected_callback(const int configuration_slot,
                                      const NetworkConnectionProfile& network_connection_profile);
    void websocket_disconnected_callback(const int configuration_slot,
                                         const NetworkConnectionProfile& network_connection_profile);
    void websocket_connection_failed(ConnectionFailedReason reason);
    void update_dm_availability_state(const int32_t evse_id, const int32_t connector_id,
                                      const ConnectorStatusEnum status);

    void message_callback(const std::string& message);

    /// \brief Helper function to determine if the requested change results in a state that the Connector(s) is/are
    /// already in \param request \return
    void handle_variable_changed(const SetVariableData& set_variable_data);
    void handle_variables_changed(const std::map<SetVariableData, SetVariableResult>& set_variable_results);
    bool validate_set_variable(const SetVariableData& set_variable_data);

    /// \brief Sets variables specified within \p set_variable_data_vector in the device model and returns the result.
    /// \param set_variable_data_vector contains data of the variables to set
    /// \param source   value source (who sets the value, for example 'csms' or 'libocpp')
    /// \param allow_read_only if true, setting VariableAttribute values with mutability ReadOnly is allowed
    /// \return Map containing the SetVariableData as a key and the  SetVariableResult as a value for each requested
    /// change
    std::map<SetVariableData, SetVariableResult>
    set_variables_internal(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source,
                           const bool allow_read_only);

    ///
    /// \brief Check if EVSE connector is reserved for another than the given id token and / or group id token.
    /// \param evse             The evse id that must be checked. Reservation will be checked for all connectors.
    /// \param id_token         The id token to check if it is reserved for that token.
    /// \param group_id_token   The group id token to check if it is reserved for that group id.
    /// \return The status of the reservation for this evse, id token and group id token.
    ///
    ReservationCheckStatus is_evse_reserved_for_other(EvseInterface& evse, const IdToken& id_token,
                                                      const std::optional<IdToken>& group_id_token) const;

    ///
    /// \brief Check if one of the connectors of the evse is available (both connectors faulted or unavailable or on of
    ///        the connectors occupied).
    /// \param evse Evse to check.
    /// \return True if at least one connector is not faulted or unavailable.
    ///
    bool is_evse_connector_available(EvseInterface& evse) const;

    ///
    /// \brief Check if the connector exists on the given evse id.
    /// \param evse_id          The evse id to check for.
    /// \param connector_type   The connector type.
    /// \return False if evse id does not exist or evse does not have the given connector type.
    ///
    bool does_connector_exist(const uint32_t evse_id, std::optional<ConnectorEnum> connector_type);

    /// \brief Get the value optional offline flag
    /// \return true if the charge point is offline. std::nullopt if it is online;
    bool is_offline();

    /// @brief Configure the message logging callback with device model parameters
    /// @param message_log_path path to file logging
    void configure_message_logging_format(const std::string& message_log_path);

    /* OCPP message requests */

    // Functional Block B: Provisioning
    void boot_notification_req(const BootReasonEnum& reason, const bool initiated_by_trigger_message = false);
    void notify_report_req(const int request_id, const std::vector<ReportData>& report_data);

    /* OCPP message handlers */

    // Functional Block B: Provisioning
    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);
    void handle_set_variables_req(Call<SetVariablesRequest> call);
    void handle_get_variables_req(const EnhancedMessage<v201::MessageType>& message);
    void handle_get_base_report_req(Call<GetBaseReportRequest> call);
    void handle_get_report_req(const EnhancedMessage<v201::MessageType>& message);
    void handle_set_network_profile_req(Call<SetNetworkProfileRequest> call);
    void handle_reset_req(Call<ResetRequest> call);

    // Function Block F: Remote transaction control
    void handle_unlock_connector(Call<UnlockConnectorRequest> call);
    void handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call);
    void handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call);
    void handle_trigger_message(Call<TriggerMessageRequest> call);

    // Generates async sending callbacks
    template <class RequestType, class ResponseType>
    std::function<ResponseType(RequestType)> send_callback(MessageType expected_response_message_type) {
        return [this, expected_response_message_type](auto request) {
            const auto enhanced_response =
                this->message_dispatcher->dispatch_call_async(ocpp::Call<RequestType>(request)).get();
            if (enhanced_response.messageType != expected_response_message_type) {
                throw UnexpectedMessageTypeFromCSMS(
                    std::string("Got unexpected message type from CSMS, expected: ") +
                    conversions::messagetype_to_string(expected_response_message_type) +
                    ", got: " + conversions::messagetype_to_string(enhanced_response.messageType));
            }
            ocpp::CallResult<ResponseType> call_result = enhanced_response.message;
            return call_result.msg;
        };
    }

protected:
    void handle_message(const EnhancedMessage<v201::MessageType>& message);
    void clear_invalid_charging_profiles();

public:
    /// \addtogroup chargepoint_constructors
    /// @{

    /// @name Constructors for 2.0.1
    /// @{

    /// \brief Construct a new ChargePoint object
    /// \param evse_connector_structure Map that defines the structure of EVSE and connectors of the chargepoint. The
    /// key represents the id of the EVSE and the value represents the number of connectors for this EVSE. The ids of
    /// the EVSEs have to increment starting with 1.
    /// \param device_model device model instance
    /// \param database_handler database handler instance
    /// \param message_queue message queue instance
    /// \param message_log_path Path to where logfiles are written to
    /// \param evse_security Pointer to evse_security that manages security related operations
    /// \param callbacks Callbacks that will be registered for ChargePoint
    ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure, std::shared_ptr<DeviceModel> device_model,
                std::shared_ptr<DatabaseHandler> database_handler,
                std::shared_ptr<MessageQueue<v201::MessageType>> message_queue, const std::string& message_log_path,
                const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks);

    /// \brief Construct a new ChargePoint object
    /// \param evse_connector_structure Map that defines the structure of EVSE and connectors of the chargepoint. The
    /// key represents the id of the EVSE and the value represents the number of connectors for this EVSE. The ids of
    /// the EVSEs have to increment starting with 1.
    /// \param device_model_storage_interface device model interface instance
    /// \param ocpp_main_path Path where utility files for OCPP are read and written to
    /// \param core_database_path Path to directory where core database is located
    /// \param message_log_path Path to where logfiles are written to
    /// \param evse_security Pointer to evse_security that manages security related operations
    /// \param callbacks Callbacks that will be registered for ChargePoint
    ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                std::unique_ptr<DeviceModelStorageInterface> device_model_storage_interface,
                const std::string& ocpp_main_path, const std::string& core_database_path,
                const std::string& sql_init_path, const std::string& message_log_path,
                const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks);

    /// \brief Construct a new ChargePoint object
    /// \param evse_connector_structure Map that defines the structure of EVSE and connectors of the chargepoint. The
    /// key represents the id of the EVSE and the value represents the number of connectors for this EVSE. The ids of
    /// the EVSEs have to increment starting with 1.
    /// \param device_model_storage_address address to device model storage (e.g. location of SQLite database)
    /// \param initialize_device_model  Set to true to initialize the device model database
    /// \param device_model_migration_path  Path to the device model database migration files
    /// \param device_model_config_path    Path to the device model config
    /// \param ocpp_main_path Path where utility files for OCPP are read and written to
    /// \param core_database_path Path to directory where core database is located
    /// \param message_log_path Path to where logfiles are written to
    /// \param evse_security Pointer to evse_security that manages security related operations; if nullptr
    /// security_configuration must be set
    /// \param callbacks Callbacks that will be registered for ChargePoint
    ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                const std::string& device_model_storage_address, const bool initialize_device_model,
                const std::string& device_model_migration_path, const std::string& device_model_config_path,
                const std::string& ocpp_main_path, const std::string& core_database_path,
                const std::string& sql_init_path, const std::string& message_log_path,
                const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks);

    /// @}  // End chargepoint 2.0.1 member group

    /// @}  // End chargepoint 2.0.1 topic

    ~ChargePoint();

    void start(BootReasonEnum bootreason = BootReasonEnum::PowerUp, bool start_connecting = true) override;

    void stop() override;

    void connect_websocket(std::optional<int32_t> network_profile_slot = std::nullopt) override;
    virtual void disconnect_websocket() override;

    void on_network_disconnected(OCPPInterfaceEnum ocpp_interface) override;

    void on_firmware_update_status_notification(int32_t request_id,
                                                const FirmwareStatusEnum& firmware_update_status) override;

    void on_session_started(const int32_t evse_id, const int32_t connector_id) override;

    Get15118EVCertificateResponse
    on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request) override;

    void on_transaction_started(const int32_t evse_id, const int32_t connector_id, const std::string& session_id,
                                const DateTime& timestamp, const ocpp::v201::TriggerReasonEnum trigger_reason,
                                const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                                const std::optional<IdToken>& group_id_token,
                                const std::optional<int32_t>& reservation_id,
                                const std::optional<int32_t>& remote_start_id,
                                const ChargingStateEnum charging_state) override;

    void on_transaction_finished(const int32_t evse_id, const DateTime& timestamp, const MeterValue& meter_stop,
                                 const ReasonEnum reason, const TriggerReasonEnum trigger_reason,
                                 const std::optional<IdToken>& id_token,
                                 const std::optional<std::string>& signed_meter_value,
                                 const ChargingStateEnum charging_state) override;

    void on_session_finished(const int32_t evse_id, const int32_t connector_id) override;

    void on_authorized(const int32_t evse_id, const int32_t connector_id, const IdToken& id_token) override;

    void on_meter_value(const int32_t evse_id, const MeterValue& meter_value) override;

    void on_unavailable(const int32_t evse_id, const int32_t connector_id) override;

    void on_enabled(const int32_t evse_id, const int32_t connector_id) override;

    void on_faulted(const int32_t evse_id, const int32_t connector_id) override;

    void on_fault_cleared(const int32_t evse_id, const int32_t connector_id) override;

    void on_reserved(const int32_t evse_id, const int32_t connector_id) override;

    void on_reservation_cleared(const int32_t evse_id, const int32_t connector_id) override;

    bool on_charging_state_changed(
        const uint32_t evse_id, const ChargingStateEnum charging_state,
        const TriggerReasonEnum trigger_reason = TriggerReasonEnum::ChargingStateChanged) override;

    AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                     const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) override;

    void on_event(const std::vector<EventData>& events) override;

    void on_log_status_notification(UploadLogStatusEnum status, int32_t requestId) override;

    void on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                           const std::optional<bool>& critical = std::nullopt,
                           const std::optional<DateTime>& timestamp = std::nullopt) override;

    void on_variable_changed(const SetVariableData& set_variable_data) override;

    void on_reservation_status(const int32_t reservation_id, const ReservationUpdateStatusEnum status) override;

    std::optional<DataTransferResponse> data_transfer_req(const CiString<255>& vendorId,
                                                          const std::optional<CiString<50>>& messageId,
                                                          const std::optional<json>& data) override;

    std::optional<DataTransferResponse> data_transfer_req(const DataTransferRequest& request) override;

    void set_message_queue_resume_delay(std::chrono::seconds delay) override {
        this->message_queue_resume_delay = delay;
    }

    std::vector<GetVariableResult> get_variables(const std::vector<GetVariableData>& get_variable_data_vector) override;

    std::map<SetVariableData, SetVariableResult>
    set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) override;
    GetCompositeScheduleResponse get_composite_schedule(const GetCompositeScheduleRequest& request) override;
    std::optional<CompositeSchedule> get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                            ChargingRateUnitEnum unit) override;
    std::vector<CompositeSchedule> get_all_composite_schedules(const int32_t duration,
                                                               const ChargingRateUnitEnum& unit) override;

    std::optional<NetworkConnectionProfile>
    get_network_connection_profile(const int32_t configuration_slot) const override;

    std::optional<int> get_priority_from_configuration_slot(const int configuration_slot) const override;

    const std::vector<int>& get_network_connection_slots() const override;

    void send_not_implemented_error(const MessageId unique_message_id, const MessageTypeId message_type_id);

    /// \brief Requests a value of a VariableAttribute specified by combination of \p component_id and \p variable_id
    /// from the device model
    /// \tparam T datatype of the value that is requested
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \return Response to request that contains status of the request and the requested value as std::optional<T> .
    /// The value is present if the status is GetVariableStatusEnum::Accepted
    template <typename T>
    RequestDeviceModelResponse<T> request_value(const Component& component_id, const Variable& variable_id,
                                                const AttributeEnum& attribute_enum) {
        return this->device_model->request_value<T>(component_id, variable_id, attribute_enum);
    }
};

} // namespace v201
} // namespace ocpp
