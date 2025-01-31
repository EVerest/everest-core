// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include "module.hpp"

#include <pybind11/pybind11.h>

#include <utils/error/error_factory.hpp>
#include <utils/error/error_manager_impl.hpp>
#include <utils/error/error_manager_req.hpp>
#include <utils/error/error_state_monitor.hpp>

std::unique_ptr<Everest::Everest>
Module::create_everest_instance(const std::string& module_id, const Everest::Config& config,
                                const Everest::RuntimeSettings& rs,
                                std::shared_ptr<Everest::MQTTAbstraction> mqtt_abstraction) {
    return std::make_unique<Everest::Everest>(module_id, config, rs.validate_schema, mqtt_abstraction,
                                              rs.telemetry_prefix, rs.telemetry_enabled);
}

Module::Module(const RuntimeSession& session) : Module(get_variable_from_env("EV_MODULE"), session) {
}

Module::Module(const std::string& module_id_, const RuntimeSession& session_) :
    module_id(module_id_), session(session_), start_time(std::chrono::system_clock::now()) {

    this->mqtt_abstraction = std::make_shared<Everest::MQTTAbstraction>(session.get_mqtt_settings());
    this->mqtt_abstraction->connect();
    this->mqtt_abstraction->spawn_main_loop_thread();

    const auto result = Everest::get_module_config(this->mqtt_abstraction, module_id);

    this->rs = std::make_unique<Everest::RuntimeSettings>(result.at("settings"));

    this->config_ = std::make_unique<Everest::Config>(session.get_mqtt_settings(), result);

    const auto& config = get_config();

    this->handle = create_everest_instance(module_id, config, *this->rs, this->mqtt_abstraction);

    // determine the fulfillments for our requirements
    const std::string& module_name = config.get_main_config().at(module_id).at("module");
    const auto module_manifest = config.get_manifests().at(module_name);

    // setup module info
    module_info = config.get_module_info(module_id);
    populate_module_info_path_from_runtime_settings(module_info, *this->rs);

    // setup implementations
    for (const auto& implementation : module_manifest.at("provides").items()) {
        const auto& implementation_id = implementation.key();
        const std::string interface_name = implementation.value().at("interface");
        const auto& interface_def = config.get_interface_definition(interface_name);
        implementations.emplace(implementation_id, create_everest_interface_from_definition(interface_def));
    }

    // setup requirements
    for (const auto& requirement : module_manifest.at("requires").items()) {
        const auto& requirement_id = requirement.key();
        const std::string interface_name = requirement.value().at("interface");
        const auto& interface_def = config.get_interface_definition(interface_name);
        requirements.emplace(requirement_id, create_everest_interface_from_definition(interface_def));
    }
}

ModuleSetup Module::say_hello() {
    handle->connect();
    handle->spawn_main_loop_thread();
    return create_setup_from_config(module_id, get_config());
}

json Module::call_command(const Fulfillment& fulfillment, const std::string& cmd_name, json args) {
    // FIXME (aw): we're releasing the GIL here, because the mqtt thread will want to aquire it when calling the
    // callbacks
    pybind11::gil_scoped_release release;
    const auto& result = handle->call_cmd(fulfillment.requirement, cmd_name, std::move(args));
    return result;
}

void Module::publish_variable(const std::string& impl_id, const std::string& var_name, json value) {
    // NOTE (aw): publish_var just sends output directly via mqtt, so we don't need to release here as opposed to
    // call_command
    handle->publish_var(impl_id, var_name, std::move(value));
}

void Module::implement_command(const std::string& impl_id, const std::string& cmd_name,
                               std::function<json(json)> command_handler) {
    auto& handler = command_handlers.emplace_back(std::move(command_handler));

    handle->provide_cmd(impl_id, cmd_name, [&handler](json args) { return handler(std::move(args)); });
}

void Module::subscribe_variable(const Fulfillment& fulfillment, const std::string& var_name,
                                std::function<void(json)> subscription_callback) {

    auto& callback = subscription_callbacks.emplace_back(std::move(subscription_callback));
    handle->subscribe_var(fulfillment.requirement, var_name, [&callback](json args) { callback(std::move(args)); });
}

void Module::raise_error(const std::string& impl_id, const Everest::error::Error& error) {
    handle->get_error_manager_impl(impl_id)->raise_error(error);
}

void Module::clear_error(const std::string& impl_id, const Everest::error::ErrorType& type) {
    handle->get_error_manager_impl(impl_id)->clear_error(type);
}

void Module::clear_error(const std::string& impl_id, const Everest::error::ErrorType& type,
                         const Everest::error::ErrorSubType& sub_type) {
    handle->get_error_manager_impl(impl_id)->clear_error(type, sub_type);
}

void Module::clear_all_errors_of_impl(const std::string& impl_id) {
    handle->get_error_manager_impl(impl_id)->clear_all_errors();
}

void Module::clear_all_errors_of_impl(const std::string& impl_id, const Everest::error::ErrorType& type) {
    handle->get_error_manager_impl(impl_id)->clear_all_errors(type);
}

std::shared_ptr<Everest::error::ErrorStateMonitor>
Module::get_error_state_monitor_impl(const std::string& impl_id) const {
    return handle->get_error_state_monitor_impl(impl_id);
}

std::shared_ptr<Everest::error::ErrorFactory> Module::get_error_factory(const std::string& impl_id) const {
    return handle->get_error_factory(impl_id);
}

void Module::subscribe_error(const Fulfillment& fulfillment, const Everest::error::ErrorType& type,
                             const Everest::error::ErrorCallback& callback,
                             const Everest::error::ErrorCallback& clear_callback) {
    handle->get_error_manager_req(fulfillment.requirement)->subscribe_error(type, callback, clear_callback);
}

void Module::subscribe_all_errors(const Fulfillment& fulfillment, const Everest::error::ErrorCallback& callback,
                                  const Everest::error::ErrorCallback& clear_callback) {
    handle->get_error_manager_req(fulfillment.requirement)->subscribe_all_errors(callback, clear_callback);
}

std::shared_ptr<Everest::error::ErrorStateMonitor>
Module::get_error_state_monitor_req(const Fulfillment& fulfillment) const {
    return handle->get_error_state_monitor_req(fulfillment.requirement);
}
