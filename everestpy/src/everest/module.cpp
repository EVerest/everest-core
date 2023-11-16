// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include "module.hpp"

#include <pybind11/pybind11.h>

std::unique_ptr<Everest::Everest> Module::create_everest_instance(const std::string& module_id,
                                                                  const RuntimeSession& session) {
    const auto& rs = session.get_runtime_settings();
    return std::make_unique<Everest::Everest>(module_id, session.get_config(), rs->validate_schema,
                                              rs->mqtt_broker_host, rs->mqtt_broker_port, rs->mqtt_everest_prefix,
                                              rs->mqtt_external_prefix, rs->telemetry_prefix, rs->telemetry_enabled);
}

static std::string get_ev_module_from_env() {
    const auto module_id = std::getenv("EV_MODULE");
    if (module_id == nullptr) {
        throw std::runtime_error("EV_MODULE needed for everestpy");
    }

    return module_id;
}

Module::Module(const RuntimeSession& session) : Module(get_ev_module_from_env(), session) {
}

Module::Module(const std::string& module_id_, const RuntimeSession& session_) :
    module_id(module_id_), session(session_), handle(create_everest_instance(module_id, session)) {

    // determine the fulfillments for our requirements
    auto& config = session.get_config();
    const std::string& module_name = config.get_main_config().at(module_id).at("module");
    auto module_manifest = config.get_manifests().at(module_name);

    // setup module info
    module_info = config.get_module_info(module_id);
    populate_module_info_path_from_runtime_settings(module_info, session.get_runtime_settings());

    // setup implementations
    for (auto& implementation : module_manifest.at("provides").items()) {
        const auto& implementation_id = implementation.key();
        const std::string interface_name = implementation.value().at("interface");
        const auto& interface_def = config.get_interface_definition(interface_name);
        implementations.emplace(implementation_id, create_everest_interface_from_definition(interface_def));
    }

    // setup requirements
    for (auto& requirement : module_manifest.at("requires").items()) {
        const auto& requirement_id = requirement.key();
        const std::string interface_name = requirement.value().at("interface");
        const auto& interface_def = config.get_interface_definition(interface_name);
        requirements.emplace(requirement_id, create_everest_interface_from_definition(interface_def));
    }
}

ModuleSetup Module::say_hello() {
    handle->connect();
    handle->spawn_main_loop_thread();

    return create_setup_from_config(module_id, session.get_config());
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

std::string Module::raise_error(const std::string& impl_id, const std::string& error_type, const std::string& message,
                                const std::string& severity) {
    return handle->raise_error(impl_id, error_type, message, severity);
}

json Module::request_clear_error_uuid(const std::string& impl_id, const std::string& uuid) {
    pybind11::gil_scoped_release release;
    const auto& result =
        handle->request_clear_error(Everest::error::RequestClearErrorOption::ClearUUID, impl_id, uuid, "");
    return result;
}

json Module::request_clear_error_all_of_type(const std::string& impl_id, const std::string& error_type) {
    pybind11::gil_scoped_release release;
    const auto& result = handle->request_clear_error(Everest::error::RequestClearErrorOption::ClearAllOfTypeOfModule,
                                                     impl_id, "", error_type);
    return result;
}

json Module::request_clear_error_all_of_module(const std::string& impl_id) {
    pybind11::gil_scoped_release release;
    const auto& result =
        handle->request_clear_error(Everest::error::RequestClearErrorOption::ClearAllOfModule, impl_id, "", "");
    return result;
}

void Module::subscribe_error(const Fulfillment& fulfillment, const std::string& error_type,
                             const JsonCallback& error_cb_, const JsonCallback& error_cleared_cb_) {
    auto& error_cb = err_susbcription_callbacks.emplace_back(std::move(error_cb_));
    auto& error_cleared_cb = err_susbcription_callbacks.emplace_back(std::move(error_cleared_cb_));
    handle->subscribe_error(fulfillment.requirement, error_type, [&error_cb](json args) { error_cb(std::move(args)); });
    handle->subscribe_error_cleared(fulfillment.requirement, error_type,
                                    [&error_cleared_cb](json args) { error_cleared_cb(std::move(args)); });
}

void Module::subscribe_all_errors(const Fulfillment& fulfillment, const JsonCallback& error_cb_,
                                  const JsonCallback& error_cleared_cb_) {
    auto& error_cb = err_susbcription_callbacks.emplace_back(std::move(error_cb_));
    auto& error_cleared_cb = err_susbcription_callbacks.emplace_back(std::move(error_cleared_cb_));
    for (const std::string& error_type : requirements.at(fulfillment.requirement.id).errors) {
        handle->subscribe_error(fulfillment.requirement, error_type,
                                [&error_cb](json args) { error_cb(std::move(args)); });
        handle->subscribe_error_cleared(fulfillment.requirement, error_type,
                                        [&error_cleared_cb](json args) { error_cleared_cb(std::move(args)); });
    }
}
