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
#include <ocpp1_6/charging_session.hpp>
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
    std::unique_ptr<Everest::SteadyTimer> heartbeat_timer;
    std::unique_ptr<Everest::SteadyTimer> boot_notification_timer;
    std::unique_ptr<Everest::SystemTimer> clock_aligned_meter_values_timer;
    std::unique_ptr<Everest::SteadyTimer> signed_firmware_update_download_timer;
    std::unique_ptr<Everest::SteadyTimer> signed_firmware_update_install_timer;
    std::chrono::time_point<date::utc_clock> clock_aligned_meter_values_time_point;
    std::map<int32_t, std::vector<MeterValue>> meter_values;
    std::mutex meter_values_mutex;
    std::map<int32_t, json> power_meter;
    std::map<int32_t, double> max_current_offered;
    std::map<int32_t, int32_t> number_of_phases_available;
    std::mutex power_meter_mutex;
    std::map<int32_t, AvailabilityType> change_availability_queue; // TODO: move to Connectors
    std::mutex change_availability_mutex;                          // TODO: move to Connectors
    std::unique_ptr<ChargingSessionHandler> charging_session_handler;
    std::map<int32_t, ChargingProfile> charge_point_max_profiles;
    std::mutex charge_point_max_profiles_mutex;
    std::map<int32_t, std::map<int32_t, ChargingProfile>> tx_default_profiles;
    std::mutex tx_default_profiles_mutex;

    std::map<int32_t, int32_t> res_conn_map;
    std::map<int32_t, std::string> reserved_id_tag_map;

    std::unique_ptr<Websocket> websocket;
    boost::shared_ptr<boost::asio::io_service::work> work;
    boost::asio::io_service io_service;
    std::thread io_service_thread;
    std::map<MessageId, std::thread> remote_start_transaction; // FIXME: this should be done differently
    std::mutex remote_start_transaction_mutex;                 // FIXME: this should be done differently
    std::map<MessageId, std::thread> remote_stop_transaction;  // FIXME: this should be done differently
    std::map<MessageId, std::thread> stop_transaction_thread;
    std::mutex remote_stop_transaction_mutex; // FIXME: this should be done differently
    std::vector<std::unique_ptr<Everest::SteadyTimer>> connection_timeout_timer;
    std::mutex connection_timeout_mutex;

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
    bool signed_firmware_update_running;
    std::mutex signed_firmware_update_mutex;

    // callbacks
    std::function<bool(int32_t connector)> enable_evse_callback;
    std::function<bool(int32_t connector)> disable_evse_callback;
    std::function<bool(int32_t connector)> pause_charging_callback;
    std::function<bool(int32_t connector)> resume_charging_callback;
    std::function<bool(int32_t connector, Reason reason)> cancel_charging_callback;
    std::function<void(int32_t connector, ocpp1_6::CiString20Type idTag)> remote_start_transaction_callback;
    std::function<bool(int32_t connector)> unlock_connector_callback;
    std::function<bool(int32_t connector, double max_current)> set_max_current_callback;
    std::function<std::string(std::string location)> upload_diagnostics_callback;
    std::function<void(std::string location)> update_firmware_callback;
    std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate,
                                    ocpp1_6::CiString20Type idTag, boost::optional<ocpp1_6::CiString20Type> parent_id)>
        reserve_now_callback;
    std::function<CancelReservationStatus(int32_t reservationId)> cancel_reservation_callback;
    std::function<void(SignedUpdateFirmwareRequest req)> signed_update_firmware_callback;

    std::function<void(SignedUpdateFirmwareRequest req)> signed_update_firmware_download_callback;
    std::function<void(SignedUpdateFirmwareRequest req, boost::filesystem::path file_path)>
        signed_update_firmware_install_callback;
    std::function<void()> switch_security_profile_callback;
    std::function<std::string(GetLogRequest msg)> upload_logs_callback;

    /// \brief This function is called after a successful connection to the Websocket
    void connected_callback();
    /// \brief This function registers callbacks that are registered in the database and have not been executed yet
    void register_scheduled_callbacks();
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
    void send_meter_value(int32_t connector, MeterValue meter_value);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, CiString50Type info,
                             ChargePointStatus status, DateTime timestamp);
    void status_notification(int32_t connector, ChargePointErrorCode errorCode, ChargePointStatus status);
    void stop_all_transactions();
    void stop_all_transactions(Reason reason);

    // new transaction handling:
    bool start_transaction(int32_t connector);
    int32_t get_meter_wh(int32_t connector);
    bool stop_transaction(int32_t connector, Reason reason);

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
    void registerSwitchSecurityProfileCallback(const std::function<void()>& callback);
    // Local Authorization List profile
    void handleSendLocalListRequest(Call<SendLocalListRequest> call);
    void handleGetLocalListVersionRequest(Call<GetLocalListVersionRequest> call);

public:
    /// \brief Creates a ChargePoint object with the provided \p configuration
    explicit ChargePoint(std::shared_ptr<ChargePointConfiguration> configuration);

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

    /// \brief Authorizes the provided \p idTag with the central system
    /// \returns the AuthorizationStatus
    AuthorizationStatus authorize_id_tag(CiString20Type idTag);

    /// \brief Provides the IdTag that was associated with the charging session on the provided \p connector
    /// \returns the IdTag if it is available, boost::none otherwise
    boost::optional<CiString20Type> get_authorized_id_tag(int32_t connector);

    /// \brief Allows the exchange of arbitrary \p data identified by a \p vendorId and \p messageId with a central
    /// system \returns the DataTransferResponse
    DataTransferResponse data_transfer(const CiString255Type& vendorId, const CiString50Type& messageId,
                                       const std::string& data);

    /// \brief Creates a new public/private key pair and sends a certificate signing request to the central system
    void signCertificate();

    /// registers a \p callback function that can be used to receive a arbitrary data transfer for the given \p
    /// vendorId and \p messageId
    void register_data_transfer_callback(const CiString255Type& vendorId, const CiString50Type& messageId,
                                         const std::function<void(const std::string data)>& callback);

    /// \brief Stores the given \p powermeter values for the given \p connector
    void receive_power_meter(int32_t connector, json powermeter);

    /// \brief Stores the given \p max_current for the given \p connector
    void receive_max_current_offered(int32_t connector, double max_current);

    /// \brief Stores the given \p number_of_phases for the given \p connector
    void receive_number_of_phases_available(int32_t connector, double number_of_phases);

    /// \brief Initiates a new charging session on the given \p connector with the given \p timestamp and \p
    /// energy_Wh_import as start energy
    /// \returns true if the session could be stated successfully
    bool start_session(int32_t connector, DateTime timestamp, double energy_Wh_import,
                       boost::optional<int32_t> reservation_id);

    /// \brief Stops a charging session on the given \p connector with the given \p timestamp and \p
    /// energy_Wh_import as stop energy
    /// \returns true if the session could be stopped successfully
    bool stop_session(int32_t connector, DateTime timestamp, double energy_Wh_import);

    /// \brief Cancels a charging session on the given \p connector with the given \p timestamp and \p energy_Wh_import
    /// as stop energy. This method evaluates if a stop transaction should already be sent or not depending on the given
    /// configuration
    bool cancel_session(int32_t connector, DateTime timestamp, double energy_Wh_import, Reason reason);

    /// \brief EV indicates that it starts charging on the given \p connector
    /// \returns true if this state change was possible
    bool start_charging(int32_t connector);

    /// \brief EV indicates that it suspends charging on the given \p connector
    /// \returns true if this state change was possible
    bool suspend_charging_ev(int32_t connector);

    /// \brief EVSE indicates that it suspends charging on the given \p connector
    /// \returns true if this state change was possible
    bool suspend_charging_evse(int32_t connector);

    /// \brief EV/EVSE indicates that it resumed charging on the given \p connector
    /// \returns true if this state change was possible
    bool resume_charging(int32_t connector);

    /// \brief EV was disconnected
    /// \returns true if this state change was possible
    bool plug_disconnected(int32_t connector);

    // /// EV/EVSE indicates that charging has finished
    // bool stop_charging(int32_t connector);

    /// EV/EVSE indicates that an error with the given \p error_code occured
    /// \returns true if this state change was possible
    bool error(int32_t connector, const ChargePointErrorCode& error);

    /// EV/EVSE indicates that a vendor specific error with the given \p vendor_error_code occured on the given \p
    /// connector \returns true if this state change was possible
    bool vendor_error(int32_t connector, CiString50Type vendor_error_code);

    /// EVSE indicates a permanent fault on the given \p connector
    /// \returns true if this state change was possible
    bool permanent_fault(int32_t connector);

    /// \brief Sends a DiagnosticStatusNotification with the given \p status
    void send_diagnostic_status_notification(DiagnosticsStatus status);
    /// \brief Sends a FirmwareUpdateStatusNotification with the given \p status
    void send_firmware_status_notification(FirmwareStatus status);
    /// \brief Sends a logStatusNotification with the given \p status for the given \p requestId
    void logStatusNotification(UploadLogStatusEnumType status, int requestId);
    /// \brief Sends a SignedFirmwareUpdateStatusNotification with the given \p status for the given \p requestId
    void signedFirmwareUpdateStatusNotification(FirmwareStatusEnumType status, int requestId);

    /// \brief registers a \p callback function that can be used to enable the evse
    void register_enable_evse_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to disable the evse
    void register_disable_evse_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to pause charging
    void register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to resume charging
    void register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to cancel charging
    void
    register_cancel_charging_callback(const std::function<bool(int32_t connector, ocpp1_6::Reason reason)>& callback);

    /// registers a \p callback function that can be used to remotely start a transaction
    void register_remote_start_transaction_callback(
        const std::function<void(int32_t connector, ocpp1_6::CiString20Type idTag)>& callback);

    /// \brief registers a \p callback function that can be used to reserve a connector for a idTag until a timeout
    /// is reached
    void register_reserve_now_callback(
        const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp1_6::DateTime expiryDate,
                                              ocpp1_6::CiString20Type idTag,
                                              boost::optional<ocpp1_6::CiString20Type> parent_id)>& callback);

    /// \brief registers a \p callback function that can be used to cancel a reservation on a connector
    void
    register_cancel_reservation_callback(const std::function<CancelReservationStatus(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to unlock the connector
    void register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to set a max_current on a given connector
    void register_set_max_current_callback(const std::function<bool(int32_t connector, double max_current)>& callback);

    /// \brief registers a \p callback function that can be used to trigger an upload of diagnostics
    void register_upload_diagnostics_callback(const std::function<std::string(std::string location)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a firmware update
    void register_update_firmware_callback(const std::function<void(std::string location)>& callback);

    /// \brief registers a \p callback function that can be used to upload logfiles
    void register_upload_logs_callback(const std::function<std::string(GetLogRequest req)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a signed firmware download process
    void register_signed_update_firmware_download_callback(
        const std::function<void(SignedUpdateFirmwareRequest req)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a signed firmware install process
    void register_signed_update_firmware_install_callback(
        const std::function<void(SignedUpdateFirmwareRequest req, boost::filesystem::path file_path)>& callback);

    /// \brief notifies the chargepoint that the signed firmware update for the given \p req was downloaded to the
    /// given \p file_path
    void notify_signed_firmware_update_downloaded(SignedUpdateFirmwareRequest req, boost::filesystem::path file_path);

    /// FIXME(piet) triggers a bootnotification and can be removed when firmware update mechanisms are implemented
    void trigger_boot_notification();

    /// for the interrupt of log uploads
    std::atomic<bool> interrupt_log_upload;
    std::condition_variable cv;
    std::mutex log_upload_mutex;

    /// \brief indicates if a signed_firmware_update is currently running
    bool is_signed_firmware_update_running();

    /// \brief setter for the signed_firwmare_update_running flag
    void set_signed_firmware_update_running(bool b);

    /// \brief called when a reservation is started
    void reservation_start(int32_t connector_id, int32_t reservation_id, std::string id_tag);

    /// \brief called when a reservation ends
    void reservation_end(int32_t connector, int32_t reservation_id, std::string reason);

    // FIXME: rework the following API functions, do we want to expose them?
    // insert plug
    // bool plug_connected(int32_t connector);
};

} // namespace ocpp1_6
