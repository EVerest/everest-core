// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <cstddef>
#include <future>
#include <map>
#include <memory>
#include <set>

#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <everest/logging.hpp>
#include <fmt/format.h>

#include <date/date.h>
#include <date/tz.h>
#include <framework/everest.hpp>
#include <utils/conversions.hpp>
#include <utils/date.hpp>
#include <utils/error.hpp>
#include <utils/error/error_database.hpp>
#include <utils/error/error_database_map.hpp>
#include <utils/error/error_factory.hpp>
#include <utils/error/error_json.hpp>
#include <utils/error/error_manager_impl.hpp>
#include <utils/error/error_manager_req.hpp>
#include <utils/error/error_manager_req_global.hpp>
#include <utils/error/error_state_monitor.hpp>
#include <utils/error/error_type_map.hpp>
#include <utils/formatter.hpp>

namespace Everest {
using json = nlohmann::json;
using json_uri = nlohmann::json_uri;
using json_validator = nlohmann::json_schema::json_validator;

const auto remote_cmd_res_timeout_seconds = 300;
const std::array<std::string, 3> TELEMETRY_RESERVED_KEYS = {{"connector_id"}};

Everest::Everest(std::string module_id_, const Config& config_, bool validate_data_with_schema,
                 std::shared_ptr<MQTTAbstraction> mqtt_abstraction, const std::string& telemetry_prefix,
                 bool telemetry_enabled) :
    mqtt_abstraction(mqtt_abstraction),
    config(config_),
    module_id(std::move(module_id_)),
    remote_cmd_res_timeout(remote_cmd_res_timeout_seconds),
    validate_data_with_schema(validate_data_with_schema),
    mqtt_everest_prefix(mqtt_abstraction->get_everest_prefix()),
    mqtt_external_prefix(mqtt_abstraction->get_external_prefix()),
    telemetry_prefix(telemetry_prefix),
    telemetry_enabled(telemetry_enabled) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Initializing EVerest framework...";

    const auto& main_config = this->config.get_main_config();
    const auto module_config_it = main_config.find(this->module_id);
    if (module_config_it == main_config.end()) {
        EVLOG_AND_THROW(EverestBaseRuntimeError("Module id '" + module_id + "' not found in config"));
    }

    this->module_name = module_config_it->at("module");
    this->module_manifest = this->config.get_manifests()[this->module_name];
    this->module_classes = this->config.get_interfaces()[this->module_name];
    this->telemetry_config = this->config.get_telemetry_config();

    this->ready_received = false;
    this->on_ready = nullptr;

    // setup error_manager_req_global if enabled + error_database + error_state_monitor
    if (this->module_manifest.contains("enable_global_errors") &&
        this->module_manifest.at("enable_global_errors").get<bool>()) {
        auto global_error_database = std::make_shared<error::ErrorDatabaseMap>();
        const error::ErrorManagerReqGlobal::SubscribeGlobalAllErrorsFunc subscribe_global_all_errors_func =
            [this](const error::ErrorCallback& callback, const error::ErrorCallback& clear_callback) {
                this->subscribe_global_all_errors(callback, clear_callback);
            };
        this->global_error_manager = std::make_shared<error::ErrorManagerReqGlobal>(
            std::make_shared<error::ErrorTypeMap>(this->config.get_error_map()), global_error_database,
            subscribe_global_all_errors_func);
        this->global_error_state_monitor = std::make_shared<error::ErrorStateMonitor>(global_error_database);
    } else {
        this->global_error_manager = nullptr;
        this->global_error_state_monitor = nullptr;
    }

    this->module_tier_mappings = config.get_module_3_tier_model_mappings(this->module_id);

    // setup error_managers, error_state_monitors, error_factories and error_databases for all implementations
    for (const std::string& impl : Config::keys(this->module_manifest.at("provides"))) {
        // setup shared database
        auto error_database = std::make_shared<error::ErrorDatabaseMap>();

        // setup error manager
        const std::string interface_name = this->module_manifest.at("provides").at(impl).at("interface");
        json interface_def = this->config.get_interface_definition(interface_name);
        std::list<std::string> allowed_error_types;
        for (const auto& error_namespace_it : interface_def["errors"].items()) {
            for (const auto& error_name_it : error_namespace_it.value().items()) {
                allowed_error_types.push_back(error_namespace_it.key() + "/" + error_name_it.key());
            }
        }
        const error::ErrorManagerImpl::PublishErrorFunc publish_raised_error = [this, impl](const error::Error& error) {
            this->publish_raised_error(impl, error);
        };
        const error::ErrorManagerImpl::PublishErrorFunc publish_cleared_error =
            [this, impl](const error::Error& error) { this->publish_cleared_error(impl, error); };
        this->impl_error_managers[impl] = std::make_shared<error::ErrorManagerImpl>(
            std::make_shared<error::ErrorTypeMap>(this->config.get_error_map()), error_database, allowed_error_types,
            publish_raised_error, publish_cleared_error);

        // setup error state monitor
        this->impl_error_state_monitors[impl] = std::make_shared<error::ErrorStateMonitor>(error_database);

        std::optional<Mapping> mapping;
        if (this->module_tier_mappings.has_value()) {
            const auto& module_tier_mapping = this->module_tier_mappings.value();
            // start with the module mapping and overwrite it (partially) with the implementation mapping
            mapping = module_tier_mapping.module;
            const auto impl_mapping = config.get_3_tier_model_mapping(this->module_id, impl);
            if (impl_mapping.has_value()) {
                if (mapping.has_value()) {
                    auto& mapping_value = mapping.value();
                    const auto& impl_mapping_value = impl_mapping.value();
                    if (mapping_value.evse != impl_mapping_value.evse) {
                        EVLOG_warning << fmt::format("Mapping value mismatch. {} ({}) evse ({}) != {} mapping evse "
                                                     "({}). Setting evse={}, please fix this in the config.",
                                                     this->module_id, this->module_name, mapping_value.evse, impl,
                                                     impl_mapping_value.evse, impl_mapping_value.evse);
                        mapping_value.evse = impl_mapping_value.evse;
                    }

                    if (not mapping_value.connector.has_value() and impl_mapping_value.connector.has_value()) {
                        mapping_value.connector = impl_mapping_value.connector;
                    }
                    if (mapping_value.connector.has_value() and impl_mapping_value.connector.has_value()) {
                        const auto& mapping_value_connector_value = mapping_value.connector.value();
                        const auto& impl_mapping_value_connector_value = impl_mapping_value.connector.value();
                        if (mapping_value_connector_value != impl_mapping_value_connector_value) {
                            EVLOG_warning
                                << fmt::format("Mapping value mismatch. {} ({}) connector ({}) != {} mapping connector "
                                               "({}). Setting connector={}, please fix this in the config.",
                                               this->module_id, this->module_name, mapping_value_connector_value, impl,
                                               impl_mapping_value_connector_value, impl_mapping_value_connector_value);
                        }
                        mapping_value.connector = impl_mapping_value_connector_value;
                    }

                } else {
                    EVLOG_info << "No module mapping, so using impl mapping here";
                    mapping = impl_mapping;
                }
            }
        }

        // setup error factory
        const ImplementationIdentifier default_origin(this->module_id, impl, mapping);
        this->error_factories[impl] = std::make_shared<error::ErrorFactory>(
            std::make_shared<error::ErrorTypeMap>(this->config.get_error_map()), default_origin);
    }

    // setup error_databases, error_managers and error_state_monitors for all requirements
    for (const Requirement& req : config.get_requirements(module_id)) {
        // setup shared database
        const std::shared_ptr<error::ErrorDatabaseMap> error_database = std::make_shared<error::ErrorDatabaseMap>();

        // setup error manager
        const std::string interface_name = this->module_manifest.at("requires").at(req.id).at("interface");
        const json interface_def = this->config.get_interface_definition(interface_name);
        std::list<std::string> allowed_error_types;
        for (const auto& error_namespace_it : interface_def.at("errors").items()) {
            for (const auto& error_name_it : error_namespace_it.value().items()) {
                allowed_error_types.push_back(error_namespace_it.key() + "/" + error_name_it.key());
            }
        }
        const error::ErrorManagerReq::SubscribeErrorFunc subscribe_error_func =
            [this, req](const error::ErrorType& type, const error::ErrorCallback& callback,
                        const error::ErrorCallback& clear_callback) {
                this->subscribe_error(req, type, callback, clear_callback);
            };
        this->req_error_managers[req] = std::make_shared<error::ErrorManagerReq>(
            std::make_shared<error::ErrorTypeMap>(this->config.get_error_map()), error_database, allowed_error_types,
            subscribe_error_func);

        // setup error state monitor
        this->req_error_state_monitors[req] = std::make_shared<error::ErrorStateMonitor>(error_database);
    }

    // register handler for global ready signal
    const auto handle_ready_wrapper = [this](const std::string&, const json& data) { this->handle_ready(data); };
    const auto everest_ready =
        std::make_shared<TypedHandler>(HandlerType::ExternalMQTT, std::make_shared<Handler>(handle_ready_wrapper));
    this->mqtt_abstraction->register_handler(fmt::format("{}ready", mqtt_everest_prefix), everest_ready, QOS::QOS2);

    this->publish_metadata();
}

void Everest::spawn_main_loop_thread() {
    BOOST_LOG_FUNCTION();
    // TODO: since the MQTT main loop has already been started before constructing this object, this is a no-op now

    this->main_loop_end = this->mqtt_abstraction->get_main_loop_future();
}

void Everest::wait_for_main_loop_end() {
    BOOST_LOG_FUNCTION();

    // FIXME (aw): check if mainloop has been started, simple assert for now
    assert(this->main_loop_end.valid());

    this->main_loop_end.get();
}

void Everest::heartbeat() {
    BOOST_LOG_FUNCTION();
    const auto heartbeat_topic = fmt::format("{}/heartbeat", this->config.mqtt_module_prefix(this->module_id));

    using namespace date;

    while (this->ready_received) {
        std::ostringstream now;
        now << date::utc_clock::now();
        this->mqtt_abstraction->publish(heartbeat_topic, json(now.str()), QOS::QOS0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Everest::publish_metadata() {
    BOOST_LOG_FUNCTION();

    const auto module_info = this->config.get_module_info(this->module_id);
    const auto manifest = this->config.get_manifests().at(module_info.name);

    json metadata = json({});
    metadata["module"] = module_info.name;
    if (manifest.contains("provides")) {
        metadata["provides"] = json({});

        for (const auto& provides : manifest.at("provides").items()) {
            metadata["provides"][provides.key()] = json({});
            metadata["provides"][provides.key()]["interface"] = provides.value().at("interface");
        }
    }

    const auto metadata_topic = fmt::format("{}/metadata", this->config.mqtt_module_prefix(this->module_id));

    this->mqtt_abstraction->publish(metadata_topic, metadata, QOS::QOS2);
}

void Everest::register_on_ready_handler(const std::function<void()>& handler) {
    BOOST_LOG_FUNCTION();

    this->on_ready = std::make_unique<std::function<void()>>(handler);
}

std::optional<ModuleTierMappings> Everest::get_3_tier_model_mapping() {
    return this->module_tier_mappings;
}

void Everest::check_code() {
    BOOST_LOG_FUNCTION();

    const json module_manifest =
        this->config.get_manifests()[this->config.get_main_config()[this->module_id]["module"].get<std::string>()];
    for (const auto& element : module_manifest.at("provides").items()) {
        const auto& impl_id = element.key();
        const auto& impl_manifest = element.value();
        const auto interface_definition = this->config.get_interface_definition(impl_manifest.at("interface"));

        std::set<std::string> cmds_not_registered;
        std::set<std::string> impl_manifest_cmds_set;
        if (interface_definition.contains("cmds")) {
            impl_manifest_cmds_set = Config::keys(interface_definition.at("cmds"));
        }
        const std::set<std::string> registered_cmds_set = this->registered_cmds[impl_id];

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

    return this->mqtt_abstraction->connect();
}

void Everest::disconnect() {
    BOOST_LOG_FUNCTION();

    this->mqtt_abstraction->disconnect();
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
    const json cmd_definition =
        get_cmd_definition(connection.at("module_id"), connection.at("implementation_id"), cmd_name, true);

    const json return_type = cmd_definition.at("result").at("type");

    std::set<std::string> arg_names = Config::keys(json_args);

    // check args against manifest
    if (this->validate_data_with_schema) {
        if (cmd_definition.at("arguments").size() != json_args.size()) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Call to {}->{}({}): Argument count does not match manifest!",
                this->config.printable_identifier(connection.at("module_id"), connection.at("implementation_id")),
                cmd_name, fmt::join(arg_names, ", "))));
        }

        std::set<std::string> unknown_arguments;
        std::set<std::string> cmd_arguments;
        if (cmd_definition.contains("arguments")) {
            cmd_arguments = Config::keys(cmd_definition.at("arguments"));
        }

        std::set_difference(arg_names.begin(), arg_names.end(), cmd_arguments.begin(), cmd_arguments.end(),
                            std::inserter(unknown_arguments, unknown_arguments.end()));

        if (!unknown_arguments.empty()) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Call to {}->{}({}): Argument names do not match manifest: {} != {}!",
                this->config.printable_identifier(connection.at("module_id"), connection.at("implementation_id")),
                cmd_name, fmt::join(arg_names, ","), fmt::join(arg_names, ","), fmt::join(cmd_arguments, ","))));
        }
    }

    if (this->validate_data_with_schema) {
        for (const auto& arg_name : arg_names) {
            try {
                json_validator validator(
                    [this](const json_uri& uri, json& schema) { this->config.ref_loader(uri, schema); },
                    format_checker);
                validator.set_root_schema(cmd_definition.at("arguments").at(arg_name));
                validator.validate(json_args.at(arg_name));
            } catch (const std::exception& e) {
                EVLOG_AND_THROW(EverestApiError(fmt::format(
                    "Call to {}->{}({}): Argument '{}' with value '{}' could not be validated with schema: {}",
                    this->config.printable_identifier(connection.at("module_id"), connection.at("implementation_id")),
                    cmd_name, fmt::join(arg_names, ","), arg_name, json_args.at(arg_name).dump(2), e.what())));
            }
        }
    }

    const std::string call_id = boost::uuids::to_string(boost::uuids::random_generator()());

    std::promise<json> res_promise;
    std::future<json> res_future = res_promise.get_future();

    const auto res_handler = [this, &res_promise, call_id, connection, cmd_name, return_type](const std::string&,
                                                                                              json data) {
        const auto& data_id = data.at("id");
        if (data_id != call_id) {
            EVLOG_debug << fmt::format("RES: data_id != call_id ({} != {})", data_id, call_id);
            return;
        }

        EVLOG_verbose << fmt::format(
            "Incoming res {} for {}->{}()", data_id,
            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name);

        res_promise.set_value(std::move(data["retval"]));
    };

    const auto cmd_topic =
        fmt::format("{}/cmd", this->config.mqtt_prefix(connection["module_id"], connection["implementation_id"]));

    const std::shared_ptr<TypedHandler> res_token =
        std::make_shared<TypedHandler>(cmd_name, call_id, HandlerType::Result, std::make_shared<Handler>(res_handler));
    this->mqtt_abstraction->register_handler(cmd_topic, res_token, QOS::QOS2);

    const json cmd_publish_data =
        json::object({{"name", cmd_name},
                      {"type", "call"},
                      {"data", json::object({{"id", call_id}, {"args", json_args}, {"origin", this->module_id}})}});

    this->mqtt_abstraction->publish(cmd_topic, cmd_publish_data, QOS::QOS2);

    // wait for result future
    const std::chrono::time_point<std::chrono::steady_clock> res_wait =
        std::chrono::steady_clock::now() + this->remote_cmd_res_timeout;
    std::future_status res_future_status;
    do {
        res_future_status = res_future.wait_until(res_wait);
    } while (res_future_status == std::future_status::deferred);

    json result;
    if (res_future_status == std::future_status::timeout) {
        EVLOG_AND_THROW(EverestTimeoutError(fmt::format(
            "Timeout while waiting for result of {}->{}()",
            this->config.printable_identifier(connection["module_id"], connection["implementation_id"]), cmd_name)));
    }
    if (res_future_status == std::future_status::ready) {
        result = res_future.get();
    }
    this->mqtt_abstraction->unregister_handler(cmd_topic, res_token);

    return result;
}

void Everest::publish_var(const std::string& impl_id, const std::string& var_name, json value) {
    BOOST_LOG_FUNCTION();

    // check arguments
    if (this->validate_data_with_schema) {
        const auto impl_intf = this->config.get_interface_definitions().at(this->module_classes.at(impl_id));

        if (!module_manifest.at("provides").contains(impl_id)) {
            EVLOG_AND_THROW(EverestApiError(
                fmt::format("Implementation '{}' not declared in manifest of module '{}'!", impl_id, this->module_id)));
        }

        if (!impl_intf.at("vars").contains(var_name)) {
            EVLOG_AND_THROW(
                EverestApiError(fmt::format("{} does not declare var '{}' in manifest!",
                                            this->config.printable_identifier(this->module_id, impl_id), var_name)));
        }

        // validate var contents before publishing
        const auto var_definition = impl_intf.at("vars").at(var_name);
        try {
            json_validator validator(
                [this](const json_uri& uri, json& schema) { this->config.ref_loader(uri, schema); }, format_checker);
            validator.set_root_schema(var_definition);
            validator.validate(value);
        } catch (const std::exception& e) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Publish var of {} with variable name '{}' with value: {}\ncould not be validated with schema: {}",
                this->config.printable_identifier(this->module_id, impl_id), var_name, value.dump(2), e.what())));
        }
    }

    const auto var_topic = fmt::format("{}/var", this->config.mqtt_prefix(this->module_id, impl_id));

    const json var_publish_data = {{"name", var_name}, {"data", value}};

    // FIXME(kai): implement an efficient way of choosing qos for each variable
    this->mqtt_abstraction->publish(var_topic, var_publish_data, QOS::QOS2);
}

void Everest::subscribe_var(const Requirement& req, const std::string& var_name, const JsonCallback& callback) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("subscribing to var: {}:{}", req.id, var_name);

    // resolve requirement
    json connections = this->config.resolve_requirement(this->module_id, req.id);
    auto& connection = connections; // this is for a min/max == 1 requirement
    if (connections.is_array()) {   // this is for every other requirement
        connection = connections[req.index];
    }

    const auto requirement_module_id = connection.at("module_id").get<std::string>();
    const auto module_name = this->config.get_module_name(requirement_module_id);
    const auto requirement_impl_id = connection.at("implementation_id").get<std::string>();
    const auto requirement_impl_manifest = this->config.get_interface_definitions().at(
        this->config.get_interfaces().at(module_name).at(requirement_impl_id));

    if (!requirement_impl_manifest.at("vars").contains(var_name)) {
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("{}->{}: Variable not defined in manifest!",
                        this->config.printable_identifier(requirement_module_id, requirement_impl_id), var_name)));
    }

    const auto requirement_manifest_vardef = requirement_impl_manifest.at("vars").at(var_name);

    const auto handler = [this, requirement_module_id, requirement_impl_id, requirement_manifest_vardef, var_name,
                          callback](const std::string&, json const& data) {
        EVLOG_verbose << fmt::format(
            "Incoming {}->{}", this->config.printable_identifier(requirement_module_id, requirement_impl_id), var_name);

        if (this->validate_data_with_schema) {
            // check data and ignore it if not matching (publishing it should have been prohibited already)
            try {
                json_validator validator(
                    [this](const json_uri& uri, json& schema) { this->config.ref_loader(uri, schema); },
                    format_checker);
                validator.set_root_schema(requirement_manifest_vardef);
                validator.validate(data);
            } catch (const std::exception& e) {
                EVLOG_warning << fmt::format("Ignoring incoming var '{}' because not matching manifest schema: {}",
                                             var_name, e.what());
                return;
            }
        }

        callback(data);
    };

    const auto var_topic = fmt::format("{}/var", this->config.mqtt_prefix(requirement_module_id, requirement_impl_id));

    // TODO(kai): multiple subscription should be perfectly fine here!
    const std::shared_ptr<TypedHandler> token =
        std::make_shared<TypedHandler>(var_name, HandlerType::SubscribeVar, std::make_shared<Handler>(handler));
    this->mqtt_abstraction->register_handler(var_topic, token, QOS::QOS2);
}

void Everest::subscribe_error(const Requirement& req, const error::ErrorType& error_type,
                              const error::ErrorCallback& callback, const error::ErrorCallback& clear_callback) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("subscribing to error: {}:{}", req.id, error_type);

    // resolve requirement
    json connections = this->config.resolve_requirement(this->module_id, req.id);
    json& connection = connections; // this is for a min/max == 1 requirement
    if (connections.is_array()) {   // this is for every other requirement
        connection = connections[req.index];
    }

    const std::string requirement_module_id = connection.at("module_id");
    const std::string module_name = this->config.get_module_name(requirement_module_id);
    const std::string requirement_impl_id = connection.at("implementation_id");
    const json requirement_impl_if = this->config.get_interface_definitions().at(
        this->config.get_interfaces().at(module_name).at(requirement_impl_id));

    // check if requirement is allowed to publish this error_type
    // split error_type at '/'
    const std::size_t pos = error_type.find('/');
    if (pos == std::string::npos) {
        EVLOG_error << fmt::format("Error type {} is not valid, ignore subscription", error_type);
        return;
    }
    const std::string error_type_namespace = error_type.substr(0, pos);
    const std::string error_type_name = error_type.substr(pos + 1);
    if (!requirement_impl_if.contains("errors") || !requirement_impl_if.at("errors").contains(error_type_namespace) ||
        !requirement_impl_if.at("errors").at(error_type_namespace).contains(error_type_name)) {
        EVLOG_error << fmt::format("{}: Error {} not listed in interface, ignore subscription!",
                                   this->config.printable_identifier(requirement_module_id, requirement_impl_id),
                                   error_type);
        return;
    }

    const auto raise_handler = [this, requirement_module_id, requirement_impl_id, error_type,
                                callback](const std::string&, json const& data) {
        EVLOG_debug << fmt::format("Incoming error {}->{}",
                                   this->config.printable_identifier(requirement_module_id, requirement_impl_id),
                                   error_type);

        callback(data.get<error::Error>());
    };

    const auto clear_handler = [this, requirement_module_id, requirement_impl_id, error_type,
                                clear_callback](const std::string&, json const& data) {
        EVLOG_debug << fmt::format("Error cleared {}->{}",
                                   this->config.printable_identifier(requirement_module_id, requirement_impl_id),
                                   error_type);
        clear_callback(data.get<error::Error>());
    };

    const std::string raise_topic =
        fmt::format("{}/error/{}", this->config.mqtt_prefix(requirement_module_id, requirement_impl_id), error_type);

    const std::string clear_topic = fmt::format(
        "{}/error-cleared/{}", this->config.mqtt_prefix(requirement_module_id, requirement_impl_id), error_type);

    const std::shared_ptr<TypedHandler> raise_token = std::make_shared<TypedHandler>(
        error_type, HandlerType::SubscribeError, std::make_shared<Handler>(raise_handler));
    const std::shared_ptr<TypedHandler> clear_token = std::make_shared<TypedHandler>(
        error_type, HandlerType::SubscribeError, std::make_shared<Handler>(clear_handler));

    this->mqtt_abstraction->register_handler(raise_topic, raise_token, QOS::QOS2);
    this->mqtt_abstraction->register_handler(clear_topic, clear_token, QOS::QOS2);
}

std::shared_ptr<error::ErrorManagerImpl> Everest::get_error_manager_impl(const std::string& impl_id) {
    if (this->impl_error_managers.find(impl_id) == this->impl_error_managers.end()) {
        EVLOG_error << fmt::format("Error manager for {} not found!", impl_id);
        return nullptr;
    }
    return this->impl_error_managers.at(impl_id);
}

std::shared_ptr<error::ErrorStateMonitor> Everest::get_error_state_monitor_impl(const std::string& impl_id) {
    if (this->impl_error_state_monitors.find(impl_id) == this->impl_error_state_monitors.end()) {
        EVLOG_error << fmt::format("Error state monitor for {} not found!", impl_id);
        return nullptr;
    }
    return this->impl_error_state_monitors.at(impl_id);
}

std::shared_ptr<error::ErrorFactory> Everest::get_error_factory(const std::string& impl_id) {
    if (this->error_factories.find(impl_id) == this->error_factories.end()) {
        EVLOG_error << fmt::format("Error factory for {} not found!", impl_id);
        return nullptr;
    }
    return this->error_factories.at(impl_id);
}

std::shared_ptr<error::ErrorManagerReq> Everest::get_error_manager_req(const Requirement& req) {
    if (this->req_error_managers.find(req) == this->req_error_managers.end()) {
        EVLOG_error << fmt::format("Error manager for {} not found!", req.id);
        return nullptr;
    }
    return this->req_error_managers.at(req);
}

std::shared_ptr<error::ErrorStateMonitor> Everest::get_error_state_monitor_req(const Requirement& req) {
    if (this->req_error_state_monitors.find(req) == this->req_error_state_monitors.end()) {
        EVLOG_error << fmt::format("Error state monitor for {} not found!", req.id);
        return nullptr;
    }
    return this->req_error_state_monitors.at(req);
}

std::shared_ptr<error::ErrorManagerReqGlobal> Everest::get_global_error_manager() const {
    if (this->global_error_manager == nullptr) {
        EVLOG_warning << "This module has no global_error_manager, returning nullptr";
    }
    return this->global_error_manager;
}

std::shared_ptr<error::ErrorStateMonitor> Everest::get_global_error_state_monitor() const {
    if (this->global_error_state_monitor == nullptr) {
        EVLOG_warning << "This module has no global_error_state_monitor, returning nullptr";
    }
    return this->global_error_state_monitor;
}

void Everest::subscribe_global_all_errors(const error::ErrorCallback& callback,
                                          const error::ErrorCallback& clear_callback) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("subscribing to all errors");

    if (not this->config.get_module_info(this->module_id).global_errors_enabled) {
        EVLOG_error << fmt::format("Module {} is not allowed to subscribe to all errors, ignore subscription",
                                   this->config.printable_identifier(this->module_id));
        return;
    }

    const auto raise_handler = [this, callback](const std::string&, json const& data) {
        error::Error error = data.get<error::Error>();
        EVLOG_debug << fmt::format(
            "Incoming error {}->{}",
            this->config.printable_identifier(error.origin.module_id, error.origin.implementation_id), error.type);
        callback(error);
    };

    const auto clear_handler = [this, clear_callback](const std::string&, json const& data) {
        error::Error error = data.get<error::Error>();
        EVLOG_debug << fmt::format(
            "Incoming error cleared {}->{}",
            this->config.printable_identifier(error.origin.module_id, error.origin.implementation_id), error.type);
        clear_callback(error);
    };

    for (const auto& [module_id, module_name] : this->config.get_module_names()) {
        const json provides = this->config.get_manifests().at(module_name).at("provides");
        for (const auto& impl : provides.items()) {
            const std::string& impl_id = impl.key();
            const std::string& interface = impl.value().at("interface");
            const json errors = this->config.get_interface_definition(interface).at("errors");
            for (const auto& error_namespace_it : errors.items()) {
                const std::string& error_type_namespace = error_namespace_it.key();
                for (const auto& error_name_it : error_namespace_it.value().items()) {
                    const std::string& error_type_name = error_name_it.key();
                    const std::string raise_topic =
                        fmt::format("{}/error/{}/{}", this->config.mqtt_prefix(module_id, impl_id),
                                    error_type_namespace, error_type_name);
                    const std::shared_ptr<TypedHandler> raise_token = std::make_shared<TypedHandler>(
                        HandlerType::SubscribeError, std::make_shared<Handler>(raise_handler));
                    this->mqtt_abstraction->register_handler(raise_topic, raise_token, QOS::QOS2);

                    const std::string clear_topic =
                        fmt::format("{}/error-cleared/{}/{}", this->config.mqtt_prefix(module_id, impl_id),
                                    error_type_namespace, error_type_name);
                    const std::shared_ptr<TypedHandler> clear_token = std::make_shared<TypedHandler>(
                        HandlerType::SubscribeError, std::make_shared<Handler>(clear_handler));
                    this->mqtt_abstraction->register_handler(clear_topic, clear_token, QOS::QOS2);
                }
            }
        }
    }
}

void Everest::publish_raised_error(const std::string& impl_id, const error::Error& error) {
    BOOST_LOG_FUNCTION();

    const auto error_topic = fmt::format("{}/error/{}", this->config.mqtt_prefix(this->module_id, impl_id), error.type);

    this->mqtt_abstraction->publish(error_topic, json(error), QOS::QOS2);
}

void Everest::publish_cleared_error(const std::string& impl_id, const error::Error& error) {
    BOOST_LOG_FUNCTION();

    const auto error_topic =
        fmt::format("{}/error-cleared/{}", this->config.mqtt_prefix(this->module_id, impl_id), error.type);

    this->mqtt_abstraction->publish(error_topic, json(error), QOS::QOS2);
}

void Everest::external_mqtt_publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();
    check_external_mqtt();
    this->mqtt_abstraction->publish(fmt::format("{}{}", this->mqtt_external_prefix, topic), data);
}

UnsubscribeToken Everest::provide_external_mqtt_handler(const std::string& topic, const StringHandler& handler) {
    BOOST_LOG_FUNCTION();
    const auto external_topic = check_external_mqtt(topic);
    return create_external_handler(
        topic, external_topic, [handler, external_topic](const std::string&, json const& data) {
            EVLOG_verbose << fmt::format("Incoming external mqtt data for topic '{}'...", external_topic);
            if (!data.is_string()) {
                EVLOG_AND_THROW(
                    EverestInternalError("External mqtt result is not a string (that should never happen)"));
            }
            handler(data.get<std::string>());
        });
}

UnsubscribeToken Everest::provide_external_mqtt_handler(const std::string& topic, const StringPairHandler& handler) {
    BOOST_LOG_FUNCTION();
    const auto external_topic = check_external_mqtt(topic);
    return create_external_handler(topic, external_topic, [handler](const std::string& topic, const json& data) {
        EVLOG_verbose << fmt::format("Incoming external mqtt data for topic '{}'...", topic);
        const std::string data_s = (data.is_string()) ? std::string(data) : data.dump();
        handler(topic, data_s);
    });
}

void Everest::telemetry_publish(const std::string& topic, const std::string& data) {
    BOOST_LOG_FUNCTION();

    this->mqtt_abstraction->publish(fmt::format("{}{}", this->telemetry_prefix, topic), data);
}

void Everest::telemetry_publish(const std::string& category, const std::string& subcategory, const std::string& type,
                                const TelemetryMap& telemetry) {
    BOOST_LOG_FUNCTION();

    if (!this->telemetry_enabled || !this->telemetry_config.has_value()) {
        // telemetry not enabled for this module instance in config
        return;
    }
    const int id = telemetry_config->id;
    const std::string id_string = std::to_string(id);
    auto telemetry_data =
        json::object({{"timestamp", Date::to_rfc3339(date::utc_clock::now())}, {"connector_id", id}, {"type", type}});

    for (auto&& [key, entry] : telemetry) {
        if (std::any_of(TELEMETRY_RESERVED_KEYS.begin(), TELEMETRY_RESERVED_KEYS.end(),
                        [&key_ = key](const auto& element) { return element == key_; })) {
            EVLOG_warning << "Telemetry key " << key << " is reserved and will be overwritten.";
        } else {
            json data;
            std::visit([&data](auto& value) { data = value; }, entry);
            telemetry_data[key] = data;
        }
    }
    const std::string topic = category + "/" + id_string + "/" + subcategory;
    this->telemetry_publish(topic, telemetry_data.dump());
}

void Everest::signal_ready() {
    BOOST_LOG_FUNCTION();

    const auto ready_topic = fmt::format("{}/ready", this->config.mqtt_module_prefix(this->module_id));

    this->mqtt_abstraction->publish(ready_topic, json(true), QOS::QOS2);
}

///
/// \brief Ready handler for global readyness (e.g. all modules are ready now).
/// This will called when receiving the global ready signal from manager.
///
void Everest::handle_ready(const json& data) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << fmt::format("handle_ready: {}", data.dump());

    bool ready = false;

    if (data.is_boolean()) {
        ready = data.get<bool>();
    }

    // ignore non-truish ready signals
    if (!ready) {
        return;
    }

    if (this->ready_received) {
        EVLOG_warning << "Ignoring repeated everest ready signal (possibly triggered by "
                         "restarting a standalone module)!";
        return;
    }
    this->ready_received = true;

    // call module ready handler
    EVLOG_debug << "Framework now ready to process events, calling module ready handler";
    if (this->on_ready != nullptr) {
        const auto on_ready_handler = *on_ready;
        on_ready_handler();
    }

    // TODO(kai): make heartbeat interval configurable, disable it completely until then
    // this->heartbeat_thread = std::thread(&Everest::heartbeat, this);
}

void Everest::provide_cmd(const std::string& impl_id, const std::string& cmd_name, const JsonCommand& handler) {
    BOOST_LOG_FUNCTION();

    // extract manifest definition of this command
    const json cmd_definition = get_cmd_definition(this->module_id, impl_id, cmd_name, false);

    if (this->registered_cmds.count(impl_id) != 0 && this->registered_cmds.at(impl_id).count(cmd_name) != 0) {
        EVLOG_AND_THROW(EverestApiError(fmt::format(
            "{}->{}(...): Handler for this cmd already registered (you can not register a cmd handler twice)!",
            this->config.printable_identifier(this->module_id, impl_id), cmd_name)));
    }

    const auto cmd_topic = fmt::format("{}/cmd", this->config.mqtt_prefix(this->module_id, impl_id));

    // define command wrapper
    const auto wrapper = [this, cmd_topic, impl_id, cmd_name, handler, cmd_definition](const std::string&, json data) {
        BOOST_LOG_FUNCTION();

        std::set<std::string> arg_names;
        if (cmd_definition.contains("arguments")) {
            arg_names = Config::keys(cmd_definition.at("arguments"));
        }

        EVLOG_verbose << fmt::format("Incoming {}->{}({}) for <handler>",
                                     this->config.printable_identifier(this->module_id, impl_id), cmd_name,
                                     fmt::join(arg_names, ","));

        // check data and ignore it if not matching (publishing it should have
        // been prohibited already)
        if (this->validate_data_with_schema) {
            try {
                for (const auto& arg_name : arg_names) {
                    if (!data.at("args").contains(arg_name)) {
                        EVLOG_AND_THROW(std::invalid_argument(
                            fmt::format("Missing argument {} for {}!", arg_name,
                                        this->config.printable_identifier(this->module_id, impl_id))));
                    }
                    json_validator validator(
                        [this](const json_uri& uri, json& schema) { this->config.ref_loader(uri, schema); },
                        format_checker);
                    validator.set_root_schema(cmd_definition.at("arguments").at(arg_name));
                    validator.validate(data.at("args").at(arg_name));
                }
            } catch (const std::exception& e) {
                EVLOG_warning << fmt::format("Ignoring incoming cmd '{}' because not matching manifest schema: {}",
                                             cmd_name, e.what());
                return;
            }
        }

        // publish results
        json res_data = json({});
        res_data["id"] = data.at("id");

        // call real cmd handler
        res_data["retval"] = handler(data.at("args"));

        // check retval agains manifest
        if (this->validate_data_with_schema) {
            try {
                // only use validator on non-null return types
                if (!(res_data.at("retval").is_null() &&
                      (!cmd_definition.contains("result") || cmd_definition.at("result").is_null()))) {
                    json_validator validator(
                        [this](const json_uri& uri, json& schema) { this->config.ref_loader(uri, schema); },
                        format_checker);
                    validator.set_root_schema(cmd_definition.at("result"));
                    validator.validate(res_data.at("retval"));
                }

            } catch (const std::exception& e) {
                EVLOG_warning << fmt::format("Ignoring return value of cmd '{}' because the validation of the result "
                                             "failed: {}\ndefinition: {}\ndata: {}",
                                             cmd_name, e.what(), cmd_definition, res_data);
                return;
            }
        }

        EVLOG_verbose << fmt::format("RETVAL: {}", res_data["retval"].dump());
        res_data["origin"] = this->module_id;

        const json res_publish_data = json::object({{"name", cmd_name}, {"type", "result"}, {"data", res_data}});

        this->mqtt_abstraction->publish(cmd_topic, res_publish_data);
    };

    const auto typed_handler =
        std::make_shared<TypedHandler>(cmd_name, HandlerType::Call, std::make_shared<Handler>(wrapper));
    this->mqtt_abstraction->register_handler(cmd_topic, typed_handler, QOS::QOS2);

    // this list of registered cmds will be used later on to check if all cmds
    // defined in manifest are provided by code
    this->registered_cmds[impl_id].insert(cmd_name);
}

void Everest::provide_cmd(const cmd& cmd) {
    BOOST_LOG_FUNCTION();

    const auto impl_id = cmd.impl_id;
    const auto cmd_name = cmd.cmd_name;
    const auto handler = cmd.cmd;
    const auto arg_types = cmd.arg_types;
    const auto return_type = cmd.return_type;

    // extract manifest definition of this command
    json cmd_definition = get_cmd_definition(this->module_id, impl_id, cmd_name, false);

    std::set<std::string> arg_names;
    for (const auto& arg_type : arg_types) {
        arg_names.insert(arg_type.first);
    }

    // check arguments of handler against manifest
    if (cmd_definition.at("arguments").size() != arg_types.size()) {
        EVLOG_AND_THROW(EverestApiError(fmt::format(
            "{}->{}({}): Argument count of cmd handler does not match manifest!",
            this->config.printable_identifier(this->module_id, impl_id), cmd_name, fmt::join(arg_names, ","))));
    }

    std::set<std::string> unknown_arguments;
    std::set<std::string> cmd_arguments;
    if (cmd_definition.contains("arguments")) {
        cmd_arguments = Config::keys(cmd_definition.at("arguments"));
    }

    std::set_difference(arg_names.begin(), arg_names.end(), cmd_arguments.begin(), cmd_arguments.end(),
                        std::inserter(unknown_arguments, unknown_arguments.end()));

    if (!unknown_arguments.empty()) {
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("{}->{}({}): Argument names of cmd handler do not match manifest: {} != {}!",
                        this->config.printable_identifier(this->module_id, impl_id), cmd_name,
                        fmt::join(arg_names, ","), fmt::join(arg_names, ","), fmt::join(cmd_arguments, ","))));
    }

    const std::string arg_name = check_args(arg_types, cmd_definition.at("arguments"));

    if (!arg_name.empty()) {
        EVLOG_AND_THROW(EverestApiError(fmt::format(
            "{}->{}({}): Cmd handler argument type '{}' for '{}' does not match manifest type '{}'!",
            this->config.printable_identifier(this->module_id, impl_id), cmd_name, fmt::join(arg_names, ","),
            fmt::join(arg_types.at(arg_name), ","), arg_name, cmd_definition.at("arguments").at(arg_name).at("type"))));
    }

    // validate return value annotations
    if (!check_arg(return_type, cmd_definition.at("result"))) {
        // FIXME (aw): this gives more output EVLOG(error) << oss.str(); than the EVTHROW, why?
        EVLOG_AND_THROW(EverestApiError(
            fmt::format("{}->{}({}): Cmd handler return type '{}' does not match manifest type '{}'!",
                        this->config.printable_identifier(this->module_id, impl_id), cmd_name,
                        fmt::join(arg_names, ","), fmt::join(return_type, ","), cmd_definition.at("result"))));
    }

    return this->provide_cmd(impl_id, cmd_name, [handler](json data) {
        // call cmd handlers (handle async or normal handlers being both:
        // methods or functions)
        // FIXME (aw): this behaviour needs to be checked, i.e. how to distinguish in json between no value and null?
        return handler(std::move(data)).value_or(nullptr);
    });
}

json Everest::get_cmd_definition(const std::string& module_id, const std::string& impl_id, const std::string& cmd_name,
                                 bool is_call) {
    BOOST_LOG_FUNCTION();

    const auto& module_name = this->config.get_module_name(module_id);
    const auto& cmds = this->config.get_module_cmds(module_name, impl_id);

    if (!this->config.module_provides(module_name, impl_id)) {
        if (!is_call) {
            EVLOG_AND_THROW(EverestApiError(fmt::format(
                "Module {} tries to provide implementation '{}' not declared in manifest!", module_name, impl_id)));
        } else {
            EVLOG_AND_THROW(EverestApiError(
                fmt::format("{} tries to call command '{}' of implementation '{}' not declared in manifest of {}",
                            this->config.printable_identifier(module_id), cmd_name, impl_id, module_name)));
        }
    }

    if (!cmds.contains(cmd_name)) {
        const std::string intf =
            this->config.get_manifests().at(module_name).at("provides").at(impl_id).at("interface");
        if (!is_call) {
            EVLOG_AND_THROW(
                EverestApiError(fmt::format("{} tries to provide cmd '{}' not declared in its interface {}!",
                                            this->config.printable_identifier(module_id, impl_id), cmd_name, intf)));
        } else {
            EVLOG_AND_THROW(EverestApiError(fmt::format("{} tries to call cmd '{}' not declared in interface {} of {}!",
                                                        this->config.printable_identifier(module_id), cmd_name, intf,
                                                        this->config.printable_identifier(module_id, impl_id))));
        }
    }

    return cmds.at(cmd_name);
}

json Everest::get_cmd_definition(const std::string& module_id, const std::string& impl_id,
                                 const std::string& cmd_name) {
    BOOST_LOG_FUNCTION();

    return get_cmd_definition(module_id, impl_id, cmd_name, false);
}

bool Everest::is_telemetry_enabled() {
    BOOST_LOG_FUNCTION();
    return (this->telemetry_enabled && this->telemetry_config.has_value());
}

std::string Everest::check_args(const Arguments& func_args, json manifest_args) {
    BOOST_LOG_FUNCTION();

    for (const auto& func_arg : func_args) {
        const auto arg_name = func_arg.first;
        const auto arg_types = func_arg.second;

        if (!check_arg(arg_types, manifest_args.at(arg_name))) {
            return arg_name;
        }
    }

    return {};
}

bool Everest::check_arg(ArgumentType arg_types, json manifest_arg) {
    BOOST_LOG_FUNCTION();

    // FIXME (aw): the error messages here need to be taken into the
    //             correct context!

    const auto& manifest_arg_type = manifest_arg.at("type");

    if (manifest_arg_type.is_string()) {
        if (manifest_arg_type == "null") {
            // arg_types should be empty if the type is null (void)
            if (arg_types.size()) {
                EVLOG_error << "expeceted 'null' type, but got another type";
                return false;
            }
            return true;
        }
        // direct comparison
        // FIXME (aw): arg_types[0] access should be checked, otherwise core dumps
        if (arg_types[0] != manifest_arg_type) {
            EVLOG_error << fmt::format("types do not match: {} != {}", arg_types[0], manifest_arg_type);
            return false;
        }
        return true;
    }

    for (std::size_t i = 0; i < arg_types.size(); i++) {
        if (arg_types[i] != manifest_arg_type.at(i)) {
            EVLOG_error << fmt::format("types do not match: {} != {}", arg_types[i], manifest_arg_type.at(i));
            return false;
        }
    }
    return true;
}

void Everest::check_external_mqtt() {
    // check if external mqtt is enabled
    if (!module_manifest.contains("enable_external_mqtt") && !module_manifest["enable_external_mqtt"]) {
        EVLOG_AND_THROW(EverestApiError(fmt::format("Module {} tries to provide an external MQTT handler, but didn't "
                                                    "set 'enable_external_mqtt' to 'true' in its manifest",
                                                    config.printable_identifier(module_id))));
    }
}

std::string Everest::check_external_mqtt(const std::string& topic) {
    check_external_mqtt();
    return fmt::format("{}{}", mqtt_external_prefix, topic);
}

UnsubscribeToken Everest::create_external_handler(const std::string& topic, const std::string& external_topic,
                                                  const StringPairHandler& handler) {
    const auto token = std::make_shared<TypedHandler>(HandlerType::ExternalMQTT, std::make_shared<Handler>(handler));
    mqtt_abstraction->register_handler(external_topic, token, QOS::QOS0);
    return [this, topic, token]() { this->mqtt_abstraction->unregister_handler(topic, token); };
}

std::optional<Mapping> get_impl_mapping(std::optional<ModuleTierMappings> module_tier_mappings,
                                        const std::string& impl_id) {
    if (not module_tier_mappings.has_value()) {
        return std::nullopt;
    }
    const auto& mapping = module_tier_mappings.value();
    if (mapping.implementations.find(impl_id) == mapping.implementations.end()) {
        // if no specific implementation mapping is given, use the module mapping
        return mapping.module;
    }
    return mapping.implementations.at(impl_id);
}
} // namespace Everest
