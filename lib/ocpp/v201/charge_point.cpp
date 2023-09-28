// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/charge_point.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>
#include <ocpp/v201/messages/FirmwareStatusNotification.hpp>
#include <ocpp/v201/messages/LogStatusNotification.hpp>

namespace ocpp {
namespace v201 {

const auto DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL = std::chrono::seconds(30);
const auto WEBSOCKET_INIT_DELAY = std::chrono::seconds(2);

bool Callbacks::all_callbacks_valid() const {
    return this->is_reset_allowed_callback != nullptr and this->reset_callback != nullptr and
           this->stop_transaction_callback != nullptr and this->pause_charging_callback != nullptr and
           this->change_availability_callback != nullptr and this->get_log_request_callback != nullptr and
           this->unlock_connector_callback != nullptr and this->remote_start_transaction_callback != nullptr and
           this->is_reservation_for_token_callback != nullptr and this->update_firmware_request_callback != nullptr and
           (!this->variable_changed_callback.has_value() or this->variable_changed_callback.value() != nullptr) and
           (!this->validate_network_profile_callback.has_value() or
            this->validate_network_profile_callback.value() != nullptr) and
           (!this->configure_network_connection_profile_callback.has_value() or
            this->configure_network_connection_profile_callback.value() != nullptr) and
           (!this->time_sync_callback.has_value() or this->time_sync_callback.value() != nullptr);
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
    websocket_connection_status(WebsocketConnectionStatusEnum::Disconnected),
    operational_state(OperationalStatusEnum::Operative),
    network_configuration_priority(0),
    disable_automatic_websocket_reconnects(false),
    reset_scheduled(false),
    reset_scheduled_evseids{},
    firmware_status(FirmwareStatusEnum::Idle),
    upload_log_status(UploadLogStatusEnum::Idle),
    bootreason(BootReasonEnum::PowerUp),
    callbacks(callbacks) {
    // Make sure the received callback struct is completely filled early before we actually start running
    if (!this->callbacks.all_callbacks_valid()) {
        EVLOG_AND_THROW(std::invalid_argument("All non-optional callbacks must be supplied"));
    }

    this->device_model = std::make_unique<DeviceModel>(std::move(device_model_storage));
    this->database_handler = std::make_unique<DatabaseHandler>(core_database_path, sql_init_path);
    this->database_handler->open_connection();

    // operational status of whole charging station
    this->database_handler->insert_availability(0, std::nullopt, OperationalStatusEnum::Operative, false);
    // intantiate and initialize evses
    for (auto const& [evse_id, number_of_connectors] : evse_connector_structure) {
        auto evse_id_ = evse_id;
        // operational status for this evse
        this->database_handler->insert_availability(evse_id, std::nullopt, OperationalStatusEnum::Operative, false);
        // used by evse to trigger StatusNotification.req
        auto status_notification_callback = [this, evse_id_](const int32_t connector_id,
                                                             const ConnectorStatusEnum& status) {
            if (this->registration_status == RegistrationStatusEnum::Accepted) {
                this->status_notification_req(evse_id_, connector_id, status);
            }
        };
        // used by evse when TransactionEvent.req to transmit meter values
        auto transaction_meter_value_callback = [this](const MeterValue& _meter_value, const Transaction& transaction,
                                                       const int32_t seq_no,
                                                       const std::optional<int32_t> reservation_id) {
            const auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(
                _meter_value, utils::get_measurands_vec(this->device_model->get_value<std::string>(
                                  ControllerComponentVariables::SampledDataTxUpdatedMeasurands)));

            if (!filtered_meter_value.sampledValue.empty()) {
                this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction,
                                            TriggerReasonEnum::MeterValuePeriodic, seq_no, std::nullopt, std::nullopt,
                                            std::nullopt, std::vector<MeterValue>(1, filtered_meter_value),
                                            std::nullopt, this->is_offline(), reservation_id);
            }
        };

        this->evses.insert(
            std::make_pair(evse_id, std::make_unique<Evse>(evse_id, number_of_connectors, status_notification_callback,
                                                           transaction_meter_value_callback)));
        for (int32_t connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
            // operational status for this connector
            this->database_handler->insert_availability(evse_id, connector_id, OperationalStatusEnum::Operative, false);
        }
    }
    this->logging = std::make_shared<ocpp::MessageLogging>(true, message_log_path, DateTime().to_rfc3339(), false,
                                                           false, false, true, true);
    this->message_queue = std::make_unique<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return this->websocket->send(message.dump()); },
        this->device_model->get_value<int>(ControllerComponentVariables::MessageAttempts),
        this->device_model->get_value<int>(ControllerComponentVariables::MessageAttemptInterval),
        this->database_handler);
}

void ChargePoint::start(BootReasonEnum bootreason) {
    this->bootreason = bootreason;
    this->start_websocket();
    this->boot_notification_req(bootreason);
    // FIXME(piet): Run state machine with correct initial state
}

void ChargePoint::start_websocket() {
    this->init_websocket();
    if (this->websocket != nullptr) {
        this->websocket->connect();
    }
}

void ChargePoint::stop() {
    this->heartbeat_timer.stop();
    this->boot_notification_timer.stop();
    this->websocket_timer.stop();
    this->websocket->disconnect(websocketpp::close::status::going_away);
    this->message_queue->stop();
}

void ChargePoint::connect_websocket() {
    if (!this->websocket->is_connected()) {
        this->disable_automatic_websocket_reconnects = false;
        this->init_websocket();
        this->websocket->connect();
    }
}

void ChargePoint::disconnect_websocket() {
    if (this->websocket->is_connected()) {
        this->disable_automatic_websocket_reconnects = true;
        this->websocket->disconnect(websocketpp::close::status::normal);
    }
}

void ChargePoint::on_firmware_update_status_notification(int32_t request_id,
                                                         const FirmwareStatusEnum& firmware_update_status) {
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
            false); // L01.FR.03
    }
}

void ChargePoint::on_session_started(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::PlugIn);
}

void ChargePoint::on_transaction_started(
    const int32_t evse_id, const int32_t connector_id, const std::string& session_id, const DateTime& timestamp,
    const ocpp::v201::TriggerReasonEnum trigger_reason, const MeterValue& meter_start, const IdToken& id_token,
    const std::optional<IdToken>& group_id_token, const std::optional<int32_t>& reservation_id,
    const std::optional<int32_t>& remote_start_id, const ChargingStateEnum charging_state) {

    this->evses.at(evse_id)->open_transaction(
        session_id, connector_id, timestamp, meter_start, id_token, group_id_token, reservation_id,
        this->device_model->get_value<int>(ControllerComponentVariables::SampledDataTxUpdatedInterval));
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
                                          const std::optional<std::string>& id_token,
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
    auto meter_values = std::make_optional(utils::get_meter_values_with_measurands_and_interval_applied(
        enhanced_transaction->meter_values,
        utils::get_measurands_vec(
            this->device_model->get_value<std::string>(ControllerComponentVariables::AlignedDataTxEndedMeasurands)),
        utils::get_measurands_vec(
            this->device_model->get_value<std::string>(ControllerComponentVariables::SampledDataTxEndedMeasurands)),
        this->device_model->get_value<int>(ControllerComponentVariables::AlignedDataTxEndedInterval),
        this->device_model->get_value<int>(ControllerComponentVariables::SampledDataTxEndedInterval)));

    if (meter_values.value().empty()) {
        meter_values.reset();
    }

    const auto seq_no = enhanced_transaction->get_seq_no();
    this->evses.at(evse_id)->release_transaction();

    const auto trigger_reason = utils::stop_reason_to_trigger_reason_enum(reason);

    std::optional<IdToken> id_token_opt;
    if (id_token.has_value()) {
        IdToken _id_token;
        _id_token.idToken = id_token.value();
        _id_token.type = IdTokenEnum::ISO14443;
        id_token_opt = _id_token;
    }

    this->transaction_event_req(TransactionEventEnum::Ended, timestamp, transaction, trigger_reason, seq_no,
                                std::nullopt, std::nullopt, id_token_opt, meter_values, std::nullopt,
                                this->is_offline(), std::nullopt);

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

void ChargePoint::on_meter_value(const int32_t evse_id, const MeterValue& meter_value) {
    this->evses.at(evse_id)->on_meter_value(meter_value);
}

void ChargePoint::on_unavailable(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::Unavailable);
}

void ChargePoint::on_operative(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::ReturnToOperativeState);
}

void ChargePoint::on_faulted(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::Error);
}

void ChargePoint::on_reserved(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::Reserve);
}

bool ChargePoint::on_charging_state_changed(const uint32_t evse_id, ChargingStateEnum charging_state) {
    if (this->evses.find(static_cast<int32_t>(evse_id)) != this->evses.end()) {
        std::unique_ptr<EnhancedTransaction>& transaction =
            this->evses.at(static_cast<int32_t>(evse_id))->get_transaction();
        if (transaction != nullptr) {
            if (transaction->chargingState == charging_state) {
                EVLOG_debug << "Trying to send charging state changed without actual change, dropping message";
            } else {
                transaction->chargingState = charging_state;
                this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction->get_transaction(),
                                            TriggerReasonEnum::ChargingStateChanged, transaction->get_seq_no(),
                                            std::nullopt,
                                            this->evses.at(static_cast<int32_t>(evse_id))->get_evse_info(),
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
    // TODO(piet): C10.FR.07

    AuthorizeResponse response;

    // C03.FR.01 && C05.FR.01: We SHALL NOT send an authorize reqeust for IdTokenType Central
    if (id_token.type == IdTokenEnum::Central or
        !this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCtrlrEnabled).value_or(true)) {
        response.idTokenInfo.status = AuthorizationStatusEnum::Accepted;
        return response;
    }

    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::LocalAuthListCtrlrEnabled)
            .value_or(false)) {
        const auto id_token_info = this->database_handler->get_local_authorization_list_entry(id_token);

        if (id_token_info.has_value()) {
            if (id_token_info.value().status == AuthorizationStatusEnum::Accepted) {
                // C14.FR.02: If found in local list we shall start charging without an AuthorizeRequest
                EVLOG_info << "Found valid entry in local authorization list";
                response.idTokenInfo = id_token_info.value();
            } else if (this->websocket_connection_status == WebsocketConnectionStatusEnum::Connected) {
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
            } else {
                EVLOG_info << "Found invalid entry in AuthCache: Sending new request";
            }
        }
    }

    if (this->websocket_connection_status == WebsocketConnectionStatusEnum::Disconnected and
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::OfflineTxForUnknownIdEnabled)
            .value_or(false)) {
        EVLOG_info << "Offline authorization due to OfflineTxForUnknownIdEnabled being enabled";
        response.idTokenInfo.status = AuthorizationStatusEnum::Accepted;
        return response;
    }

    response = this->authorize_req(id_token, certificate, ocsp_request_data);

    if (auth_cache_enabled) {
        this->database_handler->authorization_cache_insert_entry(hashed_id_token, response.idTokenInfo);
        this->update_authorization_cache_size();
    }

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

template <class T> bool ChargePoint::send(ocpp::Call<T> call) {
    this->message_queue->push(call);
    return true;
}

template <class T> std::future<EnhancedMessage<v201::MessageType>> ChargePoint::send_async(ocpp::Call<T> call) {
    return this->message_queue->push_async(call);
}

template <class T> bool ChargePoint::send(ocpp::CallResult<T> call_result) {
    return this->websocket->send(json(call_result).dump());
}

bool ChargePoint::send(CallError call_error) {
    return this->websocket->send(json(call_error).dump());
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

    const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
    if (security_profile_cv.variable.has_value()) {
        this->device_model->set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                                AttributeEnum::Actual,
                                                std::to_string(network_connection_profile.value().securityProfile));
    }

    this->websocket = std::make_unique<Websocket>(connection_options, this->evse_security, this->logging);
    this->websocket->register_connected_callback([this](const int security_profile) {
        this->message_queue->resume();
        this->websocket_connection_status = WebsocketConnectionStatusEnum::Connected;

        const auto& security_profile_cv = ControllerComponentVariables::SecurityProfile;
        if (security_profile_cv.variable.has_value()) {
            this->device_model->set_read_only_value(security_profile_cv.component, security_profile_cv.variable.value(),
                                                    AttributeEnum::Actual, std::to_string(security_profile));
        }

        if (this->registration_status == RegistrationStatusEnum::Accepted) {
            // handle offline threshold
            //  Get the current time point using steady_clock
            std::chrono::steady_clock::duration offline_duration = std::chrono::steady_clock::now() - time_disconnected;

            // B04.FR.01
            // If offline period exceeds offline threshold then send the status notification for all connectors
            if (offline_duration > std::chrono::seconds(this->device_model->get_value<int>(
                                       ControllerComponentVariables::OfflineThreshold))) {
                EVLOG_debug << "offline for more than offline threshold ";
                for (auto const& [evse_id, evse] : this->evses) {
                    evse->trigger_status_notification_callbacks();
                }
            } else {
                // B04.FR.02
                // If offline period doesn't exceed offline threshold then send the status notification for all
                // connectors that changed state
                EVLOG_debug << "offline for less than offline threshold ";
                for (auto const& [evse_id, evse] : this->evses) {
                    int number_of_connectors = evse->get_number_of_connectors();
                    for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
                        if (conn_state_per_evse[{evse_id, connector_id}] != evse->get_state(connector_id)) {
                            evse->trigger_status_notification_callback(connector_id);
                        }
                    }
                }
            }
        }
    });

    this->websocket->register_closed_callback(
        [this, connection_options, configuration_slot](const websocketpp::close::status::value reason) {
            EVLOG_warning << "Failed to connect to NetworkConfigurationPriority: "
                          << this->network_configuration_priority + 1
                          << " which is configurationSlot: " << configuration_slot;
            this->websocket_connection_status = WebsocketConnectionStatusEnum::Disconnected;
            this->message_queue->pause();

            // check if offline threshold has been defined
            if (this->device_model->get_value<int>(ControllerComponentVariables::OfflineThreshold) != 0) {
                // get the status of all the connectors
                for (auto const& [evse_id, evse] : this->evses) {
                    int number_of_connectors = evse->get_number_of_connectors();
                    EvseConnectorPair conn_states_struct_key;
                    EVLOG_debug << "evseId: " << evse_id << " numConn: " << number_of_connectors;
                    for (int connector_id = 1; connector_id <= number_of_connectors; connector_id++) {
                        conn_states_struct_key.evse_id = evse_id;
                        conn_states_struct_key.connector_id = connector_id;
                        conn_state_per_evse[conn_states_struct_key] = evse->get_state(connector_id);
                        EVLOG_debug << "conn_id: " << conn_states_struct_key.connector_id
                                    << " State: " << conn_state_per_evse[conn_states_struct_key];
                    }
                }
                // Get the current time point using steady_clock
                time_disconnected = std::chrono::steady_clock::now();
            }

            if (!this->disable_automatic_websocket_reconnects) {
                this->websocket_timer.timeout(
                    [this, reason]() {
                        if (reason != websocketpp::close::status::service_restart) {
                            this->next_network_configuration_priority();
                        }
                        this->start_websocket();
                    },
                    WEBSOCKET_INIT_DELAY);
            }
        });

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

WebsocketConnectionOptions ChargePoint::get_ws_connection_options(const int32_t configuration_slot) {
    const auto network_connection_profile_opt = this->get_network_connection_profile(configuration_slot);

    if (!network_connection_profile_opt.has_value()) {
        EVLOG_critical << "Could not retrieve NetworkProfile of configurationSlot: " << configuration_slot;
        throw std::runtime_error("Could not retrieve NetworkProfile");
    }

    const auto network_connection_profile = network_connection_profile_opt.value();
    auto ocpp_csms_url = network_connection_profile.ocppCsmsUrl.get();

    if (ocpp_csms_url.compare(0, 5, "ws://") == 0) {
        ocpp_csms_url.erase(0, 5);
    } else if (ocpp_csms_url.compare(0, 6, "wss://") == 0) {
        ocpp_csms_url.erase(0, 6);
    }

    WebsocketConnectionOptions connection_options{
        OcppProtocolVersion::v201,
        ocpp_csms_url,
        network_connection_profile.securityProfile,
        this->device_model->get_value<std::string>(ControllerComponentVariables::ChargePointId),
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
            .value_or(false)};

    return connection_options;
}

std::optional<NetworkConnectionProfile> ChargePoint::get_network_connection_profile(const int32_t configuration_slot) {
    std::vector<SetNetworkProfileRequest> network_connection_profiles = json::parse(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles));

    for (const auto& network_profile : network_connection_profiles) {
        if (network_profile.configurationSlot == configuration_slot) {
            return network_profile.connectionData;
        }
    }
    return std::nullopt;
}

void ChargePoint::next_network_configuration_priority() {
    std::vector<SetNetworkProfileRequest> network_connection_profiles = json::parse(
        this->device_model->get_value<std::string>(ControllerComponentVariables::NetworkConnectionProfiles));
    if (network_connection_profiles.size() > 1) {
        EVLOG_info << "Switching to next network configuration priority";
    }
    this->network_configuration_priority =
        (this->network_configuration_priority + 1) % (network_connection_profiles.size());
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
        this->handle_get_variables_req(json_message);
        break;
    case MessageType::GetBaseReport:
        this->handle_get_base_report_req(json_message);
        break;
    case MessageType::GetReport:
        this->handle_get_report_req(json_message);
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
        this->handle_start_transaction_event_response(message);
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
    case MessageType::HeartbeatResponse:
        this->handle_heartbeat_response(json_message);
        break;
    case MessageType::SendLocalList:
        this->handle_send_local_authorization_list_req(json_message);
        break;
    case MessageType::GetLocalListVersion:
        this->handle_get_local_authorization_list_version_req(json_message);
        break;
    default:
        if (message.messageTypeId == MessageTypeId::CALL) {
            const auto call_error = CallError(message.uniqueId, "NotImplemented", "", json({}));
            this->send(call_error);
        }
    }
}

void ChargePoint::message_callback(const std::string& message) {
    auto enhanced_message = this->message_queue->receive(message);
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
                    EVLOG_warning << "Received invalid MessageType: "
                                  << conversions::messagetype_to_string(enhanced_message.messageType)
                                  << " from CSMS while in state Pending";
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
                                                        const ComponentVariable& component_variable) {
    auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(
        meter_value, utils::get_measurands_vec(this->device_model->get_value<std::string>(component_variable)));
    for (auto& sampled_value : filtered_meter_value.sampledValue) {
        sampled_value.context = context;
    }
    return filtered_meter_value;
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
    const auto next_timestamp = this->get_next_clock_aligned_meter_value_timestamp(
        this->device_model->get_value<int>(ControllerComponentVariables::AlignedDataInterval));
    if (next_timestamp.has_value()) {
        EVLOG_debug << "Next meter value will be sent at: " << next_timestamp.value().to_rfc3339();
        this->aligned_meter_values_timer.at(
            [this]() {
                for (auto const& [evse_id, evse] : this->evses) {
                    auto _meter_value = evse->get_meter_value();
                    // this will apply configured measurands and possibly reduce the entries of sampledValue
                    // according to the configuration
                    const auto meter_value =
                        get_latest_meter_value_filtered(_meter_value, ReadingContextEnum::Sample_Clock,
                                                        ControllerComponentVariables::AlignedDataMeasurands);

                    if (evse->has_active_transaction()) {
                        // because we do not actively read meter values at clock aligned timepoint, we switch the
                        // ReadingContext here
                        for (auto& sampled_value : _meter_value.sampledValue) {
                            sampled_value.context = ReadingContextEnum::Sample_Clock;
                        }

                        // add meter value to transaction meter values
                        const auto& enhanced_transaction = evse->get_transaction();
                        enhanced_transaction->meter_values.push_back(_meter_value);
                        if (!meter_value.sampledValue.empty()) {
                            this->transaction_event_req(
                                TransactionEventEnum::Updated, DateTime(), enhanced_transaction->get_transaction(),
                                TriggerReasonEnum::MeterValueClock, enhanced_transaction->get_seq_no(), std::nullopt,
                                std::nullopt, std::nullopt, std::vector<MeterValue>(1, meter_value), std::nullopt,
                                this->is_offline(), std::nullopt);
                        }
                    } else if (!evse->has_active_transaction() and
                               this->device_model
                                   ->get_optional_value<bool>(ControllerComponentVariables::AlignedDataSendDuringIdle)
                                   .value_or(false)) {
                        if (!meter_value.sampledValue.empty()) {
                            // J01.FR.14 this is the only case where we send a MeterValue.req
                            this->meter_values_req(evse_id, std::vector<ocpp::v201::MeterValue>(1, meter_value));
                        }
                    }
                }
                this->update_aligned_data_interval();
            },
            next_timestamp.value().to_time_point());
    }
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

bool operational_status_matches_connector_status(ConnectorStatusEnum connector_status,
                                                 OperationalStatusEnum operational_status) {
    if (operational_status == OperationalStatusEnum::Operative) {
        return connector_status != ConnectorStatusEnum::Unavailable and
               connector_status != ConnectorStatusEnum::Faulted;
    } else {
        return connector_status == ConnectorStatusEnum::Unavailable or connector_status == ConnectorStatusEnum::Faulted;
    }
}

bool ChargePoint::is_already_in_state(const ChangeAvailabilityRequest& request) {
    if (!request.evse.has_value()) {
        for (const auto& [evse_id, evse] : this->evses) {
            for (int connector_id = 1; connector_id <= evse->get_number_of_connectors(); connector_id++) {
                auto status = evse->get_state(connector_id);
                if (!operational_status_matches_connector_status(status, request.operationalStatus)) {
                    return false;
                }
            }
        }
        return true;
    }

    const auto evse_id = request.evse.value().id;
    if (request.evse.value().connectorId.has_value()) {
        const auto connector_id = request.evse.value().connectorId.value();
        const auto status = this->evses.at(evse_id)->get_state(connector_id);
        return operational_status_matches_connector_status(status, request.operationalStatus);
    } else {
        for (int connector_id = 1; connector_id <= this->evses.at(evse_id)->get_number_of_connectors();
             connector_id++) {
            auto status = this->evses.at(evse_id)->get_state(connector_id);
            if (!operational_status_matches_connector_status(status, request.operationalStatus)) {
                return false;
            }
        }
        return true;
    }
}

bool ChargePoint::is_valid_evse(const EVSE& evse) {
    return this->evses.count(evse.id) and
           (!evse.connectorId.has_value() or
            this->evses.at(evse.id)->get_number_of_connectors() >= evse.connectorId.value());
}

void ChargePoint::handle_scheduled_change_availability_requests(const int32_t evse_id) {
    if (this->scheduled_change_availability_requests.count(evse_id)) {
        EVLOG_info << "Found scheduled ChangeAvailability.req for evse_id:" << evse_id;
        const auto req = this->scheduled_change_availability_requests[evse_id];
        if (!this->any_transaction_active(req.evse)) {
            EVLOG_info << "Changing availability of evse:" << evse_id;
            this->callbacks.change_availability_callback(req, true);
            this->scheduled_change_availability_requests.erase(evse_id);
        } else {
            EVLOG_info << "Cannot change availability because transaction is still active";
        }
    }
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
            this->websocket->disconnect(websocketpp::close::status::service_restart);
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
    // TODO(piet): other special handling of changed variables can be added here...
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
                    !this->evse_security->get_key_pair(ocpp::CertificateSigningUseEnum::ChargingStationCertificate)
                         .has_value()) {
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

bool ChargePoint::is_evse_reserved_for_other(const std::unique_ptr<Evse>& evse, const IdToken& id_token,
                                             const std::optional<IdToken>& group_id_token) const {
    const uint32_t connectors = evse->get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum& status = evse->get_state(static_cast<int32_t>(i));
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

bool ChargePoint::is_evse_connector_available(const std::unique_ptr<Evse>& evse) const {
    if (evse->has_active_transaction()) {
        // If an EV is connected and has no authorization yet then the status is 'Occupied' and the
        // RemoteStartRequest should still be accepted. So this is the 'occupied' check instead.
        return false;
    }

    const uint32_t connectors = evse->get_number_of_connectors();
    for (uint32_t i = 1; i <= connectors; ++i) {
        const ConnectorStatusEnum& status = evse->get_state(static_cast<int32_t>(i));

        // At least one of the connectors is available / not faulted.
        if (status != ConnectorStatusEnum::Faulted && status != ConnectorStatusEnum::Unavailable) {
            return true;
        }
    }

    // Connectors are faulted or unavailable.
    return false;
}

void ChargePoint::set_evse_connectors_unavailable(const std::unique_ptr<Evse>& evse, bool persist) {
    uint32_t number_of_connectors = evse->get_number_of_connectors();

    for (uint32_t i = 1; i <= number_of_connectors; ++i) {
        bool should_persist = persist;
        if (!should_persist &&
            evse->get_state(static_cast<int32_t>(i)) == ocpp::v201::ConnectorStatusEnum::Unavailable) {
            should_persist = true;
        }

        evse->submit_event(static_cast<int32_t>(i), ConnectorEvent::Unavailable);

        ChangeAvailabilityRequest request;
        request.operationalStatus = OperationalStatusEnum::Inoperative;
        request.evse = EVSE();
        request.evse.value().id = evse->get_evse_info().id;
        request.evse->connectorId = i;

        this->callbacks.change_availability_callback(request, should_persist);
    }
}

bool ChargePoint::is_offline() {
    bool offline = false; // false by default
    if (this->websocket_connection_status == WebsocketConnectionStatusEnum::Disconnected) {
        offline = true;
    }
    return offline;
}

void ChargePoint::security_event_notification_req(const CiString<50>& event_type,
                                                  const std::optional<CiString<255>>& tech_info,
                                                  const bool triggered_internally, const bool critical) {
    if (critical) {
        EVLOG_debug << "Sending SecurityEventNotification";
        SecurityEventNotificationRequest req;

        req.type = event_type;
        req.timestamp = DateTime().to_rfc3339();
        req.techInfo = tech_info;

        ocpp::Call<SecurityEventNotificationRequest> call(req, this->message_queue->createMessageId());
        this->send<SecurityEventNotificationRequest>(call);
    }
    if (triggered_internally and this->callbacks.security_event_callback != nullptr) {
        this->callbacks.security_event_callback(event_type, tech_info);
    }
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

void ChargePoint::notify_report_req(const int request_id, const int seq_no,
                                    const std::vector<ReportData>& report_data) {

    NotifyReportRequest req;
    req.requestId = request_id;
    req.seqNo = seq_no;
    req.generatedAt = ocpp::DateTime();
    req.reportData.emplace(report_data);

    ocpp::Call<NotifyReportRequest> call(req, this->message_queue->createMessageId());
    this->send<NotifyReportRequest>(call);
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

    AuthorizeResponse response;

    if (enhanced_message.messageType != MessageType::AuthorizeResponse) {
        response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
        return response;
    }

    ocpp::CallResult<AuthorizeResponse> call_result = enhanced_message.message;
    if (call_result.msg.idTokenInfo.cacheExpiryDateTime.has_value() or
        !this->device_model->get_optional_value<int>(ControllerComponentVariables::AuthCacheLifeTime).has_value()) {
        return call_result.msg;
    }

    // C10.FR.08
    // when CSMS does not set cacheExpiryDateTime and config variable for AuthCacheLifeTime is present use the
    // configured AuthCacheLifeTime
    response = call_result.msg;
    response.idTokenInfo.cacheExpiryDateTime = DateTime(
        date::utc_clock::now() +
        std::chrono::seconds(this->device_model->get_value<int>(ControllerComponentVariables::AuthCacheLifeTime)));
    return response;
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

    if (event_type == TransactionEventEnum::Started and (!evse.has_value() or !id_token.has_value())) {
        EVLOG_error << "Request to send TransactionEvent(Started) without given evse or id_token. These properties "
                       "are required for this eventType \"Started\"!";
        return;
    }

    this->send<TransactionEventRequest>(call);
}

void ChargePoint::meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values) {
    MeterValuesRequest req;
    req.evseId = evse_id;
    req.meterValue = meter_values;

    ocpp::Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send<MeterValuesRequest>(call);
}

void ChargePoint::handle_get_log_req(Call<GetLogRequest> call) {
    const GetLogResponse response = this->callbacks.get_log_request_callback(call.msg);

    ocpp::CallResult<GetLogResponse> call_result(response, call.uniqueId);
    this->send<GetLogResponse>(call_result);
}

void ChargePoint::notify_event_req(const std::vector<EventData>& events) {
    NotifyEventRequest req;
    req.eventData = events;
    req.generatedAt = DateTime();
    req.seqNo = 0;

    ocpp::Call<NotifyEventRequest> call(req, this->message_queue->createMessageId());
    this->send<NotifyEventRequest>(call);
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
        // get transaction messages from db (if there are any) so they can be sent again.
        message_queue->get_transaction_messages_from_db();

        // set timers
        if (msg.interval > 0) {
            this->heartbeat_timer.interval([this]() { this->heartbeat_req(); }, std::chrono::seconds(msg.interval));
        }
        this->update_aligned_data_interval();
        // B01.FR.06 Only use boot timestamp if TimeSource contains Heartbeat
        if (this->callbacks.time_sync_callback.has_value() &&
            this->device_model->get_value<std::string>(ControllerComponentVariables::TimeSource).find("Heartbeat") !=
                std::string::npos) {
            this->callbacks.time_sync_callback.value()(msg.currentTime);
        }
        for (auto const& [evse_id, evse] : this->evses) {
            evse->trigger_status_notification_callbacks();
        }

        if (this->bootreason == BootReasonEnum::RemoteReset) {
            this->security_event_notification_req(
                CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
                std::optional<CiString<255>>("Charging Station rebooted due to requested remote reset!"), true, true);
        } else if (this->bootreason == BootReasonEnum::ScheduledReset) {
            this->security_event_notification_req(
                CiString<50>(ocpp::security_events::RESET_OR_REBOOT),
                std::optional<CiString<255>>("Charging Station rebooted due to a scheduled reset!"), true, true);
        } else {
            std::string startup_message = "Charging Station powered up! Firmware version: ";
            startup_message.append(
                this->device_model->get_value<std::string>(ControllerComponentVariables::FirmwareVersion));
            this->security_event_notification_req(CiString<50>(ocpp::security_events::STARTUP_OF_THE_DEVICE),
                                                  std::optional<CiString<255>>(startup_message), true, true);
        }
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
}

void ChargePoint::handle_set_variables_req(Call<SetVariablesRequest> call) {
    const auto msg = call.msg;

    SetVariablesResponse response;

    // collection used to collect SetVariableData that has been accepted
    std::vector<SetVariableData> accepted_set_variable_data;

    // iterate over the request
    for (const auto& set_variable_data : msg.setVariableData) {
        SetVariableResult set_variable_result;
        set_variable_result.component = set_variable_data.component;
        set_variable_result.variable = set_variable_data.variable;
        set_variable_result.attributeType = set_variable_data.attributeType.value_or(AttributeEnum::Actual);

        if (this->validate_set_variable(set_variable_data)) {
            set_variable_result.attributeStatus =
                this->device_model->set_value(set_variable_data.component, set_variable_data.variable,
                                              set_variable_data.attributeType.value_or(AttributeEnum::Actual),
                                              set_variable_data.attributeValue.get());
        } else {
            set_variable_result.attributeStatus = SetVariableStatusEnum::Rejected;
        }
        response.setVariableResult.push_back(set_variable_result);

        // collect SetVariableData that has been accepted
        if (set_variable_result.attributeStatus == SetVariableStatusEnum::Accepted) {
            accepted_set_variable_data.push_back(set_variable_data);
        }
    }

    ocpp::CallResult<SetVariablesResponse> call_result(response, call.uniqueId);
    this->send<SetVariablesResponse>(call_result);

    // iterate over changed_component_variables_values_map
    for (const auto& set_variable_data : accepted_set_variable_data) {
        EVLOG_info << set_variable_data.component.name << ":" << set_variable_data.variable.name << " changed to "
                   << set_variable_data.attributeValue.get();
        // handles required behavior specified within OCPP2.0.1 (e.g. reconnect when BasicAuthPassword has changed)
        this->handle_variable_changed(set_variable_data);
        // notifies application that a variable has changed
        if (this->callbacks.variable_changed_callback.has_value()) {
            this->callbacks.variable_changed_callback.value()(set_variable_data);
        }
    }
}

void ChargePoint::handle_get_variables_req(Call<GetVariablesRequest> call) {
    const auto msg = call.msg;

    // FIXME(piet): add handling for B06.FR.16
    // FIXME(piet): add handling for B06.FR.17

    GetVariablesResponse response;

    for (const auto& get_variable_data : msg.getVariableData) {
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
        response.getVariableResult.push_back(get_variable_result);
    }

    ocpp::CallResult<GetVariablesResponse> call_result(response, call.uniqueId);
    this->send<GetVariablesResponse>(call_result);
}

void ChargePoint::handle_get_base_report_req(Call<GetBaseReportRequest> call) {
    // TODO(piet): B07.FR.02
    // TODO(piet): B07.FR.13

    const auto msg = call.msg;
    GetBaseReportResponse response;
    response.status = GenericDeviceModelStatusEnum::Accepted;

    ocpp::CallResult<GetBaseReportResponse> call_result(response, call.uniqueId);
    this->send<GetBaseReportResponse>(call_result);

    // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage /
    // BytesPerMessage
    const auto report_data = this->device_model->get_report_data(msg.reportBase);
    this->notify_report_req(msg.requestId, 0, report_data);
}

void ChargePoint::handle_get_report_req(Call<GetReportRequest> call) {
    const auto msg = call.msg;

    GetReportResponse response;

    // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage /
    // BytesPerMessage
    const auto report_data = this->device_model->get_report_data(ReportBaseEnum::FullInventory, msg.componentVariable,
                                                                 msg.componentCriteria);
    if (report_data.empty()) {
        response.status = GenericDeviceModelStatusEnum::EmptyResultSet;
    } else {
        response.status = GenericDeviceModelStatusEnum::Accepted;
    }

    ocpp::CallResult<GetReportResponse> call_result(response, call.uniqueId);
    this->send<GetReportResponse>(call_result);

    if (response.status == GenericDeviceModelStatusEnum::Accepted) {
        this->notify_report_req(msg.requestId, 0, report_data);
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
    // TODO(piet): B11.FR.02
    // TODO(piet): B11.FR.05
    // TODO(piet): B11.FR.09
    // TODO(piet): B11.FR.10

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
                std::unique_ptr<Evse>& evse = this->evses.at(evse_id);
                if (evse) {
                    this->set_evse_connectors_unavailable(evse, false);
                } else {
                    EVLOG_error << "No evse found with evse id " << evse_id;
                }
            }
        }
    }

    if (response.status == ResetStatusEnum::Accepted) {
        if (call.msg.evseId.has_value()) {
            // B11.FR.08
            this->evses.at(call.msg.evseId.value())->submit_event(1, ConnectorEvent::Unavailable);
        } else {
            // B11.FR.03
            for (auto const& [evse_id, evse] : this->evses) {
                evse->submit_event(1, ConnectorEvent::Unavailable);
            }
        }
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

void ChargePoint::handle_start_transaction_event_response(const EnhancedMessage<v201::MessageType>& message) {
    CallResult<TransactionEventResponse> call_result = message.message;
    const Call<TransactionEventRequest>& original_call = message.call_message;
    const auto& original_msg = original_call.msg;

    if (original_msg.eventType != TransactionEventEnum::Started) {
        return;
    }

    if (!original_msg.evse.has_value()) {
        EVLOG_error << "Start transaction event sent without without evse id";
        return;
    }

    if (!original_msg.idToken.has_value()) {
        EVLOG_error << "Start transaction event sent without without idToken info";
        return;
    }

    const int32_t evse_id = original_msg.evse.value().id;
    const IdToken& id_token = original_msg.idToken.value();

    const auto msg = call_result.msg;
    if (msg.idTokenInfo.has_value()) {
        // C03.FR.0x and C05.FR.01: We SHALL NOT store central information in the Authorization Cache
        // C10.FR.05
        if (id_token.type != IdTokenEnum::Central and
            this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCacheCtrlrEnabled)
                .value_or(true)) {
            this->database_handler->authorization_cache_insert_entry(utils::generate_token_hash(id_token),
                                                                     msg.idTokenInfo.value());
            this->update_authorization_cache_size();
        }
        if (msg.idTokenInfo.value().status != AuthorizationStatusEnum::Accepted) {
            if (this->device_model->get_value<bool>(ControllerComponentVariables::StopTxOnInvalidId)) {
                this->callbacks.stop_transaction_callback(evse_id, ReasonEnum::DeAuthorized);
            } else {
                if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::MaxEnergyOnInvalidId)
                        .has_value()) {
                    // TODO(piet): E05.FR.03
                    // Energy delivery to the EV SHALL be allowed until the amount of energy specified in
                    // MaxEnergyOnInvalidId has been reached.
                } else {
                    this->callbacks.pause_charging_callback(evse_id);
                }
            }
        }
    }
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
    Evse* evse_ptr = nullptr;

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
            auto measurands = utils::get_measurands_vec(this->device_model->get_value<std::string>(
                ControllerComponentVariables::SampledDataTxUpdatedMeasurands));
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

        // TODO:
        // PublishFirmwareStatusNotification
        // SignChargingStationCertificate
        // SignV2GCertificate
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

    auto send_evse_message = [&](std::function<void(int32_t evse_id, Evse & evse)> send) {
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
        auto send_meter_value = [&](int32_t evse_id, Evse& evse) {
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
        auto send_transaction = [&](int32_t evse_id, Evse& evse) {
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
            evse_ptr->trigger_status_notification_callback(msg.evse.value().connectorId.value());
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
        const auto& evse = this->evses.at(evse_id);

        if (evse != nullptr) {
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

    if (msg.evse.has_value() and !this->is_valid_evse(msg.evse.value())) {
        EVLOG_warning << "CSMS requested ChangeAvailability for invalid evse id or connector id";
        response.status = ChangeAvailabilityStatusEnum::Rejected;
        ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
        this->send<ChangeAvailabilityResponse>(call_result);
        return;
    }

    const auto transaction_active = this->any_transaction_active(msg.evse);
    const auto is_already_in_state = this->is_already_in_state(msg);

    auto evse_id = 0; // represents the whole charging station
    if (msg.evse.has_value()) {
        evse_id = msg.evse.value().id;
    }

    if (!transaction_active or is_already_in_state) {
        response.status = ChangeAvailabilityStatusEnum::Accepted;
        // remove any scheduled availability request in case no transaction is scheduled or the component is already in
        // correct state to override possible requests that have been scheduled before
        this->scheduled_change_availability_requests.erase(evse_id);
    } else {
        // add to map of scheduled operational_states. This also overrides successive ChangeAvailability.req with
        // the same EVSE, which is propably desirable
        this->scheduled_change_availability_requests[evse_id] = msg;
    }

    // send reply before applying changes to EVSE / Connector because this could trigger StatusNotification.req
    // before responding with ChangeAvailability.req
    ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->send<ChangeAvailabilityResponse>(call_result);

    if (!transaction_active) {
        // execute change availability if possible
        this->callbacks.change_availability_callback(msg, true);
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
    UpdateFirmwareResponse response = callbacks.update_firmware_request_callback(call.msg);

    ocpp::CallResult<UpdateFirmwareResponse> call_result(response, call.uniqueId);
    this->send<UpdateFirmwareResponse>(call_result);

    if ((response.status == UpdateFirmwareStatusEnum::InvalidCertificate) ||
        (response.status == UpdateFirmwareStatusEnum::RevokedCertificate)) {
        // L01.FR.02
        this->security_event_notification_req(
            CiString<50>(ocpp::security_events::INVALIDFIRMWARESIGNINGCERTIFICATE),
            std::optional<CiString<255>>("Provided signing certificate is not valid!"), true, false);
    }
}

void ChargePoint::handle_data_transfer_req(Call<DataTransferRequest> call) {
    const auto msg = call.msg;
    DataTransferResponse response;
    const auto vendor_id = msg.vendorId.get();
    const auto message_id = msg.messageId.value_or(CiString<50>()).get();
    {
        std::lock_guard<std::mutex> lock(data_transfer_callbacks_mutex);
        if (this->data_transfer_callbacks.count(vendor_id) == 0) {
            response.status = ocpp::v201::DataTransferStatusEnum::UnknownVendorId;
        } else if (this->data_transfer_callbacks.count(vendor_id) and
                   this->data_transfer_callbacks[vendor_id].count(message_id) == 0) {
            response.status = ocpp::v201::DataTransferStatusEnum::UnknownMessageId;
        } else {
            // there is a callback registered for this vendorId and messageId
            response = this->data_transfer_callbacks[vendor_id][message_id](msg.data);
        }
    }

    ocpp::CallResult<DataTransferResponse> call_result(response, call.uniqueId);
    this->send<DataTransferResponse>(call_result);
}

DataTransferResponse ChargePoint::data_transfer_req(const CiString<255>& vendorId, const CiString<50>& messageId,
                                                    const std::string& data) {
    DataTransferRequest req;
    req.vendorId = vendorId;
    req.messageId = messageId;
    req.data.emplace(data);

    DataTransferResponse response;
    ocpp::Call<DataTransferRequest> call(req, this->message_queue->createMessageId());
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

} // namespace v201
} // namespace ocpp
