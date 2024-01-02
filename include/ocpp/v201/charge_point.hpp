// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <future>
#include <set>

#include <ocpp/common/charging_station_base.hpp>

#include <ocpp/v201/average_meter_values.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/database_handler.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/evse.hpp>
#include <ocpp/v201/ocpp_types.hpp>
#include <ocpp/v201/ocsp_updater.hpp>
#include <ocpp/v201/types.hpp>
#include <ocpp/v201/utils.hpp>

#include "ocpp/v201/messages/Get15118EVCertificate.hpp"
#include <ocpp/v201/messages/Authorize.hpp>
#include <ocpp/v201/messages/BootNotification.hpp>
#include <ocpp/v201/messages/CertificateSigned.hpp>
#include <ocpp/v201/messages/ChangeAvailability.hpp>
#include <ocpp/v201/messages/ClearCache.hpp>
#include <ocpp/v201/messages/CustomerInformation.hpp>
#include <ocpp/v201/messages/DataTransfer.hpp>
#include <ocpp/v201/messages/DeleteCertificate.hpp>
#include <ocpp/v201/messages/GetBaseReport.hpp>
#include <ocpp/v201/messages/GetInstalledCertificateIds.hpp>
#include <ocpp/v201/messages/GetLocalListVersion.hpp>
#include <ocpp/v201/messages/GetLog.hpp>
#include <ocpp/v201/messages/GetReport.hpp>
#include <ocpp/v201/messages/GetTransactionStatus.hpp>
#include <ocpp/v201/messages/GetVariables.hpp>
#include <ocpp/v201/messages/Heartbeat.hpp>
#include <ocpp/v201/messages/InstallCertificate.hpp>
#include <ocpp/v201/messages/MeterValues.hpp>
#include <ocpp/v201/messages/NotifyCustomerInformation.hpp>
#include <ocpp/v201/messages/NotifyEvent.hpp>
#include <ocpp/v201/messages/NotifyReport.hpp>
#include <ocpp/v201/messages/RequestStartTransaction.hpp>
#include <ocpp/v201/messages/RequestStopTransaction.hpp>
#include <ocpp/v201/messages/Reset.hpp>
#include <ocpp/v201/messages/SecurityEventNotification.hpp>
#include <ocpp/v201/messages/SendLocalList.hpp>
#include <ocpp/v201/messages/SetNetworkProfile.hpp>
#include <ocpp/v201/messages/SetVariables.hpp>
#include <ocpp/v201/messages/SignCertificate.hpp>
#include <ocpp/v201/messages/StatusNotification.hpp>
#include <ocpp/v201/messages/TransactionEvent.hpp>
#include <ocpp/v201/messages/TriggerMessage.hpp>
#include <ocpp/v201/messages/UnlockConnector.hpp>
#include <ocpp/v201/messages/UpdateFirmware.hpp>

namespace ocpp {
namespace v201 {

class UnexpectedMessageTypeFromCSMS : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Callbacks {
    ///\brief Function to check if the callback struct is completely filled. All std::functions should hold a function,
    ///       all std::optional<std::functions> should either be emtpy or hold a function.
    ///
    ///\retval false if any of the normal callbacks are nullptr or any of the optional ones are filled with a nullptr
    ///        true otherwise
    bool all_callbacks_valid() const;

    ///
    /// \brief Callback if reset is allowed. If evse_id has a value, reset only applies to the given evse id. If it has
    ///        no value, applies to complete charging station.
    ///
    std::function<bool(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)>
        is_reset_allowed_callback;
    std::function<void(const std::optional<const int32_t> evse_id, const ResetEnum& reset_type)> reset_callback;
    std::function<void(const int32_t evse_id, const ReasonEnum& stop_reason)> stop_transaction_callback;
    std::function<void(const int32_t evse_id)> pause_charging_callback;
    ///
    /// \brief Change availability of charging station / evse / connector.
    /// \param request The request.
    /// \param persist True to persist the status after reboot.
    ///
    /// Persist is set to 'false' if the status does not need to be stored after restarting. Otherwise it is true.
    /// False is for example during a reset OnIdle where first an 'unavailable' is sent until the charging session
    /// stopped. True is for example when the CSMS sent an 'inoperative' request.
    ///
    std::function<void(const ChangeAvailabilityRequest& request, const bool persist)> change_availability_callback;
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
    std::optional<std::function<void(const ocpp::DateTime& currentTime)>> time_sync_callback;

    /// \brief callback to be called to congfigure ocpp message logging
    std::optional<std::function<void(const std::string& message, MessageDirection direction)>> ocpp_messages_callback;

    ///
    /// \brief callback function that can be used to react to a security event callback. This callback is
    /// called only if the SecurityEvent occured internally within libocpp
    /// Typically this callback is used to log security events in the security log
    ///
    std::function<void(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info)>
        security_event_callback;

    /// \brief  Callback for when a bootnotification response is received
    std::optional<std::function<void(const ocpp::v201::RegistrationStatusEnum& reg_status)>> boot_notification_callback;

    /// \brief Callback function that can be used to get (human readable) customer information based on the given
    /// arguments
    std::optional<std::function<std::string(const std::optional<CertificateHashDataType> customer_certificate,
                                            const std::optional<IdToken> id_token,
                                            const std::optional<CiString<64>> customer_identifier)>>
        get_customer_information_callback;

    /// \brief Callback function that can be called to clear customer information based on the given arguments
    std::optional<std::function<void(const std::optional<CertificateHashDataType> customer_certificate,
                                     const std::optional<IdToken> id_token,
                                     const std::optional<CiString<64>> customer_identifier)>>
        clear_customer_information_callback;

    /// \brief Callback function that can be called when all connectors are unavailable
    std::optional<std::function<void()>> all_connectors_unavailable_callback;

    /// \brief Callback function that can be used to handle arbitrary data transfers for all vendorId and
    /// messageId
    std::optional<std::function<DataTransferResponse(const DataTransferRequest& request)>> data_transfer_callback;
};

/// \brief Combines ChangeAvailabilityRequest with persist flag for scheduled Availability changes
struct AvailabilityChange {
    ChangeAvailabilityRequest request;
    bool persist;
};

/// \brief Class implements OCPP2.0.1 Charging Station
class ChargePoint : ocpp::ChargingStationBase {

private:
    // reference to evses
    std::map<int32_t, std::unique_ptr<Evse>> evses;

    // utility
    std::unique_ptr<MessageQueue<v201::MessageType>> message_queue;
    std::unique_ptr<DeviceModel> device_model;
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

    // states
    RegistrationStatusEnum registration_status;
    OperationalStatusEnum operational_state;
    FirmwareStatusEnum firmware_status;
    // The request ID in the last firmware update status received
    std::optional<int32_t> firmware_status_id;
    // The last firmware status which will be posted before the firmware is installed.
    FirmwareStatusEnum firmware_status_before_installing = FirmwareStatusEnum::SignatureVerified;
    UploadLogStatusEnum upload_log_status;
    int32_t upload_log_status_id;
    BootReasonEnum bootreason;
    int network_configuration_priority;
    bool disable_automatic_websocket_reconnects;

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

    std::map<EvseConnectorPair, ConnectorStatusEnum> conn_state_per_evse;
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
    /// \brief optional delay to resumption of message queue after reconnecting to the CSMS
    std::chrono::seconds message_queue_resume_delay = std::chrono::seconds(0);

    bool send(CallError call_error);

    // internal helper functions
    void init_websocket();
    WebsocketConnectionOptions get_ws_connection_options(const int32_t configuration_slot);
    void init_certificate_expiration_check_timers();
    void scheduled_check_client_certificate_expiration();
    void scheduled_check_v2g_certificate_expiration();

    /// \brief Gets the configured NetworkConnectionProfile based on the given \p configuration_slot . The
    /// central system uri ofthe connection options will not contain ws:// or wss:// because this method removes it if
    /// present \param network_configuration_priority \return
    std::optional<NetworkConnectionProfile> get_network_connection_profile(const int32_t configuration_slot);
    /// \brief Moves websocket network_configuration_priority to next profile
    void next_network_configuration_priority();

    /// @brief Removes all network connection profiles below the actual security profile and stores the new list in the
    /// device model
    void remove_network_connection_profiles_below_actual_security_profile();

    void handle_message(const EnhancedMessage<v201::MessageType>& message);
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
    bool validate_set_variable(const SetVariableData& set_variable_data);
    MeterValue get_latest_meter_value_filtered(const MeterValue& meter_value, ReadingContextEnum context,
                                               const RequiredComponentVariable& component_variable);

    /// \brief Changes all unoccupied connectors to unavailable. If a transaction is running schedule an availabilty
    /// change
    /// If all connectors are unavailable signal to the firmware updater that installation of the firmware update can
    /// proceed
    void change_all_connectors_to_unavailable_for_firmware_update();

    /// \brief Sets the cache lifetime value in \param id_token_info with configured AuthCacheLifeTime
    /// if it was not already set
    void update_id_token_cache_lifetime(IdTokenInfo& id_token_info);

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
    bool is_evse_reserved_for_other(const std::unique_ptr<Evse>& evse, const IdToken& id_token,
                                    const std::optional<IdToken>& group_id_token) const;

    ///
    /// \brief Check if one of the connectors of the evse is available (both connectors faulted or unavailable or on of
    ///        the connectors occupied).
    /// \param evse Evse to check.
    /// \return True if at least one connector is not faulted or unavailable.
    ///
    bool is_evse_connector_available(const std::unique_ptr<Evse>& evse) const;

    ///
    /// \brief Set all connectors of a given evse to unavailable.
    /// \param evse The evse.
    /// \param persist  True if unavailability should persist. If it is set to false, there will be a check per
    ///                 connector if it was already set to true and if that is the case, it will be persisted anyway.
    ///
    void set_evse_connectors_unavailable(const std::unique_ptr<Evse>& evse, bool persist);

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

    /* OCPP message requests */

    // Functional Block A: Security
    void security_event_notification_req(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                         const bool triggered_internally, const bool critical);
    void sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use);

    // Functional Block B: Provisioning
    void boot_notification_req(const BootReasonEnum& reason);
    void notify_report_req(const int request_id, const std::vector<ReportData>& report_data);

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
                               const std::optional<int32_t>& number_of_phases_used, const bool offline,
                               const std::optional<int32_t>& reservation_id);

    // Functional Block J: MeterValues
    void meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values);

    // Functional Block N: Diagnostics
    void notify_event_req(const std::vector<EventData>& events);
    void notify_customer_information_req(const std::string& data, const int32_t request_id);

    /* OCPP message handlers */

    // Functional Block A: Security
    void handle_certificate_signed_req(Call<CertificateSignedRequest> call);
    void handle_sign_certificate_response(CallResult<SignCertificateResponse> call_result);

    // Functional Block B: Provisioning
    void handle_boot_notification_response(CallResult<BootNotificationResponse> call_result);
    void handle_set_variables_req(Call<SetVariablesRequest> call);
    void handle_get_variables_req(const EnhancedMessage<v201::MessageType>& message);
    void handle_get_base_report_req(Call<GetBaseReportRequest> call);
    void handle_get_report_req(Call<GetReportRequest> call);
    void handle_set_network_profile_req(Call<SetNetworkProfileRequest> call);
    void handle_reset_req(Call<ResetRequest> call);

    // Functional Block C: Authorization
    void handle_clear_cache_req(Call<ClearCacheRequest> call);

    // Functional Block D: Local authorization list management
    void handle_send_local_authorization_list_req(Call<SendLocalListRequest> call);
    void handle_get_local_authorization_list_version_req(Call<GetLocalListVersionRequest> call);

    // Functional Block E: Transaction
    void handle_start_transaction_event_response(const EnhancedMessage<v201::MessageType>& message);
    void handle_get_transaction_status(const Call<GetTransactionStatusRequest> call);

    // Function Block F: Remote transaction control
    void handle_unlock_connector(Call<UnlockConnectorRequest> call);
    void handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call);
    void handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call);
    void handle_trigger_message(Call<TriggerMessageRequest> call);

    // Functional Block G: Availability
    void handle_change_availability_req(Call<ChangeAvailabilityRequest> call);
    void handle_heartbeat_response(CallResult<HeartbeatResponse> call);

    // Functional Block L: Firmware management
    void handle_firmware_update_req(Call<UpdateFirmwareRequest> call);

    // Functional Block M: ISO 15118 Certificate Management
    void handle_get_installed_certificate_ids_req(Call<GetInstalledCertificateIdsRequest> call);
    void handle_install_certificate_req(Call<InstallCertificateRequest> call);
    void handle_delete_certificate_req(Call<DeleteCertificateRequest> call);

    // Functional Block N: Diagnostics
    void handle_get_log_req(Call<GetLogRequest> call);
    void handle_customer_information_req(Call<CustomerInformationRequest> call);

    // Functional Block P: DataTransfer
    void handle_data_transfer_req(Call<DataTransferRequest> call);

    // general message handling
    template <class T> bool send(ocpp::Call<T> call) {
        this->message_queue->push(call);
        return true;
    }
    template <class T> std::future<EnhancedMessage<v201::MessageType>> send_async(ocpp::Call<T> call) {
        return this->message_queue->push_async(call);
    }
    template <class T> bool send(ocpp::CallResult<T> call_result) {
        this->message_queue->push(call_result);
        return true;
    }
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
    };

public:
    /// \brief Construct a new ChargePoint object
    /// \param evse_connector_structure Map that defines the structure of EVSE and connectors of the chargepoint. The
    /// key represents the id of the EVSE and the value represents the number of connectors for this EVSE. The ids of
    /// the EVSEs have to increment starting with 1.
    /// \param device_model_storage_address address to device model storage (e.g. location of SQLite database)
    /// \param ocpp_main_path Path where utility files for OCPP are read and written to
    /// \param core_database_path Path to directory where core database is located
    /// \param message_log_path Path to where logfiles are written to
    /// \param evse_security Pointer to evse_security that manages security related operations; if nullptr
    /// security_configuration must be set
    /// \param callbacks Callbacks that will be registered for ChargePoint
    /// \param security_configuration specifies the file paths that are required to set up the internal evse_security
    /// implementation
    ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                const std::string& device_model_storage_address, const std::string& ocpp_main_path,
                const std::string& core_database_path, const std::string& sql_init_path,
                const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                const Callbacks& callbacks);

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

    /// \brief Starts the ChargePoint, initializes and connects to the Websocket endpoint
    /// \param bootreason   Optional bootreason (default: PowerUp).
    void start(BootReasonEnum bootreason = BootReasonEnum::PowerUp);

    /// \brief Starts the websocket
    void start_websocket();

    /// \brief Stops the ChargePoint. Disconnects the websocket connection and stops MessageQueue and all timers
    void stop();

    /// \brief Initializes the websocket and connects to CSMS if it is not yet connected
    void connect_websocket();

    /// \brief Disconnects the the websocket connection to the CSMS if it is connected
    /// \param code Optional websocket close status code (default: normal).
    void disconnect_websocket(websocketpp::close::status::value code = websocketpp::close::status::normal);

    /// \brief Chargepoint notifies about new firmware update status firmware_update_status. This function should be
    ///        called during a Firmware Update to indicate the current firmware_update_status.
    /// \param request_id   The request_id. When it is -1, it will not be included in the request.
    /// \param firmware_update_status The firmware_update_status
    void on_firmware_update_status_notification(int32_t request_id, const FirmwareStatusEnum& firmware_update_status);

    /// \brief Event handler that should be called when a session has started
    /// \param evse_id
    /// \param connector_id
    void on_session_started(const int32_t evse_id, const int32_t connector_id);

    /// \brief Event handler that should be called when the EV sends a certificate request (for update or installation)
    /// \param request
    Get15118EVCertificateResponse on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request);

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
                                 const ReasonEnum reason, const std::optional<IdToken>& id_token,
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

    // \brief Notifies chargepoint that a SecurityEvent has occured. This will send a SecurityEventNotification.req to
    // the
    /// CSMS
    /// \param type type of the security event
    /// \param tech_info additional info of the security event
    void on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info);

    /// \brief Data transfer mechanism initiated by charger
    /// \param vendorId
    /// \param messageId
    /// \param data
    /// \return DataTransferResponse contaning the result from CSMS
    DataTransferResponse data_transfer_req(const CiString<255>& vendorId, const std::optional<CiString<50>>& messageId,
                                           const std::optional<std::string>& data);

    /// \brief Delay draining the message queue after reconnecting, so the CSMS can perform post-reconnect checks first
    /// \param delay The delay period (seconds)
    void set_message_queue_resume_delay(std::chrono::seconds delay) {
        this->message_queue_resume_delay = delay;
    }
};

} // namespace v201
} // namespace ocpp
