// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_MQTT_ABSTRACTION_HPP
#define UTILS_MQTT_ABSTRACTION_HPP

#include <future>

#include <nlohmann/json.hpp>

#include <utils/mqtt_settings.hpp>
#include <utils/types.hpp>

namespace Everest {
// forward declaration
class MQTTAbstractionImpl;

///
/// \brief Contains a C++ abstraction for using MQTT in EVerest modules
///
class MQTTAbstraction {
public:
    /// \brief Create a MQTTAbstraction with the provideded \p mqtt_settings
    explicit MQTTAbstraction(const MQTTSettings& mqtt_settings);

    // forbid copy assignment and copy construction
    MQTTAbstraction(MQTTAbstraction const&) = delete;
    void operator=(MQTTAbstraction const&) = delete;

    ~MQTTAbstraction();

    ///
    /// \copydoc MQTTAbstractionImpl::connect()
    bool connect();

    ///
    /// \copydoc MQTTAbstractionImpl::disconnect()
    void disconnect();

    ///
    /// \copydoc MQTTAbstractionImpl::publish(const std::string&, const nlohmann::json&)
    void publish(const std::string& topic, const nlohmann::json& json);

    ///
    /// \copydoc MQTTAbstractionImpl::publish(const std::string&, const nlohmann::json&, QOS)
    void publish(const std::string& topic, const nlohmann::json& json, QOS qos, bool retain = false);

    ///
    /// \copydoc MQTTAbstractionImpl::publish(const std::string&, const std::string&)
    void publish(const std::string& topic, const std::string& data);

    ///
    /// \copydoc MQTTAbstractionImpl::publish(const std::string&, const std::string&, QOS)
    void publish(const std::string& topic, const std::string& data, QOS qos, bool retain = false);

    ///
    /// \copydoc MQTTAbstractionImpl::subscribe(const std::string&)
    void subscribe(const std::string& topic);

    ///
    /// \copydoc MQTTAbstractionImpl::subscribe(const std::string&, QOS)
    void subscribe(const std::string& topic, QOS qos);

    ///
    /// \copydoc MQTTAbstractionImpl::unsubscribe(const std::string&)
    void unsubscribe(const std::string& topic);

    ///
    /// \copydoc MQTTAbstractionImpl::clear_retained_topics()
    void clear_retained_topics();

    ///
    /// \copydoc MQTTAbstractionImpl::get(const std::string&, QOS)
    nlohmann::json get(const std::string& topic, QOS qos);

    ///
    /// \brief Get MQTT topic prefix for the "everest" topic
    const std::string& get_everest_prefix() const;

    ///
    /// \brief Get MQTT topic prefix for external topics
    const std::string& get_external_prefix() const;

    ///
    /// \copydoc MQTTAbstractionImpl::spawn_main_loop_thread()
    std::shared_future<void> spawn_main_loop_thread();

    ///
    /// \copydoc MQTTAbstractionImpl::get_main_loop_future()
    std::shared_future<void> get_main_loop_future();

    ///
    /// \copydoc MQTTAbstractionImpl::register_handler(const std::string&, std::shared_ptr<TypedHandler>, QOS)
    void register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler, QOS qos);

    ///
    /// \copydoc MQTTAbstractionImpl::unregister_handler(const std::string&, const Token&)
    void unregister_handler(const std::string& topic, const Token& token);

private:
    std::unique_ptr<MQTTAbstractionImpl> mqtt_abstraction;
    std::string everest_prefix;
    std::string external_prefix;
};
} // namespace Everest

#endif // UTILS_MQTT_ABSTRACTION_HPP
