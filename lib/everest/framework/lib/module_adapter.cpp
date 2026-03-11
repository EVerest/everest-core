// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <framework/ModuleAdapter.hpp>

namespace Everest {
constexpr auto mqtt_provider_default_precision = 5;

ModuleBase::ModuleBase(const ModuleInfo& info) : info(info){};

void ModuleBase::invoke_init(ImplementationBase& impl) {
    impl.init();
}

void ModuleBase::invoke_ready(ImplementationBase& impl) {
    impl.ready();
}

void ModuleAdapter::check_complete() {
    // FIXME (aw): I should throw if some of my handlers are not set
    return;
}

void ModuleAdapter::gather_cmds(ImplementationBase& impl) {
    impl._gather_cmds(registered_commands);
}

MqttProvider::MqttProvider(ModuleAdapter& ev) : ev(ev){};

void MqttProvider::publish(const std::string& topic, const std::string& data, bool retain) {
    ev.ext_mqtt_publish(topic, data, retain);
}

void MqttProvider::publish(const std::string& topic, const char* data, bool retain) {
    ev.ext_mqtt_publish(topic, std::string(data), retain);
}

void MqttProvider::publish(const std::string& topic, bool data, bool retain) {
    if (data) {
        ev.ext_mqtt_publish(topic, "true", retain);
    } else {
        ev.ext_mqtt_publish(topic, "false", retain);
    }
}

void MqttProvider::publish(const std::string& topic, int data, bool retain) {
    ev.ext_mqtt_publish(topic, std::to_string(data), retain);
}

void MqttProvider::publish(const std::string& topic, double data, int precision, bool retain) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << data;
    ev.ext_mqtt_publish(topic, stream.str(), retain);
}

void MqttProvider::publish(const std::string& topic, double data, bool retain) {
    this->publish(topic, data, mqtt_provider_default_precision, retain);
}

UnsubscribeToken MqttProvider::subscribe(const std::string& topic, StringHandler handler) const {
    return ev.ext_mqtt_subscribe(topic, std::move(handler));
}

UnsubscribeToken MqttProvider::subscribe(const std::string& topic, StringPairHandler handler) const {
    return ev.ext_mqtt_subscribe_pair(topic, std::move(handler));
}

TelemetryProvider::TelemetryProvider(ModuleAdapter& ev) : ev(ev){};

void TelemetryProvider::publish(const std::string& category, const std::string& subcategory, const std::string& type,
                                const TelemetryMap& telemetry) {
    ev.telemetry_publish(category, subcategory, type, telemetry);
}

void TelemetryProvider::publish(const std::string& category, const std::string& subcategory,
                                const TelemetryMap& telemetry) {
    publish(category, subcategory, subcategory, telemetry);
}
} // namespace Everest
