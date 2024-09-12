// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/common/types.hpp>
#include <ocpp/v201/charge_point.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>
#include <ocpp/v201/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v201/messages/LogStatusNotification.hpp>
#include <ocpp/v201/notify_report_requests_splitter.hpp>

#include <optional>
#include <stdexcept>
#include <string>

using namespace std::chrono_literals;

const auto DEFAULT_MAX_CUSTOMER_INFORMATION_DATA_LENGTH = 51200;
const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL = "internal";
const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS = "csms";
const auto DEFAULT_WAIT_FOR_FUTURE_TIMEOUT = std::chrono::seconds(60);

using DatabaseException = ocpp::common::DatabaseException;

namespace ocpp {
namespace v201 {

const auto DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL = std::chrono::seconds(30);
const auto DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD = 2E5;
const auto DEFAULT_MAX_MESSAGE_SIZE = 65000;

bool Callbacks::all_callbacks_valid() const {
    return this->is_reset_allowed_callback != nullptr and this->reset_callback != nullptr and
           this->stop_transaction_callback != nullptr and this->pause_charging_callback != nullptr and
           this->connector_effective_operative_status_changed_callback != nullptr and
           this->get_log_request_callback != nullptr and this->unlock_connector_callback != nullptr and
           this->remote_start_transaction_callback != nullptr and this->is_reservation_for_token_callback != nullptr and
           this->update_firmware_request_callback != nullptr and this->security_event_callback != nullptr and
           this->set_charging_profiles_callback != nullptr and
           (!this->variable_changed_callback.has_value() or this->variable_changed_callback.value() != nullptr) and
           (!this->validate_network_profile_callback.has_value() or
            this->validate_network_profile_callback.value() != nullptr) and
           (!this->configure_network_connection_profile_callback.has_value() or
            this->configure_network_connection_profile_callback.value() != nullptr) and
           (!this->time_sync_callback.has_value() or this->time_sync_callback.value() != nullptr) and
           (!this->boot_notification_callback.has_value() or this->boot_notification_callback.value() != nullptr) and
           (!this->ocpp_messages_callback.has_value() or this->ocpp_messages_callback.value() != nullptr) and
           (!this->cs_effective_operative_status_changed_callback.has_value() or
            this->cs_effective_operative_status_changed_callback.value() != nullptr) and
           (!this->evse_effective_operative_status_changed_callback.has_value() or
            this->evse_effective_operative_status_changed_callback.value() != nullptr) and
           (!this->get_customer_information_callback.has_value() or
            this->get_customer_information_callback.value() != nullptr) and
           (!this->clear_customer_information_callback.has_value() or
            this->clear_customer_information_callback.value() != nullptr) and
           (!this->all_connectors_unavailable_callback.has_value() or
            this->all_connectors_unavailable_callback.value() != nullptr) and
           (!this->data_transfer_callback.has_value() or this->data_transfer_callback.value() != nullptr) and
           (!this->transaction_event_callback.has_value() or this->transaction_event_callback.value() != nullptr) and
           (!this->transaction_event_response_callback.has_value() or
            this->transaction_event_response_callback.value() != nullptr);
}

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         std::shared_ptr<DeviceModel> device_model, std::shared_ptr<DatabaseHandler> database_handler,
                         std::shared_ptr<MessageQueue<v201::MessageType>> message_queue,
                         const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                         const Callbacks& callbacks) :
    ocpp::ChargingStationBase(evse_security),
    message_queue(message_queue),
    device_model(device_model),
    database_handler(database_handler),
    registration_status(RegistrationStatusEnum::Rejected),
    skip_invalid_csms_certificate_notifications(false),
    reset_scheduled(false),
    reset_scheduled_evseids{},
    firmware_status(FirmwareStatusEnum::Idle),
    upload_log_status(UploadLogStatusEnum::Idle),
    bootreason(BootReasonEnum::PowerUp),
    ocsp_updater(this->evse_security, this->send_callback<GetCertificateStatusRequest, GetCertificateStatusResponse>(
                                          MessageType::GetCertificateStatusResponse)),
    monitoring_updater(
        device_model, [this](const std::vector<EventData>& events) { this->notify_event_req(events); },
        [this]() { return this->is_offline(); }),
    csr_attempt(1),
    client_certificate_expiration_check_timer([this]() { this->scheduled_check_client_certificate_expiration(); }),
    v2g_certificate_expiration_check_timer([this]() { this->scheduled_check_v2g_certificate_expiration(); }),
    callbacks(callbacks) {

    // Make sure the received callback struct is completely filled early before we actually start running
    if (!this->callbacks.all_callbacks_valid()) {
        EVLOG_AND_THROW(std::invalid_argument("All non-optional callbacks must be supplied"));
    }

    if (!this->device_model) {
        EVLOG_AND_THROW(std::invalid_argument("Device model should not be null"));
    }

    if (!this->database_handler) {
        EVLOG_AND_THROW(std::invalid_argument("Database handler should not be null"));
    }

    initialize(evse_connector_structure, message_log_path);
}

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         const std::string& device_model_storage_address, const bool initialize_device_model,
                         const std::string& device_model_migration_path, const std::string& device_model_config_path,
                         const std::string& ocpp_main_path, const std::string& core_database_path,
                         const std::string& sql_init_path, const std::string& message_log_path,
                         const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks) :
    ChargePoint(evse_connector_structure,
                std::make_unique<DeviceModelStorageSqlite>(device_model_storage_address, device_model_migration_path,
                                                           device_model_config_path, initialize_device_model),
                ocpp_main_path, core_database_path, sql_init_path, message_log_path, evse_security, callbacks) {
}

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         const std::string& device_model_storage_address, const std::string& ocpp_main_path,
                         const std::string& core_database_path, const std::string& sql_init_path,
                         const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                         const Callbacks& callbacks) :
    ChargePoint(evse_connector_structure, std::make_unique<DeviceModelStorageSqlite>(device_model_storage_address),
                ocpp_main_path, core_database_path, sql_init_path, message_log_path, evse_security, callbacks) {
}

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         std::unique_ptr<DeviceModelStorage> device_model_storage, const std::string& ocpp_main_path,
                         const std::string& core_database_path, const std::string& sql_init_path,
                         const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                         const Callbacks& callbacks) :
    ocpp::ChargingStationBase(evse_security),
    registration_status(RegistrationStatusEnum::Rejected),
    skip_invalid_csms_certificate_notifications(false),
    reset_scheduled(false),
    reset_scheduled_evseids{},
    firmware_status(FirmwareStatusEnum::Idle),
    upload_log_status(UploadLogStatusEnum::Idle),
    bootreason(BootReasonEnum::PowerUp),
    ocsp_updater(this->evse_security, this->send_callback<GetCertificateStatusRequest, GetCertificateStatusResponse>(
                                          MessageType::GetCertificateStatusResponse)),
    device_model(std::make_shared<DeviceModel>(std::move(device_model_storage))),
    monitoring_updater(
        device_model, [this](const std::vector<EventData>& events) { this->notify_event_req(events); },
        [this]() { return this->is_offline(); }),
    csr_attempt(1),
    client_certificate_expiration_check_timer([this]() { this->scheduled_check_client_certificate_expiration(); }),
    v2g_certificate_expiration_check_timer([this]() { this->scheduled_check_v2g_certificate_expiration(); }),
    callbacks(callbacks),
    stop_auth_cache_cleanup_handler(false) {
    // Make sure the received callback struct is completely filled early before we actually start running
    if (!this->callbacks.all_callbacks_valid()) {
        EVLOG_AND_THROW(std::invalid_argument("All non-optional callbacks must be supplied"));
    }

    auto database_connection = std::make_unique<common::DatabaseConnection>(fs::path(core_database_path) / "cp.db");
    this->database_handler = std::make_shared<DatabaseHandler>(std::move(database_connection), sql_init_path);

    initialize(evse_connector_structure, message_log_path);

    std::set<v201::MessageType> message_types_discard_for_queueing;
    try {
        const auto message_types_discard_for_queueing_csl = ocpp::split_string(
            this->device_model
                ->get_optional_value<std::string>(ControllerComponentVariables::MessageTypesDiscardForQueueing)
                .value_or(""),
            ',');
        std::transform(message_types_discard_for_queueing_csl.begin(), message_types_discard_for_queueing_csl.end(),
                       std::inserter(message_types_discard_for_queueing, message_types_discard_for_queueing.end()),
                       [](const std::string element) { return conversions::string_to_messagetype(element); });
    } catch (const StringToEnumException& e) {
        EVLOG_warning << "Could not convert configured MessageType value of MessageTypesDiscardForQueueing. Please "
                         "check you configuration: "
                      << e.what();
    } catch (...) {
        EVLOG_warning << "Could not apply MessageTypesDiscardForQueueing configuration";
    }

    this->message_queue = std::make_unique<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return this->connectivity_manager->send_to_websocket(message.dump()); },
        MessageQueueConfig<v201::MessageType>{
            this->device_model->get_value<int>(ControllerComponentVariables::MessageAttempts),
            this->device_model->get_value<int>(ControllerComponentVariables::MessageAttemptInterval),
            this->device_model->get_optional_value<int>(ControllerComponentVariables::MessageQueueSizeThreshold)
                .value_or(DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD),
            this->device_model->get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages)
                .value_or(false),
            message_types_discard_for_queueing,
            this->device_model->get_value<int>(ControllerComponentVariables::MessageTimeout)},
        this->database_handler);
}

ChargePoint::~ChargePoint() {
    {
        std::scoped_lock lk(this->auth_cache_cleanup_mutex);
        this->stop_auth_cache_cleanup_handler = true;
    }
    this->auth_cache_cleanup_cv.notify_one();
    this->auth_cache_cleanup_thread.join();
}

void ChargePoint::start(BootReasonEnum bootreason) {
    this->message_queue->start();

    this->bootreason = bootreason;
    // Trigger all initial status notifications and callbacks related to component state
    // Should be done before sending the BootNotification.req so that the correct states can be reported
    this->component_state_manager->trigger_all_effective_availability_changed_callbacks();
    // get transaction messages from db (if there are any) so they can be sent again.
    this->message_queue->get_persisted_messages_from_db();
    this->boot_notification_req(bootreason);
    // K01.27 - call load_charging_profiles when system boots
    this->load_charging_profiles();
    this->connectivity_manager->start();

    if (this->bootreason == BootReasonEnum::RemoteReset) {
        this->security_event_notification_req(
            CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
            std::optional<CiString<255>>("Charging Station rebooted due to requested remote reset!"), true, true);
    } else if (this->bootreason == BootReasonEnum::ScheduledReset) {
        this->security_event_notification_req(
            CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
            std::optional<CiString<255>>("Charging Station rebooted due to a scheduled reset!"), true, true);
    } else if (this->bootreason == BootReasonEnum::PowerUp) {
        std::string startup_message = "Charging Station powered up! Firmware version: ";
        startup_message.append(
            this->device_model->get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
        this->security_event_notification_req(CiString<50>(ocpp::security_events::STARTUP_OF_THE_DEVICE),
                                              std::optional<CiString<255>>(startup_message), true, true);
    } else {
        std::string startup_message = "Charging station reset or reboot. Firmware version: ";
        startup_message.append(
            this->device_model->get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
        this->security_event_notification_req(CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
                                              std::optional<CiString<255>>(startup_message), true, true);
    }
}

void ChargePoint::stop() {
    this->ocsp_updater.stop();
    this->heartbeat_timer.stop();
    this->boot_notification_timer.stop();
    this->certificate_signed_timer.stop();
    this->connectivity_manager->stop();
    this->client_certificate_expiration_check_timer.stop();
    this->v2g_certificate_expiration_check_timer.stop();
    this->monitoring_updater.stop_monitoring();
    this->message_queue->stop();
}

void ChargePoint::connect_websocket() {
    this->connectivity_manager->connect();
}

void ChargePoint::disconnect_websocket() {
    this->connectivity_manager->disconnect_websocket();
}

void ChargePoint::on_firmware_update_status_notification(int32_t request_id,
                                                         const FirmwareStatusEnum& firmware_update_status) {
    if (this->firmware_status == firmware_update_status) {
        if (request_id == -1 or
            this->firmware_status_id.has_value() and this->firmware_status_id.value() == request_id) {
            // already sent, do not send again
            return;
        }
    }
    FirmwareStatusNotificationRequest req;
    req.status = firmware_update_status;
    // Firmware status and id are stored for future trigger message request.
    this->firmware_status = req.status;

    if (request_id != -1) {
        req.requestId = request_id; // L01.FR.20
        this->firmware_status_id = request_id;
    }

    ocpp::Call<FirmwareStatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send_async<FirmwareStatusNotificationRequest>(call);

    if (req.status == FirmwareStatusEnum::Installed) {
        std::string firmwareVersionMessage = "New firmware succesfully installed! Version: ";
        firmwareVersionMessage.append(
            this->device_model->get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
        this->security_event_notification_req(CiString<50>(ocpp::security_events::FIRMWARE_UPDATED),
                                              std::optional<CiString<255>>(firmwareVersionMessage), true,
                                              true); // L01.FR.31
    } else if (req.status == FirmwareStatusEnum::InvalidSignature) {
        this->security_event_notification_req(
            CiString<50>(ocpp::security_events::INVALIDFIRMWARESIGNATURE),
            std::optional<CiString<255>>("Signature of the provided firmware is not valid!"), true,
            true); // L01.FR.03 - critical because TC_L_06_CS requires this message to be sent
    } else if (req.status == FirmwareStatusEnum::InstallVerificationFailed ||
               req.status == FirmwareStatusEnum::InstallationFailed) {
        this->restore_all_connector_states();
    }

    if (this->firmware_status_before_installing == req.status) {
        // FIXME(Kai): This is a temporary workaround, because the EVerest System module does not keep track of
        // transactions and can't inquire about their status from the OCPP modules. If the firmware status is expected
        // to become "Installing", but we still have a transaction running, the update will wait for the transaction to
        // finish, and so we send an "InstallScheduled" status. This is necessary for OCTT TC_L_15_CS to pass.
        const auto transaction_active = this->any_transaction_active(std::nullopt);
        if (transaction_active) {
            this->firmware_status = FirmwareStatusEnum::InstallScheduled;
            req.status = firmware_status;
            ocpp::Call<FirmwareStatusNotificationRequest> call(req, this->message_queue->createMessageId());
            this->send_async<FirmwareStatusNotificationRequest>(call);
        }
        this->change_all_connectors_to_unavailable_for_firmware_update();
    }
}

void ChargePoint::on_session_started(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::PlugIn);
}

Get15118EVCertificateResponse
ChargePoint::on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request) {

    Get15118EVCertificateResponse response;

    if (!this->device_model
             ->get_optional_value<bool>(ControllerComponentVariables::ContractCertificateInstallationEnabled)
             .value_or(false)) {
        EVLOG_warning << "Can not fulfill Get15118EVCertificateRequest because ContractCertificateInstallationEnabled "
                         "is configured as false!";
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    EVLOG_debug << "Received Get15118EVCertificateRequest " << request;
    auto future_res = this->send_async<Get15118EVCertificateRequest>(
        ocpp::Call<Get15118EVCertificateRequest>(request, this->message_queue->createMessageId()));

    if (future_res.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for Get15118EVCertificateRequest.conf future timed out!";
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    const auto response_message = future_res.get();
    EVLOG_debug << "Received Get15118EVCertificateResponse " << response_message.message;
    if (response_message.messageType != MessageType::Get15118EVCertificateResponse) {
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    try {
        ocpp::CallResult<Get15118EVCertificateResponse> call_result = response_message.message;
        return call_result.msg;
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(response_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->send(call_error);
        return response;
    }
}

void ChargePoint::on_transaction_started(const int32_t evse_id, const int32_t connector_id,
                                         const std::string& session_id, const DateTime& timestamp,
                                         const ocpp::v201::TriggerReasonEnum trigger_reason,
                                         const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                                         const std::optional<IdToken>& group_id_token,
                                         const std::optional<int32_t>& reservation_id,
                                         const std::optional<int32_t>& remote_start_id,
                                         const ChargingStateEnum charging_state) {

    auto& evse_handle = this->evse_manager->get_evse(evse_id);
    evse_handle.open_transaction(session_id, connector_id, timestamp, meter_start, id_token, group_id_token,
                                 reservation_id, charging_state);

    const auto meter_value = utils::get_meter_value_with_measurands_applied(
        meter_start, utils::get_measurands_vec(this->device_model->get_value<std::string>(
                         ControllerComponentVariables::SampledDataTxStartedMeasurands)));

    const auto& enhanced_transaction = evse_handle.get_transaction();
    Transaction transaction{enhanced_transaction->transactionId};
    transaction.chargingState = charging_state;
    if (remote_start_id.has_value()) {
        transaction.remoteStartId = remote_start_id.value();
        enhanced_transaction->remoteStartId = remote_start_id.value();
    }

    EVSE evse{evse_id};
    evse.connectorId.emplace(connector_id);

    std::optional<std::vector<MeterValue>> opt_meter_value;
    if (!meter_value.sampledValue.empty()) {
        opt_meter_value.emplace(1, meter_value);
    }

    this->transaction_event_req(TransactionEventEnum::Started, timestamp, transaction, trigger_reason,
                                enhanced_transaction->get_seq_no(), std::nullopt, evse, id_token, opt_meter_value,
                                std::nullopt, this->is_offline(), reservation_id);
}

void ChargePoint::on_transaction_finished(const int32_t evse_id, const DateTime& timestamp,
                                          const MeterValue& meter_stop, const ReasonEnum reason,
                                          const TriggerReasonEnum trigger_reason,
                                          const std::optional<IdToken>& id_token,
                                          const std::optional<std::string>& signed_meter_value,
                                          const ChargingStateEnum charging_state) {
    auto& evse_handle = this->evse_manager->get_evse(evse_id);
    auto& enhanced_transaction = evse_handle.get_transaction();
    if (enhanced_transaction == nullptr) {
        EVLOG_warning << "Received notification of finished transaction while no transaction was active";
        return;
    }

    enhanced_transaction->chargingState = charging_state;
    evse_handle.close_transaction(timestamp, meter_stop, reason);
    const auto transaction = enhanced_transaction->get_transaction();
    const auto transaction_id = enhanced_transaction->transactionId.get();

    std::optional<std::vector<ocpp::v201::MeterValue>> meter_values = std::nullopt;
    try {
        meter_values = std::make_optional(utils::get_meter_values_with_measurands_applied(
            this->database_handler->transaction_metervalues_get_all(enhanced_transaction->transactionId.get()),
            utils::get_measurands_vec(
                this->device_model->get_value<std::string>(ControllerComponentVariables::SampledDataTxEndedMeasurands)),
            utils::get_measurands_vec(
                this->device_model->get_value<std::string>(ControllerComponentVariables::AlignedDataTxEndedMeasurands)),
            timestamp,
            this->device_model->get_optional_value<bool>(ControllerComponentVariables::SampledDataSignReadings)
                .value_or(false),
            this->device_model->get_optional_value<bool>(ControllerComponentVariables::AlignedDataSignReadings)
                .value_or(false)));

        if (meter_values.value().empty()) {
            meter_values.reset();
        }
    } catch (const DatabaseException& e) {
        EVLOG_warning << "Could not get metervalues of transaction: " << e.what();
    }

    // E07.FR.02 The field idToken is provided when the authorization of the transaction has been ended
    const std::optional<IdToken> transaction_id_token =
        trigger_reason == ocpp::v201::TriggerReasonEnum::StopAuthorized ? id_token : std::nullopt;

    this->transaction_event_req(TransactionEventEnum::Ended, timestamp, enhanced_transaction->get_transaction(),
                                trigger_reason, enhanced_transaction->get_seq_no(), std::nullopt, std::nullopt,
                                transaction_id_token, meter_values, std::nullopt, this->is_offline(), std::nullopt);

    evse_handle.release_transaction();

    bool send_reset = false;
    if (this->reset_scheduled) {
        // Check if this evse needs to be reset or set to inoperative.
        if (!this->reset_scheduled_evseids.empty()) {
            // There is an evse id in the 'reset scheduled' list, it needs to be
            // reset because it has finished charging.
            if (this->reset_scheduled_evseids.find(evse_id) != this->reset_scheduled_evseids.end()) {
                send_reset = true;
            }
        } else {
            // No evse id is given, whole charging station needs a reset. Wait
            // for last evse id to stop charging.
            bool is_charging = false;
            for (auto const& evse : *this->evse_manager) {
                if (evse.has_active_transaction()) {
                    is_charging = true;
                    break;
                }
            }

            if (is_charging) {
                this->set_evse_connectors_unavailable(evse_handle, false);
            } else {
                send_reset = true;
            }
        }
    }

    if (send_reset) {
        // Reset evse.
        if (reset_scheduled_evseids.empty()) {
            // This was the last evse that was charging, whole charging station
            // should be rest, send reset.
            this->callbacks.reset_callback(std::nullopt, ResetEnum::OnIdle);
            this->reset_scheduled = false;
        } else {
            // Reset evse that just stopped the transaction.
            this->callbacks.reset_callback(evse_id, ResetEnum::OnIdle);
            // Remove evse id that is just reset.
            this->reset_scheduled_evseids.erase(evse_id);

            // Check if there are more evse's that should be reset.
            if (reset_scheduled_evseids.empty()) {
                // No other evse's should be reset
                this->reset_scheduled = false;
            }
        }

        this->reset_scheduled_evseids.erase(evse_id);
    }

    this->handle_scheduled_change_availability_requests(evse_id);
    this->handle_scheduled_change_availability_requests(0);
}

void ChargePoint::on_session_finished(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::PlugOut);
}

void ChargePoint::on_authorized(const int32_t evse_id, const int32_t connector_id, const IdToken& id_token) {
    auto& evse = this->evse_manager->get_evse(evse_id);
    if (!evse.has_active_transaction()) {
        // nothing to report in case transaction is not yet open
        return;
    }

    std::unique_ptr<EnhancedTransaction>& transaction = evse.get_transaction();
    if (transaction->id_token_sent) {
        // if transactions id_token_sent is set, it is assumed it has already been reported
        return;
    }

    // set id_token of enhanced_transaction and send TransactionEvent(Updated) with id_token
    transaction->set_id_token_sent();
    this->transaction_event_req(TransactionEventEnum::Updated, ocpp::DateTime(), transaction->get_transaction(),
                                TriggerReasonEnum::Authorized, transaction->get_seq_no(), std::nullopt, std::nullopt,
                                id_token, std::nullopt, std::nullopt, this->is_offline(), std::nullopt);
}

void ChargePoint::on_meter_value(const int32_t evse_id, const MeterValue& meter_value) {
    if (evse_id == 0) {
        // if evseId = 0 then store in the chargepoint metervalues
        this->aligned_data_evse0.set_values(meter_value);
    } else {
        this->evse_manager->get_evse(evse_id).on_meter_value(meter_value);
        this->update_dm_evse_power(evse_id, meter_value);
    }
}

std::string ChargePoint::get_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                                  const std::optional<IdToken> id_token,
                                                  const std::optional<CiString<64>> customer_identifier) {
    std::stringstream s;

    // Retrieve possible customer information from application that uses this library
    if (this->callbacks.get_customer_information_callback.has_value()) {
        s << this->callbacks.get_customer_information_callback.value()(customer_certificate, id_token,
                                                                       customer_identifier);
    }

    // Retrieve information from auth cache
    if (id_token.has_value()) {
        const auto hashed_id_token = utils::generate_token_hash(id_token.value());
        try {
            const auto entry = this->database_handler->authorization_cache_get_entry(hashed_id_token);
            if (entry.has_value()) {
                s << "Hashed id_token stored in cache: " + hashed_id_token + "\n";
                s << "IdTokenInfo: " << entry.value();
            }
        } catch (const DatabaseException& e) {
            EVLOG_warning << "Could not get authorization cache entry from database";
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        }
    }

    return s.str();
}

void ChargePoint::clear_customer_information(const std::optional<CertificateHashDataType> customer_certificate,
                                             const std::optional<IdToken> id_token,
                                             const std::optional<CiString<64>> customer_identifier) {
    if (this->callbacks.clear_customer_information_callback.has_value()) {
        this->callbacks.clear_customer_information_callback.value()(customer_certificate, id_token,
                                                                    customer_identifier);
    }

    if (id_token.has_value()) {
        const auto hashed_id_token = utils::generate_token_hash(id_token.value());
        try {
            this->database_handler->authorization_cache_delete_entry(hashed_id_token);
        } catch (const DatabaseException& e) {
            EVLOG_error << "Could not delete from table: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Exception while deleting from auth cache table: " << e.what();
        }
        this->update_authorization_cache_size();
    }
}

void ChargePoint::configure_message_logging_format(const std::string& message_log_path) {
    auto log_formats = this->device_model->get_value<std::string>(ControllerComponentVariables::LogMessagesFormat);
    bool log_to_console = log_formats.find("console") != log_formats.npos;
    bool detailed_log_to_console = log_formats.find("console_detailed") != log_formats.npos;
    bool log_to_file = log_formats.find("log") != log_formats.npos;
    bool log_to_html = log_formats.find("html") != log_formats.npos;
    bool log_security = log_formats.find("security") != log_formats.npos;
    bool session_logging = log_formats.find("session_logging") != log_formats.npos;
    bool message_callback = log_formats.find("callback") != log_formats.npos;
    std::function<void(const std::string& message, MessageDirection direction)> logging_callback = nullptr;
    bool log_rotation =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::LogRotation).value_or(false);
    bool log_rotation_date_suffix =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::LogRotationDateSuffix)
            .value_or(false);
    uint64_t log_rotation_maximum_file_size =
        this->device_model->get_optional_value<uint64_t>(ControllerComponentVariables::LogRotationMaximumFileSize)
            .value_or(0);
    uint64_t log_rotation_maximum_file_count =
        this->device_model->get_optional_value<uint64_t>(ControllerComponentVariables::LogRotationMaximumFileCount)
            .value_or(0);

    if (message_callback) {
        logging_callback = this->callbacks.ocpp_messages_callback.value_or(nullptr);
    }

    if (log_rotation) {
        this->logging = std::make_shared<ocpp::MessageLogging>(
            !log_formats.empty(), message_log_path, "libocpp_201", log_to_console, detailed_log_to_console, log_to_file,
            log_to_html, log_security, session_logging, logging_callback,
            ocpp::LogRotationConfig(log_rotation_date_suffix, log_rotation_maximum_file_size,
                                    log_rotation_maximum_file_count));
    } else {
        this->logging = std::make_shared<ocpp::MessageLogging>(
            !log_formats.empty(), message_log_path, DateTime().to_rfc3339(), log_to_console, detailed_log_to_console,
            log_to_file, log_to_html, log_security, session_logging, logging_callback);
    }
}

void ChargePoint::on_unavailable(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::Unavailable);
}

void ChargePoint::on_enabled(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::UnavailableCleared);
}

void ChargePoint::on_faulted(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::Error);
}

void ChargePoint::on_fault_cleared(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::ErrorCleared);
}

void ChargePoint::on_reserved(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::Reserve);
}

bool ChargePoint::on_charging_state_changed(const uint32_t evse_id, const ChargingStateEnum charging_state,
                                            const TriggerReasonEnum trigger_reason) {
    auto& evse = this->evse_manager->get_evse(evse_id);

    std::unique_ptr<EnhancedTransaction>& transaction = evse.get_transaction();
    if (transaction == nullptr) {
        EVLOG_warning << "Can not change charging state: no transaction for evse id " << evse_id;
        return false;
    }

    if (transaction->chargingState == charging_state) {
        EVLOG_debug << "Trying to send charging state changed without actual change, dropping message";
    } else {
        transaction->chargingState = charging_state;
        this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction->get_transaction(),
                                    trigger_reason, transaction->get_seq_no(), std::nullopt, std::nullopt, std::nullopt,
                                    std::nullopt, std::nullopt, this->is_offline(), std::nullopt);
    }
    return true;
}

std::optional<std::string> ChargePoint::get_evse_transaction_id(int32_t evse_id) {
    auto& evse = this->evse_manager->get_evse(evse_id);
    if (!evse.has_active_transaction()) {
        return std::nullopt;
    }
    return evse.get_transaction()->transactionId.get();
}

AuthorizeResponse ChargePoint::validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                              const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) {
    // TODO(piet): C01.FR.14
    // TODO(piet): C01.FR.15
    // TODO(piet): C01.FR.16
    // TODO(piet): C01.FR.17

    // TODO(piet): C10.FR.06

    AuthorizeResponse response;

    // C03.FR.01 && C05.FR.01: We SHALL NOT send an authorize reqeust for IdTokenType Central
    if (id_token.type == IdTokenEnum::Central or
        !this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCtrlrEnabled).value_or(true)) {
        response.idTokenInfo.status = AuthorizationStatusEnum::Accepted;
        return response;
    }

    // C07: Authorization using contract certificates
    if (id_token.type == IdTokenEnum::eMAID) {
        // Temporary variable that is set to true to avoid immediate response to allow the local auth list
        // or auth cache to be tried
        bool try_local_auth_list_or_cache = false;
        bool forwarded_to_csms = false;

        // If OCSP data is provided as argument, use it
        if (this->connectivity_manager->is_websocket_connected() and ocsp_request_data.has_value()) {
            EVLOG_info << "Online: Pass provided OCSP data to CSMS";
            response = this->authorize_req(id_token, std::nullopt, ocsp_request_data);
            forwarded_to_csms = true;
        } else if (certificate.has_value()) {
            // First try to validate the contract certificate locally
            CertificateValidationResult local_verify_result =
                this->evse_security->verify_certificate(certificate.value().get(), ocpp::LeafCertificateType::MO);
            EVLOG_info << "Local contract validation result: " << local_verify_result;

            bool central_contract_validation_allowed =
                this->device_model
                    ->get_optional_value<bool>(ControllerComponentVariables::CentralContractValidationAllowed)
                    .value_or(true);
            bool contract_validation_offline =
                this->device_model->get_optional_value<bool>(ControllerComponentVariables::ContractValidationOffline)
                    .value_or(true);
            bool local_authorize_offline =
                this->device_model->get_optional_value<bool>(ControllerComponentVariables::LocalAuthorizeOffline)
                    .value_or(true);

            // C07.FR.01: When CS is online, it shall send an AuthorizeRequest
            // C07.FR.02: The AuthorizeRequest shall at least contain the OCSP data
            // TODO: local validation results are ignored if response is based only on OCSP data, is that acceptable?
            if (this->connectivity_manager->is_websocket_connected()) {
                // If no OCSP data was provided, check for a contract root
                if (local_verify_result == CertificateValidationResult::IssuerNotFound) {
                    // C07.FR.06: Pass contract validation to CSMS when no contract root is found
                    if (central_contract_validation_allowed) {
                        EVLOG_info << "Online: No local contract root found. Pass contract validation to CSMS";
                        response = this->authorize_req(id_token, certificate, std::nullopt);
                        forwarded_to_csms = true;
                    } else {
                        EVLOG_warning << "Online: Central Contract Validation not allowed";
                        response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
                    }
                } else {
                    // Try to generate the OCSP data from the certificate chain and use that
                    const auto generated_ocsp_request_data_list = ocpp::evse_security_conversions::to_ocpp_v201(
                        this->evse_security->get_mo_ocsp_request_data(certificate.value()));
                    if (generated_ocsp_request_data_list.size() > 0) {
                        EVLOG_info << "Online: Pass generated OCSP data to CSMS";
                        response = this->authorize_req(id_token, std::nullopt, generated_ocsp_request_data_list);
                        forwarded_to_csms = true;
                    } else {
                        if (central_contract_validation_allowed) {
                            EVLOG_info << "Online: OCSP data could not be generated. Pass contract validation to CSMS";
                            response = this->authorize_req(id_token, certificate, std::nullopt);
                            forwarded_to_csms = true;
                        } else {
                            EVLOG_warning
                                << "Online: OCSP data could not be generated and CentralContractValidation not allowed";
                            response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
                        }
                    }
                }
            } else { // Offline
                // C07.FR.08: CS shall try to validate the contract locally
                if (contract_validation_offline) {
                    EVLOG_info << "Offline: contract " << local_verify_result;
                    switch (local_verify_result) {
                    // C07.FR.09: CS shall lookup the eMAID in Local Auth List or Auth Cache when
                    // local validation succeeded
                    case CertificateValidationResult::Valid:
                        // In C07.FR.09 LocalAuthorizeOffline is mentioned, this seems to be a generic config item
                        // that applies to Local Auth List and Auth Cache, but since there are no requirements about
                        // it, lets check it here
                        if (local_authorize_offline) {
                            try_local_auth_list_or_cache = true;
                        } else {
                            // No requirement states what to do when ContractValidationOffline is true
                            // and LocalAuthorizeOffline is false
                            response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
                            response.certificateStatus = AuthorizeCertificateStatusEnum::Accepted;
                        }
                        break;
                    case CertificateValidationResult::Expired:
                        response.idTokenInfo.status = AuthorizationStatusEnum::Expired;
                        response.certificateStatus = AuthorizeCertificateStatusEnum::CertificateExpired;
                        break;
                    default:
                        response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
                        break;
                    }
                } else {
                    // C07.FR.07: CS shall not allow charging
                    response.idTokenInfo.status = AuthorizationStatusEnum::NotAtThisTime;
                }
            }
        } else {
            EVLOG_warning << "Can not validate eMAID without certificate chain";
            response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
        }
        if (forwarded_to_csms) {
            // AuthorizeRequest sent to CSMS, let's show the results
            EVLOG_info << "CSMS idToken status: " << response.idTokenInfo.status;
            if (response.certificateStatus.has_value()) {
                EVLOG_info << "CSMS certificate status: " << response.certificateStatus.value();
            }
        }
        // For eMAID, we will respond here, unless the local auth list or auth cache is tried
        if (!try_local_auth_list_or_cache) {
            return response;
        }
    }

    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        std::optional<IdTokenInfo> id_token_info = std::nullopt;
        try {
            id_token_info = this->database_handler->get_local_authorization_list_entry(id_token);
        } catch (const DatabaseException& e) {
            EVLOG_warning << "Could not request local authorization list entry: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while requesting IdTokenInfo: " << e.what();
        }

        if (id_token_info.has_value()) {
            if (id_token_info.value().status == AuthorizationStatusEnum::Accepted) {
                // C14.FR.02: If found in local list we shall start charging without an AuthorizeRequest
                EVLOG_info << "Found valid entry in local authorization list";
                response.idTokenInfo = id_token_info.value();
            } else if (this->device_model
                           ->get_optional_value<bool>(ControllerComponentVariables::DisableRemoteAuthorization)
                           .value_or(false)) {
                EVLOG_info << "Found invalid entry in local authorization list but not sending Authorize.req because "
                              "RemoteAuthorization is disabled";
                response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
            } else if (this->connectivity_manager->is_websocket_connected()) {
                // C14.FR.03: If a value found but not valid we shall send an authorize request
                EVLOG_info << "Found invalid entry in local authorization list: Sending Authorize.req";
                response = this->authorize_req(id_token, certificate, ocsp_request_data);
            } else {
                // errata C13.FR.04: even in the offline state we should not authorize if present (and not accepted)
                EVLOG_info << "Found invalid entry in local authorization list whilst offline: Not authorized";
                response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
            }
            return response;
        }
    }

    const auto hashed_id_token = utils::generate_token_hash(id_token);
    const auto auth_cache_enabled =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCacheCtrlrEnabled)
            .value_or(false);

    if (auth_cache_enabled) {
        try {
            const auto cache_entry = this->database_handler->authorization_cache_get_entry(hashed_id_token);
            if (cache_entry.has_value()) {
                if ((cache_entry.value().cacheExpiryDateTime.has_value() and
                     cache_entry.value().cacheExpiryDateTime.value().to_time_point() < DateTime().to_time_point())) {
                    EVLOG_info
                        << "Found valid entry in AuthCache but expiry date passed: Removing from cache and sending "
                           "new request";
                    this->database_handler->authorization_cache_delete_entry(hashed_id_token);
                    this->update_authorization_cache_size();
                } else if (this->device_model->get_value<bool>(ControllerComponentVariables::LocalPreAuthorize) and
                           cache_entry.value().status == AuthorizationStatusEnum::Accepted) {
                    EVLOG_info << "Found valid entry in AuthCache";
                    this->database_handler->authorization_cache_update_last_used(hashed_id_token);
                    response.idTokenInfo = cache_entry.value();
                    return response;
                } else if (this->device_model
                               ->get_optional_value<bool>(ControllerComponentVariables::AuthCacheDisablePostAuthorize)
                               .value_or(false)) {
                    EVLOG_info << "Found invalid entry in AuthCache: Not sending new request because "
                                  "AuthCacheDisablePostAuthorize is enabled";
                    response.idTokenInfo = cache_entry.value();
                    return response;
                } else {
                    EVLOG_info << "Found invalid entry in AuthCache: Sending new request";
                }
            }
        } catch (const DatabaseException& e) {
            EVLOG_error << "Database Error: " << e.what();
        } catch (const json::exception& e) {
            EVLOG_warning << "Could not parse data of IdTokenInfo: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_error << "Unknown Error while parsing IdTokenInfo: " << e.what();
        }
    }

    if (!this->connectivity_manager->is_websocket_connected() and
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::OfflineTxForUnknownIdEnabled)
            .value_or(false)) {
        EVLOG_info << "Offline authorization due to OfflineTxForUnknownIdEnabled being enabled";
        response.idTokenInfo.status = AuthorizationStatusEnum::Accepted;
        return response;
    }

    // When set to true this instructs the Charging Station to not issue any AuthorizationRequests, but only use
    // Authorization Cache and Local Authorization List to determine validity of idTokens.
    if (!this->device_model->get_optional_value<bool>(ControllerComponentVariables::DisableRemoteAuthorization)
             .value_or(false)) {
        response = this->authorize_req(id_token, certificate, ocsp_request_data);

        if (auth_cache_enabled) {
            try {
                this->database_handler->authorization_cache_insert_entry(hashed_id_token, response.idTokenInfo);
            } catch (const DatabaseException& e) {
                EVLOG_error << "Could not insert into authorization cache entry: " << e.what();
            }
            this->trigger_authorization_cache_cleanup();
        }

        return response;
    }

    EVLOG_info << "Not sending Authorize.req because RemoteAuthorization is disabled";

    response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
    return response;
}

void ChargePoint::on_event(const std::vector<EventData>& events) {
    this->notify_event_req(events);
}

void ChargePoint::on_log_status_notification(UploadLogStatusEnum status, int32_t requestId) {
    LogStatusNotificationRequest request;
    request.status = status;
    request.requestId = requestId;

    // Store for use by the triggerMessage
    this->upload_log_status = status;
    this->upload_log_status_id = requestId;

    ocpp::Call<LogStatusNotificationRequest> call(request, this->message_queue->createMessageId());
    this->send<LogStatusNotificationRequest>(call);
}

void ChargePoint::on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                    const std::optional<bool>& critical) {
    auto critical_security_event = true;
    if (critical.has_value()) {
        critical_security_event = critical.value();
    } else {
        critical_security_event = utils::is_critical(event_type);
    }
    this->security_event_notification_req(event_type, tech_info, false, critical_security_event);
}

void ChargePoint::on_variable_changed(const SetVariableData& set_variable_data) {
    this->handle_variable_changed(set_variable_data);
}

bool ChargePoint::send(CallError call_error) {
    this->message_queue->push(call_error);
    return true;
}

void ChargePoint::initialize(const std::map<int32_t, int32_t>& evse_connector_structure,
                             const std::string& message_log_path) {
    this->device_model->check_integrity(evse_connector_structure);
    this->database_handler->open_connection();
    this->component_state_manager = std::make_shared<ComponentStateManager>(
        evse_connector_structure, database_handler,
        [this](auto evse_id, auto connector_id, auto status, bool initiated_by_trigger_message) {
            this->update_dm_availability_state(evse_id, connector_id, status);
            if (this->connectivity_manager == nullptr || !this->connectivity_manager->is_websocket_connected() ||
                this->registration_status != RegistrationStatusEnum::Accepted) {
                return false;
            } else {
                this->status_notification_req(evse_id, connector_id, status, initiated_by_trigger_message);
                return true;
            }
        });
    if (this->callbacks.cs_effective_operative_status_changed_callback.has_value()) {
        this->component_state_manager->set_cs_effective_availability_changed_callback(
            this->callbacks.cs_effective_operative_status_changed_callback.value());
    }
    if (this->callbacks.evse_effective_operative_status_changed_callback.has_value()) {
        this->component_state_manager->set_evse_effective_availability_changed_callback(
            this->callbacks.evse_effective_operative_status_changed_callback.value());
    }
    this->component_state_manager->set_connector_effective_availability_changed_callback(
        this->callbacks.connector_effective_operative_status_changed_callback);

    auto transaction_meter_value_callback = [this](const MeterValue& _meter_value, EnhancedTransaction& transaction) {
        if (_meter_value.sampledValue.empty() or !_meter_value.sampledValue.at(0).context.has_value()) {
            EVLOG_info << "Not sending MeterValue due to no values";
            return;
        }

        auto type = _meter_value.sampledValue.at(0).context.value();
        if (type != ReadingContextEnum::Sample_Clock and type != ReadingContextEnum::Sample_Periodic) {
            EVLOG_info << "Not sending MeterValue due to wrong context";
            return;
        }

        const auto filter_vec = utils::get_measurands_vec(this->device_model->get_value<std::string>(
            type == ReadingContextEnum::Sample_Clock ? ControllerComponentVariables::AlignedDataMeasurands
                                                     : ControllerComponentVariables::SampledDataTxUpdatedMeasurands));

        const auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(_meter_value, filter_vec);

        if (!filtered_meter_value.sampledValue.empty()) {
            const auto trigger = type == ReadingContextEnum::Sample_Clock ? TriggerReasonEnum::MeterValueClock
                                                                          : TriggerReasonEnum::MeterValuePeriodic;
            this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction, trigger,
                                        transaction.get_seq_no(), std::nullopt, std::nullopt, std::nullopt,
                                        std::vector<MeterValue>(1, filtered_meter_value), std::nullopt,
                                        this->is_offline(), std::nullopt);
        }
    };

    this->evse_manager = std::make_unique<EvseManager>(
        evse_connector_structure, *this->device_model, this->database_handler, component_state_manager,
        transaction_meter_value_callback, this->callbacks.pause_charging_callback);

    this->smart_charging_handler =
        std::make_shared<SmartChargingHandler>(*this->evse_manager, this->device_model, this->database_handler);

    this->configure_message_logging_format(message_log_path);
    this->monitoring_updater.start_monitoring();

    this->auth_cache_cleanup_thread = std::thread(&ChargePoint::cache_cleanup_handler, this);

    this->connectivity_manager =
        std::make_unique<ConnectivityManager>(*this->device_model, this->evse_security, this->logging,
                                              std::bind(&ChargePoint::message_callback, this, std::placeholders::_1));

    this->connectivity_manager->set_websocket_connected_callback(
        std::bind(&ChargePoint::websocket_connected_callback, this, std::placeholders::_1));
    this->connectivity_manager->set_websocket_disconnected_callback(
        std::bind(&ChargePoint::websocket_disconnected_callback, this));
    this->connectivity_manager->set_websocket_connection_failed_callback(
        std::bind(&ChargePoint::websocket_connection_failed, this, std::placeholders::_1));

    if (this->callbacks.configure_network_connection_profile_callback.has_value()) {
        this->connectivity_manager->set_configure_network_connection_profile_callback(
            this->callbacks.configure_network_connection_profile_callback.value());
    }
}

void ChargePoint::init_certificate_expiration_check_timers() {

    // Timers started with initial delays; callback functions are supposed to re-schedule on their own!

    // Client Certificate only needs to be checked for SecurityProfile 3; if SecurityProfile changes, timers get
    // re-initialized at reconnect
    if (this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile) == 3) {
        this->client_certificate_expiration_check_timer.timeout(std::chrono::seconds(
            this->device_model
                ->get_optional_value<int>(ControllerComponentVariables::ClientCertificateExpireCheckInitialDelaySeconds)
                .value_or(60)));
    }

    // V2G Certificate timer is started in any case; condition (V2GCertificateInstallationEnabled) is validated in
    // callback (ChargePoint::scheduled_check_v2g_certificate_expiration)
    this->v2g_certificate_expiration_check_timer.timeout(std::chrono::seconds(
        this->device_model
            ->get_optional_value<int>(ControllerComponentVariables::V2GCertificateExpireCheckInitialDelaySeconds)
            .value_or(60)));
}

void ChargePoint::remove_network_connection_profiles_below_actual_security_profile() {
    // Remove all the profiles that are a lower security level than security_level
    const auto security_level = this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile);

    auto network_connection_profiles = json::parse(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles));

    auto is_lower_security_level = [security_level](const SetNetworkProfileRequest& item) {
        return item.connectionData.securityProfile < security_level;
    };

    network_connection_profiles.erase(
        std::remove_if(network_connection_profiles.begin(), network_connection_profiles.end(), is_lower_security_level),
        network_connection_profiles.end());

    this->device_model->set_value(ControllerComponentVariables::NetworkConnectionProfiles.component,
                                  ControllerComponentVariables::NetworkConnectionProfiles.variable.value(),
                                  AttributeEnum::Actual, network_connection_profiles.dump(),
                                  VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);

    // Update the NetworkConfigurationPriority so only remaining profiles are in there
    const auto network_priority = ocpp::get_vector_from_csv(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority));

    auto in_network_profiles = [&network_connection_profiles](const std::string& item) {
        auto is_same_slot = [&item](const SetNetworkProfileRequest& profile) {
            return std::to_string(profile.configurationSlot) == item;
        };
        return std::any_of(network_connection_profiles.begin(), network_connection_profiles.end(), is_same_slot);
    };

    std::string new_network_priority;
    for (const auto& item : network_priority) {
        if (in_network_profiles(item)) {
            if (!new_network_priority.empty()) {
                new_network_priority += ',';
            }
            new_network_priority += item;
        }
    }

    this->device_model->set_value(ControllerComponentVariables::NetworkConfigurationPriority.component,
                                  ControllerComponentVariables::NetworkConfigurationPriority.variable.value(),
                                  AttributeEnum::Actual, new_network_priority,
                                  VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
}

void ChargePoint::handle_message(const EnhancedMessage<v201::MessageType>& message) {
    const auto& json_message = message.message;
    switch (message.messageType) {
    case MessageType::BootNotificationResponse:
        this->handle_boot_notification_response(json_message);
        break;
    case MessageType::SetVariables:
        this->handle_set_variables_req(json_message);
        break;
    case MessageType::GetVariables:
        this->handle_get_variables_req(message);
        break;
    case MessageType::GetBaseReport:
        this->handle_get_base_report_req(json_message);
        break;
    case MessageType::GetReport:
        this->handle_get_report_req(message);
        break;
    case MessageType::Reset:
        this->handle_reset_req(json_message);
        break;
    case MessageType::SetNetworkProfile:
        this->handle_set_network_profile_req(json_message);
        break;
    case MessageType::ChangeAvailability:
        this->handle_change_availability_req(json_message);
        break;
    case MessageType::TransactionEventResponse:
        this->handle_transaction_event_response(message);
        break;
    case MessageType::RequestStartTransaction:
        this->handle_remote_start_transaction_request(json_message);
        break;
    case MessageType::RequestStopTransaction:
        this->handle_remote_stop_transaction_request(json_message);
        break;
    case MessageType::DataTransfer:
        this->handle_data_transfer_req(json_message);
        break;
    case MessageType::GetLog:
        this->handle_get_log_req(json_message);
        break;
    case MessageType::ClearCache:
        this->handle_clear_cache_req(json_message);
        break;
    case MessageType::UpdateFirmware:
        this->handle_firmware_update_req(json_message);
        break;
    case MessageType::UnlockConnector:
        this->handle_unlock_connector(json_message);
        break;
    case MessageType::TriggerMessage:
        this->handle_trigger_message(json_message);
        break;
    case MessageType::SignCertificateResponse:
        this->handle_sign_certificate_response(json_message);
        break;
    case MessageType::HeartbeatResponse:
        this->handle_heartbeat_response(json_message);
        break;
    case MessageType::SendLocalList:
        this->handle_send_local_authorization_list_req(json_message);
        break;
    case MessageType::GetLocalListVersion:
        this->handle_get_local_authorization_list_version_req(json_message);
        break;
    case MessageType::CertificateSigned:
        this->handle_certificate_signed_req(json_message);
        break;
    case MessageType::GetTransactionStatus:
        this->handle_get_transaction_status(json_message);
        break;
    case MessageType::GetInstalledCertificateIds:
        this->handle_get_installed_certificate_ids_req(json_message);
        break;
    case MessageType::InstallCertificate:
        this->handle_install_certificate_req(json_message);
        break;
    case MessageType::DeleteCertificate:
        this->handle_delete_certificate_req(json_message);
        break;
    case MessageType::CustomerInformation:
        this->handle_customer_information_req(json_message);
        break;
    case MessageType::SetChargingProfile:
        this->handle_set_charging_profile_req(json_message);
        break;
    case MessageType::ClearChargingProfile:
        this->handle_clear_charging_profile_req(json_message);
        break;
    case MessageType::GetChargingProfiles:
        this->handle_get_charging_profiles_req(json_message);
        break;
    case MessageType::GetCompositeSchedule:
        this->handle_get_composite_schedule_req(json_message);
        break;
    case MessageType::SetMonitoringBase:
        this->handle_set_monitoring_base_req(json_message);
        break;
    case MessageType::SetMonitoringLevel:
        this->handle_set_monitoring_level_req(json_message);
        break;
    case MessageType::SetVariableMonitoring:
        this->handle_set_variable_monitoring_req(message);
        break;
    case MessageType::GetMonitoringReport:
        this->handle_get_monitoring_report_req(json_message);
        break;
    case MessageType::ClearVariableMonitoring:
        this->handle_clear_variable_monitoring_req(json_message);
        break;
    default:
        if (message.messageTypeId == MessageTypeId::CALL) {
            const auto call_error = CallError(message.uniqueId, "NotImplemented", "", json({}));
            this->send(call_error);
        }
        break;
    }
}

void ChargePoint::message_callback(const std::string& message) {
    EnhancedMessage<v201::MessageType> enhanced_message;
    try {
        enhanced_message = this->message_queue->receive(message);
    } catch (const json::exception& e) {
        this->logging->central_system("Unknown", message);
        EVLOG_error << "JSON exception during reception of message: " << e.what();
        this->send(CallError(MessageId("-1"), "RpcFrameworkError", e.what(), json({})));
        return;
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(MessageId("-1"), "FormationViolation", e.what(), json({}));
        this->send(call_error);
        return;
    }

    enhanced_message.message_size = message.size();
    auto json_message = enhanced_message.message;
    this->logging->central_system(conversions::messagetype_to_string(enhanced_message.messageType), message);
    try {
        if (this->registration_status == RegistrationStatusEnum::Accepted) {
            this->handle_message(enhanced_message);
        } else if (this->registration_status == RegistrationStatusEnum::Pending) {
            if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                this->handle_boot_notification_response(json_message);
            } else {
                // TODO(piet): Check what kind of messages we should accept in Pending state
                if (enhanced_message.messageType == MessageType::GetVariables or
                    enhanced_message.messageType == MessageType::SetVariables or
                    enhanced_message.messageType == MessageType::GetBaseReport or
                    enhanced_message.messageType == MessageType::GetReport or
                    enhanced_message.messageType == MessageType::NotifyReportResponse or
                    enhanced_message.messageType == MessageType::TriggerMessage) {
                    this->handle_message(enhanced_message);
                } else if (enhanced_message.messageType == MessageType::RequestStartTransaction) {
                    // Send rejected: B02.FR.05
                    RequestStartTransactionResponse response;
                    response.status = RequestStartStopStatusEnum::Rejected;
                    const ocpp::CallResult<RequestStartTransactionResponse> call_result(response,
                                                                                        enhanced_message.uniqueId);
                    this->send<RequestStartTransactionResponse>(call_result);
                } else if (enhanced_message.messageType == MessageType::RequestStopTransaction) {
                    // Send rejected: B02.FR.05
                    RequestStopTransactionResponse response;
                    response.status = RequestStartStopStatusEnum::Rejected;
                    const ocpp::CallResult<RequestStopTransactionResponse> call_result(response,
                                                                                       enhanced_message.uniqueId);
                    this->send<RequestStopTransactionResponse>(call_result);
                } else {
                    std::string const call_error_message =
                        "Received invalid MessageType: " +
                        conversions::messagetype_to_string(enhanced_message.messageType) +
                        " from CSMS while in state Pending";
                    EVLOG_warning << call_error_message;
                    // B02.FR.09 send CALLERROR SecurityError
                    const auto call_error =
                        CallError(enhanced_message.uniqueId, "SecurityError", call_error_message, json({}));
                    this->send(call_error);
                }
            }
        } else if (this->registration_status == RegistrationStatusEnum::Rejected) {
            if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                this->handle_boot_notification_response(json_message);
            } else if (enhanced_message.messageType == MessageType::TriggerMessage) {
                Call<TriggerMessageRequest> call(json_message);
                if (call.msg.requestedMessage == MessageTriggerEnum::BootNotification) {
                    this->handle_message(enhanced_message);
                } else {
                    const auto error_message =
                        "Received TriggerMessage with requestedMessage != BootNotification before "
                        "having received an accepted BootNotificationResponse";
                    EVLOG_warning << error_message;
                    const auto call_error = CallError(enhanced_message.uniqueId, "SecurityError", "", json({}));
                    this->send(call_error);
                }
            } else {
                const auto error_message = "Received other message than BootNotificationResponse before "
                                           "having received an accepted BootNotificationResponse";
                EVLOG_warning << error_message;
                const auto call_error = CallError(enhanced_message.uniqueId, "SecurityError", "", json({}, true));
                this->send(call_error);
            }
        }
    } catch (const EvseOutOfRangeException& e) {
        EVLOG_error << "Exception during handling of message: " << e.what();
        auto call_error = CallError(enhanced_message.uniqueId, "OccurrenceConstraintViolation", e.what(), json({}));
        this->send(call_error);
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->send(call_error);
    } catch (json::exception& e) {
        EVLOG_error << "JSON exception during handling of message: " << e.what();
        if (json_message.is_array() && json_message.size() > MESSAGE_ID) {
            auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
            this->send(call_error);
        }
    }
}

MeterValue ChargePoint::get_latest_meter_value_filtered(const MeterValue& meter_value, ReadingContextEnum context,
                                                        const RequiredComponentVariable& component_variable) {
    auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(
        meter_value, utils::get_measurands_vec(this->device_model->get_value<std::string>(component_variable)));
    for (auto& sampled_value : filtered_meter_value.sampledValue) {
        sampled_value.context = context;
    }
    return filtered_meter_value;
}

void ChargePoint::change_all_connectors_to_unavailable_for_firmware_update() {
    ChangeAvailabilityResponse response;
    response.status = ChangeAvailabilityStatusEnum::Scheduled;

    ChangeAvailabilityRequest msg;
    msg.operationalStatus = OperationalStatusEnum::Inoperative;

    const auto transaction_active = this->any_transaction_active(std::nullopt);

    if (!transaction_active) {
        // execute change availability if possible
        for (auto& evse : *this->evse_manager) {
            if (!evse.has_active_transaction()) {
                this->set_evse_connectors_unavailable(evse, false);
            }
        }
        // Check succeeded, trigger the callback if needed
        if (this->callbacks.all_connectors_unavailable_callback.has_value() &&
            this->are_all_connectors_effectively_inoperative()) {
            this->callbacks.all_connectors_unavailable_callback.value()();
        }
    } else if (response.status == ChangeAvailabilityStatusEnum::Scheduled) {
        // put all EVSEs to unavailable that do not have active transaction
        for (auto& evse : *this->evse_manager) {
            if (!evse.has_active_transaction()) {
                this->set_evse_connectors_unavailable(evse, false);
            } else {
                EVSE e;
                e.id = evse.get_id();
                msg.evse = e;
                this->scheduled_change_availability_requests[evse.get_id()] = {msg, false};
            }
        }
    }
}

void ChargePoint::restore_all_connector_states() {
    for (auto& evse : *this->evse_manager) {
        uint32_t number_of_connectors = evse.get_number_of_connectors();

        for (uint32_t i = 1; i <= number_of_connectors; ++i) {
            evse.restore_connector_operative_status(static_cast<int32_t>(i));
        }
    }
}

void ChargePoint::update_authorization_cache_size() {
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    if (auth_cache_size.variable.has_value()) {
        try {
            auto size = this->database_handler->authorization_cache_get_binary_size();
            this->device_model->set_read_only_value(auth_cache_size.component, auth_cache_size.variable.value(),
                                                    AttributeEnum::Actual, std::to_string(size),
                                                    VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
        } catch (const DatabaseException& e) {
            EVLOG_warning << "Could not get authorization cache binary size from database: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_warning << "Could not get authorization cache binary size from database" << e.what();
        }
    }
}

SendLocalListStatusEnum ChargePoint::apply_local_authorization_list(const SendLocalListRequest& request) {
    auto status = SendLocalListStatusEnum::Failed;

    auto has_duplicate_in_list = [](const std::vector<AuthorizationData>& list) {
        for (auto it1 = list.begin(); it1 != list.end(); ++it1) {
            for (auto it2 = it1 + 1; it2 != list.end(); ++it2) {
                if (it1->idToken.idToken == it2->idToken.idToken and it1->idToken.type == it2->idToken.type) {
                    return true;
                }
            }
        }
        return false;
    };

    if (request.versionNumber == 0) {
        // D01.FR.18: Do nothing, not allowed, respond with failed
    } else if (request.updateType == UpdateEnum::Full) {
        if (!request.localAuthorizationList.has_value() or request.localAuthorizationList.value().empty()) {
            try {
                this->database_handler->clear_local_authorization_list();
                status = SendLocalListStatusEnum::Accepted;
            } catch (const DatabaseException& e) {
                status = SendLocalListStatusEnum::Failed;
                EVLOG_warning << "Clearing of local authorization list failed: " << e.what();
            }
        } else {
            const auto& list = request.localAuthorizationList.value();

            auto has_no_token_info = [](const AuthorizationData& item) { return !item.idTokenInfo.has_value(); };

            if (!has_duplicate_in_list(list) and
                std::find_if(list.begin(), list.end(), has_no_token_info) == list.end()) {
                try {
                    this->database_handler->clear_local_authorization_list();
                    this->database_handler->insert_or_update_local_authorization_list(list);
                    status = SendLocalListStatusEnum::Accepted;
                } catch (const DatabaseException& e) {
                    status = SendLocalListStatusEnum::Failed;
                    EVLOG_warning << "Full update of local authorization list failed (at least partially): "
                                  << e.what();
                }
            }
        }
    } else if (request.updateType == UpdateEnum::Differential) {
        if (request.versionNumber <= this->database_handler->get_local_authorization_list_version()) {
            // D01.FR.19: Do not allow version numbers smaller than current to update differentially
            status = SendLocalListStatusEnum::VersionMismatch;
        } else if (!request.localAuthorizationList.has_value() or request.localAuthorizationList.value().empty()) {
            // D01.FR.05: Do not update database with empty list, only update version number
            status = SendLocalListStatusEnum::Accepted;
        } else if (has_duplicate_in_list(request.localAuthorizationList.value())) {
            // Do nothing with duplicate in list
        } else {
            const auto& list = request.localAuthorizationList.value();
            try {
                this->database_handler->insert_or_update_local_authorization_list(list);
                status = SendLocalListStatusEnum::Accepted;
            } catch (const DatabaseException& e) {
                status = SendLocalListStatusEnum::Failed;
                EVLOG_warning << "Differential update of authorization list failed (at least partially): " << e.what();
            }
        }
    }
    return status;
}

void ChargePoint::update_aligned_data_interval() {
    auto interval =
        std::chrono::seconds(this->device_model->get_value<int>(ControllerComponentVariables::AlignedDataInterval));
    if (interval <= 0s) {
        this->aligned_meter_values_timer.stop();
        return;
    }

    this->aligned_meter_values_timer.interval_starting_from(
        [this, interval]() {
            // J01.FR.20 if AlignedDataSendDuringIdle is true and any transaction is active, don't send clock aligned
            // meter values
            if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::AlignedDataSendDuringIdle)
                    .value_or(false)) {
                for (auto const& evse : *this->evse_manager) {
                    if (evse.has_active_transaction()) {
                        return;
                    }
                }
            }

            const bool align_timestamps =
                this->device_model->get_optional_value<bool>(ControllerComponentVariables::RoundClockAlignedTimestamps)
                    .value_or(false);

            // send evseID = 0 values
            auto meter_value = get_latest_meter_value_filtered(this->aligned_data_evse0.retrieve_processed_values(),
                                                               ReadingContextEnum::Sample_Clock,
                                                               ControllerComponentVariables::AlignedDataMeasurands);

            if (!meter_value.sampledValue.empty()) {
                if (align_timestamps) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, interval);
                }
                this->meter_values_req(0, std::vector<ocpp::v201::MeterValue>(1, meter_value));
            }
            this->aligned_data_evse0.clear_values();

            for (auto& evse : *this->evse_manager) {
                if (evse.has_active_transaction()) {
                    continue;
                }

                // this will apply configured measurands and possibly reduce the entries of sampledValue
                // according to the configuration
                auto meter_value =
                    get_latest_meter_value_filtered(evse.get_idle_meter_value(), ReadingContextEnum::Sample_Clock,
                                                    ControllerComponentVariables::AlignedDataMeasurands);

                if (align_timestamps) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, interval);
                }

                if (!meter_value.sampledValue.empty()) {
                    // J01.FR.14 this is the only case where we send a MeterValue.req
                    this->meter_values_req(evse.get_id(), std::vector<ocpp::v201::MeterValue>(1, meter_value));
                    // clear the values
                }
                evse.clear_idle_meter_values();
            }
        },
        interval, std::chrono::floor<date::days>(date::utc_clock::to_sys(date::utc_clock::now())));
}

bool ChargePoint::any_transaction_active(const std::optional<EVSE>& evse) {
    if (!evse.has_value()) {
        for (auto const& evse : *this->evse_manager) {
            if (evse.has_active_transaction()) {
                return true;
            }
        }
        return false;
    }
    return this->evse_manager->get_evse(evse.value().id).has_active_transaction();
}

bool ChargePoint::is_already_in_state(const ChangeAvailabilityRequest& request) {
    // TODO: This checks against the individual status setting. What about effective/persisted status?
    if (!request.evse.has_value()) {
        // We're addressing the whole charging station
        return (this->component_state_manager->get_cs_individual_operational_status() == request.operationalStatus);
    }
    if (!request.evse.value().connectorId.has_value()) {
        // An EVSE is addressed
        return (this->component_state_manager->get_evse_individual_operational_status(request.evse.value().id) ==
                request.operationalStatus);
    }
    // A connector is being addressed
    return (this->component_state_manager->get_connector_individual_operational_status(
                request.evse.value().id, request.evse.value().connectorId.value()) == request.operationalStatus);
}

bool ChargePoint::is_valid_evse(const EVSE& evse) {
    return this->evse_manager->does_evse_exist(evse.id) and
           (!evse.connectorId.has_value() or
            this->evse_manager->get_evse(evse.id).get_number_of_connectors() >= evse.connectorId.value());
}

void ChargePoint::handle_scheduled_change_availability_requests(const int32_t evse_id) {
    if (this->scheduled_change_availability_requests.count(evse_id)) {
        EVLOG_info << "Found scheduled ChangeAvailability.req for evse_id:" << evse_id;
        const auto req = this->scheduled_change_availability_requests[evse_id].request;
        const auto persist = this->scheduled_change_availability_requests[evse_id].persist;
        if (!this->any_transaction_active(req.evse)) {
            EVLOG_info << "Changing availability of evse:" << evse_id;
            this->execute_change_availability_request(req, persist);
            this->scheduled_change_availability_requests.erase(evse_id);
            // Check succeeded, trigger the callback if needed
            if (this->callbacks.all_connectors_unavailable_callback.has_value() &&
                this->are_all_connectors_effectively_inoperative()) {
                this->callbacks.all_connectors_unavailable_callback.value()();
            }
        } else {
            EVLOG_info << "Cannot change availability because transaction is still active";
        }
    }
}

/**
 * Determine for a component variable whether it affects the Websocket Connection Options (cf.
 * get_ws_connection_options); return true if it is furthermore writable and does not require a reconnect
 *
 * @param component_variable
 * @return
 */
static bool component_variable_change_requires_websocket_option_update_without_reconnect(
    const ComponentVariable& component_variable) {

    return component_variable == ControllerComponentVariables::RetryBackOffRandomRange ||
           component_variable == ControllerComponentVariables::RetryBackOffRepeatTimes ||
           component_variable == ControllerComponentVariables::RetryBackOffWaitMinimum ||
           component_variable == ControllerComponentVariables::NetworkProfileConnectionAttempts ||
           component_variable == ControllerComponentVariables::WebSocketPingInterval;
}

void ChargePoint::handle_variable_changed(const SetVariableData& set_variable_data) {

    ComponentVariable component_variable = {set_variable_data.component, std::nullopt, set_variable_data.variable};

    if (set_variable_data.attributeType.has_value() and
        set_variable_data.attributeType.value() != AttributeEnum::Actual) {
        return;
    }

    if (component_variable == ControllerComponentVariables::BasicAuthPassword) {
        if (this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile) < 3) {
            // TODO: A01.FR.11 log the change of BasicAuth in Security Log
            this->connectivity_manager->set_websocket_authorization_key(set_variable_data.attributeValue.get());
            this->connectivity_manager->disconnect_websocket(WebsocketCloseReason::ServiceRestart);
        }
    }
    if (component_variable == ControllerComponentVariables::HeartbeatInterval and
        this->registration_status == RegistrationStatusEnum::Accepted) {
        try {
            this->heartbeat_timer.interval([this]() { this->heartbeat_req(); },
                                           std::chrono::seconds(std::stoi(set_variable_data.attributeValue.get())));
        } catch (const std::invalid_argument& e) {
            EVLOG_error << "Invalid argument exception while updating the heartbeat interval: " << e.what();
        } catch (const std::out_of_range& e) {
            EVLOG_error << "Out of range exception while updating the heartbeat interval: " << e.what();
        }
    }
    if (component_variable == ControllerComponentVariables::AlignedDataInterval) {
        this->update_aligned_data_interval();
    }

    if (component_variable_change_requires_websocket_option_update_without_reconnect(component_variable)) {
        EVLOG_debug << "Reconfigure websocket due to relevant change of ControllerComponentVariable";
        this->connectivity_manager->set_websocket_connection_options_without_reconnect();
    }

    if (component_variable == ControllerComponentVariables::MessageAttemptInterval) {
        if (component_variable.variable.has_value()) {
            this->message_queue->update_transaction_message_retry_interval(
                this->device_model->get_value<int>(ControllerComponentVariables::MessageAttemptInterval));
        }
    }

    if (component_variable == ControllerComponentVariables::MessageAttempts) {
        if (component_variable.variable.has_value()) {
            this->message_queue->update_transaction_message_attempts(
                this->device_model->get_value<int>(ControllerComponentVariables::MessageAttempts));
        }
    }

    if (component_variable == ControllerComponentVariables::MessageTimeout) {
        if (component_variable.variable.has_value()) {
            this->message_queue->update_message_timeout(
                this->device_model->get_value<int>(ControllerComponentVariables::MessageTimeout));
        }
    }

    // TODO(piet): other special handling of changed variables can be added here...
}

void ChargePoint::handle_variables_changed(const std::map<SetVariableData, SetVariableResult>& set_variable_results) {
    // iterate over set_variable_results
    for (const auto& [set_variable_data, set_variable_result] : set_variable_results) {
        if (set_variable_result.attributeStatus == SetVariableStatusEnum::Accepted) {
            EVLOG_info << set_variable_data.component.name << ":" << set_variable_data.variable.name << " changed to "
                       << set_variable_data.attributeValue.get();
            // handles required behavior specified within OCPP2.0.1 (e.g. reconnect when BasicAuthPassword has changed)
            this->handle_variable_changed(set_variable_data);
            // notifies libocpp user application that a variable has changed
            if (this->callbacks.variable_changed_callback.has_value()) {
                this->callbacks.variable_changed_callback.value()(set_variable_data);
            }
        }
    }

    // process all triggered monitors, after a possible disconnect
    this->monitoring_updater.process_triggered_monitors();
}

bool ChargePoint::validate_set_variable(const SetVariableData& set_variable_data) {
    ComponentVariable cv = {set_variable_data.component, std::nullopt, set_variable_data.variable};
    if (cv == ControllerComponentVariables::NetworkConfigurationPriority) {
        const auto network_configuration_priorities = ocpp::get_vector_from_csv(set_variable_data.attributeValue.get());
        const auto active_security_profile =
            this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile);
        for (const auto configuration_slot : network_configuration_priorities) {
            try {
                auto network_profile_opt =
                    this->connectivity_manager->get_network_connection_profile(std::stoi(configuration_slot));
                if (!network_profile_opt.has_value()) {
                    EVLOG_warning << "Could not find network profile for configurationSlot: " << configuration_slot;
                    return false;
                }

                auto network_profile = network_profile_opt.value();

                if (network_profile.securityProfile <= active_security_profile) {
                    continue;
                }

                if (network_profile.securityProfile == 3 and
                    this->evse_security
                            ->get_leaf_certificate_info(ocpp::CertificateSigningUseEnum::ChargingStationCertificate)
                            .status != ocpp::GetCertificateInfoStatus::Accepted) {
                    EVLOG_warning << "SecurityProfile of configurationSlot: " << configuration_slot
                                  << " is 3 but no CSMS Leaf Certificate is installed";
                    return false;
                }
                if (network_profile.securityProfile >= 2 and
                    !this->evse_security->is_ca_certificate_installed(ocpp::CaCertificateType::CSMS)) {
                    EVLOG_warning << "SecurityProfile of configurationSlot: " << configuration_slot
                                  << " is >= 2 but no CSMS Root Certifciate is installed";
                    return false;
                }
            } catch (const std::invalid_argument& e) {
                EVLOG_warning << "NetworkConfigurationPriority is not an integer: " << configuration_slot;
                return false;
            }
        }
    }
    return true;
    // TODO(piet): other special validating of variables requested to change can be added here...
}

std::map<SetVariableData, SetVariableResult>
ChargePoint::set_variables_internal(const std::vector<SetVariableData>& set_variable_data_vector,
                                    const std::string& source, const bool allow_read_only) {
    std::map<SetVariableData, SetVariableResult> response;

    // iterate over the set_variable_data_vector
    for (const auto& set_variable_data : set_variable_data_vector) {
        SetVariableResult set_variable_result;
        set_variable_result.component = set_variable_data.component;
        set_variable_result.variable = set_variable_data.variable;
        set_variable_result.attributeType = set_variable_data.attributeType.value_or(AttributeEnum::Actual);

        // validates variable against business logic of the spec
        if (this->validate_set_variable(set_variable_data)) {
            // attempt to set the value includes device model validation
            set_variable_result.attributeStatus =
                this->device_model->set_value(set_variable_data.component, set_variable_data.variable,
                                              set_variable_data.attributeType.value_or(AttributeEnum::Actual),
                                              set_variable_data.attributeValue.get(), source, allow_read_only);
        } else {
            set_variable_result.attributeStatus = SetVariableStatusEnum::Rejected;
        }
        response[set_variable_data] = set_variable_result;
    }

    return response;
}

std::optional<int32_t> ChargePoint::get_transaction_evseid(const CiString<36>& transaction_id) {
    for (auto& evse : *this->evse_manager) {
        if (evse.has_active_transaction()) {
            if (transaction_id == evse.get_transaction()->get_transaction().transactionId) {
                return evse.get_id();
            }
        }
    }

    return std::nullopt;
}

bool ChargePoint::is_evse_reserved_for_other(EvseInterface& evse, const IdToken& id_token,
                                             const std::optional<IdToken>& group_id_token) const {
    const uint32_t connectors = evse.get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum status =
            evse.get_connector(static_cast<int32_t>(i))->get_effective_connector_status();
        if (status == ConnectorStatusEnum::Reserved) {
            const std::optional<CiString<36>> groupIdToken =
                group_id_token.has_value() ? group_id_token.value().idToken : std::optional<CiString<36>>{};

            if (!callbacks.is_reservation_for_token_callback(evse.get_id(), id_token.idToken, groupIdToken)) {
                return true;
            }
        }
    }
    return false;
}

bool ChargePoint::is_evse_connector_available(EvseInterface& evse) const {
    if (evse.has_active_transaction()) {
        // If an EV is connected and has no authorization yet then the status is 'Occupied' and the
        // RemoteStartRequest should still be accepted. So this is the 'occupied' check instead.
        return false;
    }

    const uint32_t connectors = evse.get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum status =
            evse.get_connector(static_cast<int32_t>(i))->get_effective_connector_status();

        // At least one of the connectors is available / not faulted.
        if (status != ConnectorStatusEnum::Faulted && status != ConnectorStatusEnum::Unavailable) {
            return true;
        }
    }

    // Connectors are faulted or unavailable.
    return false;
}

void ChargePoint::set_evse_connectors_unavailable(EvseInterface& evse, bool persist) {
    uint32_t number_of_connectors = evse.get_number_of_connectors();

    for (uint32_t i = 1; i <= number_of_connectors; ++i) {
        evse.set_connector_operative_status(static_cast<int32_t>(i), OperationalStatusEnum::Inoperative, persist);
    }
}

bool ChargePoint::is_offline() {
    return !this->connectivity_manager->is_websocket_connected();
}

void ChargePoint::security_event_notification_req(const CiString<50>& event_type,
                                                  const std::optional<CiString<255>>& tech_info,
                                                  const bool triggered_internally, const bool critical) {
    EVLOG_debug << "Sending SecurityEventNotification";
    SecurityEventNotificationRequest req;

    req.type = event_type;
    req.timestamp = DateTime().to_rfc3339();
    req.techInfo = tech_info;
    this->logging->security(json(req).dump());
    if (critical) {
        ocpp::Call<SecurityEventNotificationRequest> call(req, this->message_queue->createMessageId());
        this->send<SecurityEventNotificationRequest>(call);
    }
    if (triggered_internally and this->callbacks.security_event_callback != nullptr) {
        this->callbacks.security_event_callback(event_type, tech_info);
    }
}

void ChargePoint::sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use,
                                       const bool initiated_by_trigger_message) {
    if (this->awaited_certificate_signing_use_enum.has_value()) {
        EVLOG_warning
            << "Not sending new SignCertificate.req because still waiting for CertificateSigned.req from CSMS";
        return;
    }

    SignCertificateRequest req;

    std::optional<std::string> common;
    std::optional<std::string> country;
    std::optional<std::string> organization;

    if (certificate_signing_use == ocpp::CertificateSigningUseEnum::ChargingStationCertificate) {
        req.certificateType = ocpp::v201::CertificateSigningUseEnum::ChargingStationCertificate;
        common =
            this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ChargeBoxSerialNumber);
        organization =
            this->device_model->get_optional_value<std::string>(ControllerComponentVariables::OrganizationName);
        country =
            this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ISO15118CtrlrCountryName);
    } else {
        req.certificateType = ocpp::v201::CertificateSigningUseEnum::V2GCertificate;
        common = this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ISO15118CtrlrSeccId);
        organization = this->device_model->get_optional_value<std::string>(
            ControllerComponentVariables::ISO15118CtrlrOrganizationName);
        country =
            this->device_model->get_optional_value<std::string>(ControllerComponentVariables::ISO15118CtrlrCountryName);
    }

    if (!common.has_value()) {
        EVLOG_warning << "Missing configuration of commonName to generate CSR";
        return;
    }

    if (!country.has_value()) {
        EVLOG_warning << "Missing configuration country to generate CSR";
        return;
    }

    if (!organization.has_value()) {
        EVLOG_warning << "Missing configuration of organizationName to generate CSR";
        return;
    }

    bool should_use_tpm =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::UseTPM).value_or(false);

    const auto result = this->evse_security->generate_certificate_signing_request(
        certificate_signing_use, country.value(), organization.value(), common.value(), should_use_tpm);

    if (result.status != GetCertificateSignRequestStatus::Accepted || !result.csr.has_value()) {
        EVLOG_error << "CSR generation was unsuccessful for sign request: "
                    << ocpp::conversions::certificate_signing_use_enum_to_string(certificate_signing_use);

        std::string gen_error = "Sign certificate req failed due to:" +
                                ocpp::conversions::generate_certificate_signing_request_status_to_string(result.status);
        this->security_event_notification_req(ocpp::security_events::CSRGENERATIONFAILED,
                                              std::optional<CiString<255>>(gen_error), true, true);
        return;
    }

    req.csr = result.csr.value();

    this->awaited_certificate_signing_use_enum = certificate_signing_use;

    ocpp::Call<SignCertificateRequest> call(req, this->message_queue->createMessageId());
    this->send<SignCertificateRequest>(call, initiated_by_trigger_message);
}

void ChargePoint::boot_notification_req(const BootReasonEnum& reason, const bool initiated_by_trigger_message) {
    EVLOG_debug << "Sending BootNotification";
    BootNotificationRequest req;

    ChargingStation charging_station;
    charging_station.model = this->device_model->get_value<std::string>(ControllerComponentVariables::ChargePointModel);
    charging_station.vendorName =
        this->device_model->get_value<std::string>(ControllerComponentVariables::ChargePointVendor);
    charging_station.firmwareVersion.emplace(
        this->device_model->get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
    charging_station.serialNumber.emplace(
        this->device_model->get_value<std::string>(ControllerComponentVariables::ChargeBoxSerialNumber));

    req.reason = reason;
    req.chargingStation = charging_station;

    ocpp::Call<BootNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<BootNotificationRequest>(call, initiated_by_trigger_message);
}

void ChargePoint::notify_report_req(const int request_id, const std::vector<ReportData>& report_data) {

    NotifyReportRequest req;
    req.requestId = request_id;
    req.seqNo = 0;
    req.generatedAt = ocpp::DateTime();
    req.reportData.emplace(report_data);
    req.tbc = false;

    if (report_data.size() <= 1) {
        ocpp::Call<NotifyReportRequest> call(req, this->message_queue->createMessageId());
        this->send<NotifyReportRequest>(call);
    } else {
        NotifyReportRequestsSplitter splitter{
            req,
            this->device_model->get_optional_value<size_t>(ControllerComponentVariables::MaxMessageSize)
                .value_or(DEFAULT_MAX_MESSAGE_SIZE),
            [this]() { return this->message_queue->createMessageId(); }};
        for (const auto& msg : splitter.create_call_payloads()) {
            this->message_queue->push(msg);
        }
    }
}

AuthorizeResponse ChargePoint::authorize_req(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                             const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) {
    AuthorizeRequest req;
    req.idToken = id_token;
    req.certificate = certificate;
    req.iso15118CertificateHashData = ocsp_request_data;

    AuthorizeResponse response;
    response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;

    if (!this->connectivity_manager->is_websocket_connected()) {
        return response;
    }

    ocpp::Call<AuthorizeRequest> call(req, this->message_queue->createMessageId());
    auto future = this->send_async<AuthorizeRequest>(call);

    if (future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf(Authorize) future timed out!";
        return response;
    }

    const auto enhanced_message = future.get();

    if (enhanced_message.messageType != MessageType::AuthorizeResponse) {
        return response;
    }

    try {
        ocpp::CallResult<AuthorizeResponse> call_result = enhanced_message.message;
        return call_result.msg;
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->send(call_error);
        return response;
    }
}

void ChargePoint::status_notification_req(const int32_t evse_id, const int32_t connector_id,
                                          const ConnectorStatusEnum status, const bool initiated_by_trigger_message) {
    StatusNotificationRequest req;
    req.connectorId = connector_id;
    req.evseId = evse_id;
    req.timestamp = DateTime().to_rfc3339();
    req.connectorStatus = status;

    ocpp::Call<StatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<StatusNotificationRequest>(call, initiated_by_trigger_message);
}

void ChargePoint::heartbeat_req(const bool initiated_by_trigger_message) {
    HeartbeatRequest req;

    heartbeat_request_time = std::chrono::steady_clock::now();
    ocpp::Call<HeartbeatRequest> call(req, this->message_queue->createMessageId());
    this->send<HeartbeatRequest>(call, initiated_by_trigger_message);
}

void ChargePoint::transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                                        const ocpp::v201::Transaction& transaction,
                                        const ocpp::v201::TriggerReasonEnum& trigger_reason, const int32_t seq_no,
                                        const std::optional<int32_t>& cable_max_current,
                                        const std::optional<ocpp::v201::EVSE>& evse,
                                        const std::optional<ocpp::v201::IdToken>& id_token,
                                        const std::optional<std::vector<ocpp::v201::MeterValue>>& meter_value,
                                        const std::optional<int32_t>& number_of_phases_used, const bool offline,
                                        const std::optional<int32_t>& reservation_id,
                                        const bool initiated_by_trigger_message) {
    TransactionEventRequest req;
    req.eventType = event_type;
    req.timestamp = timestamp;
    req.transactionInfo = transaction;
    req.triggerReason = trigger_reason;
    req.seqNo = seq_no;
    req.cableMaxCurrent = cable_max_current;
    req.evse = evse;
    req.idToken = id_token;
    req.meterValue = meter_value;
    req.numberOfPhasesUsed = number_of_phases_used;
    req.offline = offline;
    req.reservationId = reservation_id;

    ocpp::Call<TransactionEventRequest> call(req, this->message_queue->createMessageId());

    // Check if id token is in the remote start map, because when a remote
    // start request is done, the first transaction event request should
    // always contain trigger reason 'RemoteStart'.
    auto it = std::find_if(
        remote_start_id_per_evse.begin(), remote_start_id_per_evse.end(),
        [&id_token, &evse](const std::pair<int32_t, std::pair<IdToken, int32_t>>& remote_start_per_evse) {
            if (id_token.has_value() && remote_start_per_evse.second.first.idToken == id_token.value().idToken) {

                if (remote_start_per_evse.first == 0) {
                    return true;
                }

                if (evse.has_value() && evse.value().id == remote_start_per_evse.first) {
                    return true;
                }
            }
            return false;
        });

    if (it != remote_start_id_per_evse.end()) {
        // Found remote start. Set remote start id and the trigger reason.
        call.msg.triggerReason = TriggerReasonEnum::RemoteStart;
        call.msg.transactionInfo.remoteStartId = it->second.second;

        remote_start_id_per_evse.erase(it);
    }

    this->send<TransactionEventRequest>(call, initiated_by_trigger_message);

    if (this->callbacks.transaction_event_callback.has_value()) {
        this->callbacks.transaction_event_callback.value()(req);
    }
}

void ChargePoint::meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values,
                                   const bool initiated_by_trigger_message) {
    MeterValuesRequest req;
    req.evseId = evse_id;
    req.meterValue = meter_values;

    ocpp::Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send<MeterValuesRequest>(call, initiated_by_trigger_message);
}

void ChargePoint::report_charging_profile_req(const int32_t request_id, const int32_t evse_id,
                                              const ChargingLimitSourceEnum source,
                                              const std::vector<ChargingProfile>& profiles, const bool tbc) {
    ReportChargingProfilesRequest req;
    req.requestId = request_id;
    req.evseId = evse_id;
    req.chargingLimitSource = source;
    req.chargingProfile = profiles;
    req.tbc = tbc;

    ocpp::Call<ReportChargingProfilesRequest> call(req, this->message_queue->createMessageId());
    this->send<ReportChargingProfilesRequest>(call);
}

void ChargePoint::report_charging_profile_req(const ReportChargingProfilesRequest& req) {
    ocpp::Call<ReportChargingProfilesRequest> call(req, this->message_queue->createMessageId());
    this->send<ReportChargingProfilesRequest>(call);
}

void ChargePoint::notify_event_req(const std::vector<EventData>& events) {
    NotifyEventRequest req;
    req.eventData = events;
    req.generatedAt = DateTime();
    req.seqNo = 0;

    ocpp::Call<NotifyEventRequest> call(req, this->message_queue->createMessageId());
    this->send<NotifyEventRequest>(call);
}

void ChargePoint::notify_customer_information_req(const std::string& data, const int32_t request_id) {
    size_t pos = 0;
    int32_t seq_no = 0;
    while (pos < data.length() or pos == 0 && data.empty()) {
        const auto req = [&]() {
            NotifyCustomerInformationRequest req;
            req.data = CiString<512>(data.substr(pos, 512));
            req.seqNo = seq_no;
            req.requestId = request_id;
            req.generatedAt = DateTime();
            req.tbc = data.length() - pos > 512;
            return req;
        }();

        ocpp::Call<NotifyCustomerInformationRequest> call(req, this->message_queue->createMessageId());
        this->send<NotifyCustomerInformationRequest>(call);

        pos += 512;
        seq_no++;
    }
}

void ChargePoint::handle_certificate_signed_req(Call<CertificateSignedRequest> call) {
    // reset these parameters
    this->csr_attempt = 1;
    this->awaited_certificate_signing_use_enum = std::nullopt;
    this->certificate_signed_timer.stop();

    CertificateSignedResponse response;
    response.status = CertificateSignedStatusEnum::Rejected;

    const auto certificate_chain = call.msg.certificateChain.get();
    ocpp::CertificateSigningUseEnum cert_signing_use;

    if (!call.msg.certificateType.has_value() or
        call.msg.certificateType.value() == CertificateSigningUseEnum::ChargingStationCertificate) {
        cert_signing_use = ocpp::CertificateSigningUseEnum::ChargingStationCertificate;
    } else {
        cert_signing_use = ocpp::CertificateSigningUseEnum::V2GCertificate;
    }

    const auto result = this->evse_security->update_leaf_certificate(certificate_chain, cert_signing_use);

    if (result == ocpp::InstallCertificateResult::Accepted) {
        response.status = CertificateSignedStatusEnum::Accepted;
        // For V2G certificates, also trigger an OCSP cache update
        if (cert_signing_use == ocpp::CertificateSigningUseEnum::V2GCertificate) {
            this->ocsp_updater.trigger_ocsp_cache_update();
        }
    }

    // Trigger a symlink update for V2G certificates
    if ((cert_signing_use == ocpp::CertificateSigningUseEnum::V2GCertificate) and
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::UpdateCertificateSymlinks)
            .value_or(false)) {
        this->evse_security->update_certificate_links(cert_signing_use);
    }

    ocpp::CallResult<CertificateSignedResponse> call_result(response, call.uniqueId);
    this->send<CertificateSignedResponse>(call_result);

    if (result != ocpp::InstallCertificateResult::Accepted) {
        this->security_event_notification_req("InvalidChargingStationCertificate",
                                              ocpp::conversions::install_certificate_result_to_string(result), true,
                                              true);
    }

    // reconnect with new certificate if valid and security profile is 3
    if (response.status == CertificateSignedStatusEnum::Accepted and
        cert_signing_use == ocpp::CertificateSigningUseEnum::ChargingStationCertificate and
        this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile) == 3) {
        this->connectivity_manager->disconnect_websocket(WebsocketCloseReason::ServiceRestart);
    }
}

void ChargePoint::handle_sign_certificate_response(CallResult<SignCertificateResponse> call_result) {
    if (!this->awaited_certificate_signing_use_enum.has_value()) {
        EVLOG_warning
            << "Received SignCertificate.conf while not awaiting a CertificateSigned.req . This should not happen.";
        return;
    }

    if (call_result.msg.status == GenericStatusEnum::Accepted) {
        // set timer waiting for certificate signed
        const auto cert_signing_wait_minimum =
            this->device_model->get_optional_value<int>(ControllerComponentVariables::CertSigningWaitMinimum);
        const auto cert_signing_repeat_times =
            this->device_model->get_optional_value<int>(ControllerComponentVariables::CertSigningRepeatTimes);

        if (!cert_signing_wait_minimum.has_value()) {
            EVLOG_warning << "No CertSigningWaitMinimum is configured, will not attempt to retry SignCertificate.req "
                             "in case CSMS doesn't send CertificateSigned.req";
            return;
        }
        if (!cert_signing_repeat_times.has_value()) {
            EVLOG_warning << "No CertSigningRepeatTimes is configured, will not attempt to retry SignCertificate.req "
                             "in case CSMS doesn't send CertificateSigned.req";
            return;
        }

        if (this->csr_attempt > cert_signing_repeat_times.value()) {
            this->csr_attempt = 1;
            this->certificate_signed_timer.stop();
            this->awaited_certificate_signing_use_enum = std::nullopt;
            return;
        }
        int retry_backoff_milliseconds =
            std::max(250, 1000 * cert_signing_wait_minimum.value()) *
            std::pow(2, this->csr_attempt); // prevent immediate repetition in case of value 0
        this->certificate_signed_timer.timeout(
            [this]() {
                EVLOG_info << "Did not receive CertificateSigned.req in time. Will retry with SignCertificate.req";
                this->csr_attempt++;
                const auto current_awaited_certificate_signing_use_enum =
                    this->awaited_certificate_signing_use_enum.value();
                this->awaited_certificate_signing_use_enum.reset();
                this->sign_certificate_req(current_awaited_certificate_signing_use_enum);
            },
            std::chrono::milliseconds(retry_backoff_milliseconds));
    } else {
        this->awaited_certificate_signing_use_enum = std::nullopt;
        this->csr_attempt = 1;
        EVLOG_warning << "SignCertificate.req has not been accepted by CSMS";
    }
}

void ChargePoint::handle_boot_notification_response(CallResult<BootNotificationResponse> call_result) {
    // TODO(piet): B01.FR.06
    // TODO(piet): B01.FR.07
    // TODO(piet): B01.FR.08
    // TODO(piet): B01.FR.09
    // TODO(piet): B01.FR.13
    EVLOG_info << "Received BootNotificationResponse: " << call_result.msg
               << "\nwith messageId: " << call_result.uniqueId;

    const auto msg = call_result.msg;

    this->registration_status = msg.status;

    if (this->registration_status == RegistrationStatusEnum::Accepted) {
        this->message_queue->set_registration_status_accepted();
        // B01.FR.06 Only use boot timestamp if TimeSource contains Heartbeat
        if (this->callbacks.time_sync_callback.has_value() &&
            this->device_model->get_value<std::string>(ControllerComponentVariables::TimeSource).find("Heartbeat") !=
                std::string::npos) {
            this->callbacks.time_sync_callback.value()(msg.currentTime);
        }

        this->remove_network_connection_profiles_below_actual_security_profile();

        // set timers
        if (msg.interval > 0) {
            this->heartbeat_timer.interval([this]() { this->heartbeat_req(); }, std::chrono::seconds(msg.interval));
        }

        // in case the BootNotification.req was triggered by a TriggerMessage.req the timer might still run
        this->boot_notification_timer.stop();

        this->init_certificate_expiration_check_timers();
        this->update_aligned_data_interval();
        this->component_state_manager->send_status_notification_all_connectors();
        this->ocsp_updater.start();
    } else {
        auto retry_interval = DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL;
        if (msg.interval > 0) {
            retry_interval = std::chrono::seconds(msg.interval);
        }
        this->boot_notification_timer.timeout(
            [this, msg]() {
                this->boot_notification_req(BootReasonEnum::PowerUp); // FIXME(piet): Choose correct reason here
            },
            retry_interval);
    }

    if (this->callbacks.boot_notification_callback.has_value()) {
        // call the registered boot notification callback
        callbacks.boot_notification_callback.value()(call_result.msg);
    }
}

void ChargePoint::handle_set_variables_req(Call<SetVariablesRequest> call) {
    const auto msg = call.msg;

    SetVariablesResponse response;

    // set variables but do not allow setting ReadOnly variables
    const auto set_variables_response =
        this->set_variables_internal(msg.setVariableData, VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS, false);
    for (const auto& [single_set_variable_data, single_set_variable_result] : set_variables_response) {
        response.setVariableResult.push_back(single_set_variable_result);
    }

    ocpp::CallResult<SetVariablesResponse> call_result(response, call.uniqueId);
    this->send<SetVariablesResponse>(call_result);

    // post handling of changed variables after the SetVariables.conf has been queued
    this->handle_variables_changed(set_variables_response);
}

void ChargePoint::handle_get_variables_req(const EnhancedMessage<v201::MessageType>& message) {
    Call<GetVariablesRequest> call = message.call_message;
    const auto msg = call.msg;

    const auto max_variables_per_message =
        this->device_model->get_value<int>(ControllerComponentVariables::ItemsPerMessageGetVariables);
    const auto max_bytes_per_message =
        this->device_model->get_value<int>(ControllerComponentVariables::BytesPerMessageGetVariables);

    // B06.FR.16
    if (msg.getVariableData.size() > max_variables_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->send(call_error);
        return;
    }

    // B06.FR.17
    if (message.message_size > max_bytes_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "FormatViolation", "", json({}));
        this->send(call_error);
        return;
    }

    GetVariablesResponse response;
    response.getVariableResult = this->get_variables(msg.getVariableData);

    ocpp::CallResult<GetVariablesResponse> call_result(response, call.uniqueId);
    this->send<GetVariablesResponse>(call_result);
}

void ChargePoint::handle_get_base_report_req(Call<GetBaseReportRequest> call) {
    const auto msg = call.msg;
    GetBaseReportResponse response;
    response.status = GenericDeviceModelStatusEnum::Accepted;

    ocpp::CallResult<GetBaseReportResponse> call_result(response, call.uniqueId);
    this->send<GetBaseReportResponse>(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        const auto report_data = this->device_model->get_base_report_data(msg.reportBase);
        this->notify_report_req(msg.requestId, report_data);
    }
}

void ChargePoint::handle_get_report_req(const EnhancedMessage<v201::MessageType>& message) {
    Call<GetReportRequest> call = message.call_message;
    const auto msg = call.msg;
    std::vector<ReportData> report_data;
    GetReportResponse response;

    const auto max_items_per_message =
        this->device_model->get_value<int>(ControllerComponentVariables::ItemsPerMessageGetReport);
    const auto max_bytes_per_message =
        this->device_model->get_value<int>(ControllerComponentVariables::BytesPerMessageGetReport);

    // B08.FR.17
    if (msg.componentVariable.has_value() and msg.componentVariable->size() > max_items_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->send(call_error);
        return;
    }

    // B08.FR.18
    if (message.message_size > max_bytes_per_message) {
        // send a CALLERROR
        const auto call_error = CallError(call.uniqueId, "FormatViolation", "", json({}));
        this->send(call_error);
        return;
    }

    // if a criteria is not supported then send a not supported response.
    auto sup_criteria =
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::SupportedCriteria);
    if (sup_criteria.has_value() and msg.componentCriteria.has_value()) {
        for (const auto& criteria : msg.componentCriteria.value()) {
            const auto variable_ = conversions::component_criterion_enum_to_string(criteria);
            if (sup_criteria.value().find(variable_) == std::string::npos) {
                EVLOG_info << "This criteria is not supported: " << variable_;
                response.status = GenericDeviceModelStatusEnum::NotSupported;
                break;
                // TODO: maybe consider adding the reason why in statusInfo
            }
        }
    }

    if (response.status != GenericDeviceModelStatusEnum::NotSupported) {

        // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage /
        // BytesPerMessage
        report_data = this->device_model->get_custom_report_data(msg.componentVariable, msg.componentCriteria);
        if (report_data.empty()) {
            response.status = GenericDeviceModelStatusEnum::EmptyResultSet;
        } else {
            response.status = GenericDeviceModelStatusEnum::Accepted;
        }
    }

    ocpp::CallResult<GetReportResponse> call_result(response, call.uniqueId);
    this->send<GetReportResponse>(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        this->notify_report_req(msg.requestId, report_data);
    }
}

void ChargePoint::handle_set_network_profile_req(Call<SetNetworkProfileRequest> call) {
    const auto msg = call.msg;

    SetNetworkProfileResponse response;

    if (!this->callbacks.validate_network_profile_callback.has_value()) {
        EVLOG_warning << "No callback registered to validate network profile";
        response.status = SetNetworkProfileStatusEnum::Rejected;
        ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->send<SetNetworkProfileResponse>(call_result);
        return;
    }

    if (msg.connectionData.securityProfile <
        this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile)) {
        EVLOG_warning << "CSMS attempted to set a network profile with a lower securityProfile";
        response.status = SetNetworkProfileStatusEnum::Rejected;
        ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->send<SetNetworkProfileResponse>(call_result);
        return;
    }

    if (this->callbacks.validate_network_profile_callback.value()(msg.configurationSlot, msg.connectionData) !=
        SetNetworkProfileStatusEnum::Accepted) {
        EVLOG_warning << "CSMS attempted to set a network profile that could not be validated.";
        response.status = SetNetworkProfileStatusEnum::Rejected;
        ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->send<SetNetworkProfileResponse>(call_result);
        return;
    }

    auto network_connection_profiles = json::parse(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles));

    int index_to_override = -1;
    int index = 0;
    for (const SetNetworkProfileRequest network_profile : network_connection_profiles) {
        if (network_profile.configurationSlot == msg.configurationSlot) {
            index_to_override = index;
        }
        index++;
    }

    if (index_to_override != -1) {
        // configurationSlot present, so we override
        network_connection_profiles[index_to_override] = msg;
    } else {
        // configurationSlot not present, so we can append
        network_connection_profiles.push_back(msg);
    }

    if (this->device_model->set_value(ControllerComponentVariables::NetworkConnectionProfiles.component,
                                      ControllerComponentVariables::NetworkConnectionProfiles.variable.value(),
                                      AttributeEnum::Actual, network_connection_profiles.dump(),
                                      VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL) != SetVariableStatusEnum::Accepted) {
        EVLOG_warning
            << "CSMS attempted to set a network profile that could not be written to the device model storage";
        response.status = SetNetworkProfileStatusEnum::Rejected;
        ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
        this->send<SetNetworkProfileResponse>(call_result);
        return;
    }

    EVLOG_info << "Received and stored a new network connection profile at configurationSlot: "
               << msg.configurationSlot;
    response.status = SetNetworkProfileStatusEnum::Accepted;
    ocpp::CallResult<SetNetworkProfileResponse> call_result(response, call.uniqueId);
    this->send<SetNetworkProfileResponse>(call_result);
}

void ChargePoint::handle_reset_req(Call<ResetRequest> call) {
    // TODO(piet): B11.FR.05

    // TODO(piet): B12.FR.05
    // TODO(piet): B12.FR.06
    EVLOG_debug << "Received ResetRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;

    ResetResponse response;

    // Check if there is an active transaction (on the given evse or if not
    // given, on one of the evse's)
    bool transaction_active = false;
    std::set<int32_t> evse_active_transactions;
    std::set<int32_t> evse_no_transactions;
    if (msg.evseId.has_value() && this->evse_manager->get_evse(msg.evseId.value()).has_active_transaction()) {
        transaction_active = true;
        evse_active_transactions.emplace(msg.evseId.value());
    } else {
        for (const auto& evse : *this->evse_manager) {
            if (evse.has_active_transaction()) {
                transaction_active = true;
                evse_active_transactions.emplace(evse.get_id());
            } else {
                evse_no_transactions.emplace(evse.get_id());
            }
        }
    }

    const auto is_reset_allowed = [&]() {
        if (!this->callbacks.is_reset_allowed_callback(msg.evseId, msg.type)) {
            return false;
        }

        // We dont need to check AllowReset if evseId is not set and can directly return true
        if (!msg.evseId.has_value()) {
            return true;
        }

        // B11.FR.10
        const auto allow_reset_cv =
            EvseComponentVariables::get_component_variable(msg.evseId.value(), EvseComponentVariables::AllowReset);
        // allow reset if AllowReset is not set or set to   true
        return this->device_model->get_optional_value<bool>(allow_reset_cv).value_or(true);
    };

    if (is_reset_allowed()) {
        // reset is allowed
        response.status = ResetStatusEnum::Accepted;
    } else {
        response.status = ResetStatusEnum::Rejected;
    }

    if (response.status == ResetStatusEnum::Accepted && transaction_active && msg.type == ResetEnum::OnIdle) {
        if (msg.evseId.has_value()) {
            // B12.FR.07
            this->reset_scheduled_evseids.insert(msg.evseId.value());
        }

        // B12.FR.01: We have to wait until transactions have ended.
        // B12.FR.07
        this->reset_scheduled = true;
        response.status = ResetStatusEnum::Scheduled;
    }

    ocpp::CallResult<ResetResponse> call_result(response, call.uniqueId);
    this->send<ResetResponse>(call_result);

    // Reset response is sent, now set evse connectors to unavailable and / or
    // stop transaction (depending on reset type)
    if (response.status != ResetStatusEnum::Rejected && transaction_active) {
        if (msg.type == ResetEnum::Immediate) {
            // B12.FR.08 and B12.FR.04
            for (const int32_t evse_id : evse_active_transactions) {
                callbacks.stop_transaction_callback(evse_id, ReasonEnum::ImmediateReset);
            }
        } else if (msg.type == ResetEnum::OnIdle && !evse_no_transactions.empty()) {
            for (const int32_t evse_id : evse_no_transactions) {
                auto& evse = this->evse_manager->get_evse(evse_id);
                this->set_evse_connectors_unavailable(evse, false);
            }
        }
    }

    if (response.status == ResetStatusEnum::Accepted) {
        this->callbacks.reset_callback(call.msg.evseId, ResetEnum::Immediate);
    }
}

void ChargePoint::handle_clear_cache_req(Call<ClearCacheRequest> call) {
    ClearCacheResponse response;
    response.status = ClearCacheStatusEnum::Rejected;

    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCacheCtrlrEnabled)
            .value_or(true)) {
        try {
            this->database_handler->authorization_cache_clear();
            this->update_authorization_cache_size();
            response.status = ClearCacheStatusEnum::Accepted;
        } catch (DatabaseException& e) {
            auto call_error = CallError(call.uniqueId, "InternalError",
                                        "Database error while clearing authorization cache", json({}, true));
            this->send(call_error);
            return;
        }
    }

    ocpp::CallResult<ClearCacheResponse> call_result(response, call.uniqueId);
    this->send<ClearCacheResponse>(call_result);
}

void ChargePoint::handle_transaction_event_response(const EnhancedMessage<v201::MessageType>& message) {
    CallResult<TransactionEventResponse> call_result = message.message;
    const Call<TransactionEventRequest>& original_call = message.call_message;
    const auto& original_msg = original_call.msg;

    if (this->callbacks.transaction_event_response_callback.has_value()) {
        this->callbacks.transaction_event_response_callback.value()(original_msg, call_result.msg);
    }

    if (original_msg.eventType == TransactionEventEnum::Ended) {
        // nothing to do for TransactionEventEnum::Ended
        return;
    }

    const auto msg = call_result.msg;

    if (!msg.idTokenInfo.has_value()) {
        // nothing to do when the response does not contain idTokenInfo
        return;
    }

    if (!original_msg.idToken.has_value()) {
        EVLOG_error
            << "TransactionEvent.conf contains idTokenInfo when no idToken was part of the TransactionEvent.req";
        return;
    }

    const IdToken& id_token = original_msg.idToken.value();

    // C03.FR.0x and C05.FR.01: We SHALL NOT store central information in the Authorization Cache
    // C10.FR.05
    if (id_token.type != IdTokenEnum::Central and
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCacheCtrlrEnabled)
            .value_or(true)) {
        try {
            this->database_handler->authorization_cache_insert_entry(utils::generate_token_hash(id_token),
                                                                     msg.idTokenInfo.value());
        } catch (const DatabaseException& e) {
            EVLOG_warning << "Could not insert into authorization cache entry: " << e.what();
        }
        this->trigger_authorization_cache_cleanup();
    }

    if (msg.idTokenInfo.value().status == AuthorizationStatusEnum::Accepted) {
        // nothing to do in case status is accepted
        return;
    }

    for (auto& evse : *this->evse_manager) {
        if (auto& transaction = evse.get_transaction();
            transaction != nullptr and transaction->transactionId == original_msg.transactionInfo.transactionId) {
            // Deal with invalid token for transaction
            auto evse_id = evse.get_id();
            if (this->device_model->get_value<bool>(ControllerComponentVariables::StopTxOnInvalidId)) {
                this->callbacks.stop_transaction_callback(evse_id, ReasonEnum::DeAuthorized);
            } else {
                if (this->device_model->get_optional_value<int32_t>(ControllerComponentVariables::MaxEnergyOnInvalidId)
                        .has_value()) {
                    // Energy delivery to the EV SHALL be allowed until the amount of energy specified in
                    // MaxEnergyOnInvalidId has been reached.
                    evse.start_checking_max_energy_on_invalid_id();
                } else {
                    this->callbacks.pause_charging_callback(evse_id);
                }
            }
            break;
        }
    }
}

void ChargePoint::handle_get_transaction_status(const Call<GetTransactionStatusRequest> call) {
    const auto msg = call.msg;

    GetTransactionStatusResponse response;
    response.messagesInQueue = false;

    if (msg.transactionId.has_value()) {
        if (this->get_transaction_evseid(msg.transactionId.value()).has_value()) {
            response.ongoingIndicator = true;
        } else {
            response.ongoingIndicator = false;
        }
        if (this->message_queue->contains_transaction_messages(msg.transactionId.value())) {
            response.messagesInQueue = true;
        }
    } else if (!this->message_queue->is_transaction_message_queue_empty()) {
        response.messagesInQueue = true;
    }

    ocpp::CallResult<GetTransactionStatusResponse> call_result(response, call.uniqueId);
    this->send<GetTransactionStatusResponse>(call_result);
}

void ChargePoint::handle_unlock_connector(Call<UnlockConnectorRequest> call) {
    const UnlockConnectorRequest& msg = call.msg;
    UnlockConnectorResponse unlock_response;

    EVSE evse = {msg.evseId, std::nullopt, msg.connectorId};

    if (this->is_valid_evse(evse)) {
        if (!this->evse_manager->get_evse(msg.evseId).has_active_transaction()) {
            unlock_response = callbacks.unlock_connector_callback(msg.evseId, msg.connectorId);
        } else {
            unlock_response.status = UnlockStatusEnum::OngoingAuthorizedTransaction;
        }
    } else {
        unlock_response.status = UnlockStatusEnum::UnknownConnector;
    }

    ocpp::CallResult<UnlockConnectorResponse> call_result(unlock_response, call.uniqueId);
    this->send<UnlockConnectorResponse>(call_result);
}

void ChargePoint::handle_trigger_message(Call<TriggerMessageRequest> call) {
    const TriggerMessageRequest& msg = call.msg;
    TriggerMessageResponse response;
    EvseInterface* evse_ptr = nullptr;

    response.status = TriggerMessageStatusEnum::Rejected;

    if (msg.evse.has_value()) {
        int32_t evse_id = msg.evse.value().id;
        evse_ptr = &this->evse_manager->get_evse(evse_id);
    }

    // F06.FR.04: First send the TriggerMessageResponse before sending the requested message
    //            so we split the functionality to be able to determine if we need to respond first.
    switch (msg.requestedMessage) {
    case MessageTriggerEnum::BootNotification:
        // F06.FR.17: Respond with rejected in case registration status is already accepted
        if (this->registration_status != RegistrationStatusEnum::Accepted) {
            response.status = TriggerMessageStatusEnum::Accepted;
        }
        break;

    case MessageTriggerEnum::LogStatusNotification:
    case MessageTriggerEnum::Heartbeat:
    case MessageTriggerEnum::FirmwareStatusNotification:
        response.status = TriggerMessageStatusEnum::Accepted;
        break;

    case MessageTriggerEnum::MeterValues:
        if (msg.evse.has_value()) {
            if (evse_ptr != nullptr and
                utils::meter_value_has_any_measurand(
                    evse_ptr->get_meter_value(), utils::get_measurands_vec(this->device_model->get_value<std::string>(
                                                     ControllerComponentVariables::AlignedDataMeasurands)))) {
                response.status = TriggerMessageStatusEnum::Accepted;
            }
        } else {
            const auto measurands = utils::get_measurands_vec(
                this->device_model->get_value<std::string>(ControllerComponentVariables::AlignedDataMeasurands));
            for (auto& evse : *this->evse_manager) {
                if (utils::meter_value_has_any_measurand(evse.get_meter_value(), measurands)) {
                    response.status = TriggerMessageStatusEnum::Accepted;
                    break;
                }
            }
        }
        break;

    case MessageTriggerEnum::TransactionEvent:
        if (msg.evse.has_value()) {
            if (evse_ptr != nullptr and evse_ptr->has_active_transaction()) {
                response.status = TriggerMessageStatusEnum::Accepted;
            }
        } else {
            for (auto const& evse : *this->evse_manager) {
                if (evse.has_active_transaction()) {
                    response.status = TriggerMessageStatusEnum::Accepted;
                    break;
                }
            }
        }
        break;

    case MessageTriggerEnum::StatusNotification:
        if (msg.evse.has_value() and msg.evse.value().connectorId.has_value()) {
            int32_t connector_id = msg.evse.value().connectorId.value();
            if (evse_ptr != nullptr and connector_id > 0 and connector_id <= evse_ptr->get_number_of_connectors()) {
                response.status = TriggerMessageStatusEnum::Accepted;
            }
        } else {
            // F06.FR.12: Reject if evse or connectorId is ommited
        }
        break;

    case MessageTriggerEnum::SignChargingStationCertificate:
        response.status = TriggerMessageStatusEnum::Accepted;
        break;
    case MessageTriggerEnum::SignV2GCertificate:
        if (this->device_model
                ->get_optional_value<bool>(ControllerComponentVariables::V2GCertificateInstallationEnabled)
                .value_or(false)) {
            response.status = TriggerMessageStatusEnum::Accepted;
        } else {
            EVLOG_warning << "CSMS requested SignV2GCertificate but V2GCertificateInstallationEnabled is configured as "
                             "false, so the TriggerMessage is rejected!";
            response.status = TriggerMessageStatusEnum::Rejected;
        }

        break;
        // TODO:
        // PublishFirmwareStatusNotification
        // SignCombinedCertificate

    default:
        response.status = TriggerMessageStatusEnum::NotImplemented;
        break;
    }

    ocpp::CallResult<TriggerMessageResponse> call_result(response, call.uniqueId);
    this->send<TriggerMessageResponse>(call_result);

    if (response.status != TriggerMessageStatusEnum::Accepted) {
        return;
    }

    auto send_evse_message = [&](std::function<void(int32_t evse_id, EvseInterface & evse)> send) {
        if (evse_ptr != nullptr) {
            send(msg.evse.value().id, *evse_ptr);
        } else {
            for (auto& evse : *this->evse_manager) {
                send(evse.get_id(), evse);
            }
        }
    };

    switch (msg.requestedMessage) {
    case MessageTriggerEnum::BootNotification:
        boot_notification_req(BootReasonEnum::Triggered);
        break;

    case MessageTriggerEnum::MeterValues: {
        auto send_meter_value = [&](int32_t evse_id, EvseInterface& evse) {
            const auto meter_value =
                get_latest_meter_value_filtered(evse.get_meter_value(), ReadingContextEnum::Trigger,
                                                ControllerComponentVariables::AlignedDataMeasurands);

            if (!meter_value.sampledValue.empty()) {
                this->meter_values_req(evse_id, std::vector<ocpp::v201::MeterValue>(1, meter_value), true);
            }
        };
        send_evse_message(send_meter_value);
    } break;

    case MessageTriggerEnum::TransactionEvent: {
        auto send_transaction = [&](int32_t evse_id, EvseInterface& evse) {
            if (!evse.has_active_transaction()) {
                return;
            }

            const auto meter_value =
                get_latest_meter_value_filtered(evse.get_meter_value(), ReadingContextEnum::Trigger,
                                                ControllerComponentVariables::SampledDataTxUpdatedMeasurands);

            std::optional<std::vector<MeterValue>> opt_meter_value;
            if (!meter_value.sampledValue.empty()) {
                opt_meter_value.emplace(1, meter_value);
            }
            const auto& enhanced_transaction = evse.get_transaction();
            this->transaction_event_req(TransactionEventEnum::Updated, DateTime(),
                                        enhanced_transaction->get_transaction(), TriggerReasonEnum::Trigger,
                                        enhanced_transaction->get_seq_no(), std::nullopt, std::nullopt, std::nullopt,
                                        opt_meter_value, std::nullopt, this->is_offline(), std::nullopt, true);
        };
        send_evse_message(send_transaction);
    } break;

    case MessageTriggerEnum::StatusNotification:
        if (evse_ptr != nullptr and msg.evse.value().connectorId.has_value()) {
            this->component_state_manager->send_status_notification_single_connector(
                msg.evse.value().id, msg.evse.value().connectorId.value());
        }
        break;

    case MessageTriggerEnum::Heartbeat:
        this->heartbeat_req(true);
        break;

    case MessageTriggerEnum::LogStatusNotification: {
        LogStatusNotificationRequest request;
        if (this->upload_log_status == UploadLogStatusEnum::Uploading) {
            request.status = UploadLogStatusEnum::Uploading;
            request.requestId = this->upload_log_status_id;
        } else {
            request.status = UploadLogStatusEnum::Idle;
        }

        ocpp::Call<LogStatusNotificationRequest> call(request, this->message_queue->createMessageId());
        this->send<LogStatusNotificationRequest>(call, true);
    } break;

    case MessageTriggerEnum::FirmwareStatusNotification: {
        FirmwareStatusNotificationRequest request;
        switch (this->firmware_status) {
        case FirmwareStatusEnum::Idle:
        case FirmwareStatusEnum::Installed: // L01.FR.25
            request.status = FirmwareStatusEnum::Idle;
            // do not set requestId when idle: L01.FR.20
            break;

        default: // So not Idle or Installed                   // L01.FR.26
            request.status = this->firmware_status;
            request.requestId = this->firmware_status_id;
            break;
        }

        ocpp::Call<FirmwareStatusNotificationRequest> call(request, this->message_queue->createMessageId());
        this->send<FirmwareStatusNotificationRequest>(call, true);
    } break;

    case MessageTriggerEnum::SignChargingStationCertificate: {
        sign_certificate_req(ocpp::CertificateSigningUseEnum::ChargingStationCertificate, true);
    } break;

    case MessageTriggerEnum::SignV2GCertificate: {
        sign_certificate_req(ocpp::CertificateSigningUseEnum::V2GCertificate, true);
    } break;

    default:
        EVLOG_error << "Sent a TriggerMessageResponse::Accepted while not following up with a message";
        break;
    }
}

void ChargePoint::handle_remote_start_transaction_request(Call<RequestStartTransactionRequest> call) {
    const auto msg = call.msg;

    RequestStartTransactionResponse response;
    response.status = RequestStartStopStatusEnum::Rejected;

    // Check if evse id is given.
    if (msg.evseId.has_value()) {
        const int32_t evse_id = msg.evseId.value();
        auto& evse = this->evse_manager->get_evse(evse_id);

        // TODO F01.FR.26 If a Charging Station with support for Smart Charging receives a
        // RequestStartTransactionRequest with an invalid ChargingProfile: The Charging Station SHALL respond
        // with RequestStartTransactionResponse with status = Rejected and optionally with reasonCode =
        // "InvalidProfile" or "InvalidSchedule".

        // F01.FR.23: Faulted or unavailable. F01.FR.24 / F02.FR.25: Occupied. Send rejected.
        const bool available = is_evse_connector_available(evse);

        // When available but there was a reservation for another token id or group token id:
        //    send rejected (F01.FR.21 & F01.FR.22)
        const bool reserved = is_evse_reserved_for_other(evse, call.msg.idToken, call.msg.groupIdToken);

        if (!available || reserved) {
            // Note: we only support TxStartPoint PowerPathClosed, so we did not implement starting a
            // transaction first (and send TransactionEventRequest (eventType = Started). Only if a transaction
            // is authorized, a TransactionEventRequest will be sent. Because of this, F01.FR.13 is not
            // implemented as well, because in the current situation, this is an impossible state. (TODO: when
            // more TxStartPoints are supported, add implementation for F01.FR.13 as well).
            EVLOG_info << "Remote start transaction requested, but connector is not available or reserved.";
        } else {
            // F02: No active transaction yet and there is an available connector, so just send 'accepted'.
            response.status = RequestStartStopStatusEnum::Accepted;

            remote_start_id_per_evse[evse_id] = {msg.idToken, msg.remoteStartId};
        }
    } else {
        // F01.FR.07 RequestStartTransactionRequest does not contain an evseId. The Charging Station MAY reject the
        // RequestStartTransactionRequest. We do this for now (send rejected) (TODO: eventually support the charging
        // station to accept no evse id. If so: add token and remote start id for evse id 0 to
        // remote_start_id_per_evse, so we know for '0' it means 'all evse id's').
        EVLOG_warning << "No evse id given. Can not remote start transaction.";
    }

    const ocpp::CallResult<RequestStartTransactionResponse> call_result(response, call.uniqueId);
    this->send<RequestStartTransactionResponse>(call_result);

    if (response.status == RequestStartStopStatusEnum::Accepted) {
        // F01.FR.01 and F01.FR.02
        this->callbacks.remote_start_transaction_callback(
            msg, this->device_model->get_value<bool>(ControllerComponentVariables::AuthorizeRemoteStart));
    }
}

void ChargePoint::handle_remote_stop_transaction_request(Call<RequestStopTransactionRequest> call) {
    const auto msg = call.msg;

    RequestStopTransactionResponse response;
    std::optional<int32_t> evseid = get_transaction_evseid(msg.transactionId);

    if (evseid.has_value()) {
        // F03.FR.07: send 'accepted' if there was an ongoing transaction with the given transaction id
        response.status = RequestStartStopStatusEnum::Accepted;
    } else {
        // F03.FR.08: send 'rejected' if there was no ongoing transaction with the given transaction id
        response.status = RequestStartStopStatusEnum::Rejected;
    }

    const ocpp::CallResult<RequestStopTransactionResponse> call_result(response, call.uniqueId);
    this->send<RequestStopTransactionResponse>(call_result);

    if (response.status == RequestStartStopStatusEnum::Accepted) {
        this->callbacks.stop_transaction_callback(evseid.value(), ReasonEnum::Remote);
    }
}

void ChargePoint::handle_change_availability_req(Call<ChangeAvailabilityRequest> call) {
    const auto msg = call.msg;
    ChangeAvailabilityResponse response;
    response.status = ChangeAvailabilityStatusEnum::Scheduled;

    // Sanity check: if we're addressing an EVSE or a connector, it must actually exist
    if (msg.evse.has_value() and !this->is_valid_evse(msg.evse.value())) {
        EVLOG_warning << "CSMS requested ChangeAvailability for invalid evse id or connector id";
        response.status = ChangeAvailabilityStatusEnum::Rejected;
        ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
        this->send<ChangeAvailabilityResponse>(call_result);
        return;
    }

    // Check if we have any transaction running on the EVSE (or any EVSE if we're addressing the whole CS)
    const auto transaction_active = this->any_transaction_active(msg.evse);
    // Check if we're already in the requested state
    const auto is_already_in_state = this->is_already_in_state(msg);

    // evse_id will be 0 if we're addressing the whole CS, and >=1 otherwise
    auto evse_id = 0;
    if (msg.evse.has_value()) {
        evse_id = msg.evse.value().id;
    }

    if (!transaction_active or is_already_in_state or
        (evse_id == 0 and msg.operationalStatus == OperationalStatusEnum::Operative)) {
        // If the chosen EVSE (or CS) has no transactions, we're already in the desired state,
        // or we're telling the whole CS to power on, we can accept the request - there's nothing stopping us.
        response.status = ChangeAvailabilityStatusEnum::Accepted;
        // Remove any scheduled availability requests for the evse_id.
        // This is relevant in case some of those requests become activated later - the current one overrides them.
        this->scheduled_change_availability_requests.erase(evse_id);
    } else {
        // We can't immediately perform the change, because we have a transaction running.
        // Schedule the request to run when the transaction finishes.
        this->scheduled_change_availability_requests[evse_id] = {msg, true};
    }

    // Respond to the CSMS before performing any changes to avoid StatusNotification.req being sent before
    // the ChangeAvailabilityResponse.
    ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->send<ChangeAvailabilityResponse>(call_result);

    if (!transaction_active) {
        // No transactions - execute the change now
        this->execute_change_availability_request(msg, true);
    } else if (response.status == ChangeAvailabilityStatusEnum::Scheduled) {
        // We can't execute the change now, but it's scheduled to run after transactions are finished.
        if (evse_id == 0) {
            // The whole CS is being addressed - we need to prevent further transactions from starting.
            // To do that, make all EVSEs without an active transaction Inoperative
            for (auto const& evse : *this->evse_manager) {
                if (!evse.has_active_transaction()) {
                    // FIXME: This will linger after the update too! We probably need another mechanism...
                    this->set_evse_operative_status(evse.get_id(), OperationalStatusEnum::Inoperative, false);
                }
            }
        } else {
            // A single EVSE is being addressed. We need to prevent further transactions from starting on it.
            // To do that, make all connectors of the EVSE without an active transaction Inoperative.
            int number_of_connectors = this->evse_manager->get_evse(evse_id).get_number_of_connectors();
            for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
                if (!this->evse_manager->get_evse(evse_id).has_active_transaction(connector_id)) {
                    // FIXME: This will linger after the update too! We probably need another mechanism...
                    this->set_connector_operative_status(evse_id, connector_id, OperationalStatusEnum::Inoperative,
                                                         false);
                }
            }
        }
    }
}

void ChargePoint::handle_heartbeat_response(CallResult<HeartbeatResponse> call) {
    if (this->callbacks.time_sync_callback.has_value() &&
        this->device_model->get_value<std::string>(ControllerComponentVariables::TimeSource).find("Heartbeat") !=
            std::string::npos) {
        // the received currentTime was the time the CSMS received the heartbeat request
        // to get a system time as accurate as possible keep the time-of-flight into account
        auto timeOfFlight = (std::chrono::steady_clock::now() - this->heartbeat_request_time) / 2;
        ocpp::DateTime currentTimeCompensated(call.msg.currentTime.to_time_point() + timeOfFlight);
        this->callbacks.time_sync_callback.value()(currentTimeCompensated);
    }
}

void ChargePoint::handle_set_charging_profile_req(Call<SetChargingProfileRequest> call) {
    EVLOG_debug << "Received SetChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    auto msg = call.msg;
    SetChargingProfileResponse response;
    response.status = ChargingProfileStatusEnum::Rejected;

    // K01.FR.29: Respond with a CallError if SmartCharging is not available for this Charging Station
    bool is_smart_charging_available =
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::SmartChargingCtrlrAvailable)
            .value_or(false);

    if (!is_smart_charging_available) {
        EVLOG_warning << "SmartChargingCtrlrAvailable is not set for Charging Station. Returning NotSupported error";

        const auto call_error =
            CallError(call.uniqueId, "NotSupported", "Charging Station does not support smart charging", json({}));
        this->send(call_error);

        return;
    }

    // K01.FR.22: Reject ChargingStationExternalConstraints profiles in SetChargingProfileRequest
    if (msg.chargingProfile.chargingProfilePurpose == ChargingProfilePurposeEnum::ChargingStationExternalConstraints) {
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "ChargingStationExternalConstraintsInSetChargingProfileRequest";
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();

        ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
        this->send<SetChargingProfileResponse>(call_result);

        return;
    }

    response = this->smart_charging_handler->validate_and_add_profile(msg.chargingProfile, msg.evseId);
    if (response.status == ChargingProfileStatusEnum::Accepted) {
        EVLOG_debug << "Accepting SetChargingProfileRequest";
        this->callbacks.set_charging_profiles_callback();
    } else {
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();
    }

    ocpp::CallResult<SetChargingProfileResponse> call_result(response, call.uniqueId);
    this->send<SetChargingProfileResponse>(call_result);
}

void ChargePoint::handle_clear_charging_profile_req(Call<ClearChargingProfileRequest> call) {
    EVLOG_debug << "Received ClearChargingProfileRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;
    ClearChargingProfileResponse response;
    response.status = ClearChargingProfileStatusEnum::Unknown;

    // K10.FR.06
    if (msg.chargingProfileCriteria.has_value() and
        msg.chargingProfileCriteria.value().chargingProfilePurpose.has_value() and
        msg.chargingProfileCriteria.value().chargingProfilePurpose.value() ==
            ChargingProfilePurposeEnum::ChargingStationExternalConstraints) {
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = "InvalidValue";
        response.statusInfo->additionalInfo = "ChargingStationExternalConstraintsInClearChargingProfileRequest";
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();
    } else {
        response = this->smart_charging_handler->clear_profiles(msg);
    }

    ocpp::CallResult<ClearChargingProfileResponse> call_result(response, call.uniqueId);
    this->send<ClearChargingProfileResponse>(call_result);
}

void ChargePoint::handle_get_charging_profiles_req(Call<GetChargingProfilesRequest> call) {
    EVLOG_debug << "Received GetChargingProfilesRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;
    GetChargingProfilesResponse response;

    const auto profiles_to_report = this->smart_charging_handler->get_reported_profiles(msg);

    response.status =
        profiles_to_report.empty() ? GetChargingProfileStatusEnum::NoProfiles : GetChargingProfileStatusEnum::Accepted;

    ocpp::CallResult<GetChargingProfilesResponse> call_result(response, call.uniqueId);
    this->send<GetChargingProfilesResponse>(call_result);

    if (response.status == GetChargingProfileStatusEnum::NoProfiles) {
        return;
    }

    // There are profiles to report.
    // Prepare ReportChargingProfileRequest(s). The message defines the properties evseId and chargingLimitSource as
    // required, so we can not report all profiles in a single ReportChargingProfilesRequest. We need to prepare a
    // single ReportChargingProfilesRequest for each combination of evseId and chargingLimitSource
    std::set<int32_t> evse_ids;                // will contain all evse_ids of the profiles
    std::set<ChargingLimitSourceEnum> sources; // will contain all sources of the profiles

    // fill evse_ids and sources sets
    for (const auto& profile : profiles_to_report) {
        evse_ids.insert(profile.evse_id);
        sources.insert(profile.source);
    }

    std::vector<ReportChargingProfilesRequest> requests_to_send;

    for (const auto evse_id : evse_ids) {
        for (const auto source : sources) {
            std::vector<ChargingProfile> original_profiles;
            for (const auto& reported_profile : profiles_to_report) {
                if (reported_profile.evse_id == evse_id and reported_profile.source == source) {
                    original_profiles.push_back(reported_profile.profile);
                }
            }
            if (not original_profiles.empty()) {
                // prepare a ReportChargingProfilesRequest
                ReportChargingProfilesRequest req;
                req.requestId = msg.requestId; // K09.FR.01
                req.evseId = evse_id;
                req.chargingLimitSource = source;
                req.chargingProfile = original_profiles;
                req.tbc = true;
                requests_to_send.push_back(req);
            }
        }
    }

    requests_to_send.back().tbc = false;

    // requests_to_send are ready, send them and define tbc property
    for (const auto& request_to_send : requests_to_send) {
        this->report_charging_profile_req(request_to_send);
    }
}

void ChargePoint::handle_get_composite_schedule_req(Call<GetCompositeScheduleRequest> call) {
    EVLOG_debug << "Received GetCompositeScheduleRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto response = this->get_composite_schedule_internal(call.msg);

    ocpp::CallResult<GetCompositeScheduleResponse> call_result(response, call.uniqueId);
    this->send<GetCompositeScheduleResponse>(call_result);
}

void ChargePoint::handle_firmware_update_req(Call<UpdateFirmwareRequest> call) {
    EVLOG_debug << "Received UpdateFirmwareRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    if (call.msg.firmware.signingCertificate.has_value() or call.msg.firmware.signature.has_value()) {
        this->firmware_status_before_installing = FirmwareStatusEnum::SignatureVerified;
    } else {
        this->firmware_status_before_installing = FirmwareStatusEnum::Downloaded;
    }
    UpdateFirmwareResponse response = callbacks.update_firmware_request_callback(call.msg);

    ocpp::CallResult<UpdateFirmwareResponse> call_result(response, call.uniqueId);
    this->send<UpdateFirmwareResponse>(call_result);

    if ((response.status == UpdateFirmwareStatusEnum::InvalidCertificate) ||
        (response.status == UpdateFirmwareStatusEnum::RevokedCertificate)) {
        // L01.FR.02
        this->security_event_notification_req(
            CiString<50>(ocpp::security_events::INVALIDFIRMWARESIGNINGCERTIFICATE),
            std::optional<CiString<255>>("Provided signing certificate is not valid!"), true,
            true); // critical because TC_L_05_CS requires this message to be sent
    }
}

void ChargePoint::handle_get_installed_certificate_ids_req(Call<GetInstalledCertificateIdsRequest> call) {
    EVLOG_debug << "Received GetInstalledCertificateIdsRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    GetInstalledCertificateIdsResponse response;

    const auto msg = call.msg;

    // prepare argument for getRootCertificate
    std::vector<ocpp::CertificateType> certificate_types;
    if (msg.certificateType.has_value()) {
        certificate_types = ocpp::evse_security_conversions::from_ocpp_v201(msg.certificateType.value());
    } else {
        certificate_types.push_back(CertificateType::CSMSRootCertificate);
        certificate_types.push_back(CertificateType::MFRootCertificate);
        certificate_types.push_back(CertificateType::MORootCertificate);
        certificate_types.push_back(CertificateType::V2GCertificateChain);
        certificate_types.push_back(CertificateType::V2GRootCertificate);
    }

    // retrieve installed certificates
    const auto certificate_hash_data_chains = this->evse_security->get_installed_certificates(certificate_types);

    // convert the common type back to the v201 type(s) for the response
    std::vector<CertificateHashDataChain> certificate_hash_data_chain_v201;
    for (const auto& certificate_hash_data_chain_entry : certificate_hash_data_chains) {
        certificate_hash_data_chain_v201.push_back(
            ocpp::evse_security_conversions::to_ocpp_v201(certificate_hash_data_chain_entry));
    }

    if (certificate_hash_data_chain_v201.empty()) {
        response.status = GetInstalledCertificateStatusEnum::NotFound;
    } else {
        response.certificateHashDataChain = certificate_hash_data_chain_v201;
        response.status = GetInstalledCertificateStatusEnum::Accepted;
    }

    ocpp::CallResult<GetInstalledCertificateIdsResponse> call_result(response, call.uniqueId);
    this->send<GetInstalledCertificateIdsResponse>(call_result);
}

void ChargePoint::handle_install_certificate_req(Call<InstallCertificateRequest> call) {
    EVLOG_debug << "Received InstallCertificateRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto msg = call.msg;
    InstallCertificateResponse response;

    const auto result = this->evse_security->install_ca_certificate(
        msg.certificate.get(), ocpp::evse_security_conversions::from_ocpp_v201(msg.certificateType));
    response.status = ocpp::evse_security_conversions::to_ocpp_v201(result);

    ocpp::CallResult<InstallCertificateResponse> call_result(response, call.uniqueId);
    this->send<InstallCertificateResponse>(call_result);
}

void ChargePoint::handle_delete_certificate_req(Call<DeleteCertificateRequest> call) {
    EVLOG_debug << "Received DeleteCertificateRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;

    const auto msg = call.msg;
    DeleteCertificateResponse response;

    const auto certificate_hash_data = ocpp::evse_security_conversions::from_ocpp_v201(msg.certificateHashData);

    const auto status = this->evse_security->delete_certificate(certificate_hash_data);

    response.status = ocpp::evse_security_conversions::to_ocpp_v201(status);

    ocpp::CallResult<DeleteCertificateResponse> call_result(response, call.uniqueId);
    this->send<DeleteCertificateResponse>(call_result);
}

void ChargePoint::handle_get_log_req(Call<GetLogRequest> call) {
    const GetLogResponse response = this->callbacks.get_log_request_callback(call.msg);

    ocpp::CallResult<GetLogResponse> call_result(response, call.uniqueId);
    this->send<GetLogResponse>(call_result);
}

void ChargePoint::handle_customer_information_req(Call<CustomerInformationRequest> call) {
    CustomerInformationResponse response;
    const auto& msg = call.msg;
    response.status = CustomerInformationStatusEnum::Accepted;

    if (!msg.report and !msg.clear) {
        EVLOG_warning << "CSMS sent CustomerInformation.req with both report and clear flags being false";
        response.status = CustomerInformationStatusEnum::Rejected;
    }

    if (!msg.customerCertificate.has_value() and !msg.idToken.has_value() and !msg.customerIdentifier.has_value()) {
        EVLOG_warning << "CSMS sent CustomerInformation.req without setting one of customerCertificate, idToken, "
                         "customerIdentifier fields";
        response.status = CustomerInformationStatusEnum::Invalid;
    }

    ocpp::CallResult<CustomerInformationResponse> call_result(response, call.uniqueId);
    this->send<CustomerInformationResponse>(call_result);

    if (response.status == CustomerInformationStatusEnum::Accepted) {
        std::string data = "";
        if (msg.report) {
            data += this->get_customer_information(msg.customerCertificate, msg.idToken, msg.customerIdentifier);
        }
        if (msg.clear) {
            this->clear_customer_information(msg.customerCertificate, msg.idToken, msg.customerIdentifier);
        }

        const auto max_customer_information_data_length =
            this->device_model->get_optional_value<int>(ControllerComponentVariables::MaxCustomerInformationDataLength)
                .value_or(DEFAULT_MAX_CUSTOMER_INFORMATION_DATA_LENGTH);
        if (data.length() > max_customer_information_data_length) {
            EVLOG_warning << "NotifyCustomerInformation.req data field is too large. Cropping it down to: "
                          << max_customer_information_data_length << "characters";
            data.erase(max_customer_information_data_length);
        }

        this->notify_customer_information_req(data, msg.requestId);
    }
}

void ChargePoint::handle_set_monitoring_base_req(Call<SetMonitoringBaseRequest> call) {
    SetMonitoringBaseResponse response;
    const auto& msg = call.msg;

    auto result = this->device_model->set_value(
        ControllerComponentVariables::ActiveMonitoringBase.component,
        ControllerComponentVariables::ActiveMonitoringBase.variable.value(), AttributeEnum::Actual,
        conversions::monitoring_base_enum_to_string(msg.monitoringBase), VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS, true);

    if (result != SetVariableStatusEnum::Accepted) {
        EVLOG_warning << "Could not persist in device model new monitoring base: "
                      << conversions::monitoring_base_enum_to_string(msg.monitoringBase);
        response.status = GenericDeviceModelStatusEnum::Rejected;
    } else {
        response.status = GenericDeviceModelStatusEnum::Accepted;

        if (msg.monitoringBase == MonitoringBaseEnum::HardWiredOnly ||
            msg.monitoringBase == MonitoringBaseEnum::FactoryDefault) {
            try {
                this->device_model->clear_custom_monitors();
            } catch (const DeviceModelStorageError& e) {
                EVLOG_warning << "Could not clear custom monitors from DB: " << e.what();
                response.status = GenericDeviceModelStatusEnum::Rejected;
            }
        }
    }

    ocpp::CallResult<SetMonitoringBaseResponse> call_result(response, call.uniqueId);
    this->send<SetMonitoringBaseResponse>(call_result);
}

void ChargePoint::handle_set_monitoring_level_req(Call<SetMonitoringLevelRequest> call) {
    SetMonitoringLevelResponse response;
    const auto& msg = call.msg;

    if (msg.severity < MonitoringLevelSeverity::MIN || msg.severity > MonitoringLevelSeverity::MAX) {
        response.status = GenericStatusEnum::Rejected;
    } else {
        auto result = this->device_model->set_value(
            ControllerComponentVariables::ActiveMonitoringLevel.component,
            ControllerComponentVariables::ActiveMonitoringLevel.variable.value(), AttributeEnum::Actual,
            std::to_string(msg.severity), VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS, true);

        if (result != SetVariableStatusEnum::Accepted) {
            EVLOG_warning << "Could not persist in device model new monitoring level: " << msg.severity;
            response.status = GenericStatusEnum::Rejected;
        } else {
            response.status = GenericStatusEnum::Accepted;
        }
    }

    ocpp::CallResult<SetMonitoringLevelResponse> call_result(response, call.uniqueId);
    this->send<SetMonitoringLevelResponse>(call_result);
}

void ChargePoint::handle_set_variable_monitoring_req(const EnhancedMessage<v201::MessageType>& message) {
    Call<SetVariableMonitoringRequest> call = message.call_message;
    SetVariableMonitoringResponse response;
    const auto& msg = call.msg;

    const auto max_items_per_message =
        this->device_model->get_value<int>(ControllerComponentVariables::ItemsPerMessageSetVariableMonitoring);
    const auto max_bytes_message =
        this->device_model->get_value<int>(ControllerComponentVariables::BytesPerMessageSetVariableMonitoring);

    // N04.FR.09
    if (msg.setMonitoringData.size() > max_items_per_message) {
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->send(call_error);
        return;
    }

    if (message.message_size > max_bytes_message) {
        const auto call_error = CallError(call.uniqueId, "FormatViolation", "", json({}));
        this->send(call_error);
        return;
    }

    try {
        response.setMonitoringResult = this->device_model->set_monitors(msg.setMonitoringData);
    } catch (const DeviceModelStorageError& e) {
        EVLOG_error << "Set monitors failed:" << e.what();
    }

    ocpp::CallResult<SetVariableMonitoringResponse> call_result(response, call.uniqueId);
    this->send<SetVariableMonitoringResponse>(call_result);
}

void ChargePoint::notify_monitoring_report_req(const int request_id,
                                               const std::vector<MonitoringData>& montoring_data) {
    static constexpr int32_t MAXIMUM_VARIABLE_SEND = 10;

    if (montoring_data.size() <= MAXIMUM_VARIABLE_SEND) {
        NotifyMonitoringReportRequest req;
        req.requestId = request_id;
        req.seqNo = 0;
        req.generatedAt = ocpp::DateTime();
        req.monitor.emplace(montoring_data);
        req.tbc = false;

        ocpp::Call<NotifyMonitoringReportRequest> call(req, this->message_queue->createMessageId());
        this->send<NotifyMonitoringReportRequest>(call);
    } else {
        // Split for larger message sizes
        int32_t sequence_num = 0;
        auto generated_at = ocpp::DateTime();

        for (int32_t i = 0; i < montoring_data.size(); i += MAXIMUM_VARIABLE_SEND) {
            // If our next index is >= than the last index then we're finished
            bool last_part = ((i + MAXIMUM_VARIABLE_SEND) >= montoring_data.size());

            NotifyMonitoringReportRequest req;
            req.requestId = request_id;
            req.seqNo = sequence_num;
            req.generatedAt = generated_at;
            req.tbc = (!last_part);

            // Construct sub-message part
            std::vector<MonitoringData> sub_data;

            for (int32_t j = i; j < MAXIMUM_VARIABLE_SEND && j < montoring_data.size(); ++j) {
                sub_data.push_back(std::move(montoring_data[i + j]));
            }

            req.monitor = sub_data;

            ocpp::Call<NotifyMonitoringReportRequest> call(req, this->message_queue->createMessageId());
            this->send<NotifyMonitoringReportRequest>(call);

            sequence_num++;
        }
    }
}

void ChargePoint::handle_get_monitoring_report_req(Call<GetMonitoringReportRequest> call) {
    GetMonitoringReportResponse response;
    const auto& msg = call.msg;

    const auto component_variables = msg.componentVariable.value_or(std::vector<ComponentVariable>());
    const auto max_variable_components_per_message =
        this->device_model->get_value<int>(ControllerComponentVariables::ItemsPerMessageGetReport);

    // N02.FR.07
    if (component_variables.size() > max_variable_components_per_message) {
        const auto call_error = CallError(call.uniqueId, "OccurenceConstraintViolation", "", json({}));
        this->send(call_error);
        return;
    }

    auto criteria = msg.monitoringCriteria.value_or(std::vector<MonitoringCriterionEnum>());
    std::vector<MonitoringData> data{};

    try {
        data = this->device_model->get_monitors(criteria, component_variables);

        if (!data.empty()) {
            response.status = GenericDeviceModelStatusEnum::Accepted;
        } else {
            response.status = GenericDeviceModelStatusEnum::EmptyResultSet;
        }
    } catch (const DeviceModelStorageError& e) {
        EVLOG_error << "Get variable monitoring failed:" << e.what();
        response.status = GenericDeviceModelStatusEnum::Rejected;
    }

    ocpp::CallResult<GetMonitoringReportResponse> call_result(response, call.uniqueId);
    this->send<GetMonitoringReportResponse>(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        // Send the result with splits if required
        notify_monitoring_report_req(msg.requestId, data);
    }
}

void ChargePoint::handle_clear_variable_monitoring_req(Call<ClearVariableMonitoringRequest> call) {
    ClearVariableMonitoringResponse response;
    const auto& msg = call.msg;

    try {
        response.clearMonitoringResult = this->device_model->clear_monitors(msg.id);
    } catch (const DeviceModelStorageError& e) {
        EVLOG_error << "Clear variable monitoring failed:" << e.what();
    }

    ocpp::CallResult<ClearVariableMonitoringResponse> call_result(response, call.uniqueId);
    this->send<ClearVariableMonitoringResponse>(call_result);
}

void ChargePoint::handle_data_transfer_req(Call<DataTransferRequest> call) {
    const auto msg = call.msg;
    DataTransferResponse response;

    if (this->callbacks.data_transfer_callback.has_value()) {
        response = this->callbacks.data_transfer_callback.value()(call.msg);
    } else {
        response.status = DataTransferStatusEnum::UnknownVendorId;
        EVLOG_warning << "Received a DataTransferRequest but no data transfer callback was registered";
    }

    ocpp::CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->send<DataTransferResponse>(call_result);
}

template <class T> bool ChargePoint::send(ocpp::Call<T> call, const bool initiated_by_trigger_message) {
    const auto message_type = conversions::string_to_messagetype(json(call).at(CALL_ACTION));
    const auto message_transmission_priority = get_message_transmission_priority(
        is_boot_notification_message(message_type), initiated_by_trigger_message,
        (this->registration_status == RegistrationStatusEnum::Accepted), is_transaction_message(message_type),
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages).value_or(false));
    switch (message_transmission_priority) {
    case MessageTransmissionPriority::SendImmediately:
        this->message_queue->push(call);
        return true;
    case MessageTransmissionPriority::SendAfterRegistrationStatusAccepted:
        this->message_queue->push(call, true);
        return true;
    case MessageTransmissionPriority::Discard:
        return false;
    }
    throw std::runtime_error("Missing handling for MessageTransmissionPriority");
}

template <class T> std::future<EnhancedMessage<v201::MessageType>> ChargePoint::send_async(ocpp::Call<T> call) {
    const auto message_type = conversions::string_to_messagetype(json(call).at(CALL_ACTION));
    const auto message_transmission_priority = get_message_transmission_priority(
        is_boot_notification_message(message_type), false,
        (this->registration_status == RegistrationStatusEnum::Accepted), is_transaction_message(message_type),
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages).value_or(false));
    switch (message_transmission_priority) {
    case MessageTransmissionPriority::SendImmediately:
        return this->message_queue->push_async(call);
    case MessageTransmissionPriority::SendAfterRegistrationStatusAccepted:
    case MessageTransmissionPriority::Discard:
        auto promise = std::promise<EnhancedMessage<MessageType>>();
        auto enhanced_message = EnhancedMessage<MessageType>();
        enhanced_message.offline = true;
        promise.set_value(enhanced_message);
        return promise.get_future();
    }
    throw std::runtime_error("Missing handling for MessageTransmissionPriority");
}

template <class T> bool ChargePoint::send(ocpp::CallResult<T> call_result) {
    this->message_queue->push(call_result);
    return true;
}

std::optional<DataTransferResponse> ChargePoint::data_transfer_req(const CiString<255>& vendorId,
                                                                   const std::optional<CiString<50>>& messageId,
                                                                   const std::optional<json>& data) {
    DataTransferRequest req;
    req.vendorId = vendorId;
    req.messageId = messageId;
    req.data = data;

    return this->data_transfer_req(req);
}

std::optional<DataTransferResponse> ChargePoint::data_transfer_req(const DataTransferRequest& request) {
    DataTransferResponse response;
    response.status = DataTransferStatusEnum::Rejected;

    ocpp::Call<DataTransferRequest> call(request, this->message_queue->createMessageId());
    auto data_transfer_future = this->send_async<DataTransferRequest>(call);

    if (!this->connectivity_manager->is_websocket_connected()) {
        return std::nullopt;
    }

    if (data_transfer_future.wait_for(DEFAULT_WAIT_FOR_FUTURE_TIMEOUT) == std::future_status::timeout) {
        EVLOG_warning << "Waiting for DataTransfer.conf future timed out";
        return std::nullopt;
    }

    auto enhanced_message = data_transfer_future.get();
    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        try {
            ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
            response = call_result.msg;
        } catch (const EnumConversionException& e) {
            EVLOG_error << "EnumConversionException during handling of message: " << e.what();
            auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
            this->send(call_error);
            return std::nullopt;
        }
    }
    if (enhanced_message.offline) {
        return std::nullopt;
    }

    return response;
}

void ChargePoint::handle_send_local_authorization_list_req(Call<SendLocalListRequest> call) {
    SendLocalListResponse response;

    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        response.status = apply_local_authorization_list(call.msg);
    } else {
        response.status = SendLocalListStatusEnum::Failed;
    }

    // Set nr of entries in device_model
    if (response.status == SendLocalListStatusEnum::Accepted) {
        try {
            this->database_handler->insert_or_update_local_authorization_list_version(call.msg.versionNumber);
            auto& local_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
            if (local_entries.variable.has_value()) {
                try {
                    auto entries = this->database_handler->get_local_authorization_list_number_of_entries();
                    this->device_model->set_read_only_value(local_entries.component, local_entries.variable.value(),
                                                            AttributeEnum::Actual, std::to_string(entries),
                                                            VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
                } catch (const DeviceModelStorageError& e) {
                    EVLOG_warning << "Could not get local list count from database:" << e.what();
                } catch (const DatabaseException& e) {
                    EVLOG_warning << "Could not get local list count from database: " << e.what();
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not get local list count from database: " << e.what();
                }
            }
        } catch (const DatabaseException& e) {
            EVLOG_warning << "Could not update local authorization list in database: " << e.what();
            response.status = SendLocalListStatusEnum::Failed;
        }
    }

    ocpp::CallResult<SendLocalListResponse> call_result(response, call.uniqueId);
    this->send<SendLocalListResponse>(call_result);
}

void ChargePoint::handle_get_local_authorization_list_version_req(Call<GetLocalListVersionRequest> call) {
    GetLocalListVersionResponse response;

    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        try {
            response.versionNumber = this->database_handler->get_local_authorization_list_version();
        } catch (const DatabaseException& e) {
            const auto call_error = CallError(call.uniqueId, "InternalError",
                                              "Unable to retrieve LocalListVersion from the database", json({}));
            this->send(call_error);
            return;
        }
    } else {
        response.versionNumber = 0;
    }

    ocpp::CallResult<GetLocalListVersionResponse> call_result(response, call.uniqueId);
    this->send<GetLocalListVersionResponse>(call_result);
}

void ChargePoint::scheduled_check_client_certificate_expiration() {

    EVLOG_info << "Checking if CSMS client certificate has expired";
    int expiry_days_count =
        this->evse_security->get_leaf_expiry_days_count(ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
    if (expiry_days_count < 30) {
        EVLOG_info << "CSMS client certificate is invalid in " << expiry_days_count
                   << " days. Requesting new certificate with certificate signing request";
        this->sign_certificate_req(ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
    } else {
        EVLOG_info << "CSMS client certificate is still valid.";
    }

    this->client_certificate_expiration_check_timer.interval(std::chrono::seconds(
        this->device_model
            ->get_optional_value<int>(ControllerComponentVariables::ClientCertificateExpireCheckIntervalSeconds)
            .value_or(12 * 60 * 60)));
}

void ChargePoint::scheduled_check_v2g_certificate_expiration() {
    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::V2GCertificateInstallationEnabled)
            .value_or(false)) {
        EVLOG_info << "Checking if V2GCertificate has expired";
        int expiry_days_count =
            this->evse_security->get_leaf_expiry_days_count(ocpp::CertificateSigningUseEnum::V2GCertificate);
        if (expiry_days_count < 30) {
            EVLOG_info << "V2GCertificate is invalid in " << expiry_days_count
                       << " days. Requesting new certificate with certificate signing request";
            this->sign_certificate_req(ocpp::CertificateSigningUseEnum::V2GCertificate);
        } else {
            EVLOG_info << "V2GCertificate is still valid.";
        }
    } else {
        if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::PnCEnabled).value_or(false)) {
            EVLOG_warning << "PnC is enabled but V2G certificate installation is not, so no certificate expiration "
                             "check is performed.";
        }
    }

    this->v2g_certificate_expiration_check_timer.interval(std::chrono::seconds(
        this->device_model
            ->get_optional_value<int>(ControllerComponentVariables::V2GCertificateExpireCheckIntervalSeconds)
            .value_or(12 * 60 * 60)));
}

void ChargePoint::websocket_connected_callback(const int security_profile) {
    this->message_queue->resume(this->message_queue_resume_delay);

    const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
    if (security_profile_cv.variable.has_value()) {
        this->device_model->set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                                AttributeEnum::Actual, std::to_string(security_profile),
                                                VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    if (this->registration_status == RegistrationStatusEnum::Accepted and
        this->time_disconnected.time_since_epoch() != 0s) {
        // handle offline threshold
        //  Get the current time point using steady_clock
        auto offline_duration = std::chrono::steady_clock::now() - this->time_disconnected;

        // B04.FR.01
        // If offline period exceeds offline threshold then send the status notification for all connectors
        if (offline_duration >
            std::chrono::seconds(this->device_model->get_value<int>(ControllerComponentVariables::OfflineThreshold))) {
            EVLOG_debug << "offline for more than offline threshold ";
            this->component_state_manager->send_status_notification_all_connectors();
        } else {
            // B04.FR.02
            // If offline period doesn't exceed offline threshold then send the status notification for all
            // connectors that changed state
            EVLOG_debug << "offline for less than offline threshold ";
            this->component_state_manager->send_status_notification_changed_connectors();
        }
        this->init_certificate_expiration_check_timers(); // re-init as timers are stopped on disconnect
    }
    this->time_disconnected = std::chrono::time_point<std::chrono::steady_clock>();

    // We have a connection again so next time it fails we should send the notification again
    this->skip_invalid_csms_certificate_notifications = false;

    if (this->callbacks.connection_state_changed_callback.has_value()) {
        this->callbacks.connection_state_changed_callback.value()(true);
    }
}

void ChargePoint::websocket_disconnected_callback() {
    this->message_queue->pause();

    // check if offline threshold has been defined
    if (this->device_model->get_value<int>(ControllerComponentVariables::OfflineThreshold) != 0) {
        // Get the current time point using steady_clock
        this->time_disconnected = std::chrono::steady_clock::now();
    }

    this->client_certificate_expiration_check_timer.stop();
    this->v2g_certificate_expiration_check_timer.stop();
    if (this->callbacks.connection_state_changed_callback.has_value()) {
        this->callbacks.connection_state_changed_callback.value()(false);
    }
}

void ChargePoint::websocket_connection_failed(ConnectionFailedReason reason) {
    switch (reason) {
    case ConnectionFailedReason::InvalidCSMSCertificate:
        if (!this->skip_invalid_csms_certificate_notifications) {
            this->security_event_notification_req(CiString<50>(ocpp::security_events::INVALIDCSMSCERTIFICATE),
                                                  std::nullopt, true, true);
            this->skip_invalid_csms_certificate_notifications = true;
        } else {
            EVLOG_debug << "Skipping InvalidCsmsCertificate SecurityEvent since it has been sent already";
        }
        break;
    }
}

void ChargePoint::trigger_authorization_cache_cleanup() {
    {
        std::scoped_lock lk(this->auth_cache_cleanup_mutex);
        this->auth_cache_cleanup_required = true;
    }
    this->auth_cache_cleanup_cv.notify_one();
}

void ChargePoint::cache_cleanup_handler() {
    // Run the update once so the ram variable gets initialized
    this->update_authorization_cache_size();

    while (true) {
        {
            // Wait for next wakeup or timeout
            std::unique_lock lk(this->auth_cache_cleanup_mutex);
            if (this->auth_cache_cleanup_cv.wait_for(lk, std::chrono::minutes(15), [&]() {
                    return this->stop_auth_cache_cleanup_handler || this->auth_cache_cleanup_required;
                })) {
                EVLOG_debug << "Triggered authorization cache cleanup";
            } else {
                EVLOG_debug << "Time based authorization cache cleanup";
            }
            this->auth_cache_cleanup_required = false;
        }

        if (this->stop_auth_cache_cleanup_handler) {
            break;
        }

        auto lifetime = this->device_model->get_optional_value<int>(ControllerComponentVariables::AuthCacheLifeTime);
        try {
            this->database_handler->authorization_cache_delete_expired_entries(
                lifetime.has_value() ? std::optional<std::chrono::seconds>(*lifetime) : std::nullopt);

            auto meta_data = this->device_model->get_variable_meta_data(
                ControllerComponentVariables::AuthCacheStorage.component,
                ControllerComponentVariables::AuthCacheStorage.variable.value());

            if (meta_data.has_value()) {
                auto max_storage = meta_data->characteristics.maxLimit;
                if (max_storage.has_value()) {
                    while (this->database_handler->authorization_cache_get_binary_size() > max_storage.value()) {
                        this->database_handler->authorization_cache_delete_nr_of_oldest_entries(1);
                    }
                }
            }
        } catch (const DatabaseException& e) {
            EVLOG_warning << "Could not delete expired authorization cache entries from database: " << e.what();
        } catch (const std::exception& e) {
            EVLOG_warning << "Could not delete expired authorization cache entries from database: " << e.what();
        }

        this->update_authorization_cache_size();
    }
}

GetCompositeScheduleResponse ChargePoint::get_composite_schedule_internal(const GetCompositeScheduleRequest& request) {
    GetCompositeScheduleResponse response;
    response.status = GenericStatusEnum::Rejected;

    auto supported_charging_rate_units =
        this->device_model->get_value<std::string>(ControllerComponentVariables::ChargingScheduleChargingRateUnit);
    auto unit_supported = supported_charging_rate_units.find(conversions::charging_rate_unit_enum_to_string(
                              request.chargingRateUnit.value())) != supported_charging_rate_units.npos;

    // K01.FR.05 & K01.FR.07
    if (this->evse_manager->does_evse_exist(request.evseId) && unit_supported) {
        auto start_time = ocpp::DateTime();
        auto end_time = ocpp::DateTime(start_time.to_time_point() + std::chrono::seconds(request.duration));

        std::vector<ChargingProfile> valid_profiles = this->smart_charging_handler->get_valid_profiles(request.evseId);

        auto schedule = this->smart_charging_handler->calculate_composite_schedule(
            valid_profiles, start_time, end_time, request.evseId, request.chargingRateUnit);

        response.schedule = schedule;
        response.status = GenericStatusEnum::Accepted;
    } else {
        auto reason = unit_supported ? ProfileValidationResultEnum::EvseDoesNotExist
                                     : ProfileValidationResultEnum::ChargingScheduleChargingRateUnitUnsupported;
        response.statusInfo = StatusInfo();
        response.statusInfo->reasonCode = conversions::profile_validation_result_to_reason_code(reason);
        response.statusInfo->additionalInfo = conversions::profile_validation_result_to_string(reason);
        EVLOG_debug << "Rejecting SetChargingProfileRequest:\n reasonCode: " << response.statusInfo->reasonCode.get()
                    << "\nadditionalInfo: " << response.statusInfo->additionalInfo->get();
    }
    return response;
}

void ChargePoint::update_dm_availability_state(const int32_t evse_id, const int32_t connector_id,
                                               const ConnectorStatusEnum status) {
    ComponentVariable charging_station = ControllerComponentVariables::ChargingStationAvailabilityState;
    ComponentVariable evse_cv =
        EvseComponentVariables::get_component_variable(evse_id, EvseComponentVariables::AvailabilityState);
    ComponentVariable connector_cv = ConnectorComponentVariables::get_component_variable(
        evse_id, connector_id, ConnectorComponentVariables::AvailabilityState);
    if (evse_cv.variable.has_value()) {
        this->device_model->set_read_only_value(
            evse_cv.component, evse_cv.variable.value(), ocpp::v201::AttributeEnum::Actual,
            conversions::connector_status_enum_to_string(status), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }
    if (connector_cv.variable.has_value()) {
        this->device_model->set_read_only_value(
            connector_cv.component, connector_cv.variable.value(), ocpp::v201::AttributeEnum::Actual,
            conversions::connector_status_enum_to_string(status), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    // if applicable to the entire charging station
    if (evse_id == 0 and charging_station.variable.has_value()) {
        this->device_model->set_read_only_value(
            charging_station.component, charging_station.variable.value(), ocpp::v201::AttributeEnum::Actual,
            conversions::connector_status_enum_to_string(status), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }
}

void ChargePoint::update_dm_evse_power(const int32_t evse_id, const MeterValue& meter_value) {
    ComponentVariable evse_power_cv =
        EvseComponentVariables::get_component_variable(evse_id, EvseComponentVariables::Power);

    if (!evse_power_cv.variable.has_value()) {
        return;
    }

    const auto power = utils::get_total_power_active_import(meter_value);
    if (!power.has_value()) {
        return;
    }

    this->device_model->set_read_only_value(evse_power_cv.component, evse_power_cv.variable.value(),
                                            AttributeEnum::Actual, std::to_string(power.value()),
                                            VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
}

void ChargePoint::set_cs_operative_status(OperationalStatusEnum new_status, bool persist) {
    this->component_state_manager->set_cs_individual_operational_status(new_status, persist);
}

void ChargePoint::set_evse_operative_status(int32_t evse_id, OperationalStatusEnum new_status, bool persist) {
    this->evse_manager->get_evse(evse_id).set_evse_operative_status(new_status, persist);
}

void ChargePoint::set_connector_operative_status(int32_t evse_id, int32_t connector_id,
                                                 OperationalStatusEnum new_status, bool persist) {
    this->evse_manager->get_evse(evse_id).set_connector_operative_status(connector_id, new_status, persist);
}

bool ChargePoint::are_all_connectors_effectively_inoperative() {
    // Check that all connectors on all EVSEs are inoperative
    for (const auto& evse : *this->evse_manager) {
        for (int connector_id = 1; connector_id <= evse.get_number_of_connectors(); connector_id++) {
            OperationalStatusEnum connector_status =
                this->component_state_manager->get_connector_effective_operational_status(evse.get_id(), connector_id);
            if (connector_status == OperationalStatusEnum::Operative) {
                return false;
            }
        }
    }
    return true;
}

void ChargePoint::execute_change_availability_request(ChangeAvailabilityRequest request, bool persist) {
    if (request.evse.has_value()) {
        if (request.evse.value().connectorId.has_value()) {
            this->set_connector_operative_status(request.evse.value().id, request.evse.value().connectorId.value(),
                                                 request.operationalStatus, persist);
        } else {
            this->set_evse_operative_status(request.evse.value().id, request.operationalStatus, persist);
        }
    } else {
        this->set_cs_operative_status(request.operationalStatus, persist);
    }
}

// K01.27 - load profiles from database
void ChargePoint::load_charging_profiles() {
    try {
        auto evses = this->database_handler->get_all_charging_profiles_group_by_evse();
        EVLOG_info << "Found " << evses.size() << " evse in the database";
        for (const auto& [evse_id, profiles] : evses) {
            for (auto profile : profiles) {
                try {
                    if (this->smart_charging_handler->validate_profile(profile, evse_id) ==
                        ProfileValidationResultEnum::Valid) {
                        this->smart_charging_handler->add_profile(profile, evse_id);
                    } else {
                        // delete if not valid anymore
                        this->database_handler->delete_charging_profile(profile.id);
                    }
                } catch (const QueryExecutionException& e) {
                    EVLOG_warning << "Failed database operation for ChargingProfiles: " << e.what();
                }
            }
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Unknown error while loading charging profiles from database: " << e.what();
    }
}

std::vector<GetVariableResult>
ChargePoint::get_variables(const std::vector<GetVariableData>& get_variable_data_vector) {
    std::vector<GetVariableResult> response;
    for (const auto& get_variable_data : get_variable_data_vector) {
        GetVariableResult get_variable_result;
        get_variable_result.component = get_variable_data.component;
        get_variable_result.variable = get_variable_data.variable;
        get_variable_result.attributeType = get_variable_data.attributeType.value_or(AttributeEnum::Actual);
        const auto request_value_response = this->device_model->request_value<std::string>(
            get_variable_data.component, get_variable_data.variable,
            get_variable_data.attributeType.value_or(AttributeEnum::Actual));
        if (request_value_response.status == GetVariableStatusEnum::Accepted and
            request_value_response.value.has_value()) {
            get_variable_result.attributeValue = request_value_response.value.value();
        }
        get_variable_result.attributeStatus = request_value_response.status;
        response.push_back(get_variable_result);
    }
    return response;
}

std::map<SetVariableData, SetVariableResult>
ChargePoint::set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) {
    // set variables and allow setting of ReadOnly variables
    const auto response = this->set_variables_internal(set_variable_data_vector, source, true);
    this->handle_variables_changed(response);
    return response;
}

GetCompositeScheduleResponse ChargePoint::get_composite_schedule(const GetCompositeScheduleRequest& request) {
    return this->get_composite_schedule_internal(request);
}

std::vector<CompositeSchedule> ChargePoint::get_all_composite_schedules(const int32_t duration_s,
                                                                        const ChargingRateUnitEnum& unit) {
    std::vector<CompositeSchedule> composite_schedules;

    const auto number_of_evses = this->evse_manager->get_number_of_evses();
    // get all composite schedules including the one for evse_id == 0
    for (int32_t evse_id = 0; evse_id <= number_of_evses; evse_id++) {
        GetCompositeScheduleRequest request;
        request.duration = duration_s;
        request.evseId = evse_id;
        request.chargingRateUnit = unit;
        auto composite_schedule_response = this->get_composite_schedule_internal(request);
        if (composite_schedule_response.status == GenericStatusEnum::Accepted and
            composite_schedule_response.schedule.has_value()) {
            composite_schedules.push_back(composite_schedule_response.schedule.value());
        } else {
            EVLOG_warning << "Could not internally retrieve composite schedule: " << composite_schedule_response;
        }
    }

    return composite_schedules;
}

} // namespace v201
} // namespace ocpp
