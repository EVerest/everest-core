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
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/GetBaseReport.hpp>
#include <ocpp/v201/messages/GetReport.hpp>
#include <ocpp/v201/messages/GetVariables.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>
#include <ocpp/v201/messages/MeterValues.hpp>
#include <ocpp/v201/messages/NotifyEvent.hpp>
#include <ocpp/v201/messages/NotifyReport.hpp>
#include <ocpp/v201/messages/Reset.hpp>
#include <ocpp/v201/messages/SetVariables.hpp>
#include <ocpp/v201/messages/StatusNotification.hpp>
#include <ocpp/v201/messages/TransactionEvent.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/UnlockConnector.hpp>

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

    // timers
    Everest::SteadyTimer heartbeat_timer;
    Everest::SteadyTimer boot_notification_timer;
    Everest::SteadyTimer aligned_meter_values_timer;

    // states
    RegistrationStatusEnum registration_status;
    WebsocketConnectionStatusEnum websocket_connection_status;
    OperationalStatusEnum operational_state;

    // callback struct
    Callbacks callbacks;

    // general message handling
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage<v201::MessageType>> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    bool send(CallError call_error);

    // internal helper functions
    void init_websocket();
    void handle_message(const json& json_message, const MessageType& message_type);
    void message_callback(const std::string& message);
    void update_aligned_data_interval();
    bool is_change_availability_possible(const ChangeAvailabilityRequest& req);
    void handle_scheduled_change_availability_requests(const int32_t evse_id);

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
    void handle_reset_req(Call<ResetRequest> call);

    // Functional Block E: Transaction
    void handle_start_transaction_event_response(CallResult<TransactionEventResponse> call_result,
                                                 const int32_t evse_id);

    // Function Block F: Remote transaction control
    void handle_unlock_connector(Call<UnlockConnectorRequest> call);

    // Functional Block G: Availability
    void handle_change_availability_req(Call<ChangeAvailabilityRequest> call);

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

    /// \brief Stops the ChargePoint. Disconnects the websocket connection and stops MessageQueue and all timers
    void stop();

    /// \brief Event handler that should be called when a session has started
    /// \param evse_id
    /// \param connector_id
    void on_session_started(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when a transaction has started
    /// \param evse_id
    /// \param connector_id
    /// \param session_id
    /// \param timestamp
    /// \param meter_start
    /// \param id_token
    /// \param reservation_id
    void on_transaction_started(const int32_t evse_id, const int32_t connector_id, const std::string& session_id,
                                const DateTime& timestamp, const MeterValue& meter_start, const IdToken& id_token,
                                const std::optional<int32_t>& reservation_id);

    /// \brief Event handler that should be called when a transaction has finished
    /// \param evse_id
    /// \param timestamp
    /// \param meter_stop
    /// \param reason
    /// \param id_token
    /// \param signed_meter_value
    void on_transaction_finished(const int32_t evse_id, const DateTime& timestamp, const MeterValue& meter_stop,
                                 const ReasonEnum reason, const std::optional<std::string>& id_token,
                                 const std::optional<std::string>& signed_meter_value);

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

    /// @brief Data transfer mechanism initiated by charger
    /// @param vendorId
    /// @param messageId
    /// @param data
    /// @return DataTransferResponse contaning the result from CSMS
    DataTransferResponse data_transfer_req(const CiString<255>& vendorId, const CiString<50>& messageId,
                                           const std::string& data);
};

} // namespace v201
} // namespace ocpp
