// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONFIG_MQTT_SETTINGS_HPP
#define UTILS_CONFIG_MQTT_SETTINGS_HPP

#include <date/date.h>
#include <date/tz.h>

namespace Everest {

/// \brief minimal MQTT connection settings needed for an initial connection of a module to the manager
struct MQTTSettings {
    std::string broker_socket_path; ///< A path to a socket the MQTT broker uses in socket mode. If this is set
                                    ///< broker_host and broker_port are ignored
    std::string broker_host;        ///< The hostname of the MQTT broker
    int broker_port = 0;            ///< The port the MQTT broker listens on
    std::string everest_prefix;     ///< MQTT topic prefix for the "everest" topic
    std::string external_prefix;    ///< MQTT topic prefix for external topics

    /// \brief Indicates if a Unix Domain Socket is used for connection to the MQTT broker
    /// \returns true is a UDS is used, false if a connection via host and port is used
    bool uses_socket() const;
};

/// \brief Creates MQTTSettings with a Unix Domain Socket with the provided \p mqtt_broker_socket_path
/// using the \p mqtt_everest_prefix and \p mqtt_external_prefix
MQTTSettings create_mqtt_settings(const std::string& mqtt_broker_socket_path, const std::string& mqtt_everest_prefix,
                                  const std::string& mqtt_external_prefix);

/// \brief Creates MQTTSettings for IP based connections with the provided \p mqtt_broker_host
/// and \p mqtt_broker_port using the \p mqtt_everest_prefix and \p mqtt_external_prefix
MQTTSettings create_mqtt_settings(const std::string& mqtt_broker_host, int mqtt_broker_port,
                                  const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix);

/// \brief Populates the given MQTTSettings \p mqtt_settings with a Unix Domain Socket with the provided \p
/// mqtt_broker_socket_path using the \p mqtt_everest_prefix and \p mqtt_external_prefix
void populate_mqtt_settings(MQTTSettings& mqtt_settings, const std::string& mqtt_broker_socket_path,
                            const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix);

/// \brief  Populates the given MQTTSettings \p mqtt_settings for IP based connections with the provided \p
/// mqtt_broker_host and \p mqtt_broker_port using the \p mqtt_everest_prefix and \p mqtt_external_prefix
void populate_mqtt_settings(MQTTSettings& mqtt_settings, const std::string& mqtt_broker_host, int mqtt_broker_port,
                            const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix);

} // namespace Everest

#endif // UTILS_CONFIG_MQTT_SETTINGS_HPP
