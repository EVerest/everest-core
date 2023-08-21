// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <future>

#include <ocpp/common/charging_station_base.hpp>

#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/utils.hpp>

#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/ChangeAvailability.hpp>
#include <ocpp/v201/messages/ClearCache.hpp>
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/GetBaseReport.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/GetReport.hpp>
#include <ocpp/v201/messages/GetVariables.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>
#include <ocpp/v201/messages/MeterValues.hpp>
#include <ocpp/v201/messages/NotifyEvent.hpp>
#include <ocpp/v201/messages/NotifyReport.hpp>
#include <ocpp/v201/messages/RequestStartTransaction.hpp>
#include <ocpp/v201/messages/RequestStopTransaction.hpp>
#include <ocpp/v201/messages/Reset.hpp>
#include <ocpp/v201/messages/SetNetworkProfile.hpp>
#include <ocpp/v201/messages/SetVariables.hpp>
#include <ocpp/v201/messages/StatusNotification.hpp>
#include <ocpp/v201/messages/TransactionEvent.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>
#include <ocpp/v201/messages/UnlockConnector.hpp>
#include <ocpp/v201/messages/UpdateFirmware.hpp>

namespace ocpp {
namespace v201 {

struct Callbacks {
    std::function<bool(const ResetEnum& reset_type)> is_reset_allowed_callback;
    std::function<void(const ResetEnum& reset_type)> reset_callback;
    std::function<void(const int32_t evse_id, const ReasonEnum& stop_reason)> stop_transaction_callback;
    std::function<void(const int32_t evse_id)> pause_charging_callback;
    std::function<void(const ChangeAvailabilityRequest& request)> change_availability_callback;
    std::function<GetLogResponse(const GetLogRequest& request)> get_log_request_callback;
    std::function<UnlockConnectorResponse(const int32_t evse_id, const int32_t connecor_id)> unlock_connector_callback;
    // callback to be called when the request can be accepted. authorize_remote_start indicates if Authorize.req needs
    // to follow or not
    std::function<void(const RequestStartTransactionRequest& request, const bool authorize_remote_start)>
        remote_start_transaction_callback;
    ///
    /// \brief Check if the current reservation for the given evse id is made for the id token / group id token.
    /// \return True if evse is reserved for the given id token / group id token, false if it is reserved for another
    ///         one.
    ///
    std::function<bool(const int32_t evse_id, const CiString<36> idToken,
                       const std::optional<CiString<36>> groupIdToken)>
        is_reservation_for_token_callback;
    std::function<UpdateFirmwareResponse(const UpdateFirmwareRequest& request)> update_firmware_request_callback;
    // callback to be called when a variable has been changed by the CSMS
    std::optional<std::function<void(const SetVariableData& set_variable_data)>> variable_changed_callback;
    // callback is called when receiving a SetNetworkProfile.req from the CSMS
    std::optional<std::function<SetNetworkProfileStatusEnum(
        const int32_t configuration_slot, const NetworkConnectionProfile& network_connection_profile)>>
        validate_network_profile_callback;
    std::optional<std::function<bool(const NetworkConnectionProfile& network_connection_profile)>>
        configure_network_connection_profile_callback;
};

/// \brief Class implements OCPP2.0.1 Charging Station
class ChargePoint : ocpp::ChargingStationBase {

private:
    // reference to evses
    std::map<int32_t, std::unique_ptr<Evse>> evses;

    // utility
    std::unique_ptr<MessageQueue<v201::MessageType>> message_queue;
    std::unique_ptr<DeviceModel> device_model;
    std::unique_ptr<DatabaseHandler> database_handler;

    std::map<int32_t, ChangeAvailabilityRequest> scheduled_change_availability_requests;

    std::map<std::string,
             std::map<std::string, std::function<DataTransferResponse(const std::optional<std::string>& msg)>>>
        data_transfer_callbacks;
    std::mutex data_transfer_callbacks_mutex;

    std::map<int32_t, std::pair<IdToken, int32_t>> remote_start_id_per_evse;

    // timers
    Everest::SteadyTimer heartbeat_timer;
    Everest::SteadyTimer boot_notification_timer;
    Everest::SteadyTimer aligned_meter_values_timer;

    // states
    RegistrationStatusEnum registration_status;
    WebsocketConnectionStatusEnum websocket_connection_status;
    OperationalStatusEnum operational_state;
    FirmwareStatusEnum firmware_status;
    int network_configuration_priority;
    bool disable_automatic_websocket_reconnects;

    // callback struct
    Callbacks callbacks;

    // general message handling
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage<v201::MessageType>> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    bool send(CallError call_error);

    // internal helper functions
    void init_websocket();
    WebsocketConnectionOptions get_ws_connection_options(const int32_t configuration_slot);

    /// \brief Gets the configured NetworkConnectionProfile based on the given \p configuration_slot . The
    /// central system uri ofthe connection options will not contain ws:// or wss:// because this method removes it if
    /// present \param network_configuration_priority \return
    std::optional<NetworkConnectionProfile> get_network_connection_profile(const int32_t configuration_slot);
    /// \brief Moves websocket network_configuration_priority to next profile
    void next_network_configuration_priority();
    void handle_message(const json& json_message, const MessageType& message_type);
    void message_callback(const std::string& message);
    void update_aligned_data_interval();
    bool is_change_availability_possible(const ChangeAvailabilityRequest& req);
    void handle_scheduled_change_availability_requests(const int32_t evse_id);
    void handle_variable_changed(const SetVariableData& set_variable_data);
    bool validate_set_variable(const SetVariableData& set_variable_data);

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
    bool is_evse_reserved_for_other(const std::unique_ptr<Evse>& evse, const IdToken& id_token,
                                    const std::optional<IdToken>& group_id_token) const;

    ///
    /// \brief Check if one of the connectors of the evse is available (both connectors faulted or unavailable or on of
    ///        the connectors occupied).
    /// \param evse Evse to check.
    /// \return True if at least one connector is not faulted or unavailable.
    ///
    bool is_evse_connector_available(const std::unique_ptr<Evse>& evse) const;

    /* OCPP message requests */

    // Functional Block B: Provisioning
    void boot_notification_req(const BootReasonEnum& reason);
    void notify_report_req(const int request_id, const int seq_no, const std::vector<ReportData>& report_data);

    // Functional Block C: Authorization
    AuthorizeResponse authorize_req(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                    const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data);

    // Functional Block G: Availability
    void status_notification_req(const int32_t evse_id, const int32_t connector_id, const ConnectorStatusEnum status);
    void heartbeat_req();

    // Functional Block E: Transactions
    void transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                               const ocpp::v201::Transaction& transaction,
                               const ocpp::v201::TriggerReasonEnum& trigger_reason, const int32_t seq_no,
                               const std::optional<int32_t>& cable_max_current,
                               const std::optional<ocpp::v201::EVSE>& evse,
                               const std::optional<ocpp::v201::IdToken>& id_token,
                               const std::optional<std::vector<ocpp::v201::MeterValue>>& meter_value,
                               const std::optional<int32_t>& number_of_phases_used, const std::optional<bool>& offline,
                               const std::optional<int32_t>& reservation_id);

    // Functional Block J: MeterValues
    void meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values);

    // Functional Block N: Diagnostics
    void handle_get_log_req(Call<GetLogRequest> call);
    void notify_event_req(const std::vector<EventData>& events);

    /* OCPP message handlers */

    // Functional Block B: Provisioning
    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);
    void handle_set_variables_req(Call<SetVariablesRequest> call);
    void handle_get_variables_req(Call<GetVariablesRequest> call);
    void handle_get_base_report_req(Call<GetBaseReportRequest> call);
    void handle_get_report_req(Call<GetReportRequest> call);
    void handle_set_network_profile_req(Call<SetNetworkProfileRequest> call);
    void handle_reset_req(Call<ResetRequest> call);

    // Functional Block C: Authorization
    void handle_clear_cache_req(Call<ClearCacheRequest> call);

    // Functional Block E: Transaction
    void handle_start_transaction_event_response(CallResult<TransactionEventResponse> call_result,
                                                 const int32_t evse_id, const IdToken& id_token);

    // Function Block F: Remote transaction control
    void handle_unlock_connector(Call<UnlockConnectorRequest> call);
    void handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call);
    void handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call);

    // Functional Block G: Availability
    void handle_change_availability_req(Call<ChangeAvailabilityRequest> call);

    // Functional Block L: Firmware management
    void handle_firmware_update_req(Call<UpdateFirmwareRequest> call);

    // Functional Block P: DataTransfer
    void handle_data_transfer_req(Call<DataTransferRequest> call);

public:
    /// \brief Construct a new ChargePoint object
    /// \param evse_connector_structure Map that defines the structure of EVSE and connectors of the chargepoint. The
    /// key represents the id of the EVSE and the value represents the number of connectors for this EVSE. The ids of
    /// the EVSEs have to increment starting with 1.
    /// \param device_model_storage_address address to device model storage (e.g. location of SQLite database)
    /// \param ocpp_main_path Path where utility files for OCPP are read and written to
    /// \param core_database_path Path to directory where core database is located
    /// \param message_log_path Path to where logfiles are written to
    /// \param callbacks Callbacks that will be registered for ChargePoint
    ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                const std::string& device_model_storage_address, const std::string& ocpp_main_path,
                const std::string& core_database_path, const std::string& sql_init_path,
                const std::string& message_log_path, const std::string& certs_path, const Callbacks& callbacks);

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    void start();

    /// \brief Starts the websocket
    void start_websocket();

    /// \brief Stops the ChargePoint. Disconnects the websocket connection and stops MessageQueue and all timers
    void stop();

    /// \brief Initializes the websocket and connects to CSMS if it is not yet connected
    void connect_websocket();

    /// \brief Disconnects the the websocket connection to the CSMS if it is connected
    void disconnect_websocket();

    /// \brief Chargepoint notifies about new firmware update status firmware_update_status. This function should be
    ///        called during a Firmware Update to indicate the current firmware_update_status.
    /// \param request_id   The request_id. When it is -1, it will not be included in the request.
    /// \param firmware_update_status The firmware_update_status should be convertable to the
    ///        ocpp::v201::FirmwareStatusEnum.
    void on_firmware_update_status_notification(int32_t request_id, std::string& firmware_update_status);

    /// \brief Event handler that should be called when a session has started
    /// \param evse_id
    /// \param connector_id
    void on_session_started(const int32_t evse_id, const int32_t connector_id);

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
    void on_transaction_started(const int32_t evse_id, const int32_t connector_id, const std::string& session_id,
                                const DateTime& timestamp, const ocpp::v201::TriggerReasonEnum trigger_reason,
                                const MeterValue& meter_start, const IdToken& id_token,
                                const std::optional<IdToken>& group_id_token,
                                const std::optional<int32_t>& reservation_id,
                                const std::optional<int32_t>& remote_start_id, const ChargingStateEnum charging_state);

    /// \brief Event handler that should be called when a transaction has finished
    /// \param evse_id
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    /// \param id_token
    /// \param signed_meter_value
    void on_transaction_finished(const int32_t evse_id, const DateTime& timestamp, const MeterValue& meter_stop,
                                 const ReasonEnum reason, const std::optional<std::string>& id_token,
                                 const std::optional<std::string>& signed_meter_value,
                                 const ChargingStateEnum charging_state);

    /// \brief Event handler that should be called when a session has finished
    /// \param evse_id
    /// \param connector_id
    void on_session_finished(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when a new meter value is present
    /// \param evse_id
    /// \param meter_value
    void on_meter_value(const int32_t evse_id, const MeterValue& meter_value);

    /// \brief Event handler that should be called when the connector on the given \p evse_id and \p connector_id
    /// becomes unavailable
    void on_unavailable(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when the connector on the given \p evse_id and \p connector_id
    /// becomes operative again
    void on_operative(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when the connector on the given evse_id and connector_id is faulted.
    /// \param evse_id          Faulted EVSE id
    /// \param connector_id     Faulted connector id
    void on_faulted(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when the connector on the given evse_id and connector_id is reserved.
    /// \param evse_id          Reserved EVSE id
    /// \param connector_id     Reserved connector id
    void on_reserved(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that will update the charging state internally when it has been changed.
    /// \param evse_id          The evse id of which the charging state has changed.
    /// \param charging_state   The new charging state.
    /// \return True on success. False if evse id does not exist.
    bool on_charging_state_changed(const uint32_t evse_id, ChargingStateEnum charging_state);

    /// \brief Validates provided \p id_token \p certificate and \p ocsp_request_data using CSMS, AuthCache or AuthList
    /// \param id_token
    /// \param certificate
    /// \param ocsp_request_data
    /// \return AuthorizeResponse containing the result of the validation
    AuthorizeResponse validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                     const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data);

    /// \brief Event handler that can be called to trigger a NotifyEvent.req with the given \p events
    /// \param events
    void on_event(const std::vector<EventData>& events);

    ///
    /// \brief Event handler that can be called to notify about the log status.
    ///
    /// This function should be called curing a Diagnostics / Log upload to indicate the current log status.
    ///
    /// \param status       Log status.
    /// \param requestId    Request id that was provided in GetLogRequest.
    ///
    void on_log_status_notification(UploadLogStatusEnum status, int32_t requestId);

    /// \brief Data transfer mechanism initiated by charger
    /// \param vendorId
    /// \param messageId
    /// \param data
    /// \return DataTransferResponse contaning the result from CSMS
    DataTransferResponse data_transfer_req(const CiString<255>& vendorId, const CiString<50>& messageId,
                                           const std::string& data);
};

} // namespace v201
} // namespace ocpp
