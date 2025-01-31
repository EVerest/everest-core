// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>

#include <utils/mqtt_abstraction.hpp>
#include <utils/mqtt_abstraction_impl.hpp>

namespace Everest {

std::unique_ptr<MQTTAbstractionImpl> create_mqtt_client(const MQTTSettings& mqtt_settings) {
    if (mqtt_settings.uses_socket()) {
        return std::make_unique<MQTTAbstractionImpl>(mqtt_settings.broker_socket_path, mqtt_settings.everest_prefix,
                                                     mqtt_settings.external_prefix);
    } else {
        return std::make_unique<MQTTAbstractionImpl>(mqtt_settings.broker_host,
                                                     std::to_string(mqtt_settings.broker_port),
                                                     mqtt_settings.everest_prefix, mqtt_settings.external_prefix);
    }
}

MQTTAbstraction::MQTTAbstraction(const MQTTSettings& mqtt_settings) :
    everest_prefix(mqtt_settings.everest_prefix),
    external_prefix(mqtt_settings.external_prefix),
    mqtt_abstraction(create_mqtt_client(mqtt_settings)) {
}

MQTTAbstraction::~MQTTAbstraction() = default;

bool MQTTAbstraction::connect() {
    BOOST_LOG_FUNCTION();
    return mqtt_abstraction->connect();
}

void MQTTAbstraction::disconnect() {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->disconnect();
}

void MQTTAbstraction::publish(const std::string& topic, const json& json) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->publish(topic, json);
}

void MQTTAbstraction::publish(const std::string& topic, const json& json, QOS qos, bool retain) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->publish(topic, json, qos, retain);
}

void MQTTAbstraction::publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->publish(topic, data);
}

void MQTTAbstraction::publish(const std::string& topic, const std::string& data, QOS qos, bool retain) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->publish(topic, data, qos, retain);
}

void MQTTAbstraction::subscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->subscribe(topic);
}

void MQTTAbstraction::subscribe(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->subscribe(topic, qos);
}

void MQTTAbstraction::unsubscribe(const std::string& topic) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->unsubscribe(topic);
}

void MQTTAbstraction::clear_retained_topics() {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->clear_retained_topics();
}

json MQTTAbstraction::get(const std::string& topic, QOS qos) {
    BOOST_LOG_FUNCTION();
    return mqtt_abstraction->get(topic, qos);
}

const std::string& MQTTAbstraction::get_everest_prefix() const {
    BOOST_LOG_FUNCTION();
    return everest_prefix;
}

const std::string& MQTTAbstraction::get_external_prefix() const {
    BOOST_LOG_FUNCTION();
    return external_prefix;
}

std::shared_future<void> MQTTAbstraction::spawn_main_loop_thread() {
    BOOST_LOG_FUNCTION();
    return mqtt_abstraction->spawn_main_loop_thread();
}

std::shared_future<void> MQTTAbstraction::get_main_loop_future() {
    BOOST_LOG_FUNCTION();
    return mqtt_abstraction->get_main_loop_future();
}

void MQTTAbstraction::register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler, QOS qos) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->register_handler(topic, handler, qos);
}

void MQTTAbstraction::unregister_handler(const std::string& topic, const Token& token) {
    BOOST_LOG_FUNCTION();
    mqtt_abstraction->unregister_handler(topic, token);
}

} // namespace Everest
