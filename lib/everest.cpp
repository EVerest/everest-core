// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <future>
#include <map>
#include <set>

#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <date/date.h>
#include <everest/logging.hpp>
#include <fmt/format.h>
#include <sigslot/signal.hpp>

#include <framework/everest.hpp>
#include <utils/conversions.hpp>

namespace Everest {
sigslot::signal<std::string, json> signalPublish;
const auto remote_cmd_ack_timeout_seconds = 4;
const auto remote_cmd_res_timeout_seconds = 300;

Everest::Everest(std::string module_id, Config config, bool validate_data_with_schema,
                 const std::string& mqtt_server_address, const std::string& mqtt_server_port) :
    mqtt_abstraction(MQTTAbstraction::get_instance(mqtt_server_address, mqtt_server_port)),
    config(std::move(config)),
    module_id(std::move(module_id)),
    remote_cmd_ack_timeout(remote_cmd_ack_timeout_seconds),
    remote_cmd_res_timeout(remote_cmd_res_timeout_seconds),
    validate_data_with_schema(validate_data_with_schema) {
    BOOST_LOG_FUNCTION();

    EVLOG(debug) << "Initializing EVerest framework...";

    this->module_name = this->config.get_main_config()[this->module_id]["module"].get<std::string>();
    this->module_manifest = this->config.get_manifests()[this->module_name];
    this->module_classes = this->config.get_interfaces()[this->module_name];

    this->ready_received = false;
    this->on_ready = nullptr;

    // register handler for global ready signal
    std::shared_ptr<Handler> everest_ready = this->mqtt_abstraction.register_handler(
        "everest/ready", [this](auto&& PH1) { handle_ready(std::forward<decltype(PH1)>(PH1)); });

    signalPublish.connect(&Everest::internal_publish, this);
}

void Everest::mainloop() {
    BOOST_LOG_FUNCTION();

    this->mqtt_abstraction.mainloop();
}

void Everest::heartbeat() {
    BOOST_LOG_FUNCTION();
    std::ostringstream heartbeat_topic_stream;
    heartbeat_topic_stream << this->config.mqtt_module_prefix(this->module_id) << "/heartbeat";
    std::string heartbeat_topic = heartbeat_topic_stream.str();

    using namespace date;

    while (this->ready_received) {
        std::ostringstream now;
        now << std::chrono::system_clock::now();
        this->internal_publish(heartbeat_topic, json(now.str()));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Everest::register_on_ready_handler(const std::function<void()>& handler) {
    BOOST_LOG_FUNCTION();

    this->on_ready = std::make_unique<std::function<void()>>(handler);
}

void Everest::check_code() {
    BOOST_LOG_FUNCTION();

    json module_manifest =
        this->config.get_manifests()[this->config.get_main_config()[this->module_id]["module"].get<std::string>()];
    for (auto& element : module_manifest["provides"].items()) {
        auto const& impl_id = element.key();
        auto impl_manifest = element.value();

        std::set<std::string> cmds_not_registered;
        std::set<std::string> impl_manifest_cmds_set;
        if (impl_manifest.contains("cmds")) {
            impl_manifest_cmds_set = Config::keys(impl_manifest["cmds"]);
        }
        std::set<std::string> registered_cmds_set = this->registered_cmds[impl_id];

        std::set_difference(impl_manifest_cmds_set.begin(), impl_manifest_cmds_set.end(), registered_cmds_set.begin(),
                            registered_cmds_set.end(), std::inserter(cmds_not_registered, cmds_not_registered.end()));

        if (!cmds_not_registered.empty()) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "{} does not provide all cmds listed in manifest! Missing cmd(s): [{}]",
                this->config.printable_identifier(module_id, impl_id), fmt::join(cmds_not_registered, " "))));
        }
    }
}

bool Everest::connect() {
    BOOST_LOG_FUNCTION();

    return this->mqtt_abstraction.connect();
}

void Everest::disconnect() {
    BOOST_LOG_FUNCTION();

    this->mqtt_abstraction.disconnect();
}

json Everest::call_cmd(const Requirement& req, const std::string& cmd_name, json json_args) {
    BOOST_LOG_FUNCTION();

    // resolve requirement
    json connections = this->config.resolve_requirement(this->module_id, req.id);
    auto& connection = connections; // this is for a min/max == 1 requirement
    if (connections.is_array()) {   // this is for every other requirement
        connection = connections[req.index];
    }

    // extract manifest definition of this command
    json cmd_definition = get_cmd_definition(connection["module_id"], connection["implementation_id"], cmd_name, true);

    std::set<std::string> arg_names = Config::keys(json_args);

    // check args against manifest
    if (this->validate_data_with_schema) {
        if (cmd_definition["arguments"].size() != json_args.size()) {
            EVLOG_AND_THROW(EverestApiError(
                fmt::format("Call to {}->{}({}): Argument cound does not match manifest!",
                            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]),
                            cmd_name, fmt::join(arg_names, ", "))));
        }

        std::set<std::string> unknown_arguments;
        std::set<std::string> cmd_arguments;
        if (cmd_definition.contains("arguments")) {
            cmd_arguments = Config::keys(cmd_definition["arguments"]);
        }

        std::set_difference(arg_names.begin(), arg_names.end(), cmd_arguments.begin(), cmd_arguments.end(),
                            std::inserter(unknown_arguments, unknown_arguments.end()));

        if (!unknown_arguments.empty()) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Call to {}->{}({}): Argument names do not match manifest: {} != {}!",
                this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name,
                fmt::join(arg_names, ","), fmt::join(arg_names, ","), fmt::join(cmd_arguments, ","))));
        }
    }

    if (this->validate_data_with_schema) {
        json_validator validator(Config::loader, Config::format_checker);
        for (auto const& arg_name : arg_names) {
            try {
                validator.set_root_schema(cmd_definition["arguments"][arg_name]);
                validator.validate(json_args[arg_name]);
            } catch (const std::exception& e) {
                EVLOG_AND_THROW(EverestApiError(fmt::format(
                    "Call to {}->{}({}): Argument '{}' with value '{}' could not be validated with schema: {}",
                    this->config.printable_identifier(connection["module_id"], connection["implementation_id"]),
                    cmd_name, fmt::join(arg_names, ","), arg_name, json_args[arg_name], e.what())));
            }
        }
    }

    std::string call_id = boost::uuids::to_string(boost::uuids::random_generator()());

    // handle acks by registering an mqtt handler on the ack-topic
    std::ostringstream ack_topic_str;
    ack_topic_str << this->config.mqtt_prefix(connection["module_id"], connection["implementation_id"]) << "/ack/"
                  << cmd_name;
    std::string ack_topic = ack_topic_str.str();

    std::promise<bool> ack_promise;
    std::future<bool> ack_future = ack_promise.get_future();

    Handler ack_handler = [this, &ack_promise, call_id, connection, cmd_name](json data) {
        if (data["id"] != call_id) {
            EVLOG(debug) << fmt::format("ACK: data_id != call_id ({} != {})", data["id"], call_id);
            return;
        }

        EVLOG(debug) << fmt::format(
            "Incoming ack {} for {}->{}()", data["id"],
            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name);

        ack_promise.set_value(true);
    };

    Token ack_token = this->mqtt_abstraction.register_handler(ack_topic, ack_handler, true);

    std::promise<json> res_promise;
    std::future<json> res_future = res_promise.get_future();

    std::ostringstream res_topic_str;
    res_topic_str << this->config.mqtt_prefix(connection["module_id"], connection["implementation_id"]) << "/res/"
                  << cmd_name;
    std::string res_topic = res_topic_str.str();

    Handler res_handler = [this, &res_promise, call_id, connection, cmd_name](json data) {
        if (data["id"] != call_id) {
            EVLOG(debug) << fmt::format("RES: data_id != call_id ({} != {})", data["id"], call_id);
            return;
        }

        EVLOG(debug) << fmt::format(
            "Incoming res {} for {}->{}()", data["id"],
            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name);

        // make sure to only return the intended parts of the incoming result to not open up the api to internals
        res_promise.set_value(
            json::object({{"retval", data["retval"]}, {"origin", data["origin"]}, {"id", data["id"]}}));
    };

    Token res_token = this->mqtt_abstraction.register_handler(res_topic, res_handler, true);

    // call cmd (e.g. publish cmd via mqtt on the cmd-topic)
    std::ostringstream cmd_topic;
    cmd_topic << this->config.mqtt_prefix(connection["module_id"], connection["implementation_id"]) << "/cmd/"
              << cmd_name;
    json cmd_publish_data = json({});
    cmd_publish_data["id"] = call_id;
    cmd_publish_data["args"] = json_args;
    cmd_publish_data["origin"] = this->module_id;

    this->mqtt_abstraction.publish(cmd_topic.str(), cmd_publish_data);

    // wait for ack future
    std::chrono::system_clock::time_point ack_wait = std::chrono::system_clock::now() + this->remote_cmd_ack_timeout;
    std::future_status ack_future_status;
    do {
        ack_future_status = ack_future.wait_until(ack_wait);
    } while (ack_future_status == std::future_status::deferred);
    if (ack_future_status == std::future_status::timeout) {
        EVLOG_AND_THROW(EverestTimeoutError(fmt::format(
            "Timeout while waiting for ack of {}->{}()",
            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name)));
    } else if (ack_future_status == std::future_status::ready) {
        EVLOG(debug) << "ack future ready";
    }
    this->mqtt_abstraction.unregister_handler(ack_topic, ack_token);

    // wait for result future
    std::chrono::system_clock::time_point res_wait = std::chrono::system_clock::now() + this->remote_cmd_res_timeout;
    std::future_status res_future_status;
    do {
        res_future_status = res_future.wait_until(res_wait);
    } while (res_future_status == std::future_status::deferred);

    json result;
    if (res_future_status == std::future_status::timeout) {
        EVLOG_AND_THROW(EverestTimeoutError(fmt::format(
            "Timeout while waiting for result of {}->{}()",
            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name)));
    } else if (res_future_status == std::future_status::ready) {
        EVLOG(debug) << "res future ready";
        result = res_future.get();
    }
    this->mqtt_abstraction.unregister_handler(res_topic, res_token);

    return result;
}

Result Everest::call_cmd(const Requirement& req, const std::string& cmd_name, Parameters args) {
    BOOST_LOG_FUNCTION();
    json result = this->call_cmd(req, cmd_name, convertTo<json>(args));
    return convertTo<Result>(result["retval"]); // FIXME: other datatype so we can return the data["origin"] as well
}

void Everest::publish_var(const std::string& impl_id, const std::string& var_name, json json_value) {
    BOOST_LOG_FUNCTION();

    // check arguments
    if (this->validate_data_with_schema) {
        auto impl_intf = this->module_classes[impl_id];

        if (!module_manifest["provides"].contains(impl_id)) {
            EVLOG_AND_THROW(EverestApiError(fmt::format("Implementation '{}' not declared in manifest of module '{}'!",
                                                        impl_id, this->config.get_main_config())));
        }

        if (!impl_intf["vars"].contains(var_name)) {
            EVLOG_AND_THROW(
                EverestApiError(fmt::format("{} does not declare var '{}' in manifest!",
                                            this->config.printable_identifier(this->module_id, impl_id), var_name)));
        }

        // validate var contents before publishing
        auto var_definition = impl_intf["vars"][var_name];
        json_validator validator(Config::loader, Config::format_checker);
        try {
            validator.set_root_schema(var_definition);
            validator.validate(json_value);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Publish var of {} with variable name '{}' with value: {}\ncould not be validated with schema: {}",
                this->config.printable_identifier(this->module_id, impl_id), var_name, json_value.dump(2), e.what())));
        }
    }

    std::ostringstream topic;
    topic << this->config.mqtt_prefix(this->module_id, impl_id) << "/var/" << var_name;
    this->mqtt_abstraction.publish(topic.str(), json_value);
}

void Everest::publish_var(const std::string& impl_id, const std::string& var_name, Value value) {
    BOOST_LOG_FUNCTION();
    return this->publish_var(impl_id, var_name, convertTo<json>(value));
}

void Everest::subscribe_var(const Requirement& req, const std::string& var_name, const JsonCallback& callback) {
    BOOST_LOG_FUNCTION();

    EVLOG(debug) << fmt::format("subscribing to var: {}:{}", req.id, var_name);

    // resolve requirement
    json connections = this->config.resolve_requirement(this->module_id, req.id);
    auto& connection = connections; // this is for a min/max == 1 requirement
    if (connections.is_array()) {   // this is for every other requirement
        connection = connections[req.index];
    }

    auto requirement_module_id = connection["module_id"].get<std::string>();
    auto module_name = this->config.get_main_config()[requirement_module_id]["module"].get<std::string>();
    auto requirement_impl_id = connection["implementation_id"].get<std::string>();
    auto requirement_impl_manifest = this->config.get_interfaces()[module_name][requirement_impl_id];

    if (!requirement_impl_manifest["vars"].contains(var_name)) {
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("{}->{}: Variable not defined in manifest!",
                        this->config.printable_identifier(requirement_module_id, requirement_impl_id), var_name)));
    }

    auto requirement_manifest_vardef = requirement_impl_manifest["vars"][var_name];

    Handler handler = [this, requirement_module_id, requirement_impl_id, requirement_manifest_vardef, var_name,
                       callback](json const& data) {
        EVLOG(debug) << fmt::format(
            "Incoming {}->{}", this->config.printable_identifier(requirement_module_id, requirement_impl_id), var_name);

        // check data and ignore it if not matching (publishing it should have been prohibited already)
        try {
            json_validator validator(Config::loader, Config::format_checker);
            validator.set_root_schema(requirement_manifest_vardef);
            validator.validate(data);
        } catch (const std::exception& e) {
            EVLOG(warning) << fmt::format("Ignoring incoming var '{}' because not matching manifest schema: {}",
                                          var_name, e.what());
            return;
        }

        callback(data);
    };

    std::ostringstream topic;
    topic << this->config.mqtt_prefix(requirement_module_id, requirement_impl_id) << "/var/" << var_name;
    EVLOG(debug) << fmt::format("Registering mqtt var handler for '{}'...", topic.str());

    // TODO(kai): multiple subscription should be perfectly fine here!
    Token token = this->mqtt_abstraction.register_handler(topic.str(), handler, true);
}

void Everest::subscribe_var(const Requirement& req, const std::string& var_name, const ValueCallback& callback) {
    BOOST_LOG_FUNCTION();
    return this->subscribe_var(req, var_name, [callback](json data) { callback(convertTo<Value>(data)); });
}

void Everest::external_mqtt_publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();

    // check if external mqtt is enabled
    if (!this->module_manifest.contains("enable_external_mqtt") &&
        this->module_manifest["enable_external_mqtt"] == false) {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Module {} tries to subscribe to an external MQTT topic, but "
                                                    "didn't set 'enable_external_mqtt' to 'true' in its manifest",
                                                    this->config.printable_identifier(this->module_id))));
    }

    this->mqtt_abstraction.publish(topic, data);
}

void Everest::provide_external_mqtt_handler(const std::string& topic, const StringHandler& handler) {
    BOOST_LOG_FUNCTION();

    if (this->registered_external_mqtt_handlers.count(topic) != 0) {
        EVLOG_AND_THROW(
            EverestApiError(fmt::format("{}->external_mqtt_handler<{}>: External MQTT Handler for this topic already "
                                        "registered (you can not register an external MQTT handler twice)!",
                                        this->config.printable_identifier(this->module_id), topic)));
    }

    // check if external mqtt is enabled
    if (!this->module_manifest.contains("enable_external_mqtt") &&
        this->module_manifest["enable_external_mqtt"] == false) {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Module {} tries to provide an external MQTT handler, but didn't "
                                                    "set 'enable_external_mqtt' to 'true' in its manifest",
                                                    this->config.printable_identifier(this->module_id))));
    }

    Handler external_handler = [this, handler, topic](json const& data) {
        EVLOG(debug) << fmt::format("Incoming external mqtt data for topic '{}'...", topic);
        if (!data.is_string()) {
            EVLOG_AND_THROW(EverestInternalError("External mqtt result is not a string (that should never happen)"));
        }
        handler(data.get<std::string>());
    };

    Token token = this->mqtt_abstraction.register_handler(topic, external_handler, true);
}

void Everest::signal_ready() {
    BOOST_LOG_FUNCTION();

    EVLOG(info) << "Sending out module ready signal...";
    std::ostringstream oss;
    oss << this->config.mqtt_module_prefix(this->module_id) << "/ready";

    this->mqtt_abstraction.publish(oss.str(), json(true));
}

///
/// \brief Ready handler for global readyness (e.g. all modules are ready now).
/// This will called when receiving the global ready signal from manager.
///
void Everest::handle_ready(json data) {
    BOOST_LOG_FUNCTION();

    EVLOG(debug) << fmt::format("handle_ready: {}", data);

    bool ready = false;

    if (data.is_boolean()) {
        ready = data.get<bool>();
    }

    // ignore non-truish ready signals
    if (!ready) {
        return;
    }

    if (this->ready_received) {
        EVLOG(warning) << "Ignoring repeated everest ready signal (possibly triggered by "
                          "restarting a standalone module)!";
        return;
    }
    this->ready_received = true;

    // call module ready handler
    EVLOG(info) << "Framework now ready to process events, calling module ready handler";
    if (this->on_ready != nullptr) {
        auto on_ready_handler = *on_ready;
        on_ready_handler();
    }

    this->heartbeat_thread = std::thread(&Everest::heartbeat, this);
}

void Everest::provide_cmd(const std::string impl_id, const std::string cmd_name, const JsonCommand handler) {
    BOOST_LOG_FUNCTION();

    // extract manifest definition of this command
    json cmd_definition = get_cmd_definition(this->module_id, impl_id, cmd_name, false);

    if (this->registered_cmds.count(impl_id) != 0 && this->registered_cmds[impl_id].count(cmd_name) != 0) {
        EVLOG_AND_THROW(EverestApiError(fmt::format(
            "{}->{}(...): Handler for this cmd already registered (you can not register a cmd handler twice)!",
            this->config.printable_identifier(this->module_id, impl_id), cmd_name)));
    }

    // define command wrapper
    Handler wrapper = [this, impl_id, cmd_name, handler, cmd_definition](json data) {
        BOOST_LOG_FUNCTION();

        std::set<std::string> arg_names;
        if (cmd_definition.contains("arguments")) {
            arg_names = Config::keys(cmd_definition["arguments"]);
        }

        EVLOG(debug) << fmt::format("Incoming {}->{}({}) for <handler>",
                                    this->config.printable_identifier(this->module_id, impl_id), cmd_name,
                                    fmt::join(arg_names, ","));

        // check data and ignore it if not matching (publishing it should have
        // been prohibited already)
        if (this->validate_data_with_schema) {
            try {
                json_validator validator(Config::loader, Config::format_checker);

                for (auto const& arg_name : arg_names) {
                    if (!data["args"].contains(arg_name)) {
                        EVLOG_AND_THROW(std::invalid_argument(
                            fmt::format("Missing argument {} for {}!", arg_name,
                                        this->config.printable_identifier(this->module_id, impl_id))));
                    }
                    validator.set_root_schema(cmd_definition["arguments"][arg_name]);
                    validator.validate(data["args"][arg_name]);
                }
            } catch (const std::exception& e) {
                EVLOG(warning) << fmt::format("Ignoring incoming cmd '{}' because not matching manifest schema: {}",
                                              cmd_name, e.what());
                return;
            }
        }

        // send back cmd ack: this will acknowledge that the command and
        // arguments were received and validated successfully
        std::ostringstream ack_topic;
        ack_topic << this->config.mqtt_prefix(this->module_id, impl_id) << "/ack/" << cmd_name;
        json ack_publish_data = json({});
        ack_publish_data["id"] = data["id"];
        ack_publish_data["origin"] = this->module_id;
        this->mqtt_abstraction.publish(ack_topic.str(), ack_publish_data);

        // publish results
        std::ostringstream res_topic;
        res_topic << this->config.mqtt_prefix(this->module_id, impl_id) << "/res/" << cmd_name;
        json res_publish_data = json({});
        res_publish_data["id"] = data["id"];

        // call real cmd handler
        res_publish_data["retval"] = handler(data["args"]);

        // check retval agains manifest
        if (this->validate_data_with_schema) {
            try {
                // only use validator on non-null return types
                if (!(res_publish_data["retval"].is_null() &&
                      (!cmd_definition.contains("result") || cmd_definition["result"].is_null()))) {
                    json_validator validator(Config::loader, Config::format_checker);
                    validator.set_root_schema(cmd_definition["result"]);
                    validator.validate(res_publish_data["retval"]);
                }

            } catch (const std::exception& e) {
                EVLOG(warning) << fmt::format("Ignoring return value of cmd '{}' because the validation of the result "
                                              "failed: {}\ndefinition: {}\ndata: {}",
                                              cmd_name, e.what(), cmd_definition, res_publish_data);
                return;
            }
        }

        EVLOG(debug) << fmt::format("RETVAL: {}", res_publish_data["retval"]);
        res_publish_data["origin"] = this->module_id;
        this->mqtt_abstraction.publish(res_topic.str(), res_publish_data);
    };

    std::ostringstream cmd_topic;
    cmd_topic << this->config.mqtt_prefix(this->module_id, impl_id) << "/cmd/" << cmd_name;
    std::string topic_name = cmd_topic.str();
    EVLOG(debug) << fmt::format("Registering mqtt cmd handler for '{}'...", topic_name);
    registered_handlers.push_back(this->mqtt_abstraction.register_handler(topic_name, wrapper));

    // this list of registered cmds will be used later on to check if all cmds
    // defined in manifest are provided by code
    this->registered_cmds[impl_id].insert(cmd_name);
}

void Everest::provide_cmd(const cmd& cmd) {
    BOOST_LOG_FUNCTION();

    auto impl_id = cmd.impl_id;
    auto cmd_name = cmd.cmd_name;
    auto handler = cmd.cmd;
    auto arg_types = cmd.arg_types;
    auto return_type = cmd.return_type;

    // extract manifest definition of this command
    json cmd_definition = get_cmd_definition(this->module_id, impl_id, cmd_name, false);

    std::set<std::string> arg_names;
    for (auto& arg_type : arg_types) {
        arg_names.insert(arg_type.first);
    }

    // check arguments of handler against manifest
    if (cmd_definition["arguments"].size() != arg_types.size()) {
        EVLOG_AND_THROW(EverestApiError(fmt::format(
            "{}->{}({}): Argument count of cmd handler does not match manifest!",
            this->config.printable_identifier(this->module_id, impl_id), cmd_name, fmt::join(arg_names, ","))));
    }

    std::set<std::string> unknown_arguments;
    std::set<std::string> cmd_arguments;
    if (cmd_definition.contains("arguments")) {
        cmd_arguments = Config::keys(cmd_definition["arguments"]);
    }

    std::set_difference(arg_names.begin(), arg_names.end(), cmd_arguments.begin(), cmd_arguments.end(),
                        std::inserter(unknown_arguments, unknown_arguments.end()));

    if (!unknown_arguments.empty()) {
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("{}->{}({}): Argument names of cmd handler do not match manifest: {} != {}!",
                        this->config.printable_identifier(this->module_id, impl_id), cmd_name,
                        fmt::join(arg_names, ","), fmt::join(arg_names, ","), fmt::join(cmd_arguments, ","))));
    }

    std::string arg_name = check_args(arg_types, cmd_definition["arguments"]);

    if (!arg_name.empty()) {
        EVLOG_AND_THROW(EverestApiError(fmt::format(
            "{}->{}({}): Cmd handler argument type '{}' for '{}' does not match manifest type '{}'!",
            this->config.printable_identifier(this->module_id, impl_id), cmd_name, fmt::join(arg_names, ","),
            fmt::join(arg_types[arg_name], ","), arg_name, cmd_definition["arguments"][arg_name]["type"])));
    }

    // validate return value annotations
    if (!check_arg(return_type, cmd_definition["result"])) {
        // FIXME (aw): this gives more output EVLOG(error) << oss.str(); than the EVTHROW, why?
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("{}->{}({}): Cmd handler return type '{}' does not match manifest type '{}'!",
                        this->config.printable_identifier(this->module_id, impl_id), cmd_name,
                        fmt::join(arg_names, ","), fmt::join(return_type, ","), cmd_definition["result"])));
    }

    return this->provide_cmd(impl_id, cmd_name, [handler](json data) {
        // call cmd handlers (handle async or normal handlers being both:
        // methods or functions)
        return convertTo<json>(handler(convertTo<Parameters>(data)));
    });
}

void Everest::internal_publish(const std::string& topic, const json& json) {
    BOOST_LOG_FUNCTION();

    this->mqtt_abstraction.publish(topic, json);
}

json Everest::get_cmd_definition(const std::string& module_id, const std::string& impl_id, const std::string& cmd_name,
                                 bool is_call) {
    BOOST_LOG_FUNCTION();

    std::string module_name = this->config.get_main_config()[module_id]["module"].get<std::string>();
    auto module_manifest = this->config.get_manifests()[module_name];
    auto module = this->config.get_interfaces()[module_name];
    auto impl_intf = module[impl_id];

    if (!module_manifest["provides"].contains(impl_id)) {
        if (!is_call) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Module {} tries to provide implementation '{}' not declared in manifest!", module_name, impl_id)));
        } else {
            EVLOG_AND_THROW(EverestApiError(
                fmt::format("{} tries to call command '{}' of implementation '{}' not declared in manifest of {}",
                            this->config.printable_identifier(module_id), cmd_name, impl_id, module_name)));
        }
    }

    if (!impl_intf["cmds"].contains(cmd_name)) {
        if (!is_call) {
            EVLOG_AND_THROW(
                EverestApiError(fmt::format("{} tries to provide cmd '{}' not declared in manifest!",
                                            this->config.printable_identifier(module_id, impl_id), cmd_name)));
        } else {
            EVLOG_AND_THROW(EverestApiError(fmt::format("{} tries to call cmd '{}' not declared in manifest of {}!",
                                                        this->config.printable_identifier(module_id), cmd_name,
                                                        this->config.printable_identifier(module_id, impl_id))));
        }
    }

    return impl_intf["cmds"][cmd_name];
}

json Everest::get_cmd_definition(const std::string& module_id, const std::string& impl_id,
                                 const std::string& cmd_name) {
    BOOST_LOG_FUNCTION();

    return get_cmd_definition(module_id, impl_id, cmd_name, false);
}

std::string Everest::check_args(const Arguments& func_args, json manifest_args) {
    BOOST_LOG_FUNCTION();

    for (auto const& func_arg : func_args) {
        auto arg_name = func_arg.first;
        auto arg_types = func_arg.second;
        std::ostringstream oss;

        if (!check_arg(arg_types, manifest_args[arg_name])) {
            return arg_name;
        }
    }

    return std::string();
}

bool Everest::check_arg(ArgumentType arg_types, json manifest_arg) {
    BOOST_LOG_FUNCTION();

    // FIXME (aw): the error messages here need to be taken into the
    //             correct context!

    if (manifest_arg["type"].is_string()) {
        if (manifest_arg["type"] == "null") {
            // arg_types should be empty if the type is null (void)
            if (arg_types.size()) {
                EVLOG(error) << "expeceted 'null' type, but got another type";
                return false;
            }
            return true;
        }
        // direct comparison
        // FIXME (aw): arg_types[0] access should be checked, otherwise core dumps
        if (arg_types[0] != manifest_arg["type"]) {
            EVLOG(error) << fmt::format("types do not match: {} != {}", arg_types[0], manifest_arg["type"]);
            return false;
        }
        return true;
    }

    for (size_t i = 0; i < arg_types.size(); i++) {
        if (arg_types[i] != manifest_arg["type"][i]) {
            EVLOG(error) << fmt::format("types do not match: {} != {}", arg_types[i], manifest_arg["type"][i]);
            return false;
        }
    }
    return true;
}
} // namespace Everest
