// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_CHARGE_POINT_IMPL_HPP
#define OCPP_V16_CHARGE_POINT_IMPL_HPP
#include <atomic>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <future>
#include <iostream>
#include <mutex>
#include <ocpp/common/support_older_cpp_versions.hpp>
#include <set>

#include <everest/timer.hpp>

#include <ocpp/common/aligned_timer.hpp>
#include <ocpp/common/charging_station_base.hpp>
#include <ocpp/common/message_dispatcher.hpp>
#include <ocpp/common/message_queue.hpp>
#include <ocpp/common/schemas.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/common/websocket/websocket.hpp>
#include <ocpp/v16/charge_point_configuration.hpp>
#include <ocpp/v16/connector.hpp>
#include <ocpp/v16/database_handler.hpp>
#include <ocpp/v16/message_dispatcher.hpp>
#include <ocpp/v16/messages/Authorize.hpp>
#include <ocpp/v16/messages/BootNotification.hpp>
#include <ocpp/v16/messages/CancelReservation.hpp>
#include <ocpp/v16/messages/CertificateSigned.hpp>
#include <ocpp/v16/messages/ChangeAvailability.hpp>
#include <ocpp/v16/messages/ChangeConfiguration.hpp>
#include <ocpp/v16/messages/ClearCache.hpp>
#include <ocpp/v16/messages/ClearChargingProfile.hpp>
#include <ocpp/v16/messages/DataTransfer.hpp>
#include <ocpp/v16/messages/DeleteCertificate.hpp>
#include <ocpp/v16/messages/DiagnosticsStatusNotification.hpp>
#include <ocpp/v16/messages/ExtendedTriggerMessage.hpp>
#include <ocpp/v16/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v16/messages/GetCompositeSchedule.hpp>
#include <ocpp/v16/messages/GetConfiguration.hpp>
#include <ocpp/v16/messages/GetDiagnostics.hpp>
#include <ocpp/v16/messages/GetInstalledCertificateIds.hpp>
#include <ocpp/v16/messages/GetLocalListVersion.hpp>
#include <ocpp/v16/messages/GetLog.hpp>
#include <ocpp/v16/messages/Heartbeat.hpp>
#include <ocpp/v16/messages/InstallCertificate.hpp>
#include <ocpp/v16/messages/LogStatusNotification.hpp>
#include <ocpp/v16/messages/MeterValues.hpp>
#include <ocpp/v16/messages/RemoteStartTransaction.hpp>
#include <ocpp/v16/messages/RemoteStopTransaction.hpp>
#include <ocpp/v16/messages/ReserveNow.hpp>
#include <ocpp/v16/messages/Reset.hpp>
#include <ocpp/v16/messages/SecurityEventNotification.hpp>
#include <ocpp/v16/messages/SendLocalList.hpp>
#include <ocpp/v16/messages/SetChargingProfile.hpp>
#include <ocpp/v16/messages/SignCertificate.hpp>
#include <ocpp/v16/messages/SignedFirmwareStatusNotification.hpp>
#include <ocpp/v16/messages/SignedUpdateFirmware.hpp>
#include <ocpp/v16/messages/StartTransaction.hpp>
#include <ocpp/v16/messages/StatusNotification.hpp>
#include <ocpp/v16/messages/StopTransaction.hpp>
#include <ocpp/v16/messages/TriggerMessage.hpp>
#include <ocpp/v16/messages/UnlockConnector.hpp>
#include <ocpp/v16/messages/UpdateFirmware.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/smart_charging.hpp>
#include <ocpp/v16/transaction.hpp>
#include <ocpp/v16/types.hpp>

// for OCPP1.6 PnC
#include <ocpp/v2/messages/Authorize.hpp>
#include <ocpp/v2/messages/CertificateSigned.hpp>
#include <ocpp/v2/messages/DeleteCertificate.hpp>
#include <ocpp/v2/messages/Get15118EVCertificate.hpp>
#include <ocpp/v2/messages/GetCertificateStatus.hpp>
#include <ocpp/v2/messages/GetInstalledCertificateIds.hpp>
#include <ocpp/v2/messages/InstallCertificate.hpp>
#include <ocpp/v2/messages/SignCertificate.hpp>
#include <ocpp/v2/messages/TriggerMessage.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a ChargePoint implementation compatible with OCPP-J 1.6
class ChargePointImpl : ocpp::ChargingStationBase {
private:
    BootReasonEnum bootreason;
    ChargePointConnectionState connection_state;
    bool boot_notification_callerror;
    std::atomic<RegistrationStatus> registration_status;
    DiagnosticsStatus diagnostics_status;
    FirmwareStatus firmware_status;
    bool firmware_update_is_pending = false;
    UploadLogStatusEnumType log_status;
    std::string message_log_path;

    std::unique_ptr<Websocket> websocket;
    std::unique_ptr<ocpp::MessageDispatcherInterface<MessageType>> message_dispatcher;
    Everest::SteadyTimer websocket_timer;
    std::unique_ptr<MessageQueue<v16::MessageType>> message_queue;
    std::map<int32_t, std::shared_ptr<Connector>> connectors;
    std::unique_ptr<SmartChargingHandler> smart_charging_handler;
    int32_t heartbeat_interval;
    bool stopped;
    bool initialized;
    std::chrono::time_point<date::utc_clock> boot_time;
    std::set<MessageType> allowed_message_types;
    std::mutex allowed_message_types_mutex;
    std::unique_ptr<ChargePointStates> status;
    std::shared_ptr<ChargePointConfiguration> configuration;
    std::shared_ptr<ocpp::v16::DatabaseHandler> database_handler;
    std::unique_ptr<Everest::SteadyTimer> boot_notification_timer;
    std::unique_ptr<Everest::SteadyTimer> heartbeat_timer;
    std::unique_ptr<ClockAlignedTimer> clock_aligned_meter_values_timer;
    std::vector<std::unique_ptr<Everest::SteadyTimer>> status_notification_timers;
    std::unique_ptr<Everest::SteadyTimer> ocsp_request_timer;
    std::unique_ptr<Everest::SteadyTimer> client_certificate_timer;
    std::unique_ptr<Everest::SteadyTimer> v2g_certificate_timer;
    std::unique_ptr<Everest::SystemTimer> change_time_offset_timer;
    std::chrono::time_point<date::utc_clock> clock_aligned_meter_values_time_point;
    std::mutex meter_values_mutex;
    std::mutex measurement_mutex;
    std::map<int32_t, AvailabilityChange> change_availability_queue; // TODO: move to Connectors
    std::mutex change_availability_mutex;                            // TODO: move to Connectors
    std::unique_ptr<TransactionHandler> transaction_handler;
    std::vector<v16::MessageType> external_notify;

    std::map<std::string,
             std::map<std::string, std::function<DataTransferResponse(const std::optional<std::string>& msg)>>>
        data_transfer_callbacks;
    std::function<DataTransferResponse(const DataTransferRequest& request)> data_transfer_callback;
    std::map<std::string, std::function<void(Call<DataTransferRequest> call)>> data_transfer_pnc_callbacks;
    std::mutex data_transfer_callbacks_mutex;
    std::map<CiString<50>, std::function<void(const KeyValue& key_value)>> configuration_key_changed_callbacks;
    std::function<void(const KeyValue& key_value)> generic_configuration_key_changed_callback;

    std::mutex stop_transaction_mutex;
    std::condition_variable stop_transaction_cv;

    std::mutex user_price_map_mutex;
    std::map<std::string, std::condition_variable> user_price_cvs;
    std::map<std::string, TariffMessage> tariff_messages_by_id_token;

    std::thread reset_thread;

    int log_status_request_id;

    FirmwareStatusEnumType signed_firmware_status;
    int signed_firmware_status_request_id;

    /// \brief optional delay to resumption of message queue after reconnecting to the CSMS
    std::chrono::seconds message_queue_resume_delay = std::chrono::seconds(0);

    // callbacks
    std::function<bool(int32_t connector)> enable_evse_callback;
    std::function<bool(int32_t connector)> disable_evse_callback;
    std::function<bool(int32_t connector)> pause_charging_callback;
    std::function<bool(int32_t connector)> resume_charging_callback;
    std::function<void(const std::string& id_token, std::vector<int32_t> referenced_connectors, bool prevalidated)>
        provide_token_callback;
    std::function<bool(int32_t connector, Reason reason)> stop_transaction_callback;
    std::function<UnlockStatus(int32_t connector)> unlock_connector_callback;
    std::function<bool(int32_t connector, int32_t max_current)> set_max_current_callback;
    std::function<bool(const ResetType& reset_type)> is_reset_allowed_callback;
    std::function<void(const ResetType& reset_type)> reset_callback;
    std::function<void(const std::string& system_time)> set_system_time_callback;
    std::function<void(const BootNotificationResponse& boot_notification_response)> boot_notification_response_callback;
    std::function<void()> signal_set_charging_profiles_callback;
    std::function<void(bool is_connected)> connection_state_changed_callback;

    std::function<GetLogResponse(const GetDiagnosticsRequest& request)> upload_diagnostics_callback;
    std::function<void(const UpdateFirmwareRequest msg)> update_firmware_callback;

    std::function<UpdateFirmwareStatusEnumType(const SignedUpdateFirmwareRequest msg)> signed_update_firmware_callback;
    std::function<void(const std::string& type, const std::string& tech_info)> security_event_callback;

    std::function<void()> all_connectors_unavailable_callback;

    std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp::DateTime expiryDate,
                                    CiString<20> idTag, std::optional<CiString<20>> parent_id)>
        reserve_now_callback;
    std::function<bool(int32_t reservation_id)> cancel_reservation_callback;
    std::function<void()> switch_security_profile_callback;
    std::function<GetLogResponse(GetLogRequest msg)> upload_logs_callback;
    std::function<void(int32_t connection_timeout)> set_connection_timeout_callback;

    std::function<void(const int32_t connector, const std::string& session_id)> transaction_started_callback;
    std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id,
                       const IdTagInfo& id_tag_info)>
        transaction_updated_callback;
    std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id)>
        transaction_stopped_callback;
    std::function<ocpp::ReservationCheckStatus(const int32_t connector, const std::string& id_token)>
        is_token_reserved_for_connector_callback;

    // iso15118 callback
    std::function<void(const int32_t connector, const ocpp::v2::Get15118EVCertificateResponse& certificate_response,
                       const ocpp::v2::CertificateActionEnum& certificate_action)>
        get_15118_ev_certificate_response_callback;

    // tariff and cost callback
    std::function<DataTransferResponse(const RunningCost& running_cost, const uint32_t number_of_decimals)>
        session_cost_callback;
    std::function<DataTransferResponse(const std::vector<DisplayMessage>& display_message)>
        set_display_message_callback;
    std::function<DataTransferResponse(const TariffMessage& message)> tariff_message_callback;

    /// \brief This function is called after a successful connection to the Websocket
    void connected_callback();
    void init_websocket();
    void init_state_machine(const std::map<int, ChargePointStatus>& connector_status_map);
    WebsocketConnectionOptions get_ws_connection_options();
    std::unique_ptr<ocpp::MessageQueue<v16::MessageType>> create_message_queue();
    void message_callback(const std::string& message);
    void handle_message(const EnhancedMessage<v16::MessageType>& message);
    void heartbeat(bool initiated_by_trigger_message = false);
    void boot_notification(bool initiated_by_trigger_message = false);
    void clock_aligned_meter_values_sample();
    void update_heartbeat_interval();
    void update_meter_values_sample_interval();
    void update_clock_aligned_meter_values_interval();
    std::optional<MeterValue> get_latest_meter_value(int32_t connector,
                                                     std::vector<MeasurandWithPhase> values_of_interest,
                                                     ReadingContext context);
    MeterValue get_signed_meter_value(const std::string& signed_value, const ReadingContext& context,
                                      const ocpp::DateTime& datetime);
    void send_meter_value(int32_t connector, MeterValue meter_value, bool initiated_by_trigger_message = false);
    void send_meter_value_on_pricing_trigger(const int32_t connector_number, std::shared_ptr<Connector> connector,
                                             const Measurement& measurement);
    void reset_pricing_triggers(const int32_t connector_number);
    void status_notification(const int32_t connector, const ChargePointErrorCode errorCode,
                             const ChargePointStatus status, const ocpp::DateTime& timestamp,
                             const std::optional<CiString<50>>& info = std::nullopt,
                             const std::optional<CiString<255>>& vendor_id = std::nullopt,
                             const std::optional<CiString<50>>& vendor_error_code = std::nullopt,
                             bool initiated_by_trigger_message = false);
    void diagnostic_status_notification(DiagnosticsStatus status, bool initiated_by_trigger_message = false);
    void firmware_status_notification(FirmwareStatus status, bool initiated_by_trigger_message = false);
    void log_status_notification(UploadLogStatusEnumType status, int requestId,
                                 bool initiated_by_trigger_message = false);
    void signed_firmware_update_status_notification(FirmwareStatusEnumType status, int requestId,
                                                    bool initiated_by_trigger_message = false);

    /// \brief Changes all unoccupied connectors to unavailable. If a transaction is running schedule an availabilty
    /// change. If all connectors are unavailable signal to the firmware updater that installation of the firmware
    /// update can proceed
    void change_all_connectors_to_unavailable_for_firmware_update();

    /// \brief Tries to resume the transactions given by \p resuming_session_ids . This function retrieves open
    /// transactions from the internal database (e.g. because of power loss). In case the \p
    /// resuming_session_ids contain the internal session_id, this function attempts to resume the transaction by
    /// initializing it and adding it to the \ref transaction_handler. If the session_id is not part of \p
    /// resuming_session_ids a StopTransaction.req is initiated to properly close the transaction.
    void try_resume_transactions(const std::set<std::string>& resuming_session_ids);
    void stop_all_transactions();
    void stop_all_transactions(Reason reason);
    bool validate_against_cache_entries(CiString<20> id_tag);

    // new transaction handling:
    void start_transaction(std::shared_ptr<Transaction> transaction);

    void stop_transaction(int32_t connector, Reason reason, std::optional<CiString<20>> id_tag_end);

    /// \brief Converts the given \p measurands_csv to a vector of Measurands
    /// \param measurands_csv
    /// \return
    std::vector<Measurand> get_measurands_vec(const std::string& measurands_csv);

    /// \brief Returns transaction data that can be used to set the transactionData field in StopTransaction.req.
    /// Filters the meter values of the transaction according to the values set within StopTxnAlignedData and
    /// StopTxnSampledData
    std::vector<TransactionData> get_filtered_transaction_data(const std::shared_ptr<Transaction>& transaction);

    /// \brief Load charging profiles if present in database
    void load_charging_profiles();

    // security
    /// \brief Creates a new public/private key pair and sends a certificate signing request to the central system for
    /// the given \p certificate_signing_use
    void sign_certificate(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                          bool initiated_by_trigger_message = false);

    /// \brief Checks if OCSP cache needs to be updated and executes update if necessary by using
    /// DataTransfer(GetCertificateStatus.req)
    void update_ocsp_cache();

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
    void handleStopTransactionResponse(const EnhancedMessage<v16::MessageType>& message);
    void handleUnlockConnectorRequest(Call<UnlockConnectorRequest> call);
    void handleHeartbeatResponse(CallResult<HeartbeatResponse> call_result);

    // smart charging profile
    void handleSetChargingProfileRequest(Call<SetChargingProfileRequest> call);
    void handleGetCompositeScheduleRequest(Call<GetCompositeScheduleRequest> call);
    void handleClearChargingProfileRequest(Call<ClearChargingProfileRequest> call);

    // plug&charge for 1.6 whitepaper
    bool is_pnc_enabled();
    void data_transfer_pnc_sign_certificate();
    void data_transfer_pnc_get_certificate_status(const ocpp::v2::OCSPRequestData& ocsp_request_data);

    void handle_data_transfer_pnc_trigger_message(Call<DataTransferRequest> call);
    void handle_data_transfer_pnc_certificate_signed(Call<DataTransferRequest> call);
    void handle_data_transfer_pnc_get_installed_certificates(Call<DataTransferRequest> call);
    void handle_data_transfer_delete_certificate(Call<DataTransferRequest> call);
    void handle_data_transfer_install_certificate(Call<DataTransferRequest> call);

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
    void securityEventNotification(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                   const bool triggered_internally, const std::optional<bool>& critical = std::nullopt,
                                   const std::optional<DateTime>& timestamp = std::nullopt);
    void switchSecurityProfile(int32_t new_security_profile, int32_t max_connection_attempts);
    // Local Authorization List profile
    void handleSendLocalListRequest(Call<SendLocalListRequest> call);
    void handleGetLocalListVersionRequest(Call<GetLocalListVersionRequest> call);

    // California Pricing
    DataTransferResponse handle_set_user_price(const std::optional<std::string>& msg);
    DataTransferResponse handle_set_session_cost(const RunningCostState& type,
                                                 const std::optional<std::string>& message);
    ///
    /// \brief Set timer to trigger sending a metervalue at a specific time.
    /// \param date_time    The date/time to send the metervalue.
    /// \param connector    The connector to set the timer for.
    ///
    void set_connector_trigger_metervalue_timer(const DateTime& date_time, std::shared_ptr<Connector> connector);

    ///
    /// \brief Set offset timer to change the 'timezone' (time offset) at the given time.
    /// \param date_time    The date / time to change the time offset.
    ///
    void set_time_offset_timer(const std::string& date_time);

    /// \brief Preprocess a ChangeAvailabilityRequest: Determine response;
    /// - if connector is 0, availability change is also propagated for all connectors
    /// - for each connector (except "0"), if transaction is ongoing the change is scheduled,
    ///    otherwise the OCPP connector id is appended to the `accepted_connector_availability_changes` vector
    void preprocess_change_availability_request(const ChangeAvailabilityRequest& request,
                                                ChangeAvailabilityResponse& response,
                                                std::vector<int32_t>& accepted_connector_availability_changes);

    /// \brief Executes availability change for the provided connectors:
    /// - if persist == true: store availability in database
    /// - submit state event (for the whole ChargePoint if "0" in set of connectors; otherwise for each connector
    /// individually)
    /// - call according EVSE enable or disable callback, respectively
    /// \param changed_connectors list of OCPP connector ids (and 0 for whole chargepoint)
    /// \param availability new availabillity
    /// \param persist if true, persists availability in database
    void execute_connectors_availability_change(const std::vector<int32_t>& changed_connectors,
                                                const ocpp::v16::AvailabilityType availability, bool persist);

    /// \brief Checks scheduled availability queue and exeuctues availability change if required
    /// \param connector for which availability change shall be checked and executed
    void execute_queued_availability_change(const int32_t connector);

public:
    /// \brief The main entrypoint for libOCPP for OCPP 1.6
    /// \param config a nlohmann json config object that contains the libocpp 1.6 config. There are example configs that
    /// work with a SteVe installation running in Docker, for example: config/v16/config-docker.json
    /// \param share_path This path contains the following files and directories and is installed by the libocpp install
    /// target
    /// \param user_config_path this points to a "user config", which we call a configuration file that's merged with
    /// the config that's provided in the "config" parameter. Here you can add, remove and overwrite settings without
    /// modifying the config passed in the first parameter directly. This is also used by libocpp to persistently modify
    /// config entries that are changed by the CSMS that should persist across restarts
    /// \param database_path this points to the location of the sqlite database that libocpp uses to keep track of
    /// connector availability, the authorization cache and auth list, charging profiles and transaction data
    /// \param sql_init_path this points to the init.sql file which contains the database schema used by libocpp for its
    /// sqlite database
    /// \param message_log_path this points to the directory in which libocpp can put OCPP communication logfiles for
    /// debugging purposes. This behavior can be controlled by the "LogMessages" (set to true by default) and
    /// "LogMessagesFormat" (set to ["log", "html", "session_logging"] by default, "console" and "console_detailed" are
    /// also available) configuration keys in the "Internal" section of the config file. Please note that this is
    /// intended for debugging purposes only as it logs all communication, including authentication messages.
    /// \param evse_security Pointer to evse_security that manages security related operations
    explicit ChargePointImpl(const std::string& config, const fs::path& share_path, const fs::path& user_config_path,
                             const fs::path& database_path, const fs::path& sql_init_path,
                             const fs::path& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                             const std::optional<SecurityConfiguration> security_configuration);

    ~ChargePointImpl() {
    }

    /// \brief Initializes the ChargePoint and all of it's connectors, the state machine and message queue. This method
    /// should be called if a more granular start of the process is necessary. Notably if it is necessary for the state
    /// machine and the connectors (and their statuses) need to be updated prior to initiating connection with the CSMS.
    /// \param connector_status_map initial state of connectors including connector 0 with reduced set of states
    /// (Available, Unavailable, Faulted)
    /// \param resuming_session_ids can optionally contain active session ids from previous executions. If empty and
    /// libocpp has transactions in its internal database that have not been stopped yet, calling this function will
    /// initiate a StopTransaction.req for those transactions. If this vector contains session_ids this function will
    /// not stop transactions with this session_id even in case it has an internal database entry for this session and
    /// it hasnt been stopped yet. Its ignored if this vector contains session_ids that are unknown to libocpp.
    ///  \return
    bool init(const std::map<int, ChargePointStatus>& connector_status_map,
              const std::set<std::string>& resuming_session_ids);

    /// \brief Starts the ChargePoint, initializes (if and only if init was not called previously) and connects to the
    /// Websocket endpoint and initializes a BootNotification.req
    /// \param connector_status_map initial state of connectors including connector 0 with reduced set of states
    /// (Available, Unavailable, Faulted)
    /// \param bootreason reason for calling the start function
    /// \param resuming_session_ids can optionally contain active session ids from previous executions. If empty and
    /// libocpp has transactions in its internal database that have not been stopped yet, calling this function will
    /// initiate a StopTransaction.req for those transactions. If this vector contains session_ids this function will
    /// not stop transactions with this session_id even in case it has an internal database entry for this session and
    /// it hasnt been stopped yet. Its ignored if this vector contains session_ids that are unknown to libocpp.
    ///  \return
    bool start(const std::map<int, ChargePointStatus>& connector_status_map, BootReasonEnum bootreason,
               const std::set<std::string>& resuming_session_ids);

    /// \brief Restarts the ChargePoint if it has been stopped before. The ChargePoint is reinitialized, connects to the
    /// websocket and starts to communicate OCPP messages again
    /// \param connector_status_map initial state of connectors including connector 0 with reduced set of states
    /// (Available, Unavailable, Faulted). connector_status_map is empty, last availability states from the persistant
    /// storage will be used
    /// \param bootreason reason for calling the restart function
    bool restart(const std::map<int, ChargePointStatus>& connector_status_map, BootReasonEnum bootreason);

    /// \brief Resets the internal state machine for the connectors using the given \p connector_status_map
    /// \param connector_status_map state of connectors including connector 0 with reduced set of states (Available,
    /// Unavailable, Faulted)
    void reset_state_machine(const std::map<int, ChargePointStatus>& connector_status_map);

    /// \brief Stops the ChargePoint, stops timers, transactions and the message queue and disconnects from the
    /// websocket
    bool stop();

    /// \brief Initializes the websocket and connects to CSMS if it is not yet connected
    void connect_websocket();

    /// \brief Disconnects the the websocket connection to the CSMS if it is connected
    void disconnect_websocket();

    /// \brief Calls the set_connection_timeout_callback that can be registered. This function is used to notify an
    /// Authorization mechanism about a changed ConnectionTimeout configuration key.
    void call_set_connection_timeout();

    // public API for Core profile

    /// \brief Authorizes the provided \p id_token against the central system, LocalAuthorizationList or
    /// AuthorizationCache depending on the values of the ConfigurationKeys LocalPreAuthorize, LocalAuthorizeOffline,
    /// LocalAuthListEnabled and AuthorizationCacheEnabled. If \p authorize_only_locally is true, no Authorize.req will
    /// be sent to the CSMS but only LocalAuthorizationList and LocalAuthorizationCache will be used for the validation
    /// \returns the EnhancedIdTagInfo that contains the result of the authorization and an optional tarriff message
    EnhancedIdTagInfo authorize_id_token(CiString<20> id_token, const bool authorize_only_locally = false);

    // for plug&charge 1.6 whitepaper

    /// \brief Uses data_transfer mechanism to authorize given \p emaid , \p certificate and
    /// \p iso15118_certificate_hash_data locally or by requesting authorzation at CSMS. This function can be called
    /// when the authorization mechanism Plug&Charge is specified as part of the ISO15118 PaymentDetailsRequest
    /// \param emaid
    /// \param certificate contract certificate that the EVCC provides
    /// \param iso15118_certificate_hash_data
    /// \return
    ocpp::v2::AuthorizeResponse data_transfer_pnc_authorize(
        const std::string& emaid, const std::optional<std::string>& certificate,
        const std::optional<std::vector<ocpp::v2::OCSPRequestData>>& iso15118_certificate_hash_data);

    /// \brief  Uses data transfer mechanism to get 15118 ev certificate from CSMS. This function can be called when the
    /// EVCC requests the update or installation of a contract certificate as part of the ISO15118
    /// CertificateInstallRequest or CertificateUpdateRequest
    /// \param connector_id
    /// \param exi_request provided by the EVCC
    /// \param iso15118_schema_version provided by the EVCC
    /// \param certificate_action Install or Update
    void data_transfer_pnc_get_15118_ev_certificate(const int32_t connector_id, const std::string& exi_request,
                                                    const std::string& iso15118_schema_version,
                                                    const ocpp::v2::CertificateActionEnum& certificate_action);

    /// \brief Allows the exchange of arbitrary \p data identified by a \p vendorId and \p messageId with a central
    /// system \returns the DataTransferResponse
    /// \param vendorId
    /// \param messageId
    /// \param data
    /// \return the DataTransferResponse from the CSMS. In case no response is received from the CSMS because the
    /// message timed out or the charging station is offline, std::nullopt is returned
    std::optional<DataTransferResponse> data_transfer(const CiString<255>& vendorId,
                                                      const std::optional<CiString<50>>& messageId,
                                                      const std::optional<std::string>& data);

    /// \brief Calculates ChargingProfiles configured by the CSMS of all connectors from now until now + given \p
    /// duration_s and  the given \p unit
    /// \param duration_s
    /// \param unit defaults to A
    /// \return ChargingSchedules of all connectors
    std::map<int32_t, ChargingSchedule>
    get_all_composite_charging_schedules(const int32_t duration_s, const ChargingRateUnit unit = ChargingRateUnit::A);

    /// \brief Calculates EnhancedChargingSchedule(s) configured by the CSMS of all connectors from now until now +
    /// given \p duration_s and the given \p unit . EnhancedChargingSchedules contain EnhancedChargingSchedulePeriod(s)
    /// that are enhanced by the stackLevel that was provided for the ChargingProfile
    /// \param duration_s
    /// \param unit
    /// defaults to A \return ChargingSchedules of all connectors
    std::map<int32_t, EnhancedChargingSchedule>
    get_all_enhanced_composite_charging_schedules(const int32_t duration_s,
                                                  const ChargingRateUnit unit = ChargingRateUnit::A);

    /// \brief Stores the given \p powermeter values for the given \p connector . This function can be called when a new
    /// meter value is present.
    /// \param connector
    /// \param measurement structure that can contain all kinds of measurands
    void on_meter_values(int32_t connector, const Measurement& measurement);

    /// \brief Stores the given \p max_current for the given \p connector offered to the EV. This function can be called
    /// when the value for the maximum current for the connector changes. It will be used to report the Measurand
    /// Current_Offered if it is configured
    /// \param connector
    /// \param max_current in Amps
    void on_max_current_offered(int32_t connector, int32_t max_current);

    /// \brief Stores the given \p max_power for the given \p connector offered to the EV. This function can be called
    /// when the value for the maximum power for the connector changes. It will be used to report the Measurand
    /// Power_Offered if it is configured
    /// \param connector
    /// \param max_power in Watts
    void on_max_power_offered(int32_t connector, int32_t max_power);

    /// \brief Notifies chargepoint that a new session with the given \p session_id has been started at the given \p
    /// connector with the given \p reason . The logs of the session will be written into \p session_logging_path if
    /// present. This function must be called when first interaction with user or EV occurs. This can be a valid
    /// authorization or the connection of cable and/or EV to the given \p connector
    /// \param connector
    /// \param session_id unique id of the session
    /// \param reason for the initiation of the session
    /// \param session_logging_path optional filesystem path to where the session log should be written
    void on_session_started(int32_t connector, const std::string& session_id, const SessionStartedReason reason,
                            const std::optional<std::string>& session_logging_path);

    /// \brief Notifies chargepoint that a session has been stopped at the given \p connector. This function must be
    /// called when the EV disconnects from the given \p connector .
    /// \param connector
    /// \param session_id
    void on_session_stopped(int32_t connector, const std::string& session_id);

    /// \brief Notifies chargepoint that a transaction at the given \p connector with the given parameters has been
    /// started. This function must be called at the point that all conditions for charging are met, for instance, EV is
    /// connected to Charge Point and user has been authorized.
    /// \param connector
    /// \param session_id
    /// \param id_token that has been used to authorize the transaction
    /// \param meter_start start meter value in Wh
    /// \param reservation_id
    /// \param timestamp of the start of transaction
    /// \param signed_meter_value e.g. in OCMF format
    void on_transaction_started(const int32_t& connector, const std::string& session_id, const std::string& id_token,
                                const double meter_start, std::optional<int32_t> reservation_id,
                                const ocpp::DateTime& timestamp, std::optional<std::string> signed_meter_value);

    /// \brief Notifies chargepoint that the transaction on the given \p connector with the given \p reason has been
    /// stopped. This function must be called at the point where one of the preconditions for charging irrevocably
    /// becomes false, for instance when a user swipes to stop the transaction and the stop is authorized or if the EV
    /// disconnects.
    /// \param connector
    /// \param session_id
    /// \param reason
    /// \param timestamp of the end of transaction
    /// \param energy_wh_import stop meter value in Wh
    /// \param id_tag_end
    /// \param signed_meter_value e.g. in OCMF format
    void on_transaction_stopped(const int32_t connector, const std::string& session_id, const Reason& reason,
                                ocpp::DateTime timestamp, float energy_wh_import,
                                std::optional<CiString<20>> id_tag_end, std::optional<std::string> signed_meter_value);

    /// \brief This function should be called when EV indicates that it suspends charging on the given \p connector
    /// \param connector
    /// \param info
    void on_suspend_charging_ev(int32_t connector, const std::optional<CiString<50>> info = std::nullopt);

    /// \brief This function should be called when EVSE indicates that it suspends charging on the given \p connector
    /// \param connector
    /// \param info
    void on_suspend_charging_evse(int32_t connector, const std::optional<CiString<50>> info = std::nullopt);

    /// \brief This function should be called when charging resumes on the given \p connector
    /// \param connector
    void on_resume_charging(int32_t connector);

    /// \brief This function should be called if an error with the given \p error_info is present. This function will
    /// trigger a StatusNotification.req containing the given \p error_info . It will change the present state of
    /// the state machine to faulted, in case the corresponding flag is set in the given \p error_info. This function
    /// can be called multiple times for different errors. Errors reported using this function stay active as long as
    /// they are cleared by \ref on_error_cleared().
    /// \param connector
    /// \param error_info Additional information related to the error
    void on_error(int32_t connector, const ErrorInfo& error_info);

    /// \brief This function should be called if an error with the given \p uuid has been cleared. If this leads to the
    /// fact that no other error is active anymore, this function will initiate a StatusNotification.req that reports
    /// the current state and no error
    ///  \param connector
    /// \param uuid of a previously reported error. If uuid is not
    /// known, the event will be ignored
    void on_error_cleared(int32_t connector, const std::string uuid);

    /// \brief Clears all previously reported errors at the same time for the given \p connector . This will
    /// clear a previously reported "Faulted" state if present
    ///  \param connector
    void on_all_errors_cleared(int32_t connector);

    /// \brief Chargepoint notifies about new log status \p log_status . This function should be called during a
    /// Diagnostics / Log upload to indicate the current \p log_status .
    /// \param request_id  A \p request_id of -1 indicates a DiagnosticsStatusNotification.req, else a
    /// LogStatusNotification.req.
    /// \param log_status The \p log_status should be either be convertable to the
    /// ocpp::v16::UploadLogStatusEnumType enum or ocpp::v16::DiagnosticsStatus enum depending on the previous request,
    /// which could have been a DiagnosticsUpload.req or a GetLog.req (Security Whitepaper)
    void on_log_status_notification(int32_t request_id, std::string log_status);

    /// \brief Chargepoint notifies about new firmware update status \p firmware_update_status . This function should be
    /// called during a Firmware Update to indicate the current \p firmware_update_status .
    /// \param request_id A \p request_id of -1 indicates a FirmwareStatusNotification.req, else a
    /// SignedFirmwareUpdateStatusNotification.req .
    /// \param firmware_update_status The \p firmware_update_status
    void on_firmware_update_status_notification(int32_t request_id,
                                                const ocpp::FirmwareStatusNotification firmware_update_status);

    /// \brief This function must be called when a reservation is started at the given \p connector .
    /// \param connector
    void on_reservation_start(int32_t connector);

    /// \brief This function must be called when a reservation ends at the given \p connector
    /// \param connector
    void on_reservation_end(int32_t connector);

    /// \brief Notifies chargepoint that the \p connector is enabled . This function should be called when the \p
    /// connector becomes functional and operational
    /// \param connector
    void on_enabled(int32_t connector);

    /// \brief Notifies chargepoint that the \p connector is disabled . This function should be called when the \p
    /// connector becomes inoperative
    /// \param connector
    void on_disabled(int32_t connector);

    /// \brief Notifies chargepoint that a ConnectionTimeout occured for the given \p connector . This function should
    /// be called when an EV is plugged in but the authorization is not present within the specified ConnectionTimeout
    void on_plugin_timeout(int32_t connector);

    /// \brief Notifies chargepoint that a SecurityEvent occurs. This will send a SecurityEventNotification.req to the
    /// CSMS
    /// \param event_type type of the security event
    /// \param tech_info additional info of the security event
    /// \param critical if set this overwrites the default criticality recommended in the OCPP 1.6 security whitepaper.
    /// A critical security event is transmitted as a message to the CSMS, a non-critical one is just written to the
    /// security log
    /// \param timestamp when this security event occured, if absent the current datetime is assumed
    void on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                           const std::optional<bool>& critical = std::nullopt,
                           const std::optional<DateTime>& timestamp = std::nullopt);

    /// \brief Handles an internal ChangeAvailabilityRequest (in the same way as if it was emitted by the CSMS).
    /// \param request
    ChangeAvailabilityResponse on_change_availability(const ChangeAvailabilityRequest& request);

    /// registers a \p callback function that can be used to receive a arbitrary data transfer for the given \p
    /// vendorId and \p messageId
    /// \param vendorId
    /// \param messageId
    /// \param callback
    void register_data_transfer_callback(
        const CiString<255>& vendorId, const CiString<50>& messageId,
        const std::function<DataTransferResponse(const std::optional<std::string>& msg)>& callback);

    /// registers a \p callback function that can be used to handle arbitrary data transfers for all vendorId an
    /// messageId
    /// \param callback
    void register_data_transfer_callback(
        const std::function<DataTransferResponse(const DataTransferRequest& request)>& callback);

    /// \brief registers a \p callback function that can be used to enable the evse. The enable_evse_callback is called
    /// when a ChangeAvailaibility.req is received.
    /// \param callback
    void register_enable_evse_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to disable the evse. The disable_evse_callback is
    /// called when a ChangeAvailaibility.req is received.
    /// \param callback
    void register_disable_evse_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to pause charging. The pause_charging_callback is
    /// called when the idTagInfo.status of a StartTransaction.conf is not Accepted
    /// \param callback
    void register_pause_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to resume charging
    /// \param callback
    void register_resume_charging_callback(const std::function<bool(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to provide an \p id_token for the given \p
    /// referenced_connectors to an authorization handler. \p prevalidated signals to the authorization handler that no
    /// further authorization is necessary. The provide_token_callback is called when a RemoteStartTransaction.req is
    /// received.
    /// \param callback
    void register_provide_token_callback(
        const std::function<void(const std::string& id_token, std::vector<int32_t> referenced_connectors,
                                 bool prevalidated)>& callback);

    /// \brief registers a \p callback function that can be used to stop a transaction. Ths stop_transaction_callback is
    /// called
    /// - when the idTagInfo.status of a StartTransaction.conf is not Accepted
    /// - when a RemoteStopTransaction.req is received
    /// - when a UnlockConnector.req is received
    /// \param callback
    void register_stop_transaction_callback(const std::function<bool(int32_t connector, Reason reason)>& callback);

    /// \brief registers a \p callback function that can be used to reserve a connector for a idTag until a timeout
    /// is reached. The reserve_now_callback is called when a ReserveNow.req is received.
    /// \param callback
    void register_reserve_now_callback(
        const std::function<ReservationStatus(int32_t reservation_id, int32_t connector, ocpp::DateTime expiryDate,
                                              CiString<20> idTag, std::optional<CiString<20>> parent_id)>& callback);

    /// \brief registers a \p callback function that can be used to cancel a reservation on a connector. Callback
    /// function should return false if the reservation could not be cancelled, else true . The
    /// cancel_reservation_callback is called when a CancelReservation.req is received
    /// \param callback
    void register_cancel_reservation_callback(const std::function<bool(int32_t reservation_id)>& callback);

    /// \brief registers a \p callback function that can be used to unlock the connector. In case a transaction is
    // active at the specified connector, the \p callback shall stop the transaction before unlocking the connector. The
    // unlock_connector_callback is called:
    /// - when receiving a UnlockConnector.req and
    /// - when a transaction has on_transaction_stopped is called and the configuration key
    /// UnlockConnectorOnEVSideDisconnect is true
    /// \param callback
    void register_unlock_connector_callback(const std::function<UnlockStatus(int32_t connector)>& callback);

    /// \brief registers a \p callback function that can be used to trigger an upload of diagnostics. This callback
    /// should trigger a process of a diagnostics upload using the given parameters of the request. This process should
    /// call the on_log_status_notification handler in order to update the status of the file upload. The
    /// upload_diagnostics_callback is called when a GetDiagnostics.req is received
    /// \param callback
    void register_upload_diagnostics_callback(
        const std::function<GetLogResponse(const GetDiagnosticsRequest& request)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a firmware update. This callback
    /// should trigger a process of a firmware update using the given parameters of the request. This process should
    /// call the on_firmware_update_status_notification handler in order to update the status of the update. The
    /// update_firmware_callback is called when a FirmwareUpdate.req is received
    /// \param callback
    void register_update_firmware_callback(const std::function<void(const UpdateFirmwareRequest msg)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a signed firmware update. This callback
    /// should trigger a process of a signed firmware update using the given parameters of the request. This process
    /// should call the on_firmware_update_status_notification handler in order to update the status of the signed
    /// firmware update. The signed_update_firmware_callback is called when a SignedUpdateFirmware.req is received.
    /// \param callback
    void register_signed_update_firmware_callback(
        const std::function<UpdateFirmwareStatusEnumType(const SignedUpdateFirmwareRequest msg)>& callback);

    /// \brief registers a \p callback function that is called when all connectors are set to unavailable.
    /// This can be used to then trigger the installation of the firmware update
    void register_all_connectors_unavailable_callback(const std::function<void()>& callback);

    /// \brief registers a \p callback function that can be used to upload logfiles. This callback
    /// should trigger a process of a log upload using the given parameters of the request. This process should
    /// call the on_log_status_notification handler in order to update the status of the file upload. The
    /// upload_logs_callback is called when a GetLog.req is received
    /// \param callback
    void register_upload_logs_callback(const std::function<GetLogResponse(GetLogRequest req)>& callback);

    /// \brief registers a \p callback function that can be used to set the authorization or plug in connection timeout.
    /// The set_connection_timeout_callback is called when the configuration key ConnectionTimeout has been changed by
    /// the CSMS.
    /// \param callback
    void register_set_connection_timeout_callback(const std::function<void(int32_t connection_timeout)>& callback);

    /// \brief registers a \p callback function that can be used to check if a reset is allowed . The
    /// is_reset_allowed_callback is called when a Reset.req is received.
    /// \param callback
    void register_is_reset_allowed_callback(const std::function<bool(const ResetType& reset_type)>& callback);

    /// \brief registers a \p callback function that can be used to trigger a reset of the chargepoint. The
    /// reset_callback is called when a Reset.req is received and a previous execution of the is_reset_allowed_callback
    /// returned true
    /// \param callback
    void register_reset_callback(const std::function<void(const ResetType& reset_type)>& callback);

    /// \brief registers a \p callback function that can be used to set the system time.
    /// \param callback
    void register_set_system_time_callback(const std::function<void(const std::string& system_time)>& callback);

    /// \brief registers a \p callback function that can be used receive the BootNotificationResponse
    /// \param callback
    void register_boot_notification_response_callback(
        const std::function<void(const BootNotificationResponse& boot_notification_response)>& callback);

    /// \brief registers a \p callback function that can be used to signal that the chargepoint received a
    /// SetChargingProfile.req . The set_charging_profiles_callback is called when a SetChargingProfile.req is received
    /// and was accepted. The registered callback could make use of the get_all_composite_charging_schedules in order to
    /// retrieve the ChargingProfiles that have been set by the CSMS.
    /// \param callback
    void register_signal_set_charging_profiles_callback(const std::function<void()>& callback);

    /// \brief registers a \p callback function that can be used when the connection state to CSMS changes. The
    /// connection_state_changed_callback is called when chargepoint has connected to or disconnected from the CSMS.
    /// \param callback
    void register_connection_state_changed_callback(const std::function<void(bool is_connected)>& callback);

    /// \brief registers a \p callback function that can be used to publish the response to a Get15118Certificate.req
    /// wrapped in a DataTransfer.req . The get_15118_ev_certificate_response_callback is called after the response to a
    /// DataTransfer.req(Get15118EVCertificate) has been accepted.
    /// \param callback
    void register_get_15118_ev_certificate_response_callback(
        const std::function<void(const int32_t connector,
                                 const ocpp::v2::Get15118EVCertificateResponse& certificate_response,
                                 const ocpp::v2::CertificateActionEnum& certificate_action)>& callback);

    /// \brief registers a \p callback function that is called when a StartTransaction.req message is sent by the
    /// chargepoint
    /// \param callback
    void register_transaction_started_callback(
        const std::function<void(const int32_t connector, const std::string& session_id)>& callback);

    /// \brief registers a \p callback function that is called when a StopTransaction.req message is sent by the
    /// chargepoint
    /// \param callback
    void register_transaction_stopped_callback(
        const std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id)>&
            callback);

    /// \brief registers a \p callback function that is called when a StartTransaction.conf message is received by the
    /// CSMS. This includes the transactionId.
    /// \param callback
    void register_transaction_updated_callback(
        const std::function<void(const int32_t connector, const std::string& session_id, const int32_t transaction_id,
                                 const IdTagInfo& id_tag_info)>
            callback);

    /// \brief registers a \p callback function that can be used to react on changed configuration keys. This
    /// callback is called when a configuration key has been changed by the CSMS
    /// \param key the configuration key for which the callback is registered
    /// \param callback executed when this configuration key changed
    void register_configuration_key_changed_callback(const CiString<50>& key,
                                                     const std::function<void(const KeyValue& key_value)>& callback);

    /// \brief registers a \p callback function that can be used to react on changed configuration key. This
    /// callback is called when a configuration key and value has been changed by the CSMS, where no key based callback
    /// is assigned
    /// \param callback executed when this configuration key changed
    void
    register_generic_configuration_key_changed_callback(const std::function<void(const KeyValue& key_value)>& callback);

    /// \brief registers a \p callback function that can be used to react to a security event callback. This callback is
    /// called only if the SecurityEvent occured internally within libocpp
    void register_security_event_callback(
        const std::function<void(const std::string& type, const std::string& tech_info)>& callback);

    /// \brief registers a \p callback function that can be used to check, if the \p connector is reserved for the given
    /// \p id_token. The is_token_reserved_for_connector_callback is called when a RemoteStartTransaction.req is
    /// received.
    /// \param callback
    void register_is_token_reserved_for_connector_callback(
        const std::function<ReservationCheckStatus(const int32_t connector, const std::string& id_token)>& callback);

    void register_session_cost_callback(
        const std::function<DataTransferResponse(const RunningCost& running_cost, const uint32_t number_of_decimals)>&
            session_cost_callback);
    void register_tariff_message_callback(
        const std::function<DataTransferResponse(const TariffMessage& message)>& tariff_message_callback);
    void register_set_display_message_callback(
        const std::function<DataTransferResponse(const std::vector<DisplayMessage>&)> set_display_message_callback);

    /// \brief Gets the configured configuration key requested in the given \p request
    /// \param request specifies the keys that should be returned. If empty or not set, all keys will be reported
    /// \return a response containing the requested key(s) including the values and unkown keys if present
    GetConfigurationResponse get_configuration_key(const GetConfigurationRequest& request);

    /// \brief Sets a custom configuration key
    /// \param key
    /// \param value
    /// \return Indicates the result of the operation
    ConfigurationStatus set_custom_configuration_key(CiString<50> key, CiString<500> value);

    /// \brief Delay draining the message queue after reconnecting, so the CSMS can perform post-reconnect checks first
    /// \param delay The delay period (seconds)
    void set_message_queue_resume_delay(std::chrono::seconds delay) {
        this->message_queue_resume_delay = delay;
    }
};

} // namespace v16
} // namespace ocpp
#endif
