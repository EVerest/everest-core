// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/charge_point.hpp>

#include <ocpp/common/constants.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/database_handler.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/device_model_storage_interface.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>
#include <ocpp/v2/evse_manager.hpp>
#include <ocpp/v2/functional_blocks/functional_block_context.hpp>
#include <ocpp/v2/message_dispatcher.hpp>
#include <ocpp/v2/notify_report_requests_splitter.hpp>

#include <ocpp/v2/functional_blocks/authorization.hpp>
#include <ocpp/v2/functional_blocks/availability.hpp>
#include <ocpp/v2/functional_blocks/data_transfer.hpp>
#include <ocpp/v2/functional_blocks/diagnostics.hpp>
#include <ocpp/v2/functional_blocks/display_message.hpp>
#include <ocpp/v2/functional_blocks/firmware_update.hpp>
#include <ocpp/v2/functional_blocks/meter_values.hpp>
#include <ocpp/v2/functional_blocks/provisioning.hpp>
#include <ocpp/v2/functional_blocks/remote_transaction_control.hpp>
#include <ocpp/v2/functional_blocks/reservation.hpp>
#include <ocpp/v2/functional_blocks/security.hpp>
#include <ocpp/v2/functional_blocks/smart_charging.hpp>
#include <ocpp/v2/functional_blocks/tariff_and_cost.hpp>
#include <ocpp/v2/functional_blocks/transaction.hpp>

#include <ocpp/v21/functional_blocks/bidirectional.hpp>

#include <ocpp/v2/messages/LogStatusNotification.hpp>
#include <ocpp/v2/messages/RequestStopTransaction.hpp>
#include <ocpp/v2/messages/TriggerMessage.hpp>

#include <optional>
#include <stdexcept>
#include <string>

using namespace std::chrono_literals;

namespace ocpp {
namespace v2 {

const auto DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD = 2E5;

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         std::shared_ptr<DeviceModel> device_model, std::shared_ptr<DatabaseHandler> database_handler,
                         std::shared_ptr<MessageQueue<v2::MessageType>> message_queue,
                         const std::string& message_log_path, const std::shared_ptr<EvseSecurity> evse_security,
                         const Callbacks& callbacks) :
    ocpp::ChargingStationBase(evse_security),
    message_queue(message_queue),
    device_model(device_model),
    database_handler(database_handler),
    registration_status(RegistrationStatusEnum::Rejected),
    skip_invalid_csms_certificate_notifications(false),
    upload_log_status(UploadLogStatusEnum::Idle),
    bootreason(BootReasonEnum::PowerUp),
    ocsp_updater(this->evse_security, this->send_callback<GetCertificateStatusRequest, GetCertificateStatusResponse>(
                                          MessageType::GetCertificateStatusResponse)),
    callbacks(callbacks) {

    if (!this->device_model) {
        EVLOG_AND_THROW(std::invalid_argument("Device model should not be null"));
    }

    // Make sure the received callback struct is completely filled early before we actually start running
    if (!this->callbacks.all_callbacks_valid(this->device_model, evse_connector_structure)) {
        EVLOG_AND_THROW(std::invalid_argument("All non-optional callbacks must be supplied"));
    }

    if (!this->database_handler) {
        EVLOG_AND_THROW(std::invalid_argument("Database handler should not be null"));
    }

    initialize(evse_connector_structure, message_log_path);
}

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         std::unique_ptr<DeviceModelStorageInterface> device_model_storage_interface,
                         const std::string& ocpp_main_path, const std::string& core_database_path,
                         const std::string& sql_init_path, const std::string& message_log_path,
                         const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks) :
    ChargePoint(
        evse_connector_structure, std::make_shared<DeviceModel>(std::move(device_model_storage_interface)),
        std::make_shared<DatabaseHandler>(
            std::make_unique<everest::db::sqlite::Connection>(fs::path(core_database_path) / "cp.db"), sql_init_path),
        nullptr /* message_queue initialized in this constructor */, message_log_path, evse_security, callbacks) {
}

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         const std::string& device_model_storage_address,
                         const std::string& device_model_migration_path, const std::string& device_model_config_path,
                         const std::string& ocpp_main_path, const std::string& core_database_path,
                         const std::string& sql_init_path, const std::string& message_log_path,
                         const std::shared_ptr<EvseSecurity> evse_security, const Callbacks& callbacks) :
    ChargePoint(evse_connector_structure,
                std::make_unique<DeviceModelStorageSqlite>(device_model_storage_address, device_model_migration_path,
                                                           device_model_config_path),
                ocpp_main_path, core_database_path, sql_init_path, message_log_path, evse_security, callbacks) {
}

ChargePoint::~ChargePoint() {
}

void ChargePoint::start(BootReasonEnum bootreason, bool start_connecting) {
    this->message_queue->start();

    this->bootreason = bootreason;
    // Trigger all initial status notifications and callbacks related to component state
    // Should be done before sending the BootNotification.req so that the correct states can be reported
    this->component_state_manager->trigger_all_effective_availability_changed_callbacks();
    // get transaction messages from db (if there are any) so they can be sent again.
    this->message_queue->get_persisted_messages_from_db();
    this->provisioning->boot_notification_req(bootreason);
    // call clear_invalid_charging_profiles when system boots
    this->clear_invalid_charging_profiles();

    if (start_connecting) {
        this->connectivity_manager->connect();
    }

    const std::string firmware_version =
        this->device_model->get_value<std::string>(ControllerComponentVariables::FirmwareVersion);

    if (this->bootreason == BootReasonEnum::RemoteReset) {
        this->security->security_event_notification_req(
            CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
            std::optional<CiString<255>>("Charging Station rebooted due to requested remote reset!"), true, true);
    } else if (this->bootreason == BootReasonEnum::ScheduledReset) {
        this->security->security_event_notification_req(
            CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
            std::optional<CiString<255>>("Charging Station rebooted due to a scheduled reset!"), true, true);
    } else if (this->bootreason == BootReasonEnum::PowerUp) {
        std::string startup_message = "Charging Station powered up! Firmware version: " + firmware_version;
        this->security->security_event_notification_req(CiString<50>(ocpp::security_events::STARTUP_OF_THE_DEVICE),
                                                        std::optional<CiString<255>>(startup_message), true, true);
    } else if (this->bootreason == BootReasonEnum::FirmwareUpdate) {
        std::string startup_message =
            "Charging station reboot after firmware update. Firmware version: " + firmware_version;
        this->security->security_event_notification_req(CiString<50>(ocpp::security_events::FIRMWARE_UPDATED),
                                                        std::optional<CiString<255>>(startup_message), true, true);
    } else {
        std::string startup_message = "Charging station reset or reboot. Firmware version: " + firmware_version;
        this->security->security_event_notification_req(CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
                                                        std::optional<CiString<255>>(startup_message), true, true);
    }
}

void ChargePoint::stop() {
    this->ocsp_updater.stop();
    this->availability->stop_heartbeat_timer();
    this->provisioning->stop_bootnotification_timer();
    this->connectivity_manager->disconnect();
    this->security->stop_certificate_expiration_check_timers();
    this->diagnostics->stop_monitoring();
    this->message_queue->stop();
    this->security->stop_certificate_signed_timer();
}

void ChargePoint::disconnect_websocket() {
    this->connectivity_manager->disconnect();
}

void ChargePoint::on_network_disconnected(OCPPInterfaceEnum ocpp_interface) {
    this->connectivity_manager->on_network_disconnected(ocpp_interface);
}

void ChargePoint::on_firmware_update_status_notification(int32_t request_id,
                                                         const FirmwareStatusEnum& firmware_update_status) {
    this->firmware_update->on_firmware_update_status_notification(request_id, firmware_update_status);
}

void ChargePoint::connect_websocket(std::optional<int32_t> network_profile_slot) {
    this->connectivity_manager->connect(network_profile_slot);
}
void ChargePoint::on_session_started(const int32_t evse_id, const int32_t connector_id) {
    this->evse_manager->get_evse(evse_id).submit_event(connector_id, ConnectorEvent::PlugIn);
}

Get15118EVCertificateResponse
ChargePoint::on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request) {

    return this->security->on_get_15118_ev_certificate_request(request);
}

void ChargePoint::on_transaction_started(
    const int32_t evse_id, const int32_t connector_id, const std::string& session_id, const DateTime& timestamp,
    const TriggerReasonEnum trigger_reason, const MeterValue& meter_start, const std::optional<IdToken>& id_token,
    const std::optional<IdToken>& group_id_token, const std::optional<int32_t>& reservation_id,
    const std::optional<int32_t>& remote_start_id, const ChargingStateEnum charging_state) {

    // This allows us to move from "Reserved" to "Occupied". We dont need to check if a reservation was placed since if
    // a transaction starts, it is always consumed and just sets the reserved flag to false and triggers a
    // StatusNotifcation. It does not trigger a StatusNotification when already in "Occupied"
    this->reservation->on_reservation_cleared(evse_id, connector_id);
    this->transaction->on_transaction_started(evse_id, connector_id, session_id, timestamp, trigger_reason, meter_start,
                                              id_token, group_id_token, reservation_id, remote_start_id,
                                              charging_state);
}

void ChargePoint::on_transaction_finished(const int32_t evse_id, const DateTime& timestamp,
                                          const MeterValue& meter_stop, const ReasonEnum reason,
                                          const TriggerReasonEnum trigger_reason,
                                          const std::optional<IdToken>& id_token,
                                          const std::optional<std::string>& signed_meter_value,
                                          const ChargingStateEnum charging_state) {
    this->transaction->on_transaction_finished(evse_id, timestamp, meter_stop, reason, trigger_reason, id_token,
                                               signed_meter_value, charging_state);
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
    this->transaction->transaction_event_req(TransactionEventEnum::Updated, ocpp::DateTime(),
                                             transaction->get_transaction(), TriggerReasonEnum::Authorized,
                                             transaction->get_seq_no(), std::nullopt, std::nullopt, id_token,
                                             std::nullopt, std::nullopt, this->is_offline(), std::nullopt);
}

void ChargePoint::on_meter_value(const int32_t evse_id, const MeterValue& meter_value) {
    this->meter_values->on_meter_value(evse_id, meter_value);
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
                                    log_rotation_maximum_file_count),
            [this](ocpp::LogRotationStatus status) {
                if (status == ocpp::LogRotationStatus::RotatedWithDeletion) {
                    const auto& security_event = ocpp::security_events::SECURITYLOGWASCLEARED;
                    std::string tech_info = "Security log was rotated and an old log was deleted in the process";
                    this->security->security_event_notification_req(CiString<50>(security_event),
                                                                    CiString<255>(tech_info), true,
                                                                    utils::is_critical(security_event));
                }
            });
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
    if (this->reservation != nullptr) {
        this->reservation->on_reserved(evse_id, connector_id);
    }
}

void ChargePoint::on_reservation_cleared(const int32_t evse_id, const int32_t connector_id) {
    if (this->reservation != nullptr) {
        this->reservation->on_reservation_cleared(evse_id, connector_id);
    }
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
        this->transaction->transaction_event_req(TransactionEventEnum::Updated, DateTime(),
                                                 transaction->get_transaction(), trigger_reason,
                                                 transaction->get_seq_no(), std::nullopt, std::nullopt, std::nullopt,
                                                 std::nullopt, std::nullopt, this->is_offline(), std::nullopt);
    }
    return true;
}

std::optional<std::string> ChargePoint::get_evse_transaction_id(int32_t evse_id) {
    const auto& tx = this->evse_manager->get_evse(evse_id).get_transaction();

    if (tx != nullptr) {
        return tx->transactionId.get();
    }

    return std::nullopt;
}

AuthorizeResponse ChargePoint::validate_token(const IdToken id_token, const std::optional<CiString<10000>>& certificate,
                                              const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) {
    return this->authorization->validate_token(id_token, certificate, ocsp_request_data);
}

void ChargePoint::on_event(const std::vector<EventData>& events) {
    this->diagnostics->notify_event_req(events);
}

void ChargePoint::on_log_status_notification(UploadLogStatusEnum status, int32_t requestId) {
    LogStatusNotificationRequest request;
    request.status = status;
    request.requestId = requestId;

    // Store for use by the triggerMessage
    this->upload_log_status = status;
    this->upload_log_status_id = requestId;

    ocpp::Call<LogStatusNotificationRequest> call(request);
    this->message_dispatcher->dispatch_call(call);
}

void ChargePoint::on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info,
                                    const std::optional<bool>& critical, const std::optional<DateTime>& timestamp) {
    auto critical_security_event = true;
    if (critical.has_value()) {
        critical_security_event = critical.value();
    } else {
        critical_security_event = utils::is_critical(event_type);
    }
    this->security->security_event_notification_req(event_type, tech_info, false, critical_security_event, timestamp);
}

void ChargePoint::on_variable_changed(const SetVariableData& set_variable_data) {
    this->provisioning->on_variable_changed(set_variable_data);
}

void ChargePoint::on_reservation_status(const int32_t reservation_id, const ReservationUpdateStatusEnum status) {
    if (reservation != nullptr) {
        this->reservation->on_reservation_status(reservation_id, status);
    }
}

void ChargePoint::on_ev_charging_needs(const NotifyEVChargingNeedsRequest& request) {
    this->smart_charging->notify_ev_charging_needs_req(request);
}

void ChargePoint::initialize(const std::map<int32_t, int32_t>& evse_connector_structure,
                             const std::string& message_log_path) {
    this->device_model->check_integrity(evse_connector_structure);
    this->database_handler->open_connection();
    this->component_state_manager = std::make_shared<ComponentStateManager>(
        evse_connector_structure, database_handler,
        [this](auto evse_id, auto connector_id, auto status, bool initiated_by_trigger_message) {
            this->update_dm_availability_state(evse_id, connector_id, status);
            if (this->connectivity_manager == nullptr or !this->connectivity_manager->is_websocket_connected() or
                this->registration_status != RegistrationStatusEnum::Accepted) {
                return false;
            } else {
                this->availability->status_notification_req(evse_id, connector_id, status,
                                                            initiated_by_trigger_message);
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
            this->transaction->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction, trigger,
                                                     transaction.get_seq_no(), std::nullopt, std::nullopt, std::nullopt,
                                                     std::vector<MeterValue>(1, filtered_meter_value), std::nullopt,
                                                     this->is_offline(), std::nullopt);
        }
    };

    this->evse_manager = std::make_unique<EvseManager>(
        evse_connector_structure, *this->device_model, this->database_handler, component_state_manager,
        transaction_meter_value_callback, this->callbacks.pause_charging_callback);
    this->configure_message_logging_format(message_log_path);

    this->connectivity_manager =
        std::make_unique<ConnectivityManager>(*this->device_model, this->evse_security, this->logging,
                                              std::bind(&ChargePoint::message_callback, this, std::placeholders::_1));

    this->connectivity_manager->set_websocket_connected_callback(
        [this](int configuration_slot, const NetworkConnectionProfile& network_connection_profile,
               const OcppProtocolVersion ocpp_version) {
            this->websocket_connected_callback(configuration_slot, network_connection_profile, ocpp_version);
        });
    this->connectivity_manager->set_websocket_disconnected_callback(
        [this](int configuration_slot, const NetworkConnectionProfile& network_connection_profile, auto) {
            this->websocket_disconnected_callback(configuration_slot, network_connection_profile);
        });
    this->connectivity_manager->set_websocket_connection_failed_callback(
        std::bind(&ChargePoint::websocket_connection_failed, this, std::placeholders::_1));

    if (this->message_queue == nullptr) {
        std::set<v2::MessageType> message_types_discard_for_queueing;
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

        this->message_queue = std::make_unique<ocpp::MessageQueue<v2::MessageType>>(
            [this](json message) -> bool { return this->connectivity_manager->send_to_websocket(message.dump()); },
            MessageQueueConfig<v2::MessageType>{
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

    this->message_dispatcher =
        std::make_unique<MessageDispatcher>(*this->message_queue, *this->device_model, registration_status);

    // Construct functional blocks.
    functional_block_context = std::make_unique<FunctionalBlockContext>(
        *this->message_dispatcher, *this->device_model, *this->connectivity_manager, *this->evse_manager,
        *this->database_handler, *this->evse_security, *this->component_state_manager, this->ocpp_version);

    this->data_transfer = std::make_unique<DataTransfer>(
        *this->functional_block_context, this->callbacks.data_transfer_callback, DEFAULT_WAIT_FOR_FUTURE_TIMEOUT);
    this->security = std::make_unique<Security>(*this->functional_block_context, *this->logging, this->ocsp_updater,
                                                this->callbacks.security_event_callback);

    if (device_model->get_optional_value<bool>(ControllerComponentVariables::ReservationCtrlrAvailable)
            .value_or(false)) {
        this->reservation = std::make_unique<Reservation>(
            *this->functional_block_context, this->callbacks.reserve_now_callback.value(),
            this->callbacks.cancel_reservation_callback.value(), this->callbacks.is_reservation_for_token_callback);
    }

    this->authorization = std::make_unique<Authorization>(*this->functional_block_context);
    this->authorization->start_auth_cache_cleanup_thread();

    this->diagnostics = std::make_unique<Diagnostics>(
        *this->functional_block_context, *this->authorization, this->callbacks.get_log_request_callback,
        this->callbacks.get_customer_information_callback, this->callbacks.clear_customer_information_callback);
    this->diagnostics->start_monitoring();

    if (device_model->get_optional_value<bool>(ControllerComponentVariables::DisplayMessageCtrlrAvailable)
            .value_or(false)) {
        this->display_message = std::make_unique<DisplayMessageBlock>(
            *this->functional_block_context, this->callbacks.get_display_message_callback.value(),
            this->callbacks.set_display_message_callback.value(),
            this->callbacks.clear_display_message_callback.value());
    }

    this->meter_values = std::make_unique<MeterValues>(*this->functional_block_context);

    this->availability = std::make_unique<Availability>(*functional_block_context, this->callbacks.time_sync_callback,
                                                        this->callbacks.all_connectors_unavailable_callback);

    if (this->callbacks.configure_network_connection_profile_callback.has_value()) {
        this->connectivity_manager->set_configure_network_connection_profile_callback(
            this->callbacks.configure_network_connection_profile_callback.value());
    }

    if (device_model->get_optional_value<bool>(ControllerComponentVariables::SmartChargingCtrlrAvailable)
            .value_or(false)) {
        this->smart_charging = std::make_unique<SmartCharging>(*this->functional_block_context,
                                                               this->callbacks.set_charging_profiles_callback,
                                                               this->callbacks.stop_transaction_callback);
    }

    this->tariff_and_cost = std::make_unique<TariffAndCost>(
        *functional_block_context, *this->meter_values, this->callbacks.tariff_message_callback,
        this->callbacks.set_running_cost_callback, this->io_context);

    this->firmware_update = std::make_unique<FirmwareUpdate>(
        *this->functional_block_context, *this->availability, *this->security,
        this->callbacks.update_firmware_request_callback, this->callbacks.all_connectors_unavailable_callback);

    this->transaction = std::make_unique<TransactionBlock>(
        *this->functional_block_context, *this->message_queue, *this->authorization, *this->availability,
        *this->smart_charging, *this->tariff_and_cost, this->callbacks.stop_transaction_callback,
        this->callbacks.pause_charging_callback, this->callbacks.transaction_event_callback,
        this->callbacks.transaction_event_response_callback, this->callbacks.reset_callback);

    this->provisioning = std::make_unique<Provisioning>(
        *this->functional_block_context, *this->message_queue, this->ocsp_updater, *this->availability,
        *this->meter_values, *this->security, *this->diagnostics, *this->transaction,
        this->callbacks.time_sync_callback, this->callbacks.boot_notification_callback,
        this->callbacks.validate_network_profile_callback, this->callbacks.is_reset_allowed_callback,
        this->callbacks.reset_callback, this->callbacks.stop_transaction_callback,
        this->callbacks.variable_changed_callback, this->registration_status);

    this->remote_transaction_control = std::make_unique<RemoteTransactionControl>(
        *this->functional_block_context, *this->transaction, *this->smart_charging, *this->meter_values,
        *this->availability, *this->firmware_update, *this->security, this->reservation.get(), *this->provisioning,
        this->callbacks.unlock_connector_callback, this->callbacks.remote_start_transaction_callback,
        this->callbacks.stop_transaction_callback, this->registration_status, this->upload_log_status,
        this->upload_log_status_id);

    bool v2x_available =
        std::any_of(evse_connector_structure.begin(), evse_connector_structure.end(), [this](const auto& entry) {
            const auto& [evse, connectors] = entry;
            return this->device_model
                ->get_optional_value<bool>(
                    V2xComponentVariables::get_component_variable(evse, V2xComponentVariables::Available))
                .value_or(false);
        });

    if (v2x_available) {
        this->bidirectional = std::make_unique<Bidirectional>(
            *this->functional_block_context, this->callbacks.update_allowed_energy_transfer_modes_callback);
    }

    Variable field_length = {"FieldLength"};
    field_length.instance = "Get15118EVCertificateResponse.exiResponse";
    this->device_model->set_value(ControllerComponents::OCPPCommCtrlr, field_length, AttributeEnum::Actual,
                                  std::to_string(ISO15118_GET_EV_CERTIFICATE_EXI_RESPONSE_SIZE),
                                  VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL, true);
}

void ChargePoint::handle_message(const EnhancedMessage<v2::MessageType>& message) {
    const auto& json_message = message.message;
    try {
        switch (message.messageType) {
        case MessageType::BootNotificationResponse:
        case MessageType::SetVariables:
        case MessageType::GetVariables:
        case MessageType::GetBaseReport:
        case MessageType::GetReport:
        case MessageType::Reset:
        case MessageType::SetNetworkProfile:
            this->provisioning->handle_message(message);
            break;
        case MessageType::ChangeAvailability:
        case MessageType::HeartbeatResponse:
            this->availability->handle_message(message);
            break;
        case MessageType::TransactionEventResponse:
        case MessageType::GetTransactionStatus:
            this->transaction->handle_message(message);
            break;
        case MessageType::RequestStartTransaction:
        case MessageType::RequestStopTransaction:
        case MessageType::UnlockConnector:
        case MessageType::TriggerMessage:
            this->remote_transaction_control->handle_message(message);
            break;
        case MessageType::DataTransfer:
            this->data_transfer->handle_message(message);
            break;
        case MessageType::GetLog:
        case MessageType::CustomerInformation:
        case MessageType::SetMonitoringBase:
        case MessageType::SetMonitoringLevel:
        case MessageType::SetVariableMonitoring:
        case MessageType::GetMonitoringReport:
        case MessageType::ClearVariableMonitoring:
            this->diagnostics->handle_message(message);
            break;
        case MessageType::ClearCache:
        case MessageType::SendLocalList:
        case MessageType::GetLocalListVersion:
            this->authorization->handle_message(message);
            break;
        case MessageType::UpdateFirmware:
            this->firmware_update->handle_message(message);
            break;
        case MessageType::ReserveNow:
        case MessageType::CancelReservation:
            if (this->reservation != nullptr) {
                this->reservation->handle_message(message);
            } else {
                send_not_implemented_error(message.uniqueId, message.messageTypeId);
            }
            break;
        case MessageType::CertificateSigned:
        case MessageType::SignCertificateResponse:
        case MessageType::GetInstalledCertificateIds:
        case MessageType::InstallCertificate:
        case MessageType::DeleteCertificate:
            this->security->handle_message(message);
            break;
        case MessageType::SetChargingProfile:
        case MessageType::ClearChargingProfile:
        case MessageType::GetChargingProfiles:
        case MessageType::GetCompositeSchedule:
        case MessageType::NotifyEVChargingNeedsResponse:
            if (this->smart_charging != nullptr) {
                this->smart_charging->handle_message(message);
            } else {
                send_not_implemented_error(message.uniqueId, message.messageTypeId);
            }
            break;
        case MessageType::GetDisplayMessages:
        case MessageType::SetDisplayMessage:
        case MessageType::ClearDisplayMessage:
            if (this->display_message != nullptr) {
                this->display_message->handle_message(message);
            } else {
                send_not_implemented_error(message.uniqueId, message.messageTypeId);
            }
            break;
        case MessageType::CostUpdated:
            if (this->tariff_and_cost != nullptr) {
                this->tariff_and_cost->handle_message(message);
            } else {
                send_not_implemented_error(message.uniqueId, message.messageTypeId);
            }

            break;
        case MessageType::NotifyAllowedEnergyTransfer:
            if (this->bidirectional != nullptr) {
                this->bidirectional->handle_message(message);
            } else {
                send_not_implemented_error(message.uniqueId, message.messageTypeId);
            }

            break;
        case MessageType::Authorize:
        case MessageType::AuthorizeResponse:
        case MessageType::BootNotification:
        case MessageType::CancelReservationResponse:
        case MessageType::CertificateSignedResponse:
        case MessageType::ChangeAvailabilityResponse:
        case MessageType::ClearCacheResponse:
        case MessageType::ClearChargingProfileResponse:
        case MessageType::ClearDisplayMessageResponse:
        case MessageType::ClearedChargingLimit:
        case MessageType::ClearedChargingLimitResponse:
        case MessageType::ClearVariableMonitoringResponse:
        case MessageType::CostUpdatedResponse:
        case MessageType::CustomerInformationResponse:
        case MessageType::DataTransferResponse:
        case MessageType::DeleteCertificateResponse:
        case MessageType::FirmwareStatusNotification:
        case MessageType::FirmwareStatusNotificationResponse:
        case MessageType::Get15118EVCertificate:
        case MessageType::Get15118EVCertificateResponse:
        case MessageType::GetBaseReportResponse:
        case MessageType::GetCertificateStatus:
        case MessageType::GetCertificateStatusResponse:
        case MessageType::GetChargingProfilesResponse:
        case MessageType::GetCompositeScheduleResponse:
        case MessageType::GetDisplayMessagesResponse:
        case MessageType::GetInstalledCertificateIdsResponse:
        case MessageType::GetLocalListVersionResponse:
        case MessageType::GetLogResponse:
        case MessageType::GetMonitoringReportResponse:
        case MessageType::GetReportResponse:
        case MessageType::GetTransactionStatusResponse:
        case MessageType::GetVariablesResponse:
        case MessageType::Heartbeat:
        case MessageType::InstallCertificateResponse:
        case MessageType::LogStatusNotification:
        case MessageType::LogStatusNotificationResponse:
        case MessageType::MeterValues:
        case MessageType::MeterValuesResponse:
        case MessageType::NotifyAllowedEnergyTransferResponse:
        case MessageType::NotifyChargingLimit:
        case MessageType::NotifyChargingLimitResponse:
        case MessageType::NotifyCustomerInformation:
        case MessageType::NotifyCustomerInformationResponse:
        case MessageType::NotifyDisplayMessages:
        case MessageType::NotifyDisplayMessagesResponse:
        case MessageType::NotifyEVChargingNeeds:
        case MessageType::NotifyEVChargingSchedule:
        case MessageType::NotifyEVChargingScheduleResponse:
        case MessageType::NotifyEvent:
        case MessageType::NotifyEventResponse:
        case MessageType::NotifyMonitoringReport:
        case MessageType::NotifyMonitoringReportResponse:
        case MessageType::NotifyReport:
        case MessageType::NotifyReportResponse:
        case MessageType::PublishFirmware:
        case MessageType::PublishFirmwareResponse:
        case MessageType::PublishFirmwareStatusNotification:
        case MessageType::PublishFirmwareStatusNotificationResponse:
        case MessageType::ReportChargingProfiles:
        case MessageType::ReportChargingProfilesResponse:
        case MessageType::RequestStartTransactionResponse:
        case MessageType::RequestStopTransactionResponse:
        case MessageType::ReservationStatusUpdate:
        case MessageType::ReservationStatusUpdateResponse:
        case MessageType::ReserveNowResponse:
        case MessageType::ResetResponse:
        case MessageType::SecurityEventNotification:
        case MessageType::SecurityEventNotificationResponse:
        case MessageType::SendLocalListResponse:
        case MessageType::SetChargingProfileResponse:
        case MessageType::SetDisplayMessageResponse:
        case MessageType::SetMonitoringBaseResponse:
        case MessageType::SetMonitoringLevelResponse:
        case MessageType::SetNetworkProfileResponse:
        case MessageType::SetVariableMonitoringResponse:
        case MessageType::SetVariablesResponse:
        case MessageType::SignCertificate:
        case MessageType::StatusNotification:
        case MessageType::StatusNotificationResponse:
        case MessageType::TransactionEvent:
        case MessageType::TriggerMessageResponse:
        case MessageType::UnlockConnectorResponse:
        case MessageType::UnpublishFirmware:
        case MessageType::UnpublishFirmwareResponse:
        case MessageType::UpdateFirmwareResponse:
        case MessageType::AdjustPeriodicEventStream:
        case MessageType::AdjustPeriodicEventStreamResponse:
        case MessageType::AFRRSignal:
        case MessageType::AFRRSignalResponse:
        case MessageType::BatterySwap:
        case MessageType::BatterySwapResponse:
        case MessageType::ChangeTransactionTariff:
        case MessageType::ChangeTransactionTariffResponse:
        case MessageType::ClearDERControl:
        case MessageType::ClearDERControlResponse:
        case MessageType::ClearTariffs:
        case MessageType::ClearTariffsResponse:
        case MessageType::ClosePeriodicEventStream:
        case MessageType::ClosePeriodicEventStreamResponse:
        case MessageType::GetCRL:
        case MessageType::GetCRLResponse:
        case MessageType::GetDERControl:
        case MessageType::GetDERControlResponse:
        case MessageType::GetPeriodicEventStream:
        case MessageType::GetPeriodicEventStreamResponse:
        case MessageType::GetTariffs:
        case MessageType::GetTariffsResponse:
        case MessageType::NotifyDERAlarm:
        case MessageType::NotifyDERAlarmResponse:
        case MessageType::NotifyDERStartStop:
        case MessageType::NotifyDERStartStopResponse:
        case MessageType::NotifyPeriodicEventStream:
        case MessageType::NotifyPeriodicEventStreamResponse:
        case MessageType::NotifyPriorityCharging:
        case MessageType::NotifyPriorityChargingResponse:
        case MessageType::NotifySettlement:
        case MessageType::NotifySettlementResponse:
        case MessageType::OpenPeriodicEventStream:
        case MessageType::OpenPeriodicEventStreamResponse:
        case MessageType::PullDynamicScheduleUpdate:
        case MessageType::PullDynamicScheduleUpdateResponse:
        case MessageType::RequestBatterySwap:
        case MessageType::RequestBatterySwapResponse:
        case MessageType::SetDefaultTariff:
        case MessageType::SetDefaultTariffResponse:
        case MessageType::SetDERControl:
        case MessageType::SetDERControlResponse:
        case MessageType::UpdateDynamicSchedule:
        case MessageType::UpdateDynamicScheduleResponse:
        case MessageType::UsePriorityCharging:
        case MessageType::UsePriorityChargingResponse:
        case MessageType::VatNumberValidation:
        case MessageType::VatNumberValidationResponse:
        case MessageType::InternalError:
            send_not_implemented_error(message.uniqueId, message.messageTypeId);
            break;
        }
    } catch (const MessageTypeNotImplementedException& e) {
        EVLOG_warning << e.what();
        send_not_implemented_error(message.uniqueId, message.messageTypeId);
    }
}

void ChargePoint::message_callback(const std::string& message) {
    EnhancedMessage<v2::MessageType> enhanced_message;
    try {
        enhanced_message = this->message_queue->receive(message);
    } catch (const json::exception& e) {
        this->logging->central_system("Unknown", message);
        EVLOG_error << "JSON exception during reception of message: " << e.what();
        std::string error_message;
        try {
            error_message = json(e.what()).dump();
        } catch (const json::exception& ex) {
            error_message = "JSON exception during reception of message: ";
            error_message += ex.what();
        }
        this->message_dispatcher->dispatch_call_error(
            CallError(MessageId("-1"), "RpcFrameworkError", error_message, json({})));
        const auto& security_event = ocpp::security_events::INVALIDMESSAGES;
        this->security->security_event_notification_req(CiString<50>(security_event, StringTooLarge::Truncate),
                                                        error_message, true, utils::is_critical(security_event));
        return;
    } catch (const StringConversionException& e) {
        this->logging->central_system("Unknown", message);
        EVLOG_error << "StringConversionException during reception of message: " << e.what();
        this->message_dispatcher->dispatch_call_error(
            CallError(MessageId("-1"), "RpcFrameworkError", e.what(), json({})));
        const auto& security_event = ocpp::security_events::INVALIDMESSAGES;
        this->security->security_event_notification_req(CiString<50>(security_event, StringTooLarge::Truncate),
                                                        CiString<255>(message, StringTooLarge::Truncate), true,
                                                        utils::is_critical(security_event));
        return;
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        auto call_error = CallError(MessageId("-1"), "FormationViolation", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
        const auto& security_event = ocpp::security_events::INVALIDMESSAGES;
        this->security->security_event_notification_req(CiString<50>(security_event, StringTooLarge::Truncate),
                                                        CiString<255>(message, StringTooLarge::Truncate), true,
                                                        utils::is_critical(security_event));
        return;
    } catch (const MalformedRpcMessage& e) {
        EVLOG_error << "MalformedRpcMessage exception during handling of message: " << e.what();
        auto call_error = CallError(MessageId("-1"), "RpcFrameworkError", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
        const auto& security_event = ocpp::security_events::INVALIDMESSAGES;
        this->security->security_event_notification_req(CiString<50>(security_event, StringTooLarge::Truncate),
                                                        CiString<255>(message, StringTooLarge::Truncate), true,
                                                        utils::is_critical(security_event));
        return;
    }

    if (enhanced_message.messageTypeId == MessageTypeId::UNKNOWN) {
        EVLOG_error << "Cannot handle message with an unknown message type";
        auto call_error = CallError(MessageId("-1"), "MessageTypeNotSupported", "", json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
        const auto& security_event = ocpp::security_events::INVALIDMESSAGES;
        this->security->security_event_notification_req(CiString<50>(security_event, StringTooLarge::Truncate),
                                                        CiString<255>(message, StringTooLarge::Truncate), true,
                                                        utils::is_critical(security_event));
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
                this->provisioning->handle_message(enhanced_message);
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
                    this->message_dispatcher->dispatch_call_result(call_result);
                } else if (enhanced_message.messageType == MessageType::RequestStopTransaction) {
                    // Send rejected: B02.FR.05
                    RequestStopTransactionResponse response;
                    response.status = RequestStartStopStatusEnum::Rejected;
                    const ocpp::CallResult<RequestStopTransactionResponse> call_result(response,
                                                                                       enhanced_message.uniqueId);
                    this->message_dispatcher->dispatch_call_result(call_result);
                } else {
                    std::string const call_error_message =
                        "Received invalid MessageType: " +
                        conversions::messagetype_to_string(enhanced_message.messageType) +
                        " from CSMS while in state Pending";
                    EVLOG_warning << call_error_message;
                    // B02.FR.09 send CALLERROR SecurityError
                    const auto call_error =
                        CallError(enhanced_message.uniqueId, "SecurityError", call_error_message, json({}));
                    this->message_dispatcher->dispatch_call_error(call_error);
                }
            }
        } else if (this->registration_status == RegistrationStatusEnum::Rejected) {
            if (enhanced_message.messageType == MessageType::BootNotificationResponse) {
                this->provisioning->handle_message(enhanced_message);
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
                    this->message_dispatcher->dispatch_call_error(call_error);
                }
            } else {
                const auto error_message = "Received other message than BootNotificationResponse before "
                                           "having received an accepted BootNotificationResponse";
                EVLOG_warning << error_message;
                const auto call_error = CallError(enhanced_message.uniqueId, "SecurityError", "", json({}, true));
                this->message_dispatcher->dispatch_call_error(call_error);
            }
        }
    } catch (const EvseOutOfRangeException& e) {
        EVLOG_error << "Exception during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "OccurrenceConstraintViolation", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const ConnectorOutOfRangeException& e) {
        EVLOG_error << "Exception during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "OccurrenceConstraintViolation", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const StringConversionException& e) {
        EVLOG_error << "StringConversionException during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const EnumConversionException& e) {
        EVLOG_error << "EnumConversionException during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const TimePointParseException& e) {
        EVLOG_error << "Exception during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (const DeviceModelError& e) {
        EVLOG_error << "DeviceModelError during handling of message: " << e.what();
        auto call_error = CallError(enhanced_message.uniqueId, "GenericError", e.what(), json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    } catch (json::exception& e) {
        EVLOG_error << "JSON exception during handling of message: " << e.what();
        if (enhanced_message.messageTypeId != MessageTypeId::CALL) {
            return; // CALLERROR shall only follow on a CALL message
        }
        if (json_message.is_array() and json_message.size() > MESSAGE_ID) {
            auto call_error = CallError(enhanced_message.uniqueId, "FormationViolation", e.what(), json({}));
            this->message_dispatcher->dispatch_call_error(call_error);
        }
    }
}

bool ChargePoint::is_offline() {
    return !this->connectivity_manager->is_websocket_connected();
}
std::optional<DataTransferResponse> ChargePoint::data_transfer_req(const CiString<255>& vendorId,
                                                                   const std::optional<CiString<50>>& messageId,
                                                                   const std::optional<json>& data) {
    return this->data_transfer->data_transfer_req(vendorId, messageId, data);
}

std::optional<DataTransferResponse> ChargePoint::data_transfer_req(const DataTransferRequest& request) {
    return this->data_transfer->data_transfer_req(request);
}

void ChargePoint::websocket_connected_callback(const int configuration_slot,
                                               const NetworkConnectionProfile& network_connection_profile,
                                               const OcppProtocolVersion ocpp_version) {
    this->message_queue->resume(this->message_queue_resume_delay);
    this->ocpp_version = ocpp_version;
    if (this->registration_status == RegistrationStatusEnum::Accepted) {
        this->connectivity_manager->confirm_successful_connection();

        if (this->time_disconnected.time_since_epoch() != 0s) {
            // handle offline threshold
            //  Get the current time point using steady_clock
            auto offline_duration = std::chrono::steady_clock::now() - this->time_disconnected;

            // B04.FR.01
            // If offline period exceeds offline threshold then send the status notification for all connectors
            if (offline_duration > std::chrono::seconds(this->device_model->get_value<int>(
                                       ControllerComponentVariables::OfflineThreshold))) {
                EVLOG_debug << "offline for more than offline threshold ";
                this->component_state_manager->send_status_notification_all_connectors();
            } else {
                // B04.FR.02
                // If offline period doesn't exceed offline threshold then send the status notification for all
                // connectors that changed state
                EVLOG_debug << "offline for less than offline threshold ";
                this->component_state_manager->send_status_notification_changed_connectors();
            }
            this->security->init_certificate_expiration_check_timers(); // re-init as timers are stopped on disconnect
        }
    }
    this->time_disconnected = std::chrono::time_point<std::chrono::steady_clock>();

    // We have a connection again so next time it fails we should send the notification again
    this->skip_invalid_csms_certificate_notifications = false;

    if (this->callbacks.connection_state_changed_callback.has_value()) {
        this->callbacks.connection_state_changed_callback.value()(true, configuration_slot, network_connection_profile,
                                                                  ocpp_version);
    }
}

void ChargePoint::websocket_disconnected_callback(const int configuration_slot,
                                                  const NetworkConnectionProfile& network_connection_profile) {
    this->message_queue->pause();

    // check if offline threshold has been defined
    if (this->device_model->get_value<int>(ControllerComponentVariables::OfflineThreshold) != 0) {
        // Get the current time point using steady_clock
        this->time_disconnected = std::chrono::steady_clock::now();
    }

    this->security->stop_certificate_expiration_check_timers();
    if (this->callbacks.connection_state_changed_callback.has_value()) {
        this->callbacks.connection_state_changed_callback.value()(false, configuration_slot, network_connection_profile,
                                                                  this->ocpp_version);
    }
}

void ChargePoint::websocket_connection_failed(ConnectionFailedReason reason) {
    switch (reason) {
    case ConnectionFailedReason::InvalidCSMSCertificate:
        if (!this->skip_invalid_csms_certificate_notifications) {
            this->security->security_event_notification_req(CiString<50>(ocpp::security_events::INVALIDCSMSCERTIFICATE),
                                                            std::nullopt, true, true);
            this->skip_invalid_csms_certificate_notifications = true;
        } else {
            EVLOG_debug << "Skipping InvalidCsmsCertificate SecurityEvent since it has been sent already";
        }
        break;
    case ConnectionFailedReason::FailedToAuthenticateAtCsms:
        const auto& security_event = ocpp::security_events::FAILEDTOAUTHENTICATEATCSMS;
        this->security->security_event_notification_req(CiString<50>(security_event), std::nullopt, true,
                                                        utils::is_critical(security_event));
        break;
    }
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
            evse_cv.component, evse_cv.variable.value(), ocpp::v2::AttributeEnum::Actual,
            conversions::connector_status_enum_to_string(status), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }
    if (connector_cv.variable.has_value()) {
        this->device_model->set_read_only_value(
            connector_cv.component, connector_cv.variable.value(), ocpp::v2::AttributeEnum::Actual,
            conversions::connector_status_enum_to_string(status), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }

    // if applicable to the entire charging station
    if (evse_id == 0 and charging_station.variable.has_value()) {
        this->device_model->set_read_only_value(
            charging_station.component, charging_station.variable.value(), ocpp::v2::AttributeEnum::Actual,
            conversions::connector_status_enum_to_string(status), VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL);
    }
}

void ChargePoint::clear_invalid_charging_profiles() {
    try {
        auto evses = this->database_handler->get_all_charging_profiles_group_by_evse();
        EVLOG_info << "Found " << evses.size() << " evse in the database";
        for (const auto& [evse_id, profiles] : evses) {
            for (auto profile : profiles) {
                try {
                    if (this->smart_charging != nullptr &&
                        this->smart_charging->conform_and_validate_profile(profile, evse_id) !=
                            ProfileValidationResultEnum::Valid) {
                        this->database_handler->delete_charging_profile(profile.id);
                    }
                } catch (const everest::db::QueryExecutionException& e) {
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
    return this->provisioning->get_variables(get_variable_data_vector);
}

std::map<SetVariableData, SetVariableResult>
ChargePoint::set_variables(const std::vector<SetVariableData>& set_variable_data_vector, const std::string& source) {
    // set variables and allow setting of ReadOnly variables
    return this->provisioning->set_variables(set_variable_data_vector, source);
}

GetCompositeScheduleResponse ChargePoint::get_composite_schedule(const GetCompositeScheduleRequest& request) {
    if (this->smart_charging == nullptr) {
        GetCompositeScheduleResponse response;
        response.status = GenericStatusEnum::Rejected;
        return response;
    }
    return this->smart_charging->get_composite_schedule(request);
}

std::optional<CompositeSchedule> ChargePoint::get_composite_schedule(int32_t evse_id, std::chrono::seconds duration,
                                                                     ChargingRateUnitEnum unit) {
    if (this->smart_charging == nullptr) {
        return std::nullopt;
    }
    return this->smart_charging->get_composite_schedule(evse_id, duration, unit);
}

std::vector<CompositeSchedule> ChargePoint::get_all_composite_schedules(const int32_t duration_s,
                                                                        const ChargingRateUnitEnum& unit) {
    if (this->smart_charging == nullptr) {
        return {};
    }
    return this->smart_charging->get_all_composite_schedules(duration_s, unit);
}

std::optional<NetworkConnectionProfile>
ChargePoint::get_network_connection_profile(const int32_t configuration_slot) const {
    return this->connectivity_manager->get_network_connection_profile(configuration_slot);
}

std::optional<int> ChargePoint::get_priority_from_configuration_slot(const int configuration_slot) const {
    return this->connectivity_manager->get_priority_from_configuration_slot(configuration_slot);
}

const std::vector<int>& ChargePoint::get_network_connection_slots() const {
    return this->connectivity_manager->get_network_connection_slots();
}

void ChargePoint::send_not_implemented_error(const MessageId unique_message_id, const MessageTypeId message_type_id) {
    if (message_type_id == MessageTypeId::CALL) {
        const auto call_error = CallError(unique_message_id, "NotImplemented", "", json({}));
        this->message_dispatcher->dispatch_call_error(call_error);
    }
}

} // namespace v2
} // namespace ocpp
