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

///
/// \brief Contains the EVerest framework that provides convenience functionality for implementing EVerest modules
///
class Everest {
public:
    Everest(std::string module_id, const Config& config, bool validate_data_with_schema,
            const std::string& mqtt_server_address, int mqtt_server_port, const std::string& mqtt_everest_prefix,
            const std::string& mqtt_external_prefix, const std::string& telemetry_prefix, bool telemetry_enabled);

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
    /// \brief Subscribes to an error of another module indentified by the given \p req and error type
    /// \p error_type. The given \p callback is called when a new error is raised
    ///
    void subscribe_error(const Requirement& req, const std::string& error_type, const JsonCallback& callback);

    ///
    /// \brief Subscribes to all errors. The given \p callback is called when a new error is raised.
    ///
    void subscribe_all_errors(const JsonCallback& callback);

    ///
    /// \brief Subscribes to an error cleared event of another module indentified by the given \p req and error type
    /// \p error_type. The given \p callback is called when an error is cleared
    ///
    void subscribe_error_cleared(const Requirement& req, const std::string& error_type, const JsonCallback& callback);

    ///
    /// \brief Subscribes to all errors cleared events. The given \p callback is called when an error is cleared.
    ///
    void subscribe_all_errors_cleared(const JsonCallback& callback);
    ///
    /// \brief Requests to clear errors
    /// If \p request_type is RequestClearErrorOption::ClearUUID, the error with the given \p uuid of the given \p
    /// impl_id is cleared. \p error_type is ignored. If \p request_type is
    /// RequestClearErrorOption::ClearAllOfTypeOfModule, all errors of the given \p impl_id with the given \p error_type
    /// are cleared. \p uuid is ignored. If \p request_type is RequestClearErrorOption::ClearAllOfModule, all errors of
    /// the given \p impl_id are cleared. \p uuid and \p error_type are ignored. Return a response json with the uuids
    /// of the cleared errors
    ///
    json request_clear_error(const error::RequestClearErrorOption request_type, const std::string& impl_id,
                             const std::string& uuid, const std::string& error_type);

    ///
    /// \brief Raises an given \p error of the given \p impl_id, with the given \p error_type. Returns the uuid of the
    /// raised error
    ///
    std::string raise_error(const std::string& impl_id, const std::string& error_type, const std::string& message,
                            const std::string& severity);

    ///
    /// \brief publishes the given \p data on the given \p topic
    ///
    void external_mqtt_publish(const std::string& topic, const std::string& data);

    ///
    /// \brief Allows a module to indicate that it provides a external mqtt \p handler at the given \p topic
    ///
    void provide_external_mqtt_handler(const std::string& topic, const StringHandler& handler);

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

    void handle_ready(json data);

    void heartbeat();

    void publish_metadata();

    static std::string check_args(const Arguments& func_args, json manifest_args);
    static bool check_arg(ArgumentType arg_types, json manifest_arg);
};
} // namespace Everest

#endif // FRAMEWORK_EVEREST_HPP
