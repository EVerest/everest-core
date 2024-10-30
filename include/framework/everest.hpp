// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef FRAMEWORK_EVEREST_HPP
#define FRAMEWORK_EVEREST_HPP

#include <chrono>
#include <future>
#include <map>
#include <set>
#include <thread>
#include <variant>

#include <everest/exceptions.hpp>

#include <utils/config.hpp>
#include <utils/error.hpp>
#include <utils/mqtt_abstraction.hpp>
#include <utils/types.hpp>

namespace Everest {
///
/// \brief A structure that contains a command definition for a cmd of a module
///
struct cmd {
    std::string impl_id;    ///< The implementation id of the command
    std::string cmd_name;   ///< The name of the command
    Command cmd;            ///< The callback function
    Arguments arg_types;    ///< The argument types
    ReturnType return_type; ///< The return type
};

using TelemetryEntry = std::variant<std::string, const char*, bool, int32_t, uint32_t, int64_t, uint64_t, double>;
using TelemetryMap = std::map<std::string, TelemetryEntry>;
using UnsubscribeToken = std::function<void()>;

namespace error {
struct ErrorDatabaseMap;
struct ErrorManagerImpl;
struct ErrorManagerReq;
struct ErrorManagerReqGlobal;
struct ErrorStateMonitor;
struct ErrorFactory;
} // namespace error

///
/// \brief Contains the EVerest framework that provides convenience functionality for implementing EVerest modules
///
class Everest {
public:
    Everest(std::string module_id, const Config& config, bool validate_data_with_schema,
            const std::string& mqtt_server_socket_path, const std::string& mqtt_server_address, int mqtt_server_port,
            const std::string& mqtt_everest_prefix, const std::string& mqtt_external_prefix,
            const std::string& telemetry_prefix, bool telemetry_enabled);

    // forbid copy assignment and copy construction
    // NOTE (aw): move assignment and construction are also not supported because we're creating explicit references to
    // our instance due to callback registration
    Everest(Everest const&) = delete;
    void operator=(Everest const&) = delete;

    json get_cmd_definition(const std::string& module_id, const std::string& impl_id, const std::string& cmd_name,
                            bool is_call);
    json get_cmd_definition(const std::string& module_id, const std::string& impl_id, const std::string& cmd_name);

    ///
    /// \brief Allows a module to indicate that it provides the given command \p cmd
    ///
    void provide_cmd(const std::string impl_id, const std::string cmd_name, const JsonCommand handler);
    void provide_cmd(const cmd& cmd);

    ///
    /// \brief Provides functionality for calling commands of other modules. The module is identified by the given \p
    /// req, the command by the given command name \p cmd_name and the needed arguments by \p args
    ///
    json call_cmd(const Requirement& req, const std::string& cmd_name, json args);

    ///
    /// \brief Publishes a variable of the given \p impl_id, names \p var_name with the given \p value
    ///
    void publish_var(const std::string& impl_id, const std::string& var_name, json value);

    ///
    /// \brief Subscribes to a variable of another module identified by the given \p req and variable name \p
    /// var_name. The given \p callback is called when a new value becomes available
    ///
    void subscribe_var(const Requirement& req, const std::string& var_name, const JsonCallback& callback);

    ///
    /// \brief Return the error manager for the given \p impl_id
    ///
    std::shared_ptr<error::ErrorManagerImpl> get_error_manager_impl(const std::string& impl_id);

    ///
    /// \brief Return the error state monitor for the given \p impl_id
    ///
    std::shared_ptr<error::ErrorStateMonitor> get_error_state_monitor_impl(const std::string& impl_id);

    ///
    /// \brief Return the error factory for the given \p impl_id
    ///
    std::shared_ptr<error::ErrorFactory> get_error_factory(const std::string& impl_id);

    ///
    /// \brief Return the error manager for the given \p req
    ///
    std::shared_ptr<error::ErrorManagerReq> get_error_manager_req(const Requirement& req);

    ///
    /// \brief Return the error state monitor for the given \p req
    std::shared_ptr<error::ErrorStateMonitor> get_error_state_monitor_req(const Requirement& req);

    ///
    /// \brief Return the global error manager, if not enabled nullptr
    std::shared_ptr<error::ErrorManagerReqGlobal> get_global_error_manager() const;

    ///
    /// \brief Return the global state monitor, if not enabled nullptr
    std::shared_ptr<error::ErrorStateMonitor> get_global_error_state_monitor() const;

    ///
    /// \brief publishes the given \p data on the given \p topic
    ///
    void external_mqtt_publish(const std::string& topic, const std::string& data);

    ///
    /// \brief Allows a module to indicate that it provides a external mqtt \p handler at the given \p topic
    ///
    UnsubscribeToken provide_external_mqtt_handler(const std::string& topic, const StringHandler& handler);

    ///
    /// \brief publishes the given telemetry \p data on the given \p topic
    ///
    void telemetry_publish(const std::string& topic, const std::string& data);

    ///
    /// \brief publishes the given telemetry \p telemetry on a topic constructed from \p category \p subcategory and \p
    /// type
    ///
    void telemetry_publish(const std::string& category, const std::string& subcategory, const std::string& type,
                           const TelemetryMap& telemetry);

    /// \returns true if telemetry is enabled
    bool is_telemetry_enabled();

    ///
    /// \returns the 3 tier model mappings for this module
    ///
    std::optional<ModuleTierMappings> get_3_tier_model_mapping();

    ///
    /// \brief Chccks if all commands of a module that are listed in its manifest are available
    ///
    void check_code();

    ///
    /// \brief Calls the connect method of the MQTTAbstraction to connect to the MQTT broker
    ///
    bool connect();

    ///
    /// \brief Calls the disconnect method of the MQTTAbstraction to disconnect from the MQTT broker
    ///
    void disconnect();

    ///
    /// \brief Initiates spawning the MQTT main loop thread
    ///
    void spawn_main_loop_thread();

    ///
    /// \brief Wait for main loop thread to end
    ///
    void wait_for_main_loop_end();

    ///
    /// \brief Ready Handler for local readyness (e.g. this module is now ready)
    ///
    void signal_ready();

    ///
    /// \brief registers a callback \p handler that is called when the global ready signal is received via mqtt
    ///
    void register_on_ready_handler(const std::function<void()>& handler);

private:
    MQTTAbstraction mqtt_abstraction;
    Config config;
    std::string module_id;
    std::map<std::string, std::shared_ptr<error::ErrorManagerImpl>> impl_error_managers; // one per implementation
    std::map<std::string, std::shared_ptr<error::ErrorStateMonitor>>
        impl_error_state_monitors;                                                             // one per implementation
    std::map<std::string, std::shared_ptr<error::ErrorFactory>> error_factories;               // one per implementation
    std::map<Requirement, std::shared_ptr<error::ErrorManagerReq>> req_error_managers;         // one per requirement
    std::map<Requirement, std::shared_ptr<error::ErrorStateMonitor>> req_error_state_monitors; // one per requirement
    std::shared_ptr<error::ErrorManagerReqGlobal> global_error_manager;   // nullptr if not enabled in manifest
    std::shared_ptr<error::ErrorStateMonitor> global_error_state_monitor; // nullptr if not enabled in manifest
    std::map<std::string, std::set<std::string>> registered_cmds;
    bool ready_received;
    std::chrono::seconds remote_cmd_res_timeout;
    bool validate_data_with_schema;
    std::unique_ptr<std::function<void()>> on_ready;
    std::thread heartbeat_thread;
    std::string module_name;
    std::future<void> main_loop_end{};
    json module_manifest;
    json module_classes;
    std::string mqtt_everest_prefix;
    std::string mqtt_external_prefix;
    std::string telemetry_prefix;
    std::optional<TelemetryConfig> telemetry_config;
    bool telemetry_enabled;
    std::optional<ModuleTierMappings> module_tier_mappings;

    void handle_ready(json data);

    void heartbeat();

    void publish_metadata();

    static std::string check_args(const Arguments& func_args, json manifest_args);
    static bool check_arg(ArgumentType arg_types, json manifest_arg);

    ///
    /// \brief Publishes the given \p error as a cleared error
    ///
    void publish_cleared_error(const std::string& impl_id, const error::Error& error);

    ///
    /// \brief Publishes the given \p error as a raised error
    ///
    void publish_raised_error(const std::string& impl_id, const error::Error& error);

    ///
    /// \brief Subscribes to an error of another module indentified by the given \p req and error type
    /// \p error_type. The given \p callback is called when a new error is raised and \p clear_callback is called when
    /// an error is cleared
    ///
    void subscribe_error(const Requirement& req, const error::ErrorType& error_type,
                         const error::ErrorCallback& callback, const error::ErrorCallback& clear_callback);

    ///
    /// \brief Subscribes globally to all errors of all modules. The given \p callback is called when a new error is
    /// raised. The given \p clear_callback is called when an error is cleared
    ///
    void subscribe_global_all_errors(const error::ErrorCallback& callback, const error::ErrorCallback& clear_callback);
};

///
/// \returns the 3 tier model mapping from a \p module_tier_mapping for the given \p impl_id
///
std::optional<Mapping> get_impl_mapping(std::optional<ModuleTierMappings> module_tier_mappings,
                                        const std::string& impl_id);

} // namespace Everest

#endif // FRAMEWORK_EVEREST_HPP
