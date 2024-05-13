// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

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

namespace ocpp {
namespace v201 {

const auto DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL = std::chrono::seconds(30);
const auto WEBSOCKET_INIT_DELAY = std::chrono::seconds(2);
const auto DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD = 2E5;
const auto DEFAULT_MAX_MESSAGE_SIZE = 65000;

bool Callbacks::all_callbacks_valid() const {
    return this->is_reset_allowed_callback != nullptr and this->reset_callback != nullptr and
           this->stop_transaction_callback != nullptr and this->pause_charging_callback != nullptr and
           this->connector_effective_operative_status_changed_callback != nullptr and
           this->get_log_request_callback != nullptr and this->unlock_connector_callback != nullptr and
           this->remote_start_transaction_callback != nullptr and this->is_reservation_for_token_callback != nullptr and
           this->update_firmware_request_callback != nullptr and
           (!this->variable_changed_callback.has_value() or this->variable_changed_callback.value() != nullptr) and
           (!this->validate_network_profile_callback.has_value() or
            this->validate_network_profile_callback.value() != nullptr) and
           (!this->configure_network_connection_profile_callback.has_value() or
            this->configure_network_connection_profile_callback.value() != nullptr) and
           (!this->time_sync_callback.has_value() or this->time_sync_callback.value() != nullptr) and
           (!this->boot_notification_callback.has_value() or this->boot_notification_callback.value() != nullptr) and
           (!this->ocpp_messages_callback.has_value() or this->ocpp_messages_callback.value() != nullptr);
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
    network_configuration_priority(0),
    disable_automatic_websocket_reconnects(false),
    skip_invalid_csms_certificate_notifications(false),
    reset_scheduled(false),
    reset_scheduled_evseids{},
    firmware_status(FirmwareStatusEnum::Idle),
    upload_log_status(UploadLogStatusEnum::Idle),
    bootreason(BootReasonEnum::PowerUp),
    ocsp_updater(
        OcspUpdater(this->evse_security, this->send_callback<GetCertificateStatusRequest, GetCertificateStatusResponse>(
                                             MessageType::GetCertificateStatusResponse))),
    csr_attempt(1),
    client_certificate_expiration_check_timer([this]() { this->scheduled_check_client_certificate_expiration(); }),
    v2g_certificate_expiration_check_timer([this]() { this->scheduled_check_v2g_certificate_expiration(); }),
    callbacks(callbacks) {
    // Make sure the received callback struct is completely filled early before we actually start running
    if (!this->callbacks.all_callbacks_valid()) {
        EVLOG_AND_THROW(std::invalid_argument("All non-optional callbacks must be supplied"));
    }

    this->device_model = std::make_unique<DeviceModel>(std::move(device_model_storage));
    this->device_model->check_integrity(evse_connector_structure);

    auto database_connection = std::make_unique<common::DatabaseConnection>(fs::path(core_database_path) / "cp.db");
    this->database_handler = std::make_shared<DatabaseHandler>(std::move(database_connection), sql_init_path);
    this->database_handler->open_connection();

    // Set up the component state manager
    this->component_state_manager = std::make_shared<ComponentStateManager>(
        evse_connector_structure, database_handler, [this](auto evse_id, auto connector_id, auto status) {
            this->update_dm_availability_state(evse_id, connector_id, status);
            if (this->websocket == nullptr || !this->websocket->is_connected() ||
                this->registration_status != RegistrationStatusEnum::Accepted) {
                return false;
            } else {
                this->status_notification_req(evse_id, connector_id, status);
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

    // intantiate and initialize evses
    for (auto const& [evse_id, number_of_connectors] : evse_connector_structure) {
        auto evse_id_ = evse_id;
        // used by evse to trigger StatusNotification.req
        auto status_notification_callback = [this, evse_id_](const int32_t connector_id,
                                                             const ConnectorStatusEnum& status) {
            if (this->registration_status == RegistrationStatusEnum::Accepted) {
                this->status_notification_req(evse_id_, connector_id, status);
            }
        };
        // used by evse when TransactionEvent.req to transmit meter values
        auto transaction_meter_value_callback = [this, evse_id_](const MeterValue& _meter_value,
                                                                 const Transaction& transaction, const int32_t seq_no,
                                                                 const std::optional<int32_t> reservation_id) {
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
                type == ReadingContextEnum::Sample_Clock
                    ? ControllerComponentVariables::AlignedDataMeasurands
                    : ControllerComponentVariables::SampledDataTxUpdatedMeasurands));

            const auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(_meter_value, filter_vec);

            if (!filtered_meter_value.sampledValue.empty()) {
                const auto trigger = type == ReadingContextEnum::Sample_Clock ? TriggerReasonEnum::MeterValueClock
                                                                              : TriggerReasonEnum::MeterValuePeriodic;
                this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction, trigger, seq_no,
                                            std::nullopt, std::nullopt, std::nullopt,
                                            std::vector<MeterValue>(1, filtered_meter_value), std::nullopt,
                                            this->is_offline(), reservation_id);
            }
        };

        auto pause_charging_callback = [this, evse_id_]() { this->callbacks.pause_charging_callback(evse_id_); };

        this->evses.insert(
            std::make_pair(evse_id, std::make_unique<Evse>(evse_id, number_of_connectors, *this->device_model,
                                                           this->database_handler, component_state_manager,
                                                           transaction_meter_value_callback, pause_charging_callback)));
    }

    // configure logging
    this->configure_message_logging_format(message_log_path);

    this->message_queue = std::make_unique<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return this->websocket->send(message.dump()); },
        MessageQueueConfig{
            this->device_model->get_value<int>(ControllerComponentVariables::MessageAttempts),
            this->device_model->get_value<int>(ControllerComponentVariables::MessageAttemptInterval),
            this->device_model->get_optional_value<int>(ControllerComponentVariables::MessageQueueSizeThreshold)
                .value_or(DEFAULT_MESSAGE_QUEUE_SIZE_THRESHOLD),
            this->device_model->get_optional_value<bool>(ControllerComponentVariables::QueueAllMessages)
                .value_or(false),
            this->device_model->get_value<int>(ControllerComponentVariables::MessageTimeout)},
        this->database_handler);
}

void ChargePoint::start(BootReasonEnum bootreason) {
    this->bootreason = bootreason;
    // Trigger all initial status notifications and callbacks related to component state
    // Should be done before sending the BootNotification.req so that the correct states can be reported
    this->component_state_manager->trigger_all_effective_availability_changed_callbacks();
    this->boot_notification_req(bootreason);
    // get transaction messages from db (if there are any) so they can be sent again.
    this->message_queue->get_transaction_messages_from_db();
    this->start_websocket();

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

void ChargePoint::start_websocket() {
    this->init_websocket();
    if (this->websocket != nullptr) {
        this->websocket->connect();
    }
}

void ChargePoint::stop() {
    this->ocsp_updater.stop();
    this->heartbeat_timer.stop();
    this->boot_notification_timer.stop();
    this->certificate_signed_timer.stop();
    this->websocket_timer.stop();
    this->client_certificate_expiration_check_timer.stop();
    this->v2g_certificate_expiration_check_timer.stop();
    this->disconnect_websocket(WebsocketCloseReason::Normal);
    this->message_queue->stop();
}

void ChargePoint::connect_websocket() {
    if (!this->websocket->is_connected()) {
        this->disable_automatic_websocket_reconnects = false;
        this->init_websocket();
        this->websocket->connect();
    }
}

void ChargePoint::disconnect_websocket(WebsocketCloseReason code) {
    if (this->websocket != nullptr) {
        this->disable_automatic_websocket_reconnects = true;
        this->websocket->disconnect(code);
    }
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
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::PlugIn);
}

Get15118EVCertificateResponse
ChargePoint::on_get_15118_ev_certificate_request(const Get15118EVCertificateRequest& request) {
    EVLOG_debug << "Received Get15118EVCertificateRequest " << request;
    auto future_res = this->send_async<Get15118EVCertificateRequest>(
        ocpp::Call<Get15118EVCertificateRequest>(request, this->message_queue->createMessageId()));
    const auto response_message = future_res.get();
    EVLOG_debug << "Received Get15118EVCertificateResponse " << response_message.message;
    if (response_message.messageType != MessageType::Get15118EVCertificateResponse) {
        Get15118EVCertificateResponse response;
        response.status = Iso15118EVCertificateStatusEnum::Failed;
        return response;
    }

    ocpp::CallResult<Get15118EVCertificateResponse> call_result = response_message.message;
    return call_result.msg;
}

void ChargePoint::on_transaction_started(const int32_t evse_id, const int32_t connector_id,
                                         const std::string& session_id, const DateTime& timestamp,
                                         const ocpp::v201::TriggerReasonEnum trigger_reason,
                                         const MeterValue& meter_start, const std::optional<IdToken>& id_token,
                                         const std::optional<IdToken>& group_id_token,
                                         const std::optional<int32_t>& reservation_id,
                                         const std::optional<int32_t>& remote_start_id,
                                         const ChargingStateEnum charging_state) {

    this->evses.at(evse_id)->open_transaction(
        session_id, connector_id, timestamp, meter_start, id_token, group_id_token, reservation_id,
        std::chrono::seconds(
            this->device_model->get_value<int>(ControllerComponentVariables::SampledDataTxUpdatedInterval)),
        std::chrono::seconds(
            this->device_model->get_value<int>(ControllerComponentVariables::SampledDataTxEndedInterval)),
        std::chrono::seconds(this->device_model->get_value<int>(ControllerComponentVariables::AlignedDataInterval)),
        std::chrono::seconds(
            this->device_model->get_value<int>(ControllerComponentVariables::AlignedDataTxEndedInterval)));
    const auto& enhanced_transaction = this->evses.at(evse_id)->get_transaction();
    enhanced_transaction->chargingState = charging_state;
    const auto meter_value = utils::get_meter_value_with_measurands_applied(
        meter_start, utils::get_measurands_vec(this->device_model->get_value<std::string>(
                         ControllerComponentVariables::SampledDataTxStartedMeasurands)));

    Transaction transaction{enhanced_transaction->transactionId};
    transaction.chargingState = charging_state;
    if (remote_start_id.has_value()) {
        transaction.remoteStartId = remote_start_id.value();
        enhanced_transaction->remoteStartId = remote_start_id.value();
    }

    auto evse = this->evses.at(evse_id)->get_evse_info();
    evse.connectorId.emplace(connector_id);

    std::optional<std::vector<MeterValue>> opt_meter_value;
    if (!meter_value.sampledValue.empty()) {
        opt_meter_value.emplace(1, meter_value);
    }

    this->transaction_event_req(TransactionEventEnum::Started, timestamp, transaction, trigger_reason,
                                enhanced_transaction->get_seq_no(), std::nullopt, evse, enhanced_transaction->id_token,
                                opt_meter_value, std::nullopt, this->is_offline(), reservation_id);
}

void ChargePoint::on_transaction_finished(const int32_t evse_id, const DateTime& timestamp,
                                          const MeterValue& meter_stop, const ReasonEnum reason,
                                          const std::optional<IdToken>& id_token,
                                          const std::optional<std::string>& signed_meter_value,
                                          const ChargingStateEnum charging_state) {
    auto& enhanced_transaction = this->evses.at(evse_id)->get_transaction();
    enhanced_transaction->chargingState = charging_state;
    if (enhanced_transaction == nullptr) {
        EVLOG_warning << "Received notification of finished transaction while no transaction was active";
        return;
    }

    this->evses.at(evse_id)->close_transaction(timestamp, meter_stop, reason);
    const auto transaction = enhanced_transaction->get_transaction();
    const auto transaction_id = enhanced_transaction->transactionId.get();
    auto meter_values = std::make_optional(utils::get_meter_values_with_measurands_applied(
        this->database_handler->transaction_metervalues_get_all(transaction_id),
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

    const auto seq_no = enhanced_transaction->get_seq_no();
    this->evses.at(evse_id)->release_transaction();

    const auto trigger_reason = utils::stop_reason_to_trigger_reason_enum(reason);

    // E07.FR.02 The field idToken is provided when the authorization of the transaction has been ended
    const std::optional<IdToken> transaction_id_token =
        trigger_reason == ocpp::v201::TriggerReasonEnum::StopAuthorized ? id_token : std::nullopt;

    this->transaction_event_req(TransactionEventEnum::Ended, timestamp, transaction, trigger_reason, seq_no,
                                std::nullopt, std::nullopt, transaction_id_token, meter_values, std::nullopt,
                                this->is_offline(), std::nullopt);

    this->database_handler->transaction_metervalues_clear(transaction_id);

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
            for (auto const& [evse_id, evse] : this->evses) {
                if (evse->has_active_transaction()) {
                    is_charging = true;
                    break;
                }
            }

            if (is_charging) {
                this->set_evse_connectors_unavailable(this->evses.at(evse_id), false);
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
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::PlugOut);
}

void ChargePoint::on_authorized(const int32_t evse_id, const int32_t connector_id, const IdToken& id_token) {
    if (this->evses.at(evse_id)->get_transaction() == nullptr) {
        // nothing to report in case transaction is not yet open
        return;
    }

    std::unique_ptr<EnhancedTransaction>& transaction =
        this->evses.at(static_cast<int32_t>(evse_id))->get_transaction();

    if (transaction->id_token.has_value()) {
        // if transactions id_token is already set, it is assumed it has already been reported
        return;
    }

    // set id_token of transaction and send TransactionEvent(Updated) with id_token
    transaction->id_token = id_token;
    this->transaction_event_req(TransactionEventEnum::Updated, ocpp::DateTime(), transaction->get_transaction(),
                                TriggerReasonEnum::Authorized, transaction->get_seq_no(), std::nullopt, std::nullopt,
                                id_token, std::nullopt, std::nullopt, this->is_offline(), std::nullopt);
}

void ChargePoint::on_meter_value(const int32_t evse_id, const MeterValue& meter_value) {
    if (evse_id == 0) {
        // if evseId = 0 then store in the chargepoint metervalues
        this->aligned_data_evse0.set_values(meter_value);
    } else {
        this->evses.at(evse_id)->on_meter_value(meter_value);
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
        const auto entry = this->database_handler->authorization_cache_get_entry(hashed_id_token);
        if (entry.has_value()) {
            s << "Hashed id_token stored in cache: " + hashed_id_token + "\n";
            s << "IdTokenInfo: " << entry.value();
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
        this->database_handler->authorization_cache_delete_entry(hashed_id_token);
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

    if (message_callback) {
        logging_callback = this->callbacks.ocpp_messages_callback.value_or(nullptr);
    }

    this->logging = std::make_shared<ocpp::MessageLogging>(
        !log_formats.empty(), message_log_path, DateTime().to_rfc3339(), log_to_console, detailed_log_to_console,
        log_to_file, log_to_html, log_security, session_logging, logging_callback);
}
void ChargePoint::on_unavailable(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::Unavailable);
}

void ChargePoint::on_enabled(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::UnavailableCleared);
}

void ChargePoint::on_faulted(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::Error);
}

void ChargePoint::on_reserved(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::Reserve);
}

bool ChargePoint::on_charging_state_changed(const uint32_t evse_id, const ChargingStateEnum charging_state,
                                            const TriggerReasonEnum trigger_reason) {
    if (this->evses.find(static_cast<int32_t>(evse_id)) != this->evses.end()) {
        std::unique_ptr<EnhancedTransaction>& transaction =
            this->evses.at(static_cast<int32_t>(evse_id))->get_transaction();
        if (transaction != nullptr) {
            if (transaction->chargingState == charging_state) {
                EVLOG_debug << "Trying to send charging state changed without actual change, dropping message";
            } else {
                transaction->chargingState = charging_state;
                this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction->get_transaction(),
                                            trigger_reason, transaction->get_seq_no(), std::nullopt, std::nullopt,
                                            std::nullopt, std::nullopt, std::nullopt, this->is_offline(), std::nullopt);
            }
            return true;
        } else {
            EVLOG_warning << "Can not change charging state: no transaction for evse id " << evse_id;
        }
    } else {
        EVLOG_warning << "Can not change charging state: evse id invalid: " << evse_id;
    }

    return false;
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
        if (this->websocket->is_connected() and ocsp_request_data.has_value()) {
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
            if (this->websocket->is_connected()) {
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
                        EVLOG_warning << "Online: OCSP data could not be generated";
                        response.idTokenInfo.status = AuthorizationStatusEnum::Invalid;
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
        const auto id_token_info = this->database_handler->get_local_authorization_list_entry(id_token);

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
            } else if (this->websocket->is_connected()) {
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
        const auto cache_entry = this->database_handler->authorization_cache_get_entry(hashed_id_token);
        if (cache_entry.has_value()) {
            if ((cache_entry.value().cacheExpiryDateTime.has_value() and
                 cache_entry.value().cacheExpiryDateTime.value().to_time_point() < DateTime().to_time_point())) {
                EVLOG_info << "Found valid entry in AuthCache but expiry date passed: Removing from cache and sending "
                              "new request";
                this->database_handler->authorization_cache_delete_entry(hashed_id_token);
                this->update_authorization_cache_size();
            } else if (this->device_model->get_value<bool>(ControllerComponentVariables::LocalPreAuthorize) and
                       cache_entry.value().status == AuthorizationStatusEnum::Accepted) {
                EVLOG_info << "Found valid entry in AuthCache";
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
    }

    if (!this->websocket->is_connected() and
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
            this->update_id_token_cache_lifetime(response.idTokenInfo);
            this->database_handler->authorization_cache_insert_entry(hashed_id_token, response.idTokenInfo);
            this->update_authorization_cache_size();
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

void ChargePoint::on_security_event(const CiString<50>& event_type, const std::optional<CiString<255>>& tech_info) {
    this->security_event_notification_req(event_type, tech_info, false, false);
}

void ChargePoint::on_variable_changed(const SetVariableData& set_variable_data) {
    this->handle_variable_changed(set_variable_data);
}

bool ChargePoint::send(CallError call_error) {
    this->message_queue->push(call_error);
    return true;
}

void ChargePoint::init_websocket() {

    if (this->device_model->get_value<std::string>(ControllerComponentVariables::ChargePointId).find(':') !=
        std::string::npos) {
        EVLOG_AND_THROW(std::runtime_error("ChargePointId must not contain \':\'"));
    }

    const auto configuration_slot =
        ocpp::get_vector_from_csv(
            this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority))
            .at(this->network_configuration_priority);
    const auto connection_options = this->get_ws_connection_options(std::stoi(configuration_slot));
    const auto network_connection_profile = this->get_network_connection_profile(std::stoi(configuration_slot));

    if (!network_connection_profile.has_value() or
        (this->callbacks.configure_network_connection_profile_callback.has_value() and
         !this->callbacks.configure_network_connection_profile_callback.value()(network_connection_profile.value()))) {
        EVLOG_warning << "NetworkConnectionProfile could not be retrieved or configuration of network with the given "
                         "profile failed";
        this->websocket_timer.timeout(
            [this]() {
                this->next_network_configuration_priority();
                this->start_websocket();
            },
            WEBSOCKET_INIT_DELAY);
        return;
    }

    EVLOG_info << "Open websocket with NetworkConfigurationPriority: " << this->network_configuration_priority + 1
               << " which is configurationSlot " << configuration_slot;

    const auto& active_network_profile_cv = ControllerComponentVariables::ActiveNetworkProfile;
    if (active_network_profile_cv.variable.has_value()) {
        this->device_model->set_read_only_value(active_network_profile_cv.component,
                                                active_network_profile_cv.variable.value(), AttributeEnum::Actual,
                                                configuration_slot);
    }

    const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
    if (security_profile_cv.variable.has_value()) {
        this->device_model->set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                                AttributeEnum::Actual,
                                                std::to_string(network_connection_profile.value().securityProfile));
    }

    this->websocket = std::make_unique<Websocket>(connection_options, this->evse_security, this->logging);
    this->websocket->register_connected_callback([this](const int security_profile) {
        this->message_queue->resume(this->message_queue_resume_delay);

        const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
        if (security_profile_cv.variable.has_value()) {
            this->device_model->set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                                    AttributeEnum::Actual, std::to_string(security_profile));
        }

        if (this->registration_status == RegistrationStatusEnum::Accepted and
            this->time_disconnected.time_since_epoch() != 0s) {
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
            this->init_certificate_expiration_check_timers(); // re-init as timers are stopped on disconnect
        }
        this->time_disconnected = std::chrono::time_point<std::chrono::steady_clock>();

        // We have a connection again so next time it fails we should send the notification again
        this->skip_invalid_csms_certificate_notifications = false;
    });

    this->websocket->register_disconnected_callback([this]() {
        this->message_queue->pause();

        // check if offline threshold has been defined
        if (this->device_model->get_value<int>(ControllerComponentVariables::OfflineThreshold) != 0) {
            // Get the current time point using steady_clock
            this->time_disconnected = std::chrono::steady_clock::now();
        }

        this->client_certificate_expiration_check_timer.stop();
        this->v2g_certificate_expiration_check_timer.stop();
    });

    this->websocket->register_closed_callback(
        [this, connection_options, configuration_slot](const WebsocketCloseReason reason) {
            EVLOG_warning << "Closed websocket of NetworkConfigurationPriority: "
                          << this->network_configuration_priority + 1 << " which is configurationSlot "
                          << configuration_slot;

            if (!this->disable_automatic_websocket_reconnects) {
                this->websocket_timer.timeout(
                    [this, reason]() {
                        if (reason != WebsocketCloseReason::ServiceRestart) {
                            this->next_network_configuration_priority();
                        }
                        this->start_websocket();
                    },
                    WEBSOCKET_INIT_DELAY);
            }
        });

    this->websocket->register_connection_failed_callback([this](ConnectionFailedReason reason) {
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
    });

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
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

WebsocketConnectionOptions ChargePoint::get_ws_connection_options(const int32_t configuration_slot) {
    const auto network_connection_profile_opt = this->get_network_connection_profile(configuration_slot);

    if (!network_connection_profile_opt.has_value()) {
        EVLOG_critical << "Could not retrieve NetworkProfile of configurationSlot: " << configuration_slot;
        throw std::runtime_error("Could not retrieve NetworkProfile");
    }

    const auto network_connection_profile = network_connection_profile_opt.value();

    auto uri = Uri::parse_and_validate(
        network_connection_profile.ocppCsmsUrl.get(),
        this->device_model->get_value<std::string>(ControllerComponentVariables::SecurityCtrlrIdentity),
        network_connection_profile.securityProfile);

    WebsocketConnectionOptions connection_options{
        OcppProtocolVersion::v201,
        uri,
        network_connection_profile.securityProfile,
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::BasicAuthPassword),
        this->device_model->get_value<int>(ControllerComponentVariables::RetryBackOffRandomRange),
        this->device_model->get_value<int>(ControllerComponentVariables::RetryBackOffRepeatTimes),
        this->device_model->get_value<int>(ControllerComponentVariables::RetryBackOffWaitMinimum),
        this->device_model->get_value<int>(ControllerComponentVariables::NetworkProfileConnectionAttempts),
        this->device_model->get_value<std::string>(ControllerComponentVariables::SupportedCiphers12),
        this->device_model->get_value<std::string>(ControllerComponentVariables::SupportedCiphers13),
        this->device_model->get_value<int>(ControllerComponentVariables::WebSocketPingInterval),
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::WebsocketPingPayload)
            .value_or("payload"),
        this->device_model->get_optional_value<int>(ControllerComponentVariables::WebsocketPongTimeout).value_or(5),
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::UseSslDefaultVerifyPaths)
            .value_or(true),
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::AdditionalRootCertificateCheck)
            .value_or(false),
        std::nullopt, // hostName
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::VerifyCsmsCommonName).value_or(true),
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::UseTPM).value_or(false),
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::VerifyCsmsAllowWildcards)
            .value_or(false),
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::IFace)};

    return connection_options;
}

std::optional<NetworkConnectionProfile> ChargePoint::get_network_connection_profile(const int32_t configuration_slot) {
    std::vector<SetNetworkProfileRequest> network_connection_profiles = json::parse(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles));

    for (const auto& network_profile : network_connection_profiles) {
        if (network_profile.configurationSlot == configuration_slot) {
            auto security_profile = network_profile.connectionData.securityProfile;
            switch (security_profile) {
            case security::OCPP_1_6_ONLY_UNSECURED_TRANSPORT_WITHOUT_BASIC_AUTHENTICATION:
                throw std::invalid_argument("security_profile = " + std::to_string(security_profile) +
                                            " not officially allowed in OCPP 2.0.1");
            default:
                break;
            }

            return network_profile.connectionData;
        }
    }
    return std::nullopt;
}

void ChargePoint::next_network_configuration_priority() {
    const auto network_connection_priorities = ocpp::get_vector_from_csv(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority));
    if (network_connection_priorities.size() > 1) {
        EVLOG_info << "Switching to next network configuration priority";
    }
    this->network_configuration_priority =
        (this->network_configuration_priority + 1) % (network_connection_priorities.size());
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
                                  AttributeEnum::Actual, network_connection_profiles.dump());

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
                                  AttributeEnum::Actual, new_network_priority);
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
    default:
        if (message.messageTypeId == MessageTypeId::CALL) {
            const auto call_error = CallError(message.uniqueId, "NotImplemented", "", json({}));
            this->send(call_error);
        }
        break;
    }
}

void ChargePoint::message_callback(const std::string& message) {
    auto enhanced_message = this->message_queue->receive(message);
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
    } catch (json::exception& e) {
        EVLOG_error << "JSON exception during handling of message: " << e.what();
        if (json_message.is_array() && json_message.size() > MESSAGE_ID) {
            auto call_error = CallError(MessageId(json_message.at(MESSAGE_ID).get<std::string>()), "FormationViolation",
                                        e.what(), json({}));
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
        for (auto const& [evse_id, evse] : this->evses) {
            if (!evse->has_active_transaction()) {
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
        for (auto const& [evse_id, evse] : this->evses) {
            if (!evse->has_active_transaction()) {
                this->set_evse_connectors_unavailable(evse, false);
            } else {
                EVSE e;
                e.id = evse_id;
                msg.evse = e;
                this->scheduled_change_availability_requests[evse_id] = {msg, false};
            }
        }
    }
}

void ChargePoint::restore_all_connector_states() {
    for (auto const& [evse_id, evse] : this->evses) {
        uint32_t number_of_connectors = evse->get_number_of_connectors();

        for (uint32_t i = 1; i <= number_of_connectors; ++i) {
            evse->restore_connector_operative_status(static_cast<int32_t>(i));
        }
    }
}

void ChargePoint::update_id_token_cache_lifetime(IdTokenInfo& id_token_info) {
    // C10.FR.08
    // when CSMS does not set cacheExpiryDateTime and config variable for AuthCacheLifeTime is present use the
    // configured AuthCacheLifeTime
    auto lifetime = this->device_model->get_optional_value<int>(ControllerComponentVariables::AuthCacheLifeTime);
    if (!id_token_info.cacheExpiryDateTime.has_value() and lifetime.has_value()) {
        id_token_info.cacheExpiryDateTime = DateTime(date::utc_clock::now() + std::chrono::seconds(lifetime.value()));
    }
}

void ChargePoint::update_authorization_cache_size() {
    auto& auth_cache_size = ControllerComponentVariables::AuthCacheStorage;
    if (auth_cache_size.variable.has_value()) {
        auto size = this->database_handler->authorization_cache_get_binary_size();
        this->device_model->set_read_only_value(auth_cache_size.component, auth_cache_size.variable.value(),
                                                AttributeEnum::Actual, std::to_string(size));
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
            this->database_handler->clear_local_authorization_list();
            status = SendLocalListStatusEnum::Accepted;
        } else {
            const auto& list = request.localAuthorizationList.value();

            auto has_no_token_info = [](const AuthorizationData& item) { return !item.idTokenInfo.has_value(); };

            if (!has_duplicate_in_list(list) and
                std::find_if(list.begin(), list.end(), has_no_token_info) == list.end()) {
                this->database_handler->clear_local_authorization_list();
                this->database_handler->insert_or_update_local_authorization_list(list);
                status = SendLocalListStatusEnum::Accepted;
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

            for (auto& item : list) {
                if (item.idTokenInfo.has_value()) {
                    this->database_handler->insert_or_update_local_authorization_list_entry(item.idToken,
                                                                                            item.idTokenInfo.value());
                } else {
                    this->database_handler->delete_local_authorization_list_entry(item.idToken);
                }
            }
            status = SendLocalListStatusEnum::Accepted;
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
                for (auto const& [evse_id, evse] : this->evses) {
                    if (evse->has_active_transaction()) {
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

            for (auto const& [evse_id, evse] : this->evses) {
                if (evse->has_active_transaction()) {
                    continue;
                }

                // this will apply configured measurands and possibly reduce the entries of sampledValue
                // according to the configuration
                auto meter_value =
                    get_latest_meter_value_filtered(evse->get_idle_meter_value(), ReadingContextEnum::Sample_Clock,
                                                    ControllerComponentVariables::AlignedDataMeasurands);

                if (align_timestamps) {
                    meter_value.timestamp = utils::align_timestamp(DateTime{}, interval);
                }

                if (!meter_value.sampledValue.empty()) {
                    // J01.FR.14 this is the only case where we send a MeterValue.req
                    this->meter_values_req(evse_id, std::vector<ocpp::v201::MeterValue>(1, meter_value));
                    // clear the values
                }
                evse->clear_idle_meter_values();
            }
        },
        interval, std::chrono::floor<date::days>(date::utc_clock::to_sys(date::utc_clock::now())));
}

bool ChargePoint::any_transaction_active(const std::optional<EVSE>& evse) {
    if (!evse.has_value()) {
        for (auto const& [evse_id, evse] : this->evses) {
            if (evse->has_active_transaction()) {
                return true;
            }
        }
        return false;
    }
    return this->evses.at(evse.value().id)->has_active_transaction();
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
    return this->evses.count(evse.id) and
           (!evse.connectorId.has_value() or
            this->evses.at(evse.id)->get_number_of_connectors() >= evse.connectorId.value());
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
            this->websocket->set_authorization_key(set_variable_data.attributeValue.get());
            this->websocket->disconnect(WebsocketCloseReason::ServiceRestart);
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
        const auto configuration_slot =
            ocpp::get_vector_from_csv(
                this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConfigurationPriority))
                .at(this->network_configuration_priority);
        const auto connection_options = this->get_ws_connection_options(std::stoi(configuration_slot));
        this->websocket->set_connection_options(connection_options);
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
}

bool ChargePoint::validate_set_variable(const SetVariableData& set_variable_data) {
    ComponentVariable cv = {set_variable_data.component, std::nullopt, set_variable_data.variable};
    if (cv == ControllerComponentVariables::NetworkConfigurationPriority) {
        const auto network_configuration_priorities = ocpp::get_vector_from_csv(set_variable_data.attributeValue.get());
        const auto active_security_profile =
            this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile);
        for (const auto configuration_slot : network_configuration_priorities) {
            try {
                auto network_profile_opt = this->get_network_connection_profile(std::stoi(configuration_slot));
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
                                    const bool allow_read_only) {
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
                                              set_variable_data.attributeValue.get(), allow_read_only);
        } else {
            set_variable_result.attributeStatus = SetVariableStatusEnum::Rejected;
        }
        response[set_variable_data] = set_variable_result;
    }

    return response;
}

std::optional<int32_t> ChargePoint::get_transaction_evseid(const CiString<36>& transaction_id) {
    for (auto const& [evse_id, evse] : evses) {
        if (evse->has_active_transaction()) {
            if (transaction_id == evse->get_transaction()->get_transaction().transactionId) {
                return evse_id;
            }
        }
    }

    return std::nullopt;
}

bool ChargePoint::is_evse_reserved_for_other(const std::unique_ptr<EvseInterface>& evse, const IdToken& id_token,
                                             const std::optional<IdToken>& group_id_token) const {
    const uint32_t connectors = evse->get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum status =
            evse->get_connector(static_cast<int32_t>(i))->get_effective_connector_status();
        if (status == ConnectorStatusEnum::Reserved) {
            const std::optional<CiString<36>> groupIdToken =
                group_id_token.has_value() ? group_id_token.value().idToken : std::optional<CiString<36>>{};

            if (!callbacks.is_reservation_for_token_callback(evse->get_evse_info().id, id_token.idToken,
                                                             groupIdToken)) {
                return true;
            }
        }
    }
    return false;
}

bool ChargePoint::is_evse_connector_available(const std::unique_ptr<EvseInterface>& evse) const {
    if (evse->has_active_transaction()) {
        // If an EV is connected and has no authorization yet then the status is 'Occupied' and the
        // RemoteStartRequest should still be accepted. So this is the 'occupied' check instead.
        return false;
    }

    const uint32_t connectors = evse->get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum status =
            evse->get_connector(static_cast<int32_t>(i))->get_effective_connector_status();

        // At least one of the connectors is available / not faulted.
        if (status != ConnectorStatusEnum::Faulted && status != ConnectorStatusEnum::Unavailable) {
            return true;
        }
    }

    // Connectors are faulted or unavailable.
    return false;
}

void ChargePoint::set_evse_connectors_unavailable(const std::unique_ptr<EvseInterface>& evse, bool persist) {
    uint32_t number_of_connectors = evse->get_number_of_connectors();

    for (uint32_t i = 1; i <= number_of_connectors; ++i) {
        evse->set_connector_operative_status(static_cast<int32_t>(i), OperationalStatusEnum::Inoperative, persist);
    }
}

bool ChargePoint::is_offline() {
    bool offline = false; // false by default
    if (!this->websocket->is_connected()) {
        offline = true;
    }
    return offline;
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

void ChargePoint::sign_certificate_req(const ocpp::CertificateSigningUseEnum& certificate_signing_use) {
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

    if (!common.has_value() or !country.has_value() or !organization.has_value()) {
        EVLOG_warning << "Missing configuration of either organizationName, commonName or country to generate CSR";
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
    this->send<SignCertificateRequest>(call);
}

void ChargePoint::boot_notification_req(const BootReasonEnum& reason) {
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
    this->send<BootNotificationRequest>(call);
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

    ocpp::Call<AuthorizeRequest> call(req, this->message_queue->createMessageId());
    auto future = this->send_async<AuthorizeRequest>(call);
    const auto enhanced_message = future.get();

    if (enhanced_message.messageType != MessageType::AuthorizeResponse) {
        AuthorizeResponse response;
        response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
        return response;
    }

    ocpp::CallResult<AuthorizeResponse> call_result = enhanced_message.message;
    return call_result.msg;
}

void ChargePoint::status_notification_req(const int32_t evse_id, const int32_t connector_id,
                                          const ConnectorStatusEnum status) {
    StatusNotificationRequest req;
    req.connectorId = connector_id;
    req.evseId = evse_id;
    req.timestamp = DateTime().to_rfc3339();
    req.connectorStatus = status;

    ocpp::Call<StatusNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<StatusNotificationRequest>(call);
}

void ChargePoint::heartbeat_req() {
    HeartbeatRequest req;

    heartbeat_request_time = std::chrono::steady_clock::now();
    ocpp::Call<HeartbeatRequest> call(req, this->message_queue->createMessageId());
    this->send<HeartbeatRequest>(call);
}

void ChargePoint::transaction_event_req(const TransactionEventEnum& event_type, const DateTime& timestamp,
                                        const ocpp::v201::Transaction& transaction,
                                        const ocpp::v201::TriggerReasonEnum& trigger_reason, const int32_t seq_no,
                                        const std::optional<int32_t>& cable_max_current,
                                        const std::optional<ocpp::v201::EVSE>& evse,
                                        const std::optional<ocpp::v201::IdToken>& id_token,
                                        const std::optional<std::vector<ocpp::v201::MeterValue>>& meter_value,
                                        const std::optional<int32_t>& number_of_phases_used, const bool offline,
                                        const std::optional<int32_t>& reservation_id) {
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

    this->send<TransactionEventRequest>(call);

    if (this->callbacks.transaction_event_callback.has_value()) {
        this->callbacks.transaction_event_callback.value()(req);
    }
}

void ChargePoint::meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values) {
    MeterValuesRequest req;
    req.evseId = evse_id;
    req.meterValue = meter_values;

    ocpp::Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send<MeterValuesRequest>(call);
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
        this->websocket->disconnect(WebsocketCloseReason::ServiceRestart);
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
    const auto set_variables_response = this->set_variables_internal(msg.setVariableData, false);
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

    if (msg.reportBase == ReportBaseEnum::SummaryInventory) {
        response.status = GenericDeviceModelStatusEnum::NotSupported;
    }

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
                                      AttributeEnum::Actual,
                                      network_connection_profiles.dump()) != SetVariableStatusEnum::Accepted) {
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
    if (msg.evseId.has_value() && this->evses.at(msg.evseId.value())->has_active_transaction()) {
        transaction_active = true;
        evse_active_transactions.emplace(msg.evseId.value());
    } else {
        for (const auto& [evse_id, evse] : this->evses) {
            if (evse->has_active_transaction()) {
                transaction_active = true;
                evse_active_transactions.emplace(evse_id);
            } else {
                evse_no_transactions.emplace(evse_id);
            }
        }
    }

    if (this->callbacks.is_reset_allowed_callback(msg.evseId, msg.type)) {
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
                std::unique_ptr<EvseInterface>& evse = this->evses.at(evse_id);
                if (evse) {
                    this->set_evse_connectors_unavailable(evse, false);
                } else {
                    EVLOG_error << "No evse found with evse id " << evse_id;
                }
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
            .value_or(true) and
        this->database_handler->authorization_cache_clear()) {
        this->update_authorization_cache_size();
        response.status = ClearCacheStatusEnum::Accepted;
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
        auto id_token_info = msg.idTokenInfo.value();
        this->update_id_token_cache_lifetime(id_token_info);
        this->database_handler->authorization_cache_insert_entry(utils::generate_token_hash(id_token), id_token_info);
        this->update_authorization_cache_size();
    }

    if (msg.idTokenInfo.value().status == AuthorizationStatusEnum::Accepted) {
        // nothing to do in case status is accepted
        return;
    }

    std::vector<int32_t> evse_ids; // this is a vector because the id token could be active for multiple transactions
    if (original_msg.evse.has_value()) {
        evse_ids.push_back(original_msg.evse.value().id);
    } else {
        // add every evse_id with active transaction and given token
        for (const auto& [evse_id, evse] : this->evses) {
            const auto& transaction = evse->get_transaction();
            if (transaction != nullptr and transaction->id_token.has_value()) {
                if (transaction->id_token.value().idToken.get() == id_token.idToken.get()) {
                    evse_ids.push_back(evse_id);
                }
            }
        }
    }

    // post handling of transactions in case status is not Accepted
    for (const auto evse_id : evse_ids) {
        if (this->device_model->get_value<bool>(ControllerComponentVariables::StopTxOnInvalidId)) {
            this->callbacks.stop_transaction_callback(evse_id, ReasonEnum::DeAuthorized);
        } else {
            if (this->device_model->get_optional_value<int32_t>(ControllerComponentVariables::MaxEnergyOnInvalidId)
                    .has_value()) {
                // Energy delivery to the EV SHALL be allowed until the amount of energy specified in
                // MaxEnergyOnInvalidId has been reached.
                this->evses.at(evse_id)->start_checking_max_energy_on_invalid_id();
            } else {
                this->callbacks.pause_charging_callback(evse_id);
            }
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
    const UnlockConnectorResponse unlock_response = callbacks.unlock_connector_callback(msg.evseId, msg.connectorId);
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
        if (this->evses.find(evse_id) != this->evses.end()) {
            evse_ptr = this->evses.at(evse_id).get();
        }
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
            for (auto const& [evse_id, evse] : this->evses) {
                if (utils::meter_value_has_any_measurand(evse->get_meter_value(), measurands)) {
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
            for (auto const& [evse_id, evse] : this->evses) {
                if (evse->has_active_transaction()) {
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
        response.status = TriggerMessageStatusEnum::Accepted;
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
            for (auto const& [evse_id, evse] : this->evses) {
                send(evse_id, *evse);
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
                this->meter_values_req(evse_id, std::vector<ocpp::v201::MeterValue>(1, meter_value));
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
                                        opt_meter_value, std::nullopt, this->is_offline(), std::nullopt);
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
        this->heartbeat_req();
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
        this->send<LogStatusNotificationRequest>(call);
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
        this->send<FirmwareStatusNotificationRequest>(call);
    } break;

    case MessageTriggerEnum::SignChargingStationCertificate: {
        sign_certificate_req(ocpp::CertificateSigningUseEnum::ChargingStationCertificate);
    } break;

    case MessageTriggerEnum::SignV2GCertificate: {
        sign_certificate_req(ocpp::CertificateSigningUseEnum::V2GCertificate);
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
        const auto it_evse{this->evses.find(evse_id)};

        if (it_evse != this->evses.cend()) {
            const auto& evse{it_evse->second};
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
            EVLOG_warning << "Invalid evse id given. Can not remote start transaction.";
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
            for (auto const& [evse_id, evse] : this->evses) {
                if (!evse->has_active_transaction()) {
                    // FIXME: This will linger after the update too! We probably need another mechanism...
                    this->set_evse_operative_status(evse_id, OperationalStatusEnum::Inoperative, false);
                }
            }
        } else {
            // A single EVSE is being addressed. We need to prevent further transactions from starting on it.
            // To do that, make all connectors of the EVSE without an active transaction Inoperative.
            int number_of_connectors = this->evses.at(evse_id)->get_number_of_connectors();
            for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
                if (!this->evses.at(evse_id)->has_active_transaction(connector_id)) {
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

DataTransferResponse ChargePoint::data_transfer_req(const CiString<255>& vendorId,
                                                    const std::optional<CiString<50>>& messageId,
                                                    const std::optional<json>& data) {
    DataTransferRequest req;
    req.vendorId = vendorId;
    req.messageId = messageId;
    req.data = data;

    return this->data_transfer_req(req);
}

DataTransferResponse ChargePoint::data_transfer_req(const DataTransferRequest& request) {
    DataTransferResponse response;
    ocpp::Call<DataTransferRequest> call(request, this->message_queue->createMessageId());
    auto data_transfer_future = this->send_async<DataTransferRequest>(call);

    auto enhanced_message = data_transfer_future.get();
    if (enhanced_message.messageType == MessageType::DataTransferResponse) {
        ocpp::CallResult<DataTransferResponse> call_result = enhanced_message.message;
        response = call_result.msg;
    }
    if (enhanced_message.offline) {
        response.status = ocpp::v201::DataTransferStatusEnum::Rejected;
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
        this->database_handler->insert_or_update_local_authorization_list_version(call.msg.versionNumber);

        auto& local_entries = ControllerComponentVariables::LocalAuthListCtrlrEntries;
        if (local_entries.variable.has_value()) {
            auto entries = this->database_handler->get_local_authorization_list_number_of_entries();
            this->device_model->set_read_only_value(local_entries.component, local_entries.variable.value(),
                                                    AttributeEnum::Actual, std::to_string(entries));
        }
    }

    ocpp::CallResult<SendLocalListResponse> call_result(response, call.uniqueId);
    this->send<SendLocalListResponse>(call_result);
}

void ChargePoint::handle_get_local_authorization_list_version_req(Call<GetLocalListVersionRequest> call) {
    GetLocalListVersionResponse response;

    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        response.versionNumber = this->database_handler->get_local_authorization_list_version();
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

void ChargePoint::update_dm_availability_state(const int32_t evse_id, const int32_t connector_id,
                                               const ConnectorStatusEnum status) {
    ComponentVariable evse_cv =
        EvseComponentVariables::get_component_variable(evse_id, EvseComponentVariables::AvailabilityState);
    ComponentVariable connector_cv = ConnectorComponentVariables::get_component_variable(
        evse_id, connector_id, ConnectorComponentVariables::AvailabilityState);
    if (evse_cv.variable.has_value()) {
        this->device_model->set_read_only_value(evse_cv.component, evse_cv.variable.value(),
                                                ocpp::v201::AttributeEnum::Actual,
                                                conversions::connector_status_enum_to_string(status));
    }
    if (connector_cv.variable.has_value()) {
        this->device_model->set_read_only_value(connector_cv.component, connector_cv.variable.value(),
                                                ocpp::v201::AttributeEnum::Actual,
                                                conversions::connector_status_enum_to_string(status));
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
                                            AttributeEnum::Actual, std::to_string(power.value()));
}

void ChargePoint::set_cs_operative_status(OperationalStatusEnum new_status, bool persist) {
    this->component_state_manager->set_cs_individual_operational_status(new_status, persist);
}

void ChargePoint::set_evse_operative_status(int32_t evse_id, OperationalStatusEnum new_status, bool persist) {
    this->evses.at(evse_id)->set_evse_operative_status(new_status, persist);
}

void ChargePoint::set_connector_operative_status(int32_t evse_id, int32_t connector_id,
                                                 OperationalStatusEnum new_status, bool persist) {
    this->evses.at(evse_id)->set_connector_operative_status(connector_id, new_status, persist);
}

bool ChargePoint::are_all_connectors_effectively_inoperative() {
    // Check that all connectors on all EVSEs are inoperative
    for (auto& [evse_id, evse] : this->evses) {
        if (evse != nullptr) {
            for (int connector_id = 1; connector_id <= evse->get_number_of_connectors(); connector_id++) {
                OperationalStatusEnum connector_status =
                    this->component_state_manager->get_connector_effective_operational_status(evse_id, connector_id);
                if (connector_status == OperationalStatusEnum::Operative) {
                    return false;
                }
            }
        }
    }
    return true;
}

EvseInterface* ChargePoint::get_evse(int32_t evse_id) {
    if (evse_id <= 0 || evse_id > this->evses.size()) {
        std::stringstream err_msg;
        err_msg << "EVSE ID " << evse_id << " out of bounds";
        throw std::logic_error(err_msg.str());
    }
    return this->evses.at(evse_id).get();
}

Connector* ChargePoint::get_connector(int32_t evse_id, int32_t connector_id) {
    if (evse_id <= 0 || evse_id > this->evses.size()) {
        std::stringstream err_msg;
        err_msg << "EVSE ID " << evse_id << " out of bounds";
        throw std::logic_error(err_msg.str());
    }
    return this->evses.at(evse_id)->get_connector(connector_id);
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
ChargePoint::set_variables(const std::vector<SetVariableData>& set_variable_data_vector) {
    // set variables and allow setting of ReadOnly variables
    const auto response = this->set_variables_internal(set_variable_data_vector, true);
    this->handle_variables_changed(response);
    return response;
}

} // namespace v201
} // namespace ocpp
