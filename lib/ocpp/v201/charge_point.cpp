// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/messages/LogStatusNotification.hpp>
#include <ocpp/v201/charge_point.hpp>

namespace ocpp {
namespace v201 {

const auto DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL = std::chrono::seconds(30);

ChargePoint::ChargePoint(const std::map<int32_t, int32_t>& evse_connector_structure,
                         const std::string& device_model_storage_address, const std::string& ocpp_main_path,
                         const std::string& core_database_path, const std::string& sql_init_path,
                         const std::string& message_log_path, const std::string& certs_path,
                         const Callbacks& callbacks) :
    ocpp::ChargingStationBase(),
    registration_status(RegistrationStatusEnum::Rejected),
    websocket_connection_status(WebsocketConnectionStatusEnum::Disconnected),
    operational_state(OperationalStatusEnum::Operative),
    callbacks(callbacks) {
    this->device_model = std::make_unique<DeviceModel>(device_model_storage_address);
    this->pki_handler = std::make_shared<ocpp::PkiHandler>(
        ocpp_main_path,
        this->device_model->get_optional_value<bool>(ControllerComponentVariables::AdditionalRootCertificateCheck)
            .value_or(false));
    this->database_handler = std::make_unique<DatabaseHandler>(core_database_path, sql_init_path);
    this->database_handler->open_connection();

    // operational status of whole charging station
    this->database_handler->insert_availability(0, std::nullopt, OperationalStatusEnum::Operative, false);
    // intantiate and initialize evses
    for (auto const& [evse_id, number_of_connectors] : evse_connector_structure) {
        // operational status for this evse
        this->database_handler->insert_availability(evse_id, std::nullopt, OperationalStatusEnum::Operative, false);
        // used by evse to trigger StatusNotification.req
        auto status_notification_callback = [this, evse_id](const int32_t connector_id,
                                                            const ConnectorStatusEnum& status) {
            if (this->registration_status == RegistrationStatusEnum::Accepted) {
                this->status_notification_req(evse_id, connector_id, status);
            }
        };
        // used by evse when TransactionEvent.req to transmit meter values
        auto transaction_meter_value_callback = [this](const MeterValue& _meter_value, const Transaction& transaction,
                                                       const int32_t seq_no,
                                                       const std::optional<int32_t> reservation_id) {
            const auto filtered_meter_value = utils::get_meter_value_with_measurands_applied(
                _meter_value, utils::get_measurands_vec(this->device_model->get_value<std::string>(
                                  ControllerComponentVariables::SampledDataTxUpdatedMeasurands)));
            this->transaction_event_req(TransactionEventEnum::Updated, DateTime(), transaction,
                                        TriggerReasonEnum::MeterValuePeriodic, seq_no, std::nullopt, std::nullopt,
                                        std::nullopt, std::vector<MeterValue>(1, filtered_meter_value), std::nullopt,
                                        std::nullopt, reservation_id);
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
        this->device_model->get_value<int>(ControllerComponentVariables::MessageAttemptInterval));
}

void ChargePoint::start() {
    this->init_websocket();
    this->websocket->connect(this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile));
    this->boot_notification_req(BootReasonEnum::PowerUp);
    // FIXME(piet): Run state machine with correct initial state
}

void ChargePoint::stop() {
    this->heartbeat_timer.stop();
    this->boot_notification_timer.stop();
    this->websocket->disconnect(websocketpp::close::status::going_away);
    this->message_queue->stop();
}

void ChargePoint::on_session_started(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::PlugIn);
}

void ChargePoint::on_transaction_started(const int32_t evse_id, const int32_t connector_id,
                                         const std::string& session_id, const DateTime& timestamp,
                                         const MeterValue& meter_start, const IdToken& id_token,
                                         const std::optional<int32_t>& reservation_id) {

    this->evses.at(evse_id)->open_transaction(
        session_id, connector_id, timestamp, meter_start, id_token, reservation_id,
        this->device_model->get_value<int>(ControllerComponentVariables::SampledDataTxUpdatedInterval));
    const auto& enhanced_transaction = this->evses.at(evse_id)->get_transaction();
    const auto meter_value = utils::get_meter_value_with_measurands_applied(
        meter_start, utils::get_measurands_vec(this->device_model->get_value<std::string>(
                         ControllerComponentVariables::SampledDataTxUpdatedMeasurands)));

    Transaction transaction{enhanced_transaction->transactionId};

    auto evse = this->evses.at(evse_id)->get_evse_info();
    evse.connectorId.emplace(connector_id);

    this->transaction_event_req(TransactionEventEnum::Started, timestamp, transaction,
                                TriggerReasonEnum::ChargingStateChanged, enhanced_transaction->get_seq_no(),
                                std::nullopt, evse, enhanced_transaction->id_token,
                                std::vector<MeterValue>(1, meter_value), std::nullopt, std::nullopt, reservation_id);
}

void ChargePoint::on_transaction_finished(const int32_t evse_id, const DateTime& timestamp,
                                          const MeterValue& meter_stop, const ReasonEnum reason,
                                          const std::optional<std::string>& id_token,
                                          const std::optional<std::string>& signed_meter_value) {
    const auto& enhanced_transaction = this->evses.at(evse_id)->get_transaction();
    if (enhanced_transaction == nullptr) {
        EVLOG_warning << "Received notification of finished transaction while no transaction was active";
        return;
    }

    this->evses.at(evse_id)->close_transaction(timestamp, meter_stop, reason);
    const auto transaction = enhanced_transaction->get_transaction();
    const auto meter_values = utils::get_meter_values_with_measurands_and_interval_applied(
        enhanced_transaction->meter_values,
        utils::get_measurands_vec(
            this->device_model->get_value<std::string>(ControllerComponentVariables::AlignedDataTxEndedMeasurands)),
        utils::get_measurands_vec(
            this->device_model->get_value<std::string>(ControllerComponentVariables::SampledDataTxEndedMeasurands)),
        this->device_model->get_value<int>(ControllerComponentVariables::AlignedDataTxEndedInterval),
        this->device_model->get_value<int>(ControllerComponentVariables::SampledDataTxEndedInterval));
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
                                std::nullopt, std::nullopt, id_token_opt, meter_values, std::nullopt, std::nullopt,
                                std::nullopt);

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

AuthorizeResponse ChargePoint::validate_token(const IdToken id_token, const std::optional<CiString<5500>>& certificate,
                                              const std::optional<std::vector<OCSPRequestData>>& ocsp_request_data) {
    // TODO(piet): C01.FR.14
    // TODO(piet): C01.FR.15
    // TODO(piet): C01.FR.16
    // TODO(piet): C01.FR.17

    // TODO(piet): C10.FR.05
    // TODO(piet): C10.FR.06
    // TODO(piet): C10.FR.07
    // TODO(piet): C10.FR.09

    AuthorizeResponse response;
    IdTokenInfo id_token_info;

    // C01.FR.01
    if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCtrlrEnabled).value_or(true)) {
        if (this->device_model->get_optional_value<bool>(ControllerComponentVariables::AuthCacheCtrlrEnabled)
                .value_or(true)) {
            const auto cache_entry =
                this->database_handler->get_auth_cache_entry(utils::sha256(id_token.idToken.get()));
            if (cache_entry.has_value() and
                this->device_model->get_value<bool>(ControllerComponentVariables::LocalPreAuthorize) and
                cache_entry.value().status == AuthorizationStatusEnum::Accepted and
                (!cache_entry.value().cacheExpiryDateTime.has_value() or
                 cache_entry.value().cacheExpiryDateTime.value().to_time_point() > DateTime().to_time_point())) {
                response.idTokenInfo = cache_entry.value();
                return response;
            }
        }
        // C01.FR.02
        response = this->authorize_req(id_token, certificate, ocsp_request_data);
        // C10.FR.08
        if (!response.idTokenInfo.cacheExpiryDateTime.has_value() and
            this->device_model->get_optional_value<int>(ControllerComponentVariables::AuthCacheLifeTime).has_value()) {
            // when CSMS does not set cacheExpiryDateTime and config variable for AuthCacheLifeTime is present
            response.idTokenInfo.cacheExpiryDateTime =
                DateTime(date::utc_clock::now() + std::chrono::seconds(this->device_model->get_value<int>(
                                                      ControllerComponentVariables::AuthCacheLifeTime)));
        }
        this->database_handler->insert_auth_cache_entry(utils::sha256(id_token.idToken.get()), response.idTokenInfo);
        return response;

    } else {
        id_token_info.status = AuthorizationStatusEnum::Accepted;
        response.idTokenInfo = id_token_info;
        return response;
    }
}

void ChargePoint::on_event(const std::vector<EventData>& events) {
    this->notify_event_req(events);
}

void ChargePoint::on_log_status_notification(UploadLogStatusEnum status, int32_t requestId)
{
    LogStatusNotificationRequest request;
    request.status = status;
    request.requestId = requestId;

    ocpp::Call<LogStatusNotificationRequest> call(request, this->message_queue->createMessageId());
    this->send<LogStatusNotificationRequest>(call);
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

    WebsocketConnectionOptions connection_options{
        OcppProtocolVersion::v201,
        this->device_model->get_value<std::string>(ControllerComponentVariables::CentralSystemURI),
        this->device_model->get_value<int>(ControllerComponentVariables::SecurityProfile),
        this->device_model->get_value<std::string>(ControllerComponentVariables::ChargePointId),
        this->device_model->get_optional_value<std::string>(ControllerComponentVariables::BasicAuthPassword),
        this->device_model->get_value<int>(ControllerComponentVariables::WebsocketReconnectInterval),
        this->device_model->get_value<std::string>(ControllerComponentVariables::SupportedCiphers12),
        this->device_model->get_value<std::string>(ControllerComponentVariables::SupportedCiphers13),
        0,
        "payload",
        true,
        false}; // TOD(Piet): fix this hard coded params

    this->websocket = std::make_unique<Websocket>(connection_options, this->pki_handler, this->logging);
    this->websocket->register_connected_callback([this](const int security_profile) {
        this->message_queue->resume();
        this->websocket_connection_status = WebsocketConnectionStatusEnum::Disconnected;
    });
    this->websocket->register_disconnected_callback([this]() {
        this->websocket_connection_status = WebsocketConnectionStatusEnum::Disconnected;
        this->message_queue->pause();
    });

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

void ChargePoint::handle_message(const json& json_message, const MessageType& message_type) {
    switch (message_type) {
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
    case MessageType::ChangeAvailability:
        this->handle_change_availability_req(json_message);
        break;
    case MessageType::TransactionEventResponse:
        // handled by transaction_event_req future
        break;
    case MessageType::DataTransfer:
        this->handle_data_transfer_req(json_message);
        break;
    case MessageType::GetLog:
        this->handle_get_log_req(json_message);
        break;
    }
}

void ChargePoint::message_callback(const std::string& message) {
    auto enhanced_message = this->message_queue->receive(message);
    auto json_message = enhanced_message.message;
    this->logging->central_system(conversions::messagetype_to_string(enhanced_message.messageType), message);

    if (this->registration_status == RegistrationStatusEnum::Accepted) {
        this->handle_message(json_message, enhanced_message.messageType);
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
                this->handle_message(json_message, enhanced_message.messageType);
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
                this->handle_message(json_message, enhanced_message.messageType);
            } else {
                const auto error_message = "Received TriggerMessage with requestedMessage != BootNotification before "
                                           "having received an accepted BootNotificationResponse";
                EVLOG_warning << error_message;
                const auto call_error = CallError(enhanced_message.uniqueId, "SecurityError", "", json({}));
                this->send(call_error);
            }
        } else {
            const auto error_message = "Received other message than BootNotificationResponse before "
                                       "having received an accepted BootNotificationResponse";
            EVLOG_warning << error_message;
            const auto call_error = CallError(enhanced_message.uniqueId, "SecurityError", "", json({}));
            this->send(call_error);
        }
    }
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
                    // because we do not actively read meter values at clock aligned timepoint, we switch the
                    // ReadingContext here
                    for (auto& sampled_value : _meter_value.sampledValue) {
                        sampled_value.context = ReadingContextEnum::Sample_Clock;
                    }

                    // this will apply configured measurands and possibly reduce the entries of sampledValue
                    // according to the configuration
                    const auto meter_value = utils::get_meter_value_with_measurands_applied(
                        _meter_value, utils::get_measurands_vec(this->device_model->get_value<std::string>(
                                          ControllerComponentVariables::AlignedDataMeasurands)));

                    if (evse->has_active_transaction()) {
                        // add meter value to transaction meter values
                        const auto& enhanced_transaction = evse->get_transaction();
                        enhanced_transaction->meter_values.push_back(_meter_value);
                        this->transaction_event_req(
                            TransactionEventEnum::Updated, DateTime(), enhanced_transaction->get_transaction(),
                            TriggerReasonEnum::MeterValueClock, enhanced_transaction->get_seq_no(), std::nullopt,
                            std::nullopt, std::nullopt, std::vector<MeterValue>(1, meter_value), std::nullopt,
                            std::nullopt, std::nullopt);
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

bool ChargePoint::is_change_availability_possible(const ChangeAvailabilityRequest& req) {
    if (req.evse.has_value() and this->evses.at(req.evse.value().id)->has_active_transaction()) {
        return false;
    } else {
        for (auto const& [evse_id, evse] : this->evses) {
            if (evse->has_active_transaction()) {
                return false;
            }
        }
    }
    return true;
}

void ChargePoint::handle_scheduled_change_availability_requests(const int32_t evse_id) {
    if (this->scheduled_change_availability_requests.count(evse_id)) {
        EVLOG_info << "Found scheduled ChangeAvailability.req for evse_id:" << evse_id;
        const auto req = this->scheduled_change_availability_requests[evse_id];
        if (this->is_change_availability_possible(req)) {
            EVLOG_info << "Changing availability of evse:" << evse_id;
            this->callbacks.change_availability_callback(req);
            this->scheduled_change_availability_requests.erase(evse_id);
        } else {
            EVLOG_info << "Cannot change availability because transaction is still active";
        }
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

    if (enhanced_message.messageType == MessageType::AuthorizeResponse) {
        ocpp::CallResult<AuthorizeResponse> call_result = enhanced_message.message;
        return call_result.msg;
    } else {
        AuthorizeResponse response;
        response.idTokenInfo.status = AuthorizationStatusEnum::Unknown;
        return response;
    }
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
                                        const std::optional<int32_t>& number_of_phases_used,
                                        const std::optional<bool>& offline,
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

    if (event_type == TransactionEventEnum::Started) {
        auto future = this->send_async<TransactionEventRequest>(call);
        const auto enhanced_message = future.get();
        if (enhanced_message.messageType == MessageType::TransactionEventResponse) {
            this->handle_start_transaction_event_response(enhanced_message.message, evse.value().id);
        } else if (enhanced_message.offline) {
            // TODO(piet): offline handling
        }
    } else {
        this->send<TransactionEventRequest>(call);
    }
}

void ChargePoint::meter_values_req(const int32_t evse_id, const std::vector<MeterValue>& meter_values) {
    MeterValuesRequest req;
    req.evseId = evse_id;
    req.meterValue = meter_values;

    ocpp::Call<MeterValuesRequest> call(req, this->message_queue->createMessageId());
    this->send<MeterValuesRequest>(call);
}

void ChargePoint::handle_get_log_req(Call<GetLogRequest> call)
{
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
        // set timers
        if (msg.interval > 0) {
            this->heartbeat_timer.interval([this]() { this->heartbeat_req(); }, std::chrono::seconds(msg.interval));
        }
        this->update_aligned_data_interval();

        for (auto const& [evse_id, evse] : this->evses) {
            evse->trigger_status_notification_callbacks();
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

    for (const auto& set_variable_data : msg.setVariableData) {
        SetVariableResult set_variable_result;
        set_variable_result.component = set_variable_data.component;
        set_variable_result.variable = set_variable_data.variable;
        set_variable_result.attributeType = set_variable_data.attributeType.value_or(AttributeEnum::Actual);
        set_variable_result.attributeStatus = this->device_model->set_value(
            set_variable_data.component, set_variable_data.variable,
            set_variable_data.attributeType.value_or(AttributeEnum::Actual), set_variable_data.attributeValue.get());

        response.setVariableResult.push_back(set_variable_result);
    }

    ocpp::CallResult<SetVariablesResponse> call_result(response, call.uniqueId);
    this->send<SetVariablesResponse>(call_result);
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

    // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage / BytesPerMessage
    const auto report_data = this->device_model->get_report_data(msg.reportBase);
    this->notify_report_req(msg.requestId, 0, report_data);
}

void ChargePoint::handle_get_report_req(Call<GetReportRequest> call) {
    const auto msg = call.msg;

    GetReportResponse response;

    // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage / BytesPerMessage
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

void ChargePoint::handle_reset_req(Call<ResetRequest> call) {
    // TODO(piet): B11.FR.02
    // TODO(piet): B11.FR.05
    // TODO(piet): B11.FR.09
    // TODO(piet): B11.FR.10

    // TODO(piet): B12.FR.01
    // TODO(piet): B12.FR.02
    // TODO(piet): B12.FR.03
    // TODO(piet): B12.FR.04
    // TODO(piet): B12.FR.05
    // TODO(piet): B12.FR.06
    // TODO(piet): B12.FR.07
    // TODO(piet): B12.FR.08
    // TODO(piet): B12.FR.09
    EVLOG_debug << "Received ResetRequest: " << call.msg << "\nwith messageId: " << call.uniqueId;
    const auto msg = call.msg;

    ResetResponse response;

    if (this->callbacks.is_reset_allowed_callback(msg.type)) {
        // reset is allowed
        response.status = ResetStatusEnum::Accepted;
    } else {
        response.status = ResetStatusEnum::Rejected;
    }

    if (/* transaction is active */ false) {
        if (msg.type == ResetEnum::OnIdle and !msg.evseId.has_value()) {
            // B12.FR.03
        } else if (msg.type == ResetEnum::Immediate and !msg.evseId.has_value()) {
            // B12.FR.04
        } else if (msg.type == ResetEnum::OnIdle and msg.evseId.has_value()) {
            // B12.FR.07
        } else if (msg.type == ResetEnum::Immediate and msg.evseId.has_value()) {
            // B12.FR.08
        }
    }

    ocpp::CallResult<ResetResponse> call_result(response, call.uniqueId);
    this->send<ResetResponse>(call_result);

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
        this->callbacks.reset_callback(ResetEnum::Immediate);
    }
}

void ChargePoint::handle_start_transaction_event_response(CallResult<TransactionEventResponse> call_result,
                                                          const int32_t evse_id) {
    const auto msg = call_result.msg;
    if (msg.idTokenInfo.has_value() and msg.idTokenInfo.value().status != AuthorizationStatusEnum::Accepted) {
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

void ChargePoint::handle_unlock_connector(Call<UnlockConnectorRequest> call) {
    const UnlockConnectorRequest& msg = call.msg;
    const UnlockConnectorResponse unlock_response = callbacks.unlock_connector_callback(msg.evseId, msg.connectorId);
    ocpp::CallResult<UnlockConnectorResponse> call_result(unlock_response, call.uniqueId);
    this->send<UnlockConnectorResponse>(call_result);
}

void ChargePoint::handle_change_availability_req(Call<ChangeAvailabilityRequest> call) {
    const auto msg = call.msg;
    ChangeAvailabilityResponse response;
    response.status = ChangeAvailabilityStatusEnum::Scheduled;

    const auto is_change_availability_possible = this->is_change_availability_possible(msg);

    if (is_change_availability_possible) {
        response.status = ChangeAvailabilityStatusEnum::Accepted;
    } else {
        auto evse_id = 0; // represents the whole charging station
        if (msg.evse.has_value()) {
            evse_id = msg.evse.value().id;
        }
        // add to map of scheduled operational_states. This also overrides successive ChangeAvailability.req with the
        // same EVSE, which is propably desirable
        this->scheduled_change_availability_requests[evse_id] = msg;
    }

    // send reply before applying changes to EVSE / Connector because this could trigger StatusNotification.req before
    // responding with ChangeAvailability.req
    ocpp::CallResult<ChangeAvailabilityResponse> call_result(response, call.uniqueId);
    this->send<ChangeAvailabilityResponse>(call_result);

    // execute change availability if possible
    if (is_change_availability_possible) {
        this->callbacks.change_availability_callback(msg);
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

} // namespace v201
} // namespace ocpp
