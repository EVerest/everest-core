// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_CHARGE_POINT_HPP
#define OCPP_V16_CHARGE_POINT_HPP

#include <ocpp/common/cistring.hpp>
#include <ocpp/common/evse_security.hpp>
#include <ocpp/common/evse_security_impl.hpp>
#include <ocpp/common/support_older_cpp_versions.hpp>
#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/types.hpp>

#include <ocpp/v16/messages/DataTransfer.hpp>
#include <ocpp/v16/messages/GetConfiguration.hpp>
#include <ocpp/v16/messages/GetDiagnostics.hpp>
#include <ocpp/v16/messages/GetLog.hpp>
#include <ocpp/v16/messages/SignedUpdateFirmware.hpp>
#include <ocpp/v16/messages/UpdateFirmware.hpp>

// for OCPP1.6 PnC
#include "ocpp/v16/messages/ChangeAvailability.hpp"
#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/CertificateSigned.hpp>
#include <ocpp/v201/messages/DeleteCertificate.hpp>
#include <ocpp/v201/messages/Get15118EVCertificate.hpp>
#include <ocpp/v201/messages/GetCertificateStatus.hpp>
#include <ocpp/v201/messages/GetInstalledCertificateIds.hpp>
#include <ocpp/v201/messages/InstallCertificate.hpp>
#include <ocpp/v201/messages/SignCertificate.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>

namespace ocpp {
namespace v16 {
class ChargePointImpl;

/// \brief Contains a ChargePoint implementation compatible with OCPP-J 1.6
class ChargePoint {
private:
    std::unique_ptr<ChargePointImpl> charge_point;

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
    /// \param evse_security Pointer to evse_security that manages security related operations; if nullptr
    /// security_configuration must be set
    /// \param security_configuration specifies the file paths that are required to set up the internal evse_security
    /// implementation
    explicit ChargePoint(const std::string& config, const fs::path& share_path, const fs::path& user_config_path,
                         const fs::path& database_path, const fs::path& sql_init_path, const fs::path& message_log_path,
                         const std::shared_ptr<EvseSecurity> evse_security,
                         const std::optional<SecurityConfiguration> security_configuration = std::nullopt);

    ~ChargePoint();

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint and initializes a
    /// BootNotification.req
    /// \param connector_status_map initial state of connectors including connector 0 with reduced set of states
    /// (Available, Unavailable, Faulted). connector_status_map is empty, last availability states from the persistant
    /// storage will be used
    /// \return
    bool start(const std::map<int, ChargePointStatus>& connector_status_map = {});

    /// \brief Restarts the ChargePoint if it has been stopped before. The ChargePoint is reinitialized, connects to the
    /// websocket and starts to communicate OCPP messages again
    /// \param connector_status_map initial state of connectors including connector 0 with reduced set of states
    /// (Available, Unavailable, Faulted). connector_status_map is empty, last availability states from the persistant
    /// storage will be used
    bool restart(const std::map<int, ChargePointStatus>& connector_status_map = {});

    // \brief Resets the internal state machine for the connectors using the given \p connector_status_map
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
    /// LocalAuthListEnabled and AuthorizationCacheEnabled
    /// \returns the IdTagInfo
    IdTagInfo authorize_id_token(CiString<20> id_token);

    // for plug&charge 1.6 whitepaper

    /// \brief Uses data_transfer mechanism to authorize given \p emaid , \p certificate and
    /// \p iso15118_certificate_hash_data locally or by requesting authorzation at CSMS. This function can be called
    /// when the authorization mechanism Plug&Charge is specified as part of the ISO15118 PaymentDetailsRequest
    /// \param emaid
    /// \param certificate contract certificate that the EVCC provides
    /// \param iso15118_certificate_hash_data
    /// \return
    ocpp::v201::AuthorizeResponse data_transfer_pnc_authorize(
        const std::string& emaid, const std::optional<std::string>& certificate,
        const std::optional<std::vector<ocpp::v201::OCSPRequestData>>& iso15118_certificate_hash_data);

    /// \brief  Uses data transfer mechanism to get 15118 ev certificate from CSMS. This function can be called when the
    /// EVCC requests the update or installation of a contract certificate as part of the ISO15118
    /// CertificateInstallRequest or CertificateUpdateRequest
    /// \param connector_id
    /// \param exi_request provided by the EVCC
    /// \param iso15118_schema_version provided by the EVCC
    /// \param certificate_action Install or Update
    void data_transfer_pnc_get_15118_ev_certificate(const int32_t connector_id, const std::string& exi_request,
                                                    const std::string& iso15118_schema_version,
                                                    const ocpp::v201::CertificateActionEnum& certificate_action);

    /// \brief Allows the exchange of arbitrary \p data identified by a \p vendorId and \p messageId with a central
    /// system \returns the DataTransferResponse
    /// \param vendorId
    /// \param messageId
    /// \param data
    /// \return
    DataTransferResponse data_transfer(const CiString<255>& vendorId, const std::optional<CiString<50>>& messageId,
                                       const std::optional<std::string>& data);

    /// \brief Calculates ChargingProfiles configured by the CSMS of all connectors from now until now + given \p
    /// duration_s
    /// \param duration_s
    /// \return ChargingSchedules of all connectors
    std::map<int32_t, ChargingSchedule> get_all_composite_charging_schedules(const int32_t duration_s);

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
                                const int32_t& meter_start, std::optional<int32_t> reservation_id,
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
    void on_suspend_charging_ev(int32_t connector);

    /// \brief This function should be called when EVSE indicates that it suspends charging on the given \p connector
    /// \param connector
    void on_suspend_charging_evse(int32_t connector);

    /// \brief This function should be called when charging resumes on the given \p connector
    /// \param connector
    void on_resume_charging(int32_t connector);

    /// \brief This function should be called if an error with the given \p error_code is present. This function will
    /// trigger a StatusNotification.req containing the given \p error_code . It will not change the present state of
    /// the state machine.
    /// \param connector
    /// \param error_code
    /// \param info Additional free format information related to the error
    /// \param vendor_id This identifies the vendor-specific implementation
    /// \param vendor_error_code This contains the vendor-specific error code
    void on_error(int32_t connector, const ChargePointErrorCode& error_code,
                  const std::optional<CiString<50>>& info = std::nullopt,
                  const std::optional<CiString<255>>& vendor_id = std::nullopt,
                  const std::optional<CiString<50>>& vendor_error_code = std::nullopt);

    /// \brief This function should be called if a fault is detected that prevents further charging operations. The \p
    /// error_code indicates the reason for the fault.
    /// \param info Additional free format information related to the error
    /// \param vendor_id This identifies the vendor-specific implementation
    /// \param vendor_error_code This contains the vendor-specific error code
    void on_fault(int32_t connector, const ChargePointErrorCode& error_code,
                  const std::optional<CiString<50>>& info = std::nullopt,
                  const std::optional<CiString<255>>& vendor_id = std::nullopt,
                  const std::optional<CiString<50>>& vendor_error_code = std::nullopt);

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
    /// be called when an EV is plugged in but the authorization is present within the specified ConnectionTimeout
    void on_plugin_timeout(int32_t connector);

    // \brief Notifies chargepoint that a SecurityEvent occurs. This will send a SecurityEventNotification.req to the
    /// CSMS
    /// \param type type of the security event
    /// \param tech_info additional info of the security event
    void on_security_event(const std::string& type, const std::string& tech_info);

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

    /// \brief registers a \p callback function that can be used to unlock the connector. The unlock_connector_callback
    /// is called:
    /// - when receiving a UnlockConnector.req and
    /// - when a transaction has on_transaction_stopped is called and the configuration key
    /// UnlockConnectorOnEVSideDisconnect is true
    /// \param callback
    void register_unlock_connector_callback(const std::function<bool(int32_t connector)>& callback);

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
                                 const ocpp::v201::Get15118EVCertificateResponse& certificate_response,
                                 const ocpp::v201::CertificateActionEnum& certificate_action)>& callback);

    /// \brief registers a \p callback function that can be used to publish the response when transaction starts
    /// \param callback
    void register_transaction_started_callback(
        const std::function<void(const int32_t connector, const int32_t transaction_id)>& callback);

    /// \brief registers a \p callback function that can be used to react on changed configuration keys. This
    /// callback is called when a configuration key has been changed by the CSMS
    /// \param key the configuration key for which the callback is registered
    /// \param callback executed when this configuration key changed
    void register_configuration_key_changed_callback(const CiString<50>& key,
                                                     const std::function<void(const KeyValue& key_value)>& callback);

    /// \brief registers a \p callback function that can be used to react to a security event callback. This callback is
    /// called only if the SecurityEvent occured internally within libocpp
    void register_security_event_callback(
        const std::function<void(const std::string& type, const std::string& tech_info)>& callback);

    /// \brief Gets the configured configuration key requested in the given \p request
    /// \param request specifies the keys that should be returned. If empty or not set, all keys will be reported
    /// \return a response containing the requested key(s) including the values and unkown keys if present
    GetConfigurationResponse get_configuration_key(const GetConfigurationRequest& request);

    /// \brief Sets a custom configuration key
    /// \param key
    /// \param value
    /// \return Indicates the result of the operation
    ConfigurationStatus set_custom_configuration_key(CiString<50> key, CiString<500> value);

    /// \brief registers a \p callback function that can be used to check, if the \p connector is reserved for the given
    /// \p id_token. The is_token_reserved_for_connector_callback is called when a RemoteStartTransaction.req is
    /// received.
    /// \param callback
    void register_is_token_reserved_for_connector_callback(
        const std::function<bool(const int32_t connector, const std::string& id_token)>& callback);

    /// \brief Delay draining the message queue after reconnecting, so the CSMS can perform post-reconnect checks first
    /// \param delay The delay period (seconds)
    void set_message_queue_resume_delay(std::chrono::seconds delay);
};

} // namespace v16
} // namespace ocpp
#endif
