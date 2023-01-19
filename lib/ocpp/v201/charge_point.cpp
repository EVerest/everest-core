// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/charge_point.hpp>

namespace ocpp {
namespace v201 {

const auto DEFAULT_BOOT_NOTIFICATION_RETRY_INTERVAL = std::chrono::seconds(30);

ChargePoint::ChargePoint(const json& config, const std::string& ocpp_main_path, const std::string& message_log_path) :
    ocpp::ChargingStationBase(),
    registration_status(RegistrationStatusEnum::Rejected),
    websocket_connection_status(WebsocketConnectionStatusEnum::Disconnected) {
    this->device_model_manager = std::make_shared<DeviceModelManager>(config, ocpp_main_path);
    this->pki_handler = std::make_shared<ocpp::PkiHandler>(ocpp_main_path, false); //FIXME(piet): Fix second parameter

    for (int evse_id = 1; evse_id <= this->device_model_manager->get_number_of_connectors(); evse_id++) {
        this->evses.insert(std::make_pair(
            evse_id, std::make_unique<Evse>(
                         evse_id, 1, [this, evse_id](const int32_t connector_id, const ConnectorStatusEnum& status) {
                             if (this->registration_status == RegistrationStatusEnum::Accepted) {
                                 this->status_notification_req(evse_id, connector_id, status);
                             }
                         })));
    }

    this->logging = std::make_shared<ocpp::MessageLogging>(true, message_log_path, DateTime().to_rfc3339(), false,
                                                           false, false, true, true);
    this->message_queue = std::make_unique<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return this->websocket->send(message.dump()); },
        this->device_model_manager->get_message_attempts_transaction_event(),
        this->device_model_manager->get_message_attempt_interval_transaction_event());
}

void ChargePoint::start() {
    this->init_websocket();
    this->websocket->connect(this->device_model_manager->get_security_profile());
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

void ChargePoint::on_session_finished(const int32_t evse_id, const int32_t connector_id) {
    this->evses.at(evse_id)->submit_event(connector_id, ConnectorEvent::PlugOut);
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

    if (this->device_model_manager->get_charge_point_id().find(':') != std::string::npos) {
        EVLOG_AND_THROW(std::runtime_error("ChargePointId must not contain \':\'"));
    }

    // FIXME(piet): get password properly from configuration
    boost::optional<std::string> basic_auth_password;
    basic_auth_password.emplace("DEADBEEFDEADBEEF");

    WebsocketConnectionOptions connection_options{OcppProtocolVersion::v201,
                                                  this->device_model_manager->get_central_system_uri(),
                                                  this->device_model_manager->get_security_profile(),
                                                  this->device_model_manager->get_charge_point_id(),
                                                  basic_auth_password,
                                                  this->device_model_manager->get_websocket_reconnect_interval(),
                                                  this->device_model_manager->get_supported_ciphers12(),
                                                  this->device_model_manager->get_supported_ciphers13(),
                                                  0,
                                                  "payload",
                                                  true,
                                                  false}; //TOD(Piet): fix this hard coded params

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

void ChargePoint::boot_notification_req(const BootReasonEnum& reason) {
    EVLOG_debug << "Sending BootNotification";
    BootNotificationRequest req;

    ChargingStation charging_station;
    charging_station.model = this->device_model_manager->get_charge_point_model();
    charging_station.vendorName = this->device_model_manager->get_charge_point_vendor();
    charging_station.firmwareVersion.emplace(this->device_model_manager->get_firmware_version());
    charging_station.serialNumber.emplace(this->device_model_manager->get_charge_box_serial_number());

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
        if (msg.interval > 0) {
            this->heartbeat_timer.interval([this]() { this->heartbeat_req(); }, std::chrono::seconds(msg.interval));
        }
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
        set_variable_result.attributeType = set_variable_data.attributeType.get_value_or(AttributeEnum::Actual);
        set_variable_result.attributeStatus = this->device_model_manager->set_variable(set_variable_data);

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
        get_variable_result.attributeType = get_variable_data.attributeType.get_value_or(AttributeEnum::Actual);
        const auto status_value_pair = this->device_model_manager->get_variable(get_variable_data);
        get_variable_result.attributeStatus = status_value_pair.first;
        if (status_value_pair.second.has_value()) {
            get_variable_result.attributeValue.emplace(status_value_pair.second.value());
        }

        response.getVariableResult.push_back(get_variable_result);
    }

    ocpp::CallResult<GetVariablesResponse> call_result(response, call.uniqueId);
    this->send<GetVariablesResponse>(call_result);
}

void ChargePoint::handle_get_base_report_req(Call<GetBaseReportRequest> call) {
    const auto msg = call.msg;

    // TODO(piet): B07.FR.02
    // TODO(piet): B07.FR.13

    GetBaseReportResponse response;
    response.status = GenericDeviceModelStatusEnum::Accepted;

    ocpp::CallResult<GetBaseReportResponse> call_result(response, call.uniqueId);
    this->send<GetBaseReportResponse>(call_result);

    // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage / BytesPerMessage
    const auto report_data = this->device_model_manager->get_report_data(msg.reportBase);
    this->notify_report_req(msg.requestId, 0, report_data);
}

void ChargePoint::handle_get_report_req(Call<GetReportRequest> call) {
    const auto msg = call.msg;

    GetReportResponse response;

    // TODO(piet): Propably split this up into several NotifyReport.req depending on ItemsPerMessage / BytesPerMessage
    const auto report_data = this->device_model_manager->get_report_data(ReportBaseEnum::FullInventory,
                                                                         msg.componentVariable, msg.componentCriteria);
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

} // namespace v201
} // namespace ocpp
