// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <framework/ModuleAdapter.hpp>
#include <utils/error/error_database.hpp>
#include <utils/error/error_database_map.hpp>
#include <utils/error/error_manager_req_global.hpp>
#include <utils/mqtt_config_service.hpp>
#include <utils/types.hpp>

#include <ld-ev.hpp>

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace module::stub {

class MQTTStub : public Everest::MQTTAbstraction {
private:
    const std::string m_everest_prefix{"everest/"};
    const std::string m_external_prefix{"external/"};

    std::map<std::string, std::shared_ptr<TypedHandler>> m_handlers;

public:
    using MQTTRequest = Everest::MQTTRequest;
    using QOS = Everest::QOS;

    struct publish_log_t {
        std::string topic;
        std::string msg;
    };
    using log_t = std::deque<publish_log_t>;

    MQTTStub() = default;

    log_t publish_log{};

    void clear() {
        publish_log.clear();
    }

    // ========================================================================
    // methods called by the module

    bool connect() override {
        return true;
    }
    void disconnect() override {
    }
    void publish(const std::string& topic, const json& json) override {
        auto data = json.dump();
        publish(topic, data);
    }
    void publish(const std::string& topic, const json& json, QOS qos, bool retain = false) override {
        publish(topic, json);
    }
    void publish(const std::string& topic, const std::string& data) override {
        // topics published from the module
        std::printf("publish(%s) %s\n", topic.c_str(), data.c_str());
        publish_log.push_back({topic, data});
    }
    void publish(const std::string& topic, const std::string& data, QOS qos, bool retain = false) override {
        publish(topic, data);
    }
    void subscribe(const std::string& topic) override {
        std::printf("subscribe(%s)\n", topic.c_str());
    }
    void subscribe(const std::string& topic, QOS qos) override {
        subscribe(topic);
    }
    void unsubscribe(const std::string& topic) override {
        std::printf("unsubscribe(%s)\n", topic.c_str());
    }
    void clear_retained_topics() override {
    }
    json get(const std::string& topic, QOS qos, std::size_t retries = 0) override {
        return {};
    }
    json get(const MQTTRequest& request, std::size_t retries = 0) override {
        return {};
    }
    const std::string& get_everest_prefix() const override {
        return m_everest_prefix;
    }
    const std::string& get_external_prefix() const override {
        return m_external_prefix;
    }
    std::shared_future<void> spawn_main_loop_thread() override {
        return {};
    }
    std::shared_future<void> get_main_loop_future() override {
        return {};
    }
    void register_handler(const std::string& topic, std::shared_ptr<TypedHandler> handler, QOS qos) override {
        std::printf("register(%s)\n", topic.c_str());
        m_handlers.emplace(topic, std::move(handler));
    }
    void unregister_handler(const std::string& topic, const Token& token) override {
        std::printf("unregister(%s)\n", topic.c_str());
    }

    // ========================================================================
    // methods to interact with handlers

    void call(HandlerType type, const std::string_view& topic, const json& obj) {
        bool found{false};
        const std::string tmp_topic{topic};
        if (const auto it = m_handlers.find(tmp_topic); it != m_handlers.end()) {
            if (it->second->type == type) {
                std::printf("call(%s) %s\n", topic.data(), obj.dump().c_str());
                (*it->second->handler)(tmp_topic, obj);
                found = true;
            }
        }
        if (!found) {
            std::printf("no MQTT handler for %s (%d)\n", topic.data(), static_cast<int>(type));
        }
    }

    void runtime_config_set(const std::string_view& topic, const json& obj) {
        call(HandlerType::ConfigurationRequest, topic, obj);
    }

    void runtime_config_set(const std::string_view& id, const std::string_view& name, const std::string& value) {
        std::string topic{m_everest_prefix};
        topic += "modules/";
        topic += id;
        topic += "/config/set_request";
        Everest::config::SetRequest req;
        req.identifier.module_id = id;
        req.identifier.configuration_parameter_name = name;
        req.value = value;
        json obj = req;
        call(HandlerType::ConfigurationRequest, topic, obj);
    }
};

class ErrorDatabaseStub : public Everest::error::ErrorDatabase {
public:
    using ErrorPtr = Everest::error::ErrorPtr;
    using ErrorFilter = Everest::error::ErrorFilter;

    void add_error(ErrorPtr error) override {
    }
    std::list<ErrorPtr> get_errors(const std::list<ErrorFilter>& filters) const override {
        return {};
    }
    std::list<ErrorPtr> edit_errors(const std::list<ErrorFilter>& filters, EditErrorFunc edit_func) override {
        return {};
    }
    std::list<ErrorPtr> remove_errors(const std::list<ErrorFilter>& filters) override {
        return {};
    }
};

class ExtendedModuleAdapter {
public:
    class Hooks {
    public:
        virtual ~Hooks() = default;
        virtual json call_fn(const std::string& topic, const json& value) = 0;
    };

    using ConfigServiceClient = ::Everest::config::ConfigServiceClient;
    using Error = ::Everest::error::Error;
    using ErrorCallback = ::Everest::error::ErrorCallback;
    using ErrorDatabase = ::Everest::error::ErrorDatabase;
    using ErrorDatabaseMap = ::Everest::error::ErrorDatabaseMap;
    using ErrorFactory = ::Everest::error::ErrorFactory;
    using ErrorManagerImpl = ::Everest::error::ErrorManagerImpl;
    using ErrorManagerReq = ::Everest::error::ErrorManagerReq;
    using ErrorManagerReqGlobal = ::Everest::error::ErrorManagerReqGlobal;
    using ErrorStateMonitor = ::Everest::error::ErrorStateMonitor;
    using ErrorType = ::Everest::error::ErrorType;
    using ErrorTypeMap = ::Everest::error::ErrorTypeMap;
    using ModuleAdapter = ::Everest::ModuleAdapter;
    using TelemetryMap = ::Everest::TelemetryMap;

private:
    // friend MQTTStub;

    const std::string m_module_id{"module_id"};
    const std::string m_implementation_id{"impl_id"};
    ImplementationIdentifier m_default_origin;
    std::shared_ptr<ErrorDatabase> m_error_database;
    std::shared_ptr<ErrorDatabaseMap> m_error_database_map;
    std::shared_ptr<ErrorFactory> m_error_factory;
    std::shared_ptr<ErrorManagerImpl> m_error_manager;
    std::shared_ptr<ErrorManagerReq> m_error_manager_req;
    std::shared_ptr<ErrorManagerReqGlobal> m_error_manager_req_global;
    std::shared_ptr<ErrorStateMonitor> m_error_state_monitor;
    std::shared_ptr<ErrorTypeMap> m_error_type_map;
    std::shared_ptr<ConfigServiceClient> m_config_service_client;
    std::map<std::string, Command> m_module_commands;
    std::unordered_map<std::string, std::string> m_module_names;
    std::shared_ptr<MQTTStub> m_mqtt;
    Hooks* handler{nullptr};

    Result call_fn(const Requirement&, const std::string& topic, Parameters value) {
        Result result;
        if (handler != nullptr) {
            result = handler->call_fn(topic, value);
        } else {
            std::printf("call_fn(%s) with no handler\n", topic.c_str());
        }
        return result;
    }
    void publish_fn(const std::string& topic, const std::string& id, Value val) {
        std::string full_topic = m_mqtt->get_everest_prefix() + id + topic;
        m_mqtt->publish(full_topic, val);
    }
    void subscribe_fn(const Requirement& req, const std::string& topic, ValueCallback cb) {
        std::string full_topic = m_mqtt->get_everest_prefix() + req.id + topic;
        const auto inner = [cb](const std::string&, Value arg) { cb(arg); };
        const auto handler =
            std::make_shared<TypedHandler>(HandlerType::SubscribeVar, std::make_shared<Handler>(inner));
        m_mqtt->register_handler(full_topic, handler, MQTTStub::QOS::QOS2);
    }
    std::shared_ptr<ErrorManagerImpl> get_error_manager_impl_fn(const std::string&) {
        return m_error_manager;
    }
    std::shared_ptr<ErrorStateMonitor> get_error_state_monitor_impl_fn(const std::string&) {
        return m_error_state_monitor;
    }
    std::shared_ptr<ErrorManagerReqGlobal> get_global_error_manager_fn() {
        return m_error_manager_req_global;
    }
    std::shared_ptr<ErrorStateMonitor> get_global_error_state_monitor_fn() {
        return m_error_state_monitor;
    }
    std::shared_ptr<ErrorFactory> get_error_factory_fn(const std::string&) {
        return m_error_factory;
    }
    std::shared_ptr<ErrorManagerReq> get_error_manager_req_fn(const Requirement&) {
        return m_error_manager_req;
    }
    std::shared_ptr<ErrorStateMonitor> get_error_state_monitor_req_fn(const Requirement&) {
        return m_error_state_monitor;
    }
    void ext_mqtt_publish_fn(const std::string& topic, const std::string& val) {
        std::string full_topic = m_mqtt->get_external_prefix() + topic;
        m_mqtt->publish(full_topic, val);
    }
    std::function<void()> ext_mqtt_subscribe_fn(const std::string& topic, StringHandler) {
        std::string full_topic = m_mqtt->get_external_prefix() + topic;
        m_mqtt->subscribe(full_topic);
        return {};
    }
    std::function<void()> ext_mqtt_subscribe_pair_fn(const std::string& topic, const StringPairHandler& handler) {
        std::string full_topic = m_mqtt->get_external_prefix() + topic;
        m_mqtt->subscribe(full_topic);
        return {};
    }
    void telemetry_publish_fn(const std::string&, const std::string&, const std::string&, const TelemetryMap&) {
    }
    std::optional<ModuleTierMappings> get_mapping_fn() {
        return {};
    }
    std::shared_ptr<ConfigServiceClient> get_config_service_client_fn() {
        return m_config_service_client;
    }

public:
    ExtendedModuleAdapter() : m_default_origin(m_module_id, m_implementation_id) {
        m_mqtt = std::make_shared<MQTTStub>();
        m_error_type_map = std::make_shared<ErrorTypeMap>();
        m_error_database_map = std::make_shared<ErrorDatabaseMap>();
        m_error_database = std::make_shared<ErrorDatabaseStub>();
        m_error_manager = std::make_shared<ErrorManagerImpl>(
            m_error_type_map, m_error_database_map, std::list<ErrorType>(), [](const Error&) {}, [](const Error&) {});
        m_error_state_monitor = std::make_shared<ErrorStateMonitor>(m_error_database_map);
        m_error_manager_req_global = std::make_shared<ErrorManagerReqGlobal>(
            m_error_type_map, m_error_database, [](const ErrorCallback&, const ErrorCallback&) {});
        m_error_factory = std::make_shared<ErrorFactory>(m_error_type_map, m_default_origin);
        m_error_manager_req = std::make_shared<ErrorManagerReq>(
            m_error_type_map, m_error_database_map, std::list<ErrorType>(),
            [](const ErrorType&, const ErrorCallback&, const ErrorCallback&) { std::printf("subscribe_error\n"); });
        m_config_service_client = std::make_shared<ConfigServiceClient>(m_mqtt, m_module_id, m_module_names);
    }

    void clear() {
        m_mqtt->clear();
    }

    const MQTTStub::log_t& get_module_publish_log() {
        return m_mqtt->publish_log;
    }

    operator ModuleAdapter() {
        ModuleAdapter result;
        result.call = [this](auto&&... ts) { return call_fn(ts...); };
        result.publish = [this](auto&&... ts) { publish_fn(ts...); };
        result.subscribe = [this](auto&&... ts) { subscribe_fn(ts...); };
        result.get_error_manager_impl = [this](auto&&... ts) { return get_error_manager_impl_fn(ts...); };
        result.get_error_state_monitor_impl = [this](auto&&... ts) { return get_error_state_monitor_impl_fn(ts...); };
        result.get_error_factory = [this](auto&&... ts) { return get_error_factory_fn(ts...); };
        result.get_error_manager_req = [this](auto&&... ts) { return get_error_manager_req_fn(ts...); };
        result.get_error_state_monitor_req = [this](auto&&... ts) { return get_error_state_monitor_req_fn(ts...); };
        result.get_global_error_manager = [this](auto&&... ts) { return get_global_error_manager_fn(ts...); };
        result.get_global_error_state_monitor = [this](auto&&... ts) {
            return get_global_error_state_monitor_fn(ts...);
        };
        result.ext_mqtt_publish = [this](auto&&... ts) { return ext_mqtt_publish_fn(ts...); };
        result.ext_mqtt_subscribe = [this](auto&&... ts) { return ext_mqtt_subscribe_fn(ts...); };
        result.ext_mqtt_subscribe_pair = [this](auto&&... ts) { return ext_mqtt_subscribe_pair_fn(ts...); };
        result.telemetry_publish = [this](auto&&... ts) { return telemetry_publish_fn(ts...); };
        result.get_mapping = [this](auto&&... ts) { return get_mapping_fn(ts...); };
        result.get_config_service_client = [this](auto&&... ts) { return get_config_service_client_fn(ts...); };
        return result;
    }

    // obtain interface functions from the module
    void register_commands(const std::vector<::Everest::cmd>& cmds) {
        for (const auto& i : cmds) {
            std::printf("module command: %s added\n", i.cmd_name.c_str());
            m_module_commands.insert({i.cmd_name, i.cmd});
        }
    }

    Hooks* set_handler(Hooks* ptr) {
        Hooks* tmp = handler;
        handler = ptr;
        return tmp;
    }

    // ========================================================================
    // configuration support

    void runtime_config_set(const std::string_view& name, const std::string& value) {
        m_mqtt->runtime_config_set(m_module_id, name, value);
    }

    // ========================================================================
    // MQTT publish - send the mgs to any subscribe handlers from the module

    void mqtt_publish(HandlerType type, const std::string_view& topic, const json& obj) {
        m_mqtt->call(type, topic, obj);
    }

    void mqtt_publish(HandlerType type, const std::string_view& topic, const std::string& msg) {
        try {
            auto obj = json::parse(msg);
            mqtt_publish(type, topic, obj);
        } catch (...) {
        }
    }

    // ========================================================================
    // call interfaces provided by the module

    json call(const std::string_view& cmd, const json& args) {
        json result;
        if (auto it = m_module_commands.find(std::string{cmd}); it != m_module_commands.end()) {
            std::printf("cmd %s(%s)\n", cmd.data(), args.dump().c_str());
            Parameters p = args;
            result = it->second(p);
            std::printf("cmd result %s(%s)\n", cmd.data(), result.dump().c_str());
        } else {
            std::printf("cmd %s not found\n", cmd.data());
        }
        return result;
    }
};

} // namespace module::stub
