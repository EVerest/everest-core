// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <utils/config/mqtt_settings.hpp>

namespace Everest {

bool MQTTSettings::uses_socket() const {
    if (not broker_socket_path.empty()) {
        return true;
    }
    return false;
}

MQTTSettings create_mqtt_settings(const std::string& mqtt_broker_socket_path, const std::string& mqtt_everest_prefix,
                                  const std::string& mqtt_external_prefix) {
    MQTTSettings mqtt_settings;
    populate_mqtt_settings(mqtt_settings, mqtt_broker_socket_path, mqtt_everest_prefix, mqtt_external_prefix);
    return mqtt_settings;
}

MQTTSettings create_mqtt_settings(const std::string& mqtt_broker_host, int mqtt_broker_port,
                                  const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix) {
    MQTTSettings mqtt_settings;
    populate_mqtt_settings(mqtt_settings, mqtt_broker_host, mqtt_broker_port, mqtt_everest_prefix,
                           mqtt_external_prefix);
    return mqtt_settings;
}

void populate_mqtt_settings(MQTTSettings& mqtt_settings, const std::string& mqtt_broker_socket_path,
                            const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix) {
    mqtt_settings.broker_socket_path = mqtt_broker_socket_path;
    mqtt_settings.everest_prefix = mqtt_everest_prefix;
    mqtt_settings.external_prefix = mqtt_external_prefix;
}

void populate_mqtt_settings(MQTTSettings& mqtt_settings, const std::string& mqtt_broker_host, int mqtt_broker_port,
                            const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix) {
    mqtt_settings.broker_host = mqtt_broker_host;
    mqtt_settings.broker_port = mqtt_broker_port;
    mqtt_settings.everest_prefix = mqtt_everest_prefix;
    mqtt_settings.external_prefix = mqtt_external_prefix;
}

} // namespace Everest
