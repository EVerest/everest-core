// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.3
//

#include "ld-ev.hpp"

#include "SunspecReader.hpp"
#include "main/sunspec_readerImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

// FIXME (aw): could this way of keeping static variables be changed somehow?
static Everest::ModuleAdapter adapter{};
static Everest::PtrContainer<SunspecReader> mod_ptr{};

// per module configs
static main::Conf main_config;
static Conf module_conf;
static ModuleInfo module_info;

void LdEverest::init(ModuleConfigs module_configs, const ModuleInfo& mod_info) {
    EVLOG(debug) << "init() called on module SunspecReader";

    // populate config for provided implementations
    auto main_config_input = std::move(module_configs["main"]);
    main_config.ip_address = boost::get<std::string>(main_config_input["ip_address"]);
    main_config.port = boost::get<int>(main_config_input["port"]);
    main_config.unit = boost::get<int>(main_config_input["unit"]);
    main_config.query = boost::get<std::string>(main_config_input["query"]);
    main_config.read_interval = boost::get<int>(main_config_input["read_interval"]);

    module_info = mod_info;

    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module SunspecReader";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for SunspecReader";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for SunspecReader";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for SunspecReader";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for SunspecReader";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for SunspecReader";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module SunspecReader";

    adapter.check_complete();

    auto p_main = std::make_unique<main::sunspec_readerImpl>(&adapter, mod_ptr, main_config);
    adapter.gather_cmds(*p_main);

    static Everest::MqttProvider mqtt_provider(adapter);

    static SunspecReader module(module_info, mqtt_provider, std::move(p_main), module_conf);

    mod_ptr.set(&module);

    return adapter.registered_commands;
}

} // namespace module

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::LdEverest::init, init)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::LdEverest::ready, ready)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::everest_register, everest_register)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_call_handler, everest_register_call_cmd_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_publish_handler, everest_register_publish_var_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_subscribe_handler, everest_register_subscribe_var_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_ext_mqtt_publish_handler, everest_register_external_mqtt_publish_callback)
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr,cppcoreguidelines-avoid-non-const-global-variables)
BOOST_DLL_ALIAS(module::register_ext_mqtt_subscribe_handler, everest_register_external_mqtt_handler_callback)
