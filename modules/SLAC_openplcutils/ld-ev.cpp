//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include "ld-ev.hpp"

#include "SLAC_openplcutils.hpp"
#include "main/ISO15118_3_SLACImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

static Everest::ModuleAdapter adapter{};
static Everest::PtrContainer<SLAC_openplcutils> mod_ptr{};

// per module configs
static main::Conf main_config;
static Conf module_conf;

void LdEverest::init(ModuleConfigs module_configs) {
    EVLOG(debug) << "init() called on module SLAC_openplcutils";

    // populate config for provided implementations
    auto main_config_input = std::move(module_configs["main"]);
    main_config.device = boost::get<std::string>(main_config_input["device"]);
    main_config.sid = boost::get<std::string>(main_config_input["sid"]);
    main_config.nmk = boost::get<std::string>(main_config_input["nmk"]);
    main_config.nid = boost::get<std::string>(main_config_input["nid"]);
    main_config.timeout = boost::get<int>(main_config_input["timeout"]);
    main_config.time_to_sound = boost::get<int>(main_config_input["time_to_sound"]);
    main_config.number_of_sounds = boost::get<int>(main_config_input["number_of_sounds"]);

    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module SLAC_openplcutils";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for SLAC_openplcutils";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for SLAC_openplcutils";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for SLAC_openplcutils";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for SLAC_openplcutils";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for SLAC_openplcutils";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module SLAC_openplcutils";

    adapter.check_complete();

    auto p_main = std::make_unique<main::ISO15118_3_SLACImpl>(&adapter, mod_ptr, main_config);
    adapter.gather_cmds(*p_main);

    static SLAC_openplcutils module(std::move(p_main), module_conf);

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
