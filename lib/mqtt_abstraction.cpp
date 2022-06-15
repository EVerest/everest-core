// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <utils/mqtt_abstraction.hpp>
#include <utils/mqtt_abstraction_impl.hpp>

namespace Everest {
MQTTAbstraction::MQTTAbstraction(const std::string& mqtt_server_address, const std::string& mqtt_server_port) :
    mqtt_abstraction(MQTTAbstractionImpl::get_instance(mqtt_server_address, mqtt_server_port)) {
    EVLOG_debug << "initialized mqtt_abstraction";
}

bool MQTTAbstraction::connect() {
    BOOST_LOG_FUNCTION();
    return mqtt_abstraction.connect();
}

void MQTTAbstraction::disconnect() {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.disconnect();
}

void MQTTAbstraction::publish(const std::string& topic, const json& json) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.publish(topic, json);
}

void MQTTAbstraction::publish(const std::string& topic, const json& json, QOS qos) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.publish(topic, json, qos);
}

void MQTTAbstraction::publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.publish(topic, data);
}

void MQTTAbstraction::publish(const std::string& topic, const std::string& data, QOS qos) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.publish(topic, data, qos);
}

void MQTTAbstraction::subscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.subscribe(topic);
}

void MQTTAbstraction::subscribe(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.subscribe(topic, qos);
}

void MQTTAbstraction::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.unsubscribe(topic);
}

std::future<void> MQTTAbstraction::spawn_main_loop_thread() {
    BOOST_LOG_FUNCTION();
    return mqtt_abstraction.spawn_main_loop_thread();
}

void MQTTAbstraction::register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler,
                                       bool allow_multiple_handlers, QOS qos) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.register_handler(topic, handler, allow_multiple_handlers, qos);
}

void MQTTAbstraction::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction.unregister_handler(topic, token);
}

} // namespace Everest
