//
// AUTO GENERATED - DO NOT EDIT!
// template version 0.0.1
//

#include "ld-ev.hpp"

#include "EvseManager.hpp"
#include "evse/evse_managerImpl.hpp"
#include "evse_energy_control/evse_manager_energy_controlImpl.hpp"

#include <boost/dll/alias.hpp>

namespace module {

static Everest::ModuleAdapter adapter {};
static Everest::PtrContainer<EvseManager> mod_ptr {};

// per module configs
static evse::Conf evse_config;
static evse_energy_control::Conf evse_energy_control_config;
static Conf module_conf;

void LdEverest::init(ModuleConfigs module_configs) {
    EVLOG(debug) << "init() called on module EvseManager";

    // populate config for provided implementations
    auto evse_config_input = std::move(module_configs["evse"]);

    auto evse_energy_control_config_input = std::move(module_configs["evse_energy_control"]);


    module_conf.three_phases = boost::get<bool>(module_configs["!module"]["three_phases"]);
    module_conf.has_ventilation = boost::get<bool>(module_configs["!module"]["has_ventilation"]);
    module_conf.country_code = boost::get<std::string>(module_configs["!module"]["country_code"]);
    module_conf.rcd_enabled = boost::get<bool>(module_configs["!module"]["rcd_enabled"]);

    mod_ptr->init();
}

void LdEverest::ready() {
    EVLOG(debug) << "ready() called on module EvseManager";
    mod_ptr->ready();
}

void register_call_handler(Everest::ModuleAdapter::CallFunc call) {
    EVLOG(debug) << "registering call_cmd callback for EvseManager";
    adapter.call = std::move(call);
}

void register_publish_handler(Everest::ModuleAdapter::PublishFunc publish) {
    EVLOG(debug) << "registering publish_var callback for EvseManager";
    adapter.publish = std::move(publish);
}

void register_subscribe_handler(Everest::ModuleAdapter::SubscribeFunc subscribe) {
    EVLOG(debug) << "registering subscribe_var callback for EvseManager";
    adapter.subscribe = std::move(subscribe);
}

void register_ext_mqtt_publish_handler(Everest::ModuleAdapter::ExtMqttPublishFunc ext_mqtt_publish) {
    EVLOG(debug) << "registering external_mqtt_publish callback for EvseManager";
    adapter.ext_mqtt_publish = std::move(ext_mqtt_publish);
}

void register_ext_mqtt_subscribe_handler(Everest::ModuleAdapter::ExtMqttSubscribeFunc ext_mqtt_subscribe) {
    EVLOG(debug) << "registering external_mqtt_handler callback for EvseManager";
    adapter.ext_mqtt_subscribe = std::move(ext_mqtt_subscribe);
}

std::vector<Everest::cmd> everest_register() {
    EVLOG(debug) << "everest_register() called on module EvseManager";

    adapter.check_complete();

    auto p_evse = std::make_unique<evse::evse_managerImpl>(&adapter, mod_ptr, evse_config);
    adapter.gather_cmds(*p_evse);

    auto p_evse_energy_control = std::make_unique<evse_energy_control::evse_manager_energy_controlImpl>(&adapter, mod_ptr, evse_energy_control_config);
    adapter.gather_cmds(*p_evse_energy_control);

    auto r_bsp = std::make_unique<board_support_ACIntf>(&adapter, "bsp");
    auto r_powermeter = std::make_unique<powermeterIntf>(&adapter, "powermeter");

    static Everest::MqttProvider mqtt_provider(adapter);

    static EvseManager module(mqtt_provider, std::move(p_evse), std::move(p_evse_energy_control), std::move(r_bsp), std::move(r_powermeter), module_conf);

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
