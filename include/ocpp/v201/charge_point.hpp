// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <future>
#include <memory>
#include <set>

#include <ocpp/common/charging_station_base.hpp>

#include <ocpp/v201/average_meter_values.hpp>
#include <ocpp/v201/charge_point_callbacks.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage.hpp>
#include <ocpp/v201/evse_manager.hpp>
#include <ocpp/v201/monitoring_updater.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/ocsp_updater.hpp>
#include <ocpp/v201/smart_charging.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/utils.hpp>

#include "ocpp/v201/messages/Get15118EVCertificate.hpp"
#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/CertificateSigned.hpp>
#include <ocpp/v201/messages/ChangeAvailability.hpp>
#include <ocpp/v201/messages/ClearCache.hpp>
#include <ocpp/v201/messages/ClearChargingProfile.hpp>
#include <ocpp/v201/messages/ClearDisplayMessage.hpp>
#include <ocpp/v201/messages/ClearVariableMonitoring.hpp>
#include <ocpp/v201/messages/CostUpdated.hpp>
#include <ocpp/v201/messages/CustomerInformation.hpp>
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/DeleteCertificate.hpp>
#include <ocpp/v201/messages/GetBaseReport.hpp>
#include <ocpp/v201/messages/GetChargingProfiles.hpp>
#include <ocpp/v201/messages/GetCompositeSchedule.hpp>
#include <ocpp/v201/messages/GetDisplayMessages.hpp>
#include <ocpp/v201/messages/GetInstalledCertificateIds.hpp>
#include <ocpp/v201/messages/GetLocalListVersion.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/GetMonitoringReport.hpp>
#include <ocpp/v201/messages/GetReport.hpp>
#include <ocpp/v201/messages/GetTransactionStatus.hpp>
#include <ocpp/v201/messages/GetVariables.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>
#include <ocpp/v201/messages/InstallCertificate.hpp>
#include <ocpp/v201/messages/MeterValues.hpp>
#include <ocpp/v201/messages/NotifyCustomerInformation.hpp>
#include <ocpp/v201/messages/NotifyEvent.hpp>
#include <ocpp/v201/messages/NotifyMonitoringReport.hpp>
#include <ocpp/v201/messages/NotifyReport.hpp>
#include <ocpp/v201/messages/ReportChargingProfiles.hpp>
#include <ocpp/v201/messages/RequestStartTransaction.hpp>
#include <ocpp/v201/messages/RequestStopTransaction.hpp>
#include <ocpp/v201/messages/Reset.hpp>
#include <ocpp/v201/messages/SecurityEventNotification.hpp>
#include <ocpp/v201/messages/SendLocalList.hpp>
#include <ocpp/v201/messages/SetChargingProfile.hpp>
#include <ocpp/v201/messages/SetDisplayMessage.hpp>
#include <ocpp/v201/messages/SetMonitoringBase.hpp>
#include <ocpp/v201/messages/SetMonitoringLevel.hpp>
#include <ocpp/v201/messages/SetNetworkProfile.hpp>
#include <ocpp/v201/messages/SetVariableMonitoring.hpp>
#include <ocpp/v201/messages/SetVariables.hpp>
#include <ocpp/v201/messages/SignCertificate.hpp>
#include <ocpp/v201/messages/StatusNotification.hpp>
#include <ocpp/v201/messages/TransactionEvent.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>
#include <ocpp/v201/messages/UnlockConnector.hpp>
#include <ocpp/v201/messages/UpdateFirmware.hpp>

#include "component_state_manager.hpp"

namespace ocpp {
namespace v201 {

class UnexpectedMessageTypeFromCSMS : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/// \brief Combines ChangeAvailabilityRequest with persist flag for scheduled Availability changes
struct AvailabilityChange {
    ChangeAvailabilityRequest request;
    bool persist;
};

/// \brief Interface class for OCPP2.0.1 Charging Station
class ChargePointInterface {
public:
    virtual ~ChargePointInterface() = default;

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    /// \param bootreason   Optional bootreason (default: PowerUp).
    virtual void start(BootReasonEnum bootreason = BootReasonEnum::PowerUp) = 0;

    /// \brief Stops the ChargePoint. Disconnects the websocket connection and stops MessageQueue and all timers
    virtual void stop() = 0;

    /// \brief Initializes the websocket and connects to CSMS if it is not yet connected
    virtual void connect_websocket() = 0;

    /// \brief Disconnects the the websocket connection to the CSMS if it is connected
    virtual void disconnect_websocket() = 0;

    ///
    /// \brief Can be called when a network is disconnected, for example when an ethernet cable is removed.
    ///
    /// This is introduced because the websocket can take several minutes to timeout when a network interface becomes
    /// unavailable, whereas the system can detect this sooner.
    ///
    /// \param configuration_slot   The slot of the network connection profile that is disconnected.
    ///
    virtual void on_network_disconnected(int32_t configuration_slot) = 0;

    ///
    /// \brief Can be called when a network is disconnected, for example when an ethernet cable is removed.
    ///
    /// This is introduced because the websocket can take several minutes to timeout when a network interface becomes
    /// unavailable, whereas the system can detect this sooner.
    ///
    /// \param ocpp_interface       The interface that is disconnected.
    ///
    virtual void on_network_disconnected(OCPPInterfaceEnum ocpp_interface) = 0;

    /// \brief Switch to a specific network connection profile given the configuration slot.
    ///
    /// Switch will only be done when the configuration slot has a higher priority.
    ///
    /// \param configuration_slot Slot in which the configuration is stored
    /// \return true if the switch is possible.
    virtual bool on_try_switch_network_connection_profile(const int32_t configuration_slot) = 0;

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

    /// \brief Event handler that will update the charging state internally when it has been changed.
    /// \param evse_id          The evse id of which the charging state has changed.
    /// \param charging_state   The new charging state.
    /// \param trigger_reason   The trigger reason of the event. Defaults to ChargingStateChanged
    /// \return True on success. False if evse id does not exist.
    virtual bool
    on_charging_state_changed(const uint32_t evse_id, const ChargingStateEnum charging_state,
                              const TriggerReasonEnum trigger_reason = TriggerReasonEnum::ChargingStateChanged) = 0;

    /// \brief Gets the transaction id for a certain \p evse_id if there is an active transaction
    /// \param evse_id The evse to tet the transaction for
    /// \return The transaction id if a transaction is active, otherwise nullopt
    virtual std::optional<std::string> get_evse_transaction_id(int32_t evse_id) = 0;

    /// \brief Validates provided \p id_token \p certificate and \p ocsp_request_data using CSMS, AuthCache or AuthList
    /// \param id_token
    /// \param certificate
    /// \param ocsp_request_data
    /// \return AuthorizeResponse containing the result of the validation
    virtual AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                             const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) = 0;

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

    /// \brief Switches the operative status of the CS
    /// \param new_status: The new operative status to switch to
    /// \param persist: True if the updated state should be persisted in the database
    virtual void set_cs_operative_status(OperationalStatusEnum new_status, bool persist) = 0;

    /// \brief Switches the operative status of an EVSE
    /// \param evse_id: The ID of the EVSE, empty if the CS is addressed
    /// \param new_status: The new operative status to switch to
    /// \param persist: True if the updated state should be persisted in the database
    virtual void set_evse_operative_status(int32_t evse_id, OperationalStatusEnum new_status, bool persist) = 0;

    /// \brief Switches the operative status of the CS, an EVSE, or a connector, and recomputes effective statuses
    /// \param evse_id: The ID of the EVSE, empty if the CS is addressed
    /// \param connector_id: The ID of the connector, empty if an EVSE or the CS is addressed
    /// \param new_status: The new operative status to switch to
    /// \param persist: True if the updated state should be persisted in the database
    virtual void set_connector_operative_status(int32_t evse_id, int32_t connector_id, OperationalStatusEnum new_status,
                                                bool persist) = 0;

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
    get_network_connection_profile(const int32_t configuration_slot) = 0;

    /// \brief Get the priority of the given configuration slot.
    /// \param configuration_slot   The configuration slot to get the priority from.
    /// \return The priority if the configuration slot exists.
    ///
    virtual std::optional<int> get_configuration_slot_priority(const int configuration_slot) = 0;

    /// @brief Get the network connection priorities.
    /// Each item in the vector contains the configured configuration slots, where the slot with index 0 has the highest
    /// priority.
    /// @return The network connection priorities
    ///
    virtual const std::vector<int>& get_network_connection_priorities() const = 0;
};

/// \brief Class implements OCPP2.0.1 Charging Station
class ChargePoint : public ChargePointInterface, private ocpp::ChargingStationBase {

private:
    std::shared_ptr<DeviceModel> device_model;
    std::unique_ptr<EvseManager> evse_manager;
    std::unique_ptr<ConnectivityManager> connectivity_manager;

    // utility
    std::shared_ptr<MessageQueue<v201::MessageType>> message_queue;
    std::shared_ptr<DatabaseHandler> database_handler;

    std::map<int32_t, AvailabilityChange> scheduled_change_availability_requests;

    std::map<int32_t, std::pair<IdToken, int32_t>> remote_start_id_per_evse;

    // timers
    Everest::SteadyTimer heartbeat_timer;
    Everest::SteadyTimer boot_notification_timer;
    Everest::SteadyTimer client_certificate_expiration_check_timer;
    Everest::SteadyTimer v2g_certificate_expiration_check_timer;
    ClockAlignedTimer aligned_meter_values_timer;

    // time keeping
    std::chrono::time_point<std::chrono::steady_clock> heartbeat_request_time;

    Everest::SteadyTimer certificate_signed_timer;

    // threads and synchronization
    bool auth_cache_cleanup_required;
    std::condition_variable auth_cache_cleanup_cv;
    std::mutex auth_cache_cleanup_mutex;
    std::thread auth_cache_cleanup_thread;
    std::atomic_bool stop_auth_cache_cleanup_handler;

    // states
    RegistrationStatusEnum registration_status;
    FirmwareStatusEnum firmware_status;
    // The request ID in the last firmware update status received
    std::optional<int32_t> firmware_status_id;
    // The last firmware status which will be posted before the firmware is installed.
    FirmwareStatusEnum firmware_status_before_installing = FirmwareStatusEnum::SignatureVerified;
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
    AverageMeterValues aligned_data_evse0; // represents evseId = 0 meter value

    /// \brief Used when an 'OnIdle' reset is requested, to perform the reset after the charging has stopped.
    bool reset_scheduled;
    /// \brief If `reset_scheduled` is true and the reset is for a specific evse id, it will be stored in this member.
    std::set<int32_t> reset_scheduled_evseids;

    int csr_attempt;
    std::optional<ocpp::CertificateSigningUseEnum> awaited_certificate_signing_use_enum;

    // callback struct
    Callbacks callbacks;

    /// \brief Handler for automatic or explicit OCSP cache updates
    OcspUpdater ocsp_updater;

    /// \brief Updater for triggered monitors
    MonitoringUpdater monitoring_updater;

    /// \brief optional delay to resumption of message queue after reconnecting to the CSMS
    std::chrono::seconds message_queue_resume_delay = std::chrono::seconds(0);

    bool send(CallError call_error);

    // internal helper functions
    void initialize(const std::map<int32_t, int32_t>& evse_connector_structure, const std::string& message_log_path);
    void init_certificate_expiration_check_timers();
    void scheduled_check_client_certificate_expiration();
    void scheduled_check_v2g_certificate_expiration();
    void websocket_connected_callback(const int configuration_slot,
                                      const NetworkConnectionProfile& network_connection_profile);
    void websocket_disconnected_callback(const int configuration_slot,
                                         const NetworkConnectionProfile& network_connection_profile);
    void websocket_connection_failed(ConnectionFailedReason reason);
    void update_dm_availability_state(const int32_t evse_id, const int32_t connector_id,
                                      const ConnectorStatusEnum status);
    void update_dm_evse_power(const int32_t evse_id, const MeterValue& meter_value);

    void trigger_authorization_cache_cleanup();
    void cache_cleanup_handler();
    GetCompositeScheduleResponse
    get_composite_schedule_internal(const GetCompositeScheduleRequest& request,
                                    const std::set<ChargingProfilePurposeEnum>& profiles_to_ignore = {});

    /// \brief Removes all network connection profiles below the actual security profile and stores the new list in the
    /// device model
    void remove_network_connection_profiles_below_actual_security_profile();

    void message_callback(const std::string& message);
    void update_aligned_data_interval();

    /// \brief Helper function to determine if there is any active transaction for the given \p evse
    /// \param evse if optional is not set, this function will check if there is any transaction active f or the whole
    /// charging station
    /// \return
    bool any_transaction_active(const std::optional<EVSE>& evse);

    /// \brief Helper function to determine if the requested change results in a state that the Connector(s) is/are
    /// already in \param request \return
    bool is_already_in_state(const ChangeAvailabilityRequest& request);
    bool is_valid_evse(const EVSE& evse);
    void handle_scheduled_change_availability_requests(const int32_t evse_id);
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

    MeterValue get_latest_meter_value_filtered(const MeterValue& meter_value, ReadingContextEnum context,
                                               const RequiredComponentVariable& component_variable);

    /// \brief Changes all unoccupied connectors to unavailable. If a transaction is running schedule an availabilty
    /// change
    /// If all connectors are unavailable signal to the firmware updater that installation of the firmware update can
    /// proceed
    void change_all_connectors_to_unavailable_for_firmware_update();

    /// \brief Restores all connectors to their persisted state
    void restore_all_connector_states();

    ///\brief Calculate and update the authorization cache size in the device model
    ///
    void update_authorization_cache_size();

    ///\brief Apply a local list request to the database if allowed
    ///
    ///\param request The local list request to apply
    ///\retval Accepted if applied, otherwise will return either Failed or VersionMismatch
    SendLocalListStatusEnum apply_local_authorization_list(const SendLocalListRequest& request);

    ///
    /// \brief Get evseid for the given transaction id.
    /// \param transaction_id   The transactionid
    /// \return The evse id belonging the the transaction id. std::nullopt if there is no transaction with the given
    ///         transaction id.
    ///
    std::optional<int32_t> get_transaction_evseid(const CiString<36>& transaction_id);

    ///
    /// \brief Check if EVSE connector is reserved for another than the given id token and / or group id token.
    /// \param evse             The evse id that must be checked. Reservation will be checked for all connectors.
    /// \param id_token         The id token to check if it is reserved for that token.
    /// \param group_id_token   The group id token to check if it is reserved for that group id.
    /// \return True when one of the EVSE connectors is reserved for another id token or group id token than the given
    ///         tokens.
    ///         If id_token is different than reserved id_token, but group_id_token is equal to reserved group_id_token,
    ///         returns true.
    ///         If both are different, returns true.
    ///         If id_token is equal to reserved id_token or group_id_token is equal, return false.
    ///         If there is no reservation, return false.
    ///
    bool is_evse_reserved_for_other(EvseInterface& evse, const IdToken& id_token,
                                    const std::optional<IdToken>& group_id_token) const;

    ///
    /// \brief Check if one of the connectors of the evse is available (both connectors faulted or unavailable or on of
    ///        the connectors occupied).
    /// \param evse Evse to check.
    /// \return True if at least one connector is not faulted or unavailable.
    ///
    bool is_evse_connector_available(EvseInterface& evse) const;

    ///
    /// \brief Set all connectors of a given evse to unavailable.
    /// \param evse The evse.
    /// \param persist  True if unavailability should persist. If it is set to false, there will be a check per
    ///                 connector if it was already set to true and if that is the case, it will be persisted anyway.
    ///
    void set_evse_connectors_unavailable(EvseInterface& evse, bool persist);

    /// \brief Get the value optional offline flag
    /// \return true if the charge point is offline. std::nullopt if it is online;
    bool is_offline();

    /// \brief Returns customer information based on the given arguments. This function also executes the
    /// get_customer_information_callback in case it is present
    /// \param customer_certificate Certificate of the customer this request refers to
    /// \param id_token IdToken of the customer this request refers to
    /// \param customer_identifier A (e.g. vendor specific) identifier of the customer this request refers to. This
    /// field contains a custom identifier other than IdToken and Certificate
    /// \return customer information
    std::string get_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                         const std::optional<IdToken> id_token,
                                         const std::optional<CiString<64>> customer_identifier);

    /// \brief Clears customer information based on the given arguments. This function also executes the
    /// clear_customer_information_callback in case it is present
    /// \param customer_certificate Certificate of the customer this request refers to
    /// \param id_token IdToken of the customer this request refers to
    /// \param customer_identifier A (e.g. vendor specific) identifier of the customer this request refers to. This
    /// field contains a custom identifier other than IdToken and Certificate
    void clear_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                    const std::optional<IdToken> id_token,
                                    const std::optional<CiString<64>> customer_identifier);

    /// @brief Configure the message logging callback with device model parameters
    /// @param message_log_path path to file logging
    void configure_message_logging_format(const std::string& message_log_path);

    ///
    /// \brief Create cost and / or tariff message and call the callbacks to send it, if tariff and / or cost is
    /// enabled.
    /// \param response             The TransactionEventResponse where the tariff and cost information is added to.
    /// \param original_message     The original TransactionEventRequest, which contains some information we need as
    ///                             well.
    /// \param original_transaction_event_response  The original json from the response.
    ///
    void handle_cost_and_tariff(const TransactionEventResponse& response,
                                const TransactionEventRequest& original_message,
                                const json& original_transaction_event_response);

    ///
    /// \brief Check if multilanguage setting (variable) is enabled.
    /// \return True if enabled.
    ///
    bool is_multilanguage_enabled() const;

    ///
    /// \brief Check if tariff setting (variable) is enabled.
    /// \return True if enabled.
    ///
    bool is_tariff_enabled() const;

    ///
    /// \brief Check if cost setting (variable) is enabled.
    /// \return True if enabled.
    ///
    bool is_cost_enabled() const;

    /* OCPP message requests */

    // Functional Block A: Security
    void security_event_notification_req(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                         const bool triggered_internally, const bool critical,
                                         const std::optional<DateTime>& timestamp = std::nullopt);
    void sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                              const bool initiated_by_trigger_message = false);

    // Functional Block B: Provisioning
    void boot_notification_req(const BootReasonEnum& reason, const bool initiated_by_trigger_message = false);
    void notify_report_req(const int request_id, const std::vector<ReportData>& report_data);

    // Functional Block C: Authorization
    AuthorizeResponse authorize_req(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                    const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data);

    // Functional Block G: Availability
    void status_notification_req(const int32_t evse_id, const int32_t connector_id, const ConnectorStatusEnum status,
                                 const bool initiated_by_trigger_message = false);
    void heartbeat_req(const bool initiated_by_trigger_message = false);

    // Functional Block E: Transactions
    void transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                               const ocpp::v201::Transaction& transaction,
                               const ocpp::v201::TriggerReasonEnum& trigger_reason, const int32_t seq_no,
                               const std::optional<int32_t>& cable_max_current,
                               const std::optional<ocpp::v201::EVSE>& evse,
                               const std::optional<ocpp::v201::IdToken>& id_token,
                               const std::optional<std::vector<ocpp::v201::MeterValue>>& meter_value,
                               const std::optional<int32_t>& number_of_phases_used, const bool offline,
                               const std::optional<int32_t>& reservation_id,
                               const bool initiated_by_trigger_message = false);

    // Functional Block J: MeterValues
    void meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values,
                          const bool initiated_by_trigger_message = false);

    // Functional Block K: Smart Charging
    void report_charging_profile_req(const int32_t request_id, const int32_t evse_id,
                                     const ChargingLimitSourceEnum source, const std::vector<ChargingProfile>& profiles,
                                     const bool tbc);
    void report_charging_profile_req(const ReportChargingProfilesRequest& req);

    // Functional Block N: Diagnostics
    void notify_event_req(const std::vector<EventData>& events);
    void notify_customer_information_req(const std::string& data, const int32_t request_id);
    void notify_monitoring_report_req(const int request_id, const std::vector<MonitoringData>& montoring_data);

    /* OCPP message handlers */

    // Functional Block A: Security
    void handle_certificate_signed_req(Call<CertificateSignedRequest> call);
    void handle_sign_certificate_response(CallResult<SignCertificateResponse> call_result);

    // Functional Block B: Provisioning
    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);
    void handle_set_variables_req(Call<SetVariablesRequest> call);
    void handle_get_variables_req(const EnhancedMessage<v201::MessageType>& message);
    void handle_get_base_report_req(Call<GetBaseReportRequest> call);
    void handle_get_report_req(const EnhancedMessage<v201::MessageType>& message);
    void handle_set_network_profile_req(Call<SetNetworkProfileRequest> call);
    void handle_reset_req(Call<ResetRequest> call);

    // Functional Block C: Authorization
    void handle_clear_cache_req(Call<ClearCacheRequest> call);

    // Functional Block D: Local authorization list management
    void handle_send_local_authorization_list_req(Call<SendLocalListRequest> call);
    void handle_get_local_authorization_list_version_req(Call<GetLocalListVersionRequest> call);

    // Functional Block E: Transaction
    void handle_transaction_event_response(const EnhancedMessage<v201::MessageType>& message);
    void handle_get_transaction_status(const Call<GetTransactionStatusRequest> call);

    // Function Block F: Remote transaction control
    void handle_unlock_connector(Call<UnlockConnectorRequest> call);
    void handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call);
    void handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call);
    void handle_trigger_message(Call<TriggerMessageRequest> call);

    // Functional Block G: Availability
    void handle_change_availability_req(Call<ChangeAvailabilityRequest> call);
    void handle_heartbeat_response(CallResult<HeartbeatResponse> call);

    // Functional Block I: TariffAndCost
    void handle_costupdated_req(const Call<CostUpdatedRequest> call);

    // Functional Block K: Smart Charging
    void handle_set_charging_profile_req(Call<SetChargingProfileRequest> call);
    void handle_clear_charging_profile_req(Call<ClearChargingProfileRequest> call);
    void handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call);
    void handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call);

    // Functional Block L: Firmware management
    void handle_firmware_update_req(Call<UpdateFirmwareRequest> call);

    // Functional Block M: ISO 15118 Certificate Management
    void handle_get_installed_certificate_ids_req(Call<GetInstalledCertificateIdsRequest> call);
    void handle_install_certificate_req(Call<InstallCertificateRequest> call);
    void handle_delete_certificate_req(Call<DeleteCertificateRequest> call);

    // Functional Block N: Diagnostics
    void handle_get_log_req(Call<GetLogRequest> call);
    void handle_customer_information_req(Call<CustomerInformationRequest> call);

    void handle_set_monitoring_base_req(Call<SetMonitoringBaseRequest> call);
    void handle_set_monitoring_level_req(Call<SetMonitoringLevelRequest> call);
    void handle_set_variable_monitoring_req(const EnhancedMessage<v201::MessageType>& message);
    void handle_get_monitoring_report_req(Call<GetMonitoringReportRequest> call);
    void handle_clear_variable_monitoring_req(Call<ClearVariableMonitoringRequest> call);

    // Functional Block O: DisplayMessage
    void handle_get_display_message(Call<GetDisplayMessagesRequest> call);
    void handle_set_display_message(Call<SetDisplayMessageRequest> call);
    void handle_clear_display_message(Call<ClearDisplayMessageRequest> call);

    // Functional Block P: DataTransfer
    void handle_data_transfer_req(Call<DataTransferRequest> call);

    // general message handling
    template <class T> bool send(ocpp::Call<T> call, const bool initiated_by_trigger_message = false);

    template <class T> std::future<EnhancedMessage<v201::MessageType>> send_async(ocpp::Call<T> call);

    template <class T> bool send(ocpp::CallResult<T> call_result);

    // Generates async sending callbacks
    template <class RequestType, class ResponseType>
    std::function<ResponseType(RequestType)> send_callback(MessageType expected_response_message_type) {
        return [this, expected_response_message_type](auto request) {
            MessageId message_id = MessageId(to_string(this->uuid_generator()));
            const auto enhanced_response =
                this->send_async<RequestType>(ocpp::Call<RequestType>(request, message_id)).get();
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

    /// \brief Checks if all connectors are effectively inoperative.
    /// If this is the case, calls the all_connectors_unavailable_callback
    /// This is used e.g. to allow firmware updates once all transactions have finished
    bool are_all_connectors_effectively_inoperative();

    /// \brief Immediately execute the given \param request to change the operational state of a component
    /// If \param persist is set to true, the change will be persisted across a reboot
    void execute_change_availability_request(ChangeAvailabilityRequest request, bool persist);

protected:
    std::shared_ptr<SmartChargingHandlerInterface> smart_charging_handler;

    void handle_message(const EnhancedMessage<v201::MessageType>& message);
    void clear_invalid_charging_profiles();

public:
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
    /// \param device_model_storage device model storage instance
    /// \param ocpp_main_path Path where utility files for OCPP are read and written to
    /// \param core_database_path Path to directory where core database is located
    /// \param message_log_path Path to where logfiles are written to
    /// \param evse_security Pointer to evse_security that manages security related operations
    /// \param callbacks Callbacks that will be registered for ChargePoint
    ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                std::unique_ptr<DeviceModelStorage> device_model_storage, const std::string& ocpp_main_path,
                const std::string& core_database_path, const std::string& sql_init_path,
                const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                const Callbacks& callbacks);

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

    ~ChargePoint();

    void start(BootReasonEnum bootreason = BootReasonEnum::PowerUp) override;

    void stop() override;

    virtual void connect_websocket() override;
    virtual void disconnect_websocket() override;

    void on_network_disconnected(int32_t configuration_slot) override;

    void on_network_disconnected(OCPPInterfaceEnum ocpp_interface) override;

    bool on_try_switch_network_connection_profile(const int32_t configuration_slot) override;

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

    bool on_charging_state_changed(
        const uint32_t evse_id, const ChargingStateEnum charging_state,
        const TriggerReasonEnum trigger_reason = TriggerReasonEnum::ChargingStateChanged) override;

    std::optional<std::string> get_evse_transaction_id(int32_t evse_id) override;

    AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                     const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) override;

    void on_event(const std::vector<EventData>& events) override;

    void on_log_status_notification(UploadLogStatusEnum status, int32_t requestId) override;

    void on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                           const std::optional<bool>& critical = std::nullopt,
                           const std::optional<DateTime>& timestamp = std::nullopt) override;

    void on_variable_changed(const SetVariableData& set_variable_data) override;

    std::optional<DataTransferResponse> data_transfer_req(const CiString<255>& vendorId,
                                                          const std::optional<CiString<50>>& messageId,
                                                          const std::optional<json>& data) override;

    std::optional<DataTransferResponse> data_transfer_req(const DataTransferRequest& request) override;

    void set_cs_operative_status(OperationalStatusEnum new_status, bool persist) override;

    void set_evse_operative_status(int32_t evse_id, OperationalStatusEnum new_status, bool persist) override;

    void set_connector_operative_status(int32_t evse_id, int32_t connector_id, OperationalStatusEnum new_status,
                                        bool persist) override;

    void set_message_queue_resume_delay(std::chrono::seconds delay) override {
        this->message_queue_resume_delay = delay;
    }

    std::vector<GetVariableResult> get_variables(const std::vector<GetVariableData>& get_variable_data_vector) override;

    std::map<SetVariableData, SetVariableResult>
    set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) override;

    GetCompositeScheduleResponse get_composite_schedule(const GetCompositeScheduleRequest& request) override;

    std::vector<CompositeSchedule> get_all_composite_schedules(const int32_t duration,
                                                               const ChargingRateUnitEnum& unit) override;

    std::optional<NetworkConnectionProfile> get_network_connection_profile(const int32_t configuration_slot) override;

    std::optional<int> get_configuration_slot_priority(const int configuration_slot) override;

    const std::vector<int>& get_network_connection_priorities() const override;

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
