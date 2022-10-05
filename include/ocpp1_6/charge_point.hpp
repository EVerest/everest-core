// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <atomic>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <future>
#include <iostream>
#include <mutex>
#include <set>

#include <boost/asio.hpp>

#include <everest/timer.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>
#include <ocpp1_6/charge_point_state_machine.hpp>
#include <ocpp1_6/database_handler.hpp>
#include <ocpp1_6/message_queue.hpp>
#include <ocpp1_6/messages/Authorize.hpp>
#include <ocpp1_6/messages/BootNotification.hpp>
#include <ocpp1_6/messages/CancelReservation.hpp>
#include <ocpp1_6/messages/CertificateSigned.hpp>
#include <ocpp1_6/messages/ChangeAvailability.hpp>
#include <ocpp1_6/messages/ChangeConfiguration.hpp>
#include <ocpp1_6/messages/ClearCache.hpp>
#include <ocpp1_6/messages/ClearChargingProfile.hpp>
#include <ocpp1_6/messages/DataTransfer.hpp>
#include <ocpp1_6/messages/DeleteCertificate.hpp>
#include <ocpp1_6/messages/DiagnosticsStatusNotification.hpp>
#include <ocpp1_6/messages/ExtendedTriggerMessage.hpp>
#include <ocpp1_6/messages/FirmwareStatusNotification.hpp>
#include <ocpp1_6/messages/GetCompositeSchedule.hpp>
#include <ocpp1_6/messages/GetConfiguration.hpp>
#include <ocpp1_6/messages/GetDiagnostics.hpp>
#include <ocpp1_6/messages/GetInstalledCertificateIds.hpp>
#include <ocpp1_6/messages/GetLocalListVersion.hpp>
#include <ocpp1_6/messages/GetLog.hpp>
#include <ocpp1_6/messages/Heartbeat.hpp>
#include <ocpp1_6/messages/InstallCertificate.hpp>
#include <ocpp1_6/messages/LogStatusNotification.hpp>
#include <ocpp1_6/messages/MeterValues.hpp>
#include <ocpp1_6/messages/RemoteStartTransaction.hpp>
#include <ocpp1_6/messages/RemoteStopTransaction.hpp>
#include <ocpp1_6/messages/ReserveNow.hpp>
#include <ocpp1_6/messages/Reset.hpp>
#include <ocpp1_6/messages/SecurityEventNotification.hpp>
#include <ocpp1_6/messages/SendLocalList.hpp>
#include <ocpp1_6/messages/SetChargingProfile.hpp>
#include <ocpp1_6/messages/SignCertificate.hpp>
#include <ocpp1_6/messages/SignedFirmwareStatusNotification.hpp>
#include <ocpp1_6/messages/SignedUpdateFirmware.hpp>
#include <ocpp1_6/messages/StartTransaction.hpp>
#include <ocpp1_6/messages/StatusNotification.hpp>
#include <ocpp1_6/messages/StopTransaction.hpp>
#include <ocpp1_6/messages/TriggerMessage.hpp>
#include <ocpp1_6/messages/UnlockConnector.hpp>
#include <ocpp1_6/messages/UpdateFirmware.hpp>
#include <ocpp1_6/transaction.hpp>
#include <ocpp1_6/types.hpp>
#include <ocpp1_6/websocket/websocket.hpp>

namespace ocpp1_6 {

/// \brief Contains a ChargePoint implementation compatible with OCPP-J 1.6
class ChargePoint {
private:
    ChargePointConnectionState connection_state;
    std::unique_ptr<MessageQueue> message_queue;
    int32_t heartbeat_interval;
    bool initialized;
    bool stopped;
    std::chrono::time_point<date::utc_clock> boot_time;
    std::set<MessageType> allowed_message_types;
    std::mutex allowed_message_types_mutex;
    RegistrationStatus registration_status;
    std::unique_ptr<ChargePointStates> status;
    std::shared_ptr<ChargePointConfiguration> configuration;
    std::unique_ptr<DatabaseHandler> database_handler;
    std::unique_ptr<Everest::SteadyTimer> heartbeat_timer;
    std::unique_ptr<Everest::SteadyTimer> boot_notification_timer;
    std::unique_ptr<Everest::SystemTimer> clock_aligned_meter_values_timer;
    std::chrono::time_point<date::utc_clock> clock_aligned_meter_values_time_point;
    std::map<int32_t, std::vector<MeterValue>> meter_values;
    std::mutex meter_values_mutex;
    std::map<int32_t, json> power_meter;
    std::map<int32_t, double> max_current_offered;
    std::map<int32_t, int32_t> number_of_phases_available;
    std::mutex power_meter_mutex;
    std::map<int32_t, AvailabilityType> change_availability_queue; // TODO: move to Connectors
    std::mutex change_availability_mutex;                          // TODO: move to Connectors
    std::unique_ptr<TransactionHandler> transaction_handler;
    std::map<int32_t, ChargingProfile> charge_point_max_profiles;
    std::mutex charge_point_max_profiles_mutex;
    std::map<int32_t, std::map<int32_t, ChargingProfile>> tx_default_profiles;
    std::mutex tx_default_profiles_mutex;

    std::unique_ptr<Websocket> websocket;
    boost::shared_ptr<boost::asio::io_service::work> work;
    boost::asio::io_service io_service;
    std::thread io_service_thread;
    std::map<MessageId, std::thread> remote_start_transaction; // FIXME: this should be done differently
    std::mutex remote_start_transaction_mutex;                 // FIXME: this should be done differently
    std::map<MessageId, std::thread> remote_stop_transaction;  // FIXME: this should be done differently
    std::map<MessageId, std::thread> stop_transaction_thread;
    std::mutex remote_stop_transaction_mutex; // FIXME: this should be done differently

    std::map<std::string, std::map<std::string, std::function<void(const std::string data)>>> data_transfer_callbacks;
    std::mutex data_transfer_callbacks_mutex;

    std::mutex stop_transaction_mutex;

    std::thread reset_thread;
    DiagnosticsStatus diagnostics_status;
    FirmwareStatus firmware_status;
    UploadLogStatusEnumType log_status;
    int log_status_request_id;

    FirmwareStatusEnumType signed_firmware_status;
    int signed_firmware_status_request_id;

    // callbacks
    std::function<bool(int32_t connector)> enable_evse_callback;
    std::function<bool(int32_t connector)> disable_evse_callback;
    std::function<bool(int32_t connector)> pause_charging_callback;
    std::function<bool(int32_t connector)> resume_charging_callback;
    std::function<void(const std::string& id_token, int32_t connector, bool prevalidated)> provide_token_callback;
    std::function<bool(int32_t connector, Reason reason)> stop_transaction_callback;
    std::function<bool(int32_t connector)> unlock_connector_callback;
    std::function<bool(int32_t connector, int32_t max_current)> set_max_current_callback;
    std::function<bool(const ResetType& reset_type)> reset_callback;
    std::function<void(const std::string& system_time)> set_system_time_callback;
    std::function<ocpp1_6::GetLogResponse(const ocpp1_6::GetDiagnosticsRequest& request)> upload_diagnostics_callback;
    std::function<void(const ocpp1_6::UpdateFirmwareRequest msg)> update_firmware_callback;

    std::function<ocpp1_6::UpdateFirmwareStatusEnumType(const ocpp1_6::SignedUpdateFirmwareRequest msg)>
        signed_update_firmware_callback;

    std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate,
                                    ocpp1_6::CiString20Type idTag, boost::optional<ocpp1_6::CiString20Type> parent_id)>
        reserve_now_callback;
    std::function<bool(int32_t reservation_id)> cancel_reservation_callback;
    std::function<void()> switch_security_profile_callback;
    std::function<ocpp1_6::GetLogResponse(GetLogRequest msg)> upload_logs_callback;
    std::function<void(int32_t connection_timeout)> set_connection_timeout_callback;

    /// \brief This function is called after a successful connection to the Websocket
    void connected_callback();
    void init_websocket(int32_t security_profile);
    void message_callback(const std::string& message);
    void handle_message(const json& json_message, MessageType message_type);
    bool allowed_to_send_message(json::array_t message_type);
    template <class T> bool send(Call<T> call);
    template <class T> std::future<EnhancedMessage> send_async(Call<T> call);
    template <class T> bool send(CallResult<T> call_result);
    bool send(CallError call_error);
    void heartbeat();
    void boot_notification();
    void clock_aligned_meter_values_sample();
    void connection_timeout(int32_t connector);
    void update_heartbeat_interval();
    void update_meter_values_sample_interval();
    void update_clock_aligned_meter_values_interval();
    MeterValue get_latest_meter_value(int32_t connector, std::vector<MeasurandWithPhase> values_of_interest,
                                      ReadingContext context);
    MeterValue get_signed_meter_value(const std::string& signed_value, const ReadingContext& context,
                                      const DateTime& datetime);
    void send_meter_value(int32_t connector, MeterValue meter_value);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, CiString50Type info,
                             ChargePointStatus status, DateTime timestamp);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status);
    void diagnostic_status_notification(DiagnosticsStatus status);
    void firmware_status_notification(FirmwareStatus status);
    void log_status_notification(UploadLogStatusEnumType status, int requestId);
    void signed_firmware_update_status_notification(FirmwareStatusEnumType status, int requestId);

    void stop_all_transactions();
    void stop_all_transactions(Reason reason);
    bool validate_against_cache_entries(CiString20Type id_tag);

    // new transaction handling:
    void start_transaction(std::shared_ptr<Transaction> transaction);
    int32_t get_meter_wh(int32_t connector);

    /// \brief Sends StopTransaction.req for all transactions for which meter_stop or time_end is not set in the
    /// database's Transaction table
    void stop_pending_transactions();
    void stop_transaction(int32_t connector, Reason reason, boost::optional<CiString20Type> id_tag_end);

    // security
    /// \brief Creates a new public/private key pair and sends a certificate signing request to the central system
    void sign_certificate();

    // core profile
    void handleBootNotificationResponse(
        CallResult<BootNotificationResponse> call_result); // TODO(kai):: async/promise based version?
    void handleChangeAvailabilityRequest(Call<ChangeAvailabilityRequest> call);
    void handleChangeConfigurationRequest(Call<ChangeConfigurationRequest> call);
    void handleClearCacheRequest(Call<ClearCacheRequest> call);
    void handleDataTransferRequest(Call<DataTransferRequest> call);
    void handleGetConfigurationRequest(Call<GetConfigurationRequest> call);
    void handleRemoteStartTransactionRequest(Call<RemoteStartTransactionRequest> call);
    void handleRemoteStopTransactionRequest(Call<RemoteStopTransactionRequest> call);
    void handleResetRequest(Call<ResetRequest> call);
    void handleStartTransactionResponse(CallResult<StartTransactionResponse> call_result);
    void handleStopTransactionResponse(CallResult<StopTransactionResponse> call_result);
    void handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call);

    // smart charging profile
    void handleSetChargingProfileRequest(Call<SetChargingProfileRequest> call);
    void handleGetCompositeScheduleRequest(Call<GetCompositeScheduleRequest> call);
    void handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call);
    std::vector<ChargingProfile> get_valid_profiles(std::map<int32_t, ocpp1_6::ChargingProfile> profiles,
                                                    date::utc_clock::time_point start_time,
                                                    date::utc_clock::time_point end_time);

    ChargingSchedule create_composite_schedule(std::vector<ChargingProfile> charging_profiles,
                                               date::utc_clock::time_point start_time,
                                               date::utc_clock::time_point end_time, int32_t duration_s);

    /// \brief ReserveNow.req(connectorId, expiryDate, idTag, reservationId, [parentIdTag]): tries to perform the
    /// reservation and sends a reservation response. The reservation response: ReserveNow::Status
    void handleReserveNowRequest(Call<ReserveNowRequest> call);

    /// \brief Receives CancelReservation.req(reservationId)
    /// The reservation response:  CancelReservationStatus: `Accepted` if the reservationId was found, else
    /// `Rejected`
    void handleCancelReservationRequest(Call<CancelReservationRequest> call);

    // RemoteTrigger profile
    void handleTriggerMessageRequest(Call<TriggerMessageRequest> call);

    // FirmwareManagement profile
    void handleGetDiagnosticsRequest(Call<GetDiagnosticsRequest> call);
    void handleUpdateFirmwareRequest(Call<UpdateFirmwareRequest> call);

    // Security profile
    void handleExtendedTriggerMessageRequest(Call<ExtendedTriggerMessageRequest> call);
    void handleCertificateSignedRequest(Call<CertificateSignedRequest> call);
    void handleGetInstalledCertificateIdsRequest(Call<GetInstalledCertificateIdsRequest> call);
    void handleDeleteCertificateRequest(Call<DeleteCertificateRequest> call);
    void handleInstallCertificateRequest(Call<InstallCertificateRequest> call);
    void handleGetLogRequest(Call<GetLogRequest> call);
    void handleSignedUpdateFirmware(Call<SignedUpdateFirmwareRequest> call);
    void securityEventNotification(const SecurityEvent& type, const std::string& tech_info);
    void switchSecurityProfile(int32_t new_security_profile);
    // Local Authorization List profile
    void handleSendLocalListRequest(Call<SendLocalListRequest> call);
    void handleGetLocalListVersionRequest(Call<GetLocalListVersionRequest> call);

public:
    /// \brief Creates a ChargePoint object with the provided \p configuration
    explicit ChargePoint(std::shared_ptr<ChargePointConfiguration> configuration, const std::string& database_path,
                         const std::string &sql_init_path);

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    bool start();

    /// \brief Restarts the ChargePoint if it has been stopped before. The ChargePoint reinitialized, connects to the
    /// websocket and starts to communicate OCPP messages again.
    bool restart();

    /// \brief Stops the ChargePoint, stops timers, transactions and the message queue, disconnects from the websocket
    bool stop();

    /// \brief connects the websocket if it is not yet connected
    void connect_websocket();

    /// \brief Disconnects the the websocket if it is connected
    void disconnect_websocket();

    // public API for Core profile

    /// \brief Authorizes the provided \p id_token against the central system
    /// \returns the IdTagInfo
    IdTagInfo authorize_id_token(CiString20Type id_token);

    /// \brief Allows the exchange of arbitrary \p data identified by a \p vendorId and \p messageId with a central
    /// system \returns the DataTransferResponse
    DataTransferResponse data_transfer(const CiString255Type& vendorId, const CiString50Type& messageId,
                                       const std::string& data);

    /// \brief Stores the given \p powermeter values for the given \p connector
    void on_meter_values(int32_t connector, json powermeter);

    /// \brief Stores the given \p max_current for the given \p connector
    void on_max_current_offered(int32_t connector, int32_t max_current);

    /// \brief Notifies chargepoint that a new session with the given \p session_id has been started at the given \p
    /// connector with the given \p reason
    void on_session_started(int32_t connector, const std::string& session_id, const std::string& reason);

    /// \brief Notifies chargepoint that a session has been stopped at the given \p
    /// connector
    void on_session_stopped(int32_t connector);

    /// \brief Notifies chargepoint that a transaction at the given \p connector with the given parameters has been
    /// started
    void on_transaction_started(const int32_t& connector, const std::string& session_id, const std::string& id_token,
                                const int32_t& meter_start, boost::optional<int32_t> reservation_id,
                                const DateTime& timestamp, boost::optional<std::string> signed_meter_value);

    /// \brief Notifies chargepoint that the transaction on the given \p connector with the given \p reason has been
    /// stopped.
    void on_transaction_stopped(const int32_t connector, const std::string& session_id, const Reason& reason,
                                DateTime timestamp, float energy_wh_import, boost::optional<CiString20Type> id_tag_end,
                                boost::optional<std::string> signed_meter_value);

    /// \brief EV indicates that it suspends charging on the given \p connector
    /// \returns true if this state change was possible
    void on_suspend_charging_ev(int32_t connector);

    /// \brief EVSE indicates that it suspends charging on the given \p connector
    /// \returns true if this state change was possible
    void on_suspend_charging_evse(int32_t connector);

    /// \brief EV/EVSE indicates that it resumed charging on the given \p connector
    /// \returns true if this state change was possible
    void on_resume_charging(int32_t connector);

    /// \brief EV/EVSE indicates that an error with the given \p error_code occured
    /// \returns true if this state change was possible
    void on_error(int32_t connector, const ChargePointErrorCode& error);

    /// \brief Chargepoint notifies about new log status \p log_status
    void on_log_status_notification(int32_t request_id, std::string log_status);

    /// \brief Chargepoint notifies about new firmware update status \p firmware_update_status
    void on_firmware_update_status_notification(int32_t request_id, std::string firmware_update_status);

    /// \brief called when a reservation is started at the given \p connector
    void on_reservation_start(int32_t connector);

    /// \brief called when a reservation ends at the given \p connector
    void on_reservation_end(int32_t connector);

    /// registers a \p callback function that can be used to receive a arbitrary data transfer for the given \p
    /// vendorId and \p messageId
    void register_data_transfer_callback(const CiString255Type& vendorId, const CiString50Type& messageId,
                                         const std::function<void(const std::string data)>& callback);

    /// \brief registers a \p callback function that can be used to enable the evse
    void register_enable_evse_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to disable the evse
    void register_disable_evse_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to pause charging
    void register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to resume charging
    void register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to provide an \p id_token for the given \p connector to
    /// an authorization handler. \p prevalidated signals to the authorization handler that no further authorization is
    /// necessary
    void register_provide_token_callback(
        const std::function<void(const std::string& id_token, int32_t connector, bool prevalidated)>& callback);

    /// \brief registers a \p callback function that can be used to stop a transaction
    void
    register_stop_transaction_callback(const std::function<bool(int32_t connector, ocpp1_6::Reason reason)>& callback);

    /// \brief registers a \p callback function that can be used to reserve a connector for a idTag until a timeout
    /// is reached
    void register_reserve_now_callback(
        const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate,
                                              ocpp1_6::CiString20Type idTag,
                                              boost::optional<ocpp1_6::CiString20Type> parent_id)>& callback);

    /// \brief registers a \p callback function that can be used to cancel a reservation on a connector. Callback
    /// function returns -1 if the reservation could not be cancelled, else it returns the connector id of the cancelled
    /// reservation
    void register_cancel_reservation_callback(const std::function<bool(int32_t reservation_id)>& callback);

    /// \brief registers a \p callback function that can be used to unlock the connector
    void register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to set a max_current on a given connector
    void register_set_max_current_callback(const std::function<bool(int32_t connector, double max_current)>& callback);

    /// \brief registers a \p callback function that can be used to trigger an upload of diagnostics
    void register_upload_diagnostics_callback(
        const std::function<ocpp1_6::GetLogResponse(const ocpp1_6::GetDiagnosticsRequest& request)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a firmware update
    void
    register_update_firmware_callback(const std::function<void(const ocpp1_6::UpdateFirmwareRequest msg)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a firmware update
    void register_signed_update_firmware_callback(
        const std::function<ocpp1_6::UpdateFirmwareStatusEnumType(const ocpp1_6::SignedUpdateFirmwareRequest msg)>&
            callback);

    /// \brief registers a \p callback function that can be used to upload logfiles
    void register_upload_logs_callback(const std::function<ocpp1_6::GetLogResponse(GetLogRequest req)>& callback);

    /// \brief registers a \p callback function that can be used to set that authorization or plug in connection timeout
    void register_set_connection_timeout_callback(const std::function<void(int32_t connection_timeout)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a reset of the chargepoint
    void register_reset_callback(const std::function<bool(const ResetType& reset_type)>& callback);

    /// \brief registera \p callback function that can be used to set the system time of the chargepoint
    void register_set_system_time_callback(const std::function<void(const std::string& system_time)>& callback);
};

} // namespace ocpp1_6
