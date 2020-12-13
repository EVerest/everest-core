//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include "ld-ev.hpp"

#include "PowerIn.hpp"
#include "main/power_inImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

static Everest::ModuleAdapter adapter{};
static Everest::PtrContainer<PowerIn> mod_ptr{};

// per module configs
static main::Conf main_config;
static Conf module_conf;

void LdEverest::init(ModuleConfigs module_configs) {
    EVLOG(debug) << "init() called on module PowerIn";

    // populate config for provided implementations
    auto main_config_input = std::move(module_configs["main"]);
    main_config.max_current = boost::get<double>(main_config_input["max_current"]);
    main_config.phase_count = boost::get<int>(main_config_input["phase_count"]);

    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module PowerIn";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for PowerIn";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for PowerIn";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for PowerIn";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for PowerIn";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for PowerIn";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module PowerIn";

    adapter.check_complete();

    auto p_main = std::make_unique<main::power_inImpl>(&adapter, mod_ptr, main_config);
    adapter.gather_cmds(*p_main);

    static PowerIn module(std::move(p_main), module_conf);

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
