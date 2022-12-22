// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/v201/charge_point.hpp>

namespace ocpp {
namespace v201 {

ChargePoint::ChargePoint(const json& config, const std::string& ocpp_main_path, const std::string& message_log_path) :
    ocpp::ChargePoint() {
    this->pki_handler = std::make_shared<ocpp::PkiHandler>(ocpp_main_path);
    this->configuration =
        std::make_shared<ChargePointConfiguration>(config, ocpp_main_path, this->pki_handler);
    this->logging = std::make_shared<ocpp::MessageLogging>(true, message_log_path, false, false, false, true);
    this->message_queue = std::make_unique<ocpp::MessageQueue<v201::MessageType>>(
        [this](json message) -> bool { return this->websocket->send(message.dump()); }, 5, 30);
}

bool ChargePoint::start() {
    this->init_websocket();
    this->websocket->connect(this->configuration->get_security_profile());
    return true;
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
    WebsocketConnectionOptions connection_options{OcppProtocolVersion::v201,
                                                  this->configuration->get_central_system_uri(),
                                                  this->configuration->get_security_profile(),
                                                  this->configuration->get_charge_point_id(),
                                                  boost::none,
                                                  this->configuration->get_websocket_reconnect_interval(),
                                                  this->configuration->get_supported_ciphers12(),
                                                  this->configuration->get_supported_ciphers13()};

    this->websocket = std::make_unique<Websocket>(connection_options, this->pki_handler, this->logging);
    this->websocket->register_connected_callback([this](const int security_profile) {
        this->message_queue->resume();
        this->connection_state = ChargePointConnectionState::Connected;
        this->boot_notification_req();
    });
    this->websocket->register_disconnected_callback([this]() {
        this->message_queue->pause(); //
    });

    this->websocket->register_message_callback([this](const std::string& message) { this->message_callback(message); });
}

void ChargePoint::handle_message(const json& json_message, MessageType message_type) {
    switch (message_type) {
    case MessageType::BootNotificationResponse:
        this->handle_boot_notification_response(json_message);
    }
}

void ChargePoint::message_callback(const std::string& message) {
    EVLOG_info << "Received Message: " << message;
    auto enhanced_message = this->message_queue->receive(message);
    auto json_message = enhanced_message.message;
    this->handle_message(json_message, enhanced_message.messageType);
}

void ChargePoint::boot_notification_req() {
    EVLOG_debug << "Sending BootNotification";
    BootNotificationRequest req;

    ChargingStation charging_station;
    charging_station.model = this->configuration->get_charge_point_model();
    charging_station.vendorName = this->configuration->get_charge_point_vendor();

    req.reason = BootReasonEnum::PowerUp;
    req.chargingStation = charging_station;

    ocpp::Call<BootNotificationRequest> call(req, this->message_queue->createMessageId());
    this->send<BootNotificationRequest>(call);
}

void ChargePoint::handle_boot_notification_response(CallResult<BootNotificationResponse> call_result) {
    EVLOG_info << "Received BootNotificationResponse: " << call_result.msg
               << "\nwith messageId: " << call_result.uniqueId;
}

} // namespace v201
} // namespace ocpp
